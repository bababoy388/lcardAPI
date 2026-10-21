#include "server.h"
#include "httplib.h"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include <sstream>
#include <iomanip>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <vector>
#include <sys/stat.h>

// ============================================================
// Глобальные переменные
// ============================================================
std::mutex             g_bufMutex;
std::vector<Sample>    g_ringBuffer;
const size_t           RING_SIZE = 100000;

std::atomic<bool>      g_running{false};
std::atomic<bool>      g_stopRequested{false};
std::atomic<long long> g_totalSamples{0};

std::atomic<bool>      g_restartRequested{false};

std::atomic<bool>      g_configDirty{false};

std::atomic<int>       g_phase{PHASE_IDLE};
std::atomic<long long> g_phaseStartNs{0};
std::atomic<long long> g_recordStartEpochNs{0};

std::string            g_lastError;
AppConfig              g_cfg;
int                    g_activeChannels = 0;

// Имя текущего открытого файла
std::string            g_currentFileName;

// Файл
static std::mutex      g_fileMutex;
static FILE*           g_dataFile = nullptr;
static long long       g_fileStartSampleIdx = 0;

// ============================================================
// CP1251 → UTF-8
// ============================================================
static std::string cp1251_to_utf8(const std::string& src) {
#ifdef _WIN32
    // ваша текущая реализация с MultiByteToWideChar / WideCharToMultiByte
    if (src.empty()) return src;
    int wlen = MultiByteToWideChar(1251, 0, src.c_str(), -1, NULL, 0);
    if (wlen <= 0) return src;
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(1251, 0, src.c_str(), -1, &wstr[0], wlen);
    int u8len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (u8len <= 0) return src;
    std::string u8str(u8len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &u8str[0], u8len, NULL, NULL);
    if (!u8str.empty() && u8str.back() == '\0') u8str.pop_back();
    return u8str;
#else
    // Linux: L-Card отдаёт UTF-8/ASCII
    return src;
#endif
}

// ============================================================
// JSON: экранирование строк
// ============================================================
static std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if ((unsigned char)c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

// ============================================================
// Буфер
// ============================================================
void addSamples(const double* data, int points, int channels, double fs) {
    std::lock_guard<std::mutex> lock(g_bufMutex);
    for (int k = 0; k < points; k++) {
        Sample s;
        s.time = (double)g_totalSamples.load() / fs;
        for (int ch = 0; ch < 4; ch++) s.values[ch] = 0.0;
        for (int ch = 0; ch < channels; ch++) {
            s.values[ch] = data[k * channels + ch];
        }
        g_ringBuffer.push_back(s);
        g_totalSamples++;
    }
    if (g_ringBuffer.size() > RING_SIZE) {
        g_ringBuffer.erase(g_ringBuffer.begin(),
                           g_ringBuffer.begin() +
                           (g_ringBuffer.size() - RING_SIZE));
    }
}

std::string getDataCSV(int n) {
    std::lock_guard<std::mutex> lock(g_bufMutex);
    size_t total = g_ringBuffer.size();
    size_t start = (total > (size_t)n) ? total - n : 0;

    std::ostringstream csv;
    csv << "time";
    for (int ch = 0; ch < g_activeChannels; ch++) csv << ",ch" << (ch + 1);
    csv << "\n";

    for (size_t i = start; i < total; i++) {
        csv << std::fixed << std::setprecision(6) << g_ringBuffer[i].time;
        for (int ch = 0; ch < g_activeChannels; ch++) {
            csv << "," << std::setprecision(6) << g_ringBuffer[i].values[ch];
        }
        csv << "\n";
    }
    return csv.str();
}

void clearBuffer() {
    std::lock_guard<std::mutex> lock(g_bufMutex);
    g_ringBuffer.clear();
    g_totalSamples = 0;
}

size_t bufferSize() {
    std::lock_guard<std::mutex> lock(g_bufMutex);
    return g_ringBuffer.size();
}

long long totalSamples() {
    return g_totalSamples.load();
}

void setLastError(const std::string& err) {
    std::lock_guard<std::mutex> lock(g_bufMutex);
    g_lastError = cp1251_to_utf8(err);
}

std::string getLastError() {
    std::lock_guard<std::mutex> lock(g_bufMutex);
    return g_lastError;
}

// ============================================================
// Имя файла для текущего цикла
// ============================================================
std::string generateDataFileName() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif

    char ts[32];
    std::strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", &tm_buf);

    std::string base = g_cfg.output_file;
    const std::string ext = ".csv";
    if (base.size() >= ext.size() &&
        base.compare(base.size() - ext.size(), ext.size(), ext) == 0) {
        base = base.substr(0, base.size() - ext.size());
    }
    if (base.empty()) base = "data";

    return base + "_" + std::string(ts) + ".csv";
}

// ============================================================
// Наносекунды (Unix epoch) → "YYYY-MM-DD HH:MM:SS.uuuuuu"
// ============================================================
static std::string formatTimestampNs(long long ns) {
    time_t sec = (time_t)(ns / 1000000000LL);
    long long frac = ns % 1000000000LL;
    if (frac < 0) { frac += 1000000000LL; sec -= 1; }

    long long micros = frac / 1000;

    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &sec);
#else
    localtime_r(&sec, &tm_buf);
#endif

    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);

    char out[96];
    std::snprintf(out, sizeof(out), "%s.%06lld", buf, micros);
    return std::string(out);
}

// ============================================================
// Файл данных
// ============================================================
bool isDataFileOpen() {
    std::lock_guard<std::mutex> lock(g_fileMutex);
    return g_dataFile != nullptr;
}

bool openDataFile() {
    std::lock_guard<std::mutex> lock(g_fileMutex);
    if (g_dataFile) return true;

    std::string fname = generateDataFileName();

    g_dataFile = fopen(fname.c_str(), "w");
    if (!g_dataFile) {
        setLastError("Не удалось создать файл данных: " + fname);
        return false;
    }

    fprintf(g_dataFile, "timestamp");
    for (int ch = 0; ch < g_activeChannels; ch++) {
        fprintf(g_dataFile, ",ch%d", ch + 1);
    }
    fprintf(g_dataFile, "\n");
    fflush(g_dataFile);

    g_fileStartSampleIdx = g_totalSamples.load();

    g_currentFileName = fname;
    printf("Открыт новый файл данных: %s\n", fname.c_str());
    return true;
}

void closeDataFile() {
    std::lock_guard<std::mutex> lock(g_fileMutex);
    if (g_dataFile) {
        fflush(g_dataFile);
        fclose(g_dataFile);
        g_dataFile = nullptr;
        printf("Файл данных закрыт: %s\n", g_currentFileName.c_str());
        g_currentFileName.clear();
    }
}

void writeBlockToFile(const double* data, int points, int channels,
                      double fs, long long startSampleIdx) {
    std::lock_guard<std::mutex> lock(g_fileMutex);
    if (!g_dataFile) return;

    long long t0ns = g_recordStartEpochNs.load();

    for (int k = 0; k < points; k++) {
        long long localIdx = (startSampleIdx + k) - g_fileStartSampleIdx;
        long long offsetNs = (long long)((long double)localIdx * 1e9L / fs);
        long long absNs = t0ns + offsetNs;

        std::string ts = formatTimestampNs(absNs);
        fprintf(g_dataFile, "%s", ts.c_str());

        for (int ch = 0; ch < channels; ch++) {
            fprintf(g_dataFile, ",%.6f", data[k * channels + ch]);
        }
        fprintf(g_dataFile, "\n");
    }
    fflush(g_dataFile);
}

// ============================================================
/// === НОВОЕ === Работа с ltrd через управляющее соединение
// ============================================================

// Открывает управляющее соединение с локальным ltrd.
// Возвращает LTR_OK и заполненный TLTR, либо код ошибки.
static INT openControlConnection(TLTR* ltr) {
    LTR_Init(ltr);
    ltr->cc = LTR_CC_CHNUM_CONTROL;

    INT res = LTR_OpenSvcControl(ltr, LTRD_ADDR_DEFAULT, LTRD_PORT_DEFAULT);
    return res;
}

// Парсит "192.168.1.100" → DWORD (host order).
static bool parseIpToDword(const std::string& s, DWORD* out) {
    unsigned int a, b, c, d;
    if (sscanf(s.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) != 4) return false;
    if (a > 255 || b > 255 || c > 255 || d > 255) return false;
    *out = ((DWORD)a << 24) | ((DWORD)b << 16) | ((DWORD)c << 8) | (DWORD)d;
    return true;
}

// DWORD → "192.168.1.100"
static std::string dwordToIp(DWORD ip) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
             (unsigned)((ip >> 24) & 0xFF),
             (unsigned)((ip >> 16) & 0xFF),
             (unsigned)((ip >> 8) & 0xFF),
             (unsigned)(ip & 0xFF));
    return std::string(buf);
}

// ============================================================
// HTTP-сервер
// ============================================================
static const char* phaseName(int p) {
    switch (p) {
        case PHASE_RECORDING: return "recording";
        case PHASE_SLEEPING:  return "sleeping";
        default:              return "idle";
    }
}

void runHttpServer() {
    httplib::Server svr;

    // --- GET / ---
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string html =
            "<html><head><meta charset='utf-8'><title>L-Card API</title></head><body>"
            "<h1>L-Card LTR24 HTTP API</h1>"
            "<h2>Просмотр</h2><ul>"
            "<li><a href='/status'>GET /status</a></li>"
            "<li><a href='/config'>GET /config</a></li>"
            "<li><a href='/data?n=100'>GET /data?n=100</a></li>"
            "<li><a href='/data/file/info'>GET /data/file/info</a></li>"
            "<li><a href='/api/ltrd/ip'>GET /api/ltrd/ip</a> — список IP-крейтов</li>"
            "<li><a href='/api/ltrd/crates'>GET /api/ltrd/crates</a> — подключённые крейты</li>"
            "</ul>"
            "<h2>Управление</h2><ul>"
            "<li>GET /start — запуск цикла</li>"
            "<li>GET /stop — остановка</li>"
            "<li>GET /buffer/clear</li>"
            "<li>POST /config?freq_hz=...&record_time=...&sleep_time=...</li>"
            "<li>POST /api/ltrd/ip?ip=...&autoconnect=true&reconnect=true&permanent=true</li>"
            "<li>DELETE /api/ltrd/ip?ip=...</li>"
            "</ul></body></html>";
        res.set_content(html, "text/html; charset=utf-8");
    });

    // --- GET /status ---
    svr.Get("/status", [](const httplib::Request&, httplib::Response& res) {
        double phaseElapsed = 0.0;
        long long ps = g_phaseStartNs.load();
        if (ps > 0) {
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            phaseElapsed = (double)(now - ps) / 1e9;
        }

        std::ostringstream json;
        json << "{"
             << "\"running\":"          << (g_running.load() ? "true" : "false") << ","
             << "\"phase\":\""          << phaseName(g_phase.load()) << "\","
             << "\"phase_elapsed\":"    << std::fixed << std::setprecision(3) << phaseElapsed << ","
             << "\"samples\":"          << totalSamples() << ","
             << "\"buffer_size\":"      << bufferSize() << ","
             << "\"file_open\":"        << (isDataFileOpen() ? "true" : "false") << ","
             << "\"file_name\":\""      << json_escape(g_currentFileName) << "\","
             << "\"active_channels\":"  << g_activeChannels << ","
             << "\"freq_hz\":"          << g_cfg.freq_hz << ","
             << "\"record_time\":"      << g_cfg.record_time << ","
             << "\"sleep_time\":"       << g_cfg.sleep_time << ","
             << "\"last_error\":\""     << json_escape(getLastError()) << "\""
             << "}";
        res.set_content(json.str(), "application/json");
    });

    // --- GET /start ---
    svr.Get("/start", [](const httplib::Request&, httplib::Response& res) {
        g_running = true;
        res.set_content("{\"ok\":true,\"action\":\"started\"}", "application/json");
    });

    // --- GET /stop ---
    svr.Get("/stop", [](const httplib::Request&, httplib::Response& res) {
        g_running = false;
        res.set_content("{\"ok\":true,\"action\":\"stopped\"}", "application/json");
    });

    // --- GET /data?n=1000 ---
    svr.Get("/data", [](const httplib::Request& req, httplib::Response& res) {
        int n = 1000;
        if (req.has_param("n")) {
            try { n = std::stoi(req.get_param_value("n")); } catch (...) {}
            if (n < 1) n = 1;
            if (n > 50000) n = 50000;
        }
        res.set_content(getDataCSV(n), "text/csv");
    });

        // --- Get /api/ltrd/reset ---
    svr.Get("/api/ltrd/reset", [](const httplib::Request& req, httplib::Response& res) {
        std::string csn = req.has_param("csn") ? req.get_param_value("csn") : "";
        int slot = 1;
        if (req.has_param("slot")) {
            try { slot = std::stoi(req.get_param_value("slot")); } catch (...) {}
        }

        TLTR ltr;
        LTR_Init(&ltr);
        ltr.cc = LTR_CC_CHNUM_CONTROL;

        INT openRes = LTR_OpenSvcControl(&ltr, LTRD_ADDR_DEFAULT, LTRD_PORT_DEFAULT);
        if (openRes != LTR_OK) {
            res.status = 503;
            std::string err = "{\"error\":\"" +
                json_escape(LTR_GetErrorString(openRes)) + "\"}";
            res.set_content(err, "application/json");
            return;
        }

        // Сигнатура: (hsrv, crate_iface, csn, slot, flags)
        INT rstRes = LTR_ResetModule(&ltr,
                                     LTR_CRATE_IFACE_UNKNOWN,
                                     csn.empty() ? nullptr : csn.c_str(),
                                     slot,
                                     0);
        LTR_Close(&ltr);

        if (rstRes == LTR_OK) {
            std::ostringstream json;
            json << "{\"ok\":true,\"action\":\"reset\",\"slot\":" << slot << "}";
            res.set_content(json.str(), "application/json");
        } else {
            res.status = 500;
            std::string err = "{\"error\":\"" +
                json_escape(LTR_GetErrorString(rstRes)) + "\"}";
            res.set_content(err, "application/json");
        }
    });

    // --- Get /api/ltrd/restart ---
    svr.Get("/api/ltrd/restart", [](const httplib::Request&, httplib::Response& res) {
        TLTR ltr;
        LTR_Init(&ltr);
        ltr.cc = LTR_CC_CHNUM_CONTROL;

        INT openRes = LTR_OpenSvcControl(&ltr, LTRD_ADDR_DEFAULT, LTRD_PORT_DEFAULT);
        if (openRes != LTR_OK) {
            res.status = 503;
            std::string err = "{\"error\":\"" +
                json_escape(LTR_GetErrorString(openRes)) + "\"}";
            res.set_content(err, "application/json");
            return;
        }

        INT rstRes = LTR_ServerRestart(&ltr);
        LTR_Close(&ltr);

        if (rstRes == LTR_OK) {
            res.set_content("{\"ok\":true,\"action\":\"server_restart\"}",
                            "application/json");
        } else {
            res.status = 500;
            std::string err = "{\"error\":\"" +
                json_escape(LTR_GetErrorString(rstRes)) + "\"}";
            res.set_content(err, "application/json");
        }
    });

    // --- GET /data/file/info ---
    svr.Get("/data/file/info", [](const httplib::Request&, httplib::Response& res) {
        struct stat st;
        long long size = 0;
        bool exists = false;

        std::string fname = g_currentFileName;
        if (!fname.empty()) {
            exists = (_stat(fname.c_str(), &st) == 0);
            if (exists) size = st.st_size;
        }

        std::ostringstream json;
        json << "{"
             << "\"file\":\"" << json_escape(fname) << "\","
             << "\"exists\":" << (exists ? "true" : "false") << ","
             << "\"open\":"   << (isDataFileOpen() ? "true" : "false") << ","
             << "\"size_bytes\":" << size
             << "}";
        res.set_content(json.str(), "application/json");
    });

    // --- GET /config ---
    svr.Get("/config", [](const httplib::Request&, httplib::Response& res) {
        std::ostringstream json;
        json << "{"
             << "\"csn\":\"" << json_escape(g_cfg.csn) << "\","
             << "\"slot\":" << g_cfg.slot << ","
             << "\"freq_hz\":" << g_cfg.freq_hz << ","
             << "\"format\":" << g_cfg.format << ","
             << "\"record_time\":" << g_cfg.record_time << ","
             << "\"sleep_time\":" << g_cfg.sleep_time << ","
             << "\"output_file\":\"" << json_escape(g_cfg.output_file) << "\","
             << "\"write_to_file\":" << (g_cfg.write_to_file ? "true" : "false") << ","
             << "\"block_size\":" << g_cfg.block_size << ","
             << "\"channels\":[";
        for (int i = 0; i < 4; i++) {
            if (i) json << ",";
            json << "{"
                 << "\"enable\":" << (g_cfg.channels[i].enable ? "true" : "false") << ","
                 << "\"range\":" << (int)g_cfg.channels[i].range << ","
                 << "\"ac\":" << (g_cfg.channels[i].ac ? "true" : "false") << ","
                 << "\"icp\":" << (g_cfg.channels[i].icp ? "true" : "false")
                 << "}";
        }
        json << "]}";
        res.set_content(json.str(), "application/json");
    });

    // --- POST /config ---
    svr.Post("/config", [](const httplib::Request& req, httplib::Response& res) {
        bool changed = false;

        auto getInt = [&](const char* name, int& out) {
            if (!req.has_param(name)) return;
            try { out = std::stoi(req.get_param_value(name)); changed = true; } catch (...) {}
        };
        auto getDouble = [&](const char* name, double& out) {
            if (!req.has_param(name)) return;
            try { out = std::stod(req.get_param_value(name)); changed = true; } catch (...) {}
        };

        if (req.has_param("csn")) { g_cfg.csn = req.get_param_value("csn"); changed = true; }
        getInt("slot",       g_cfg.slot);
        getInt("freq_hz",    g_cfg.freq_hz);
        getInt("format",     g_cfg.format);
        getInt("block_size", g_cfg.block_size);
        getDouble("record_time", g_cfg.record_time);
        getDouble("sleep_time",  g_cfg.sleep_time);

        if (req.has_param("output_file")) {
            g_cfg.output_file = req.get_param_value("output_file");
            changed = true;
        }
        if (req.has_param("write_to_file")) {
            std::string v = req.get_param_value("write_to_file");
            g_cfg.write_to_file = (v == "true" || v == "1");
            changed = true;
        }

        for (int i = 0; i < 4; i++) {
            char key[32];

            snprintf(key, sizeof(key), "ch%d_enable", i);
            if (req.has_param(key)) {
                std::string v = req.get_param_value(key);
                g_cfg.channels[i].enable = (v == "true" || v == "1") ? TRUE : FALSE;
                changed = true;
            }
            snprintf(key, sizeof(key), "ch%d_range", i);
            if (req.has_param(key)) {
                try {
                    int r = std::stoi(req.get_param_value(key));
                    g_cfg.channels[i].range = (r == 10) ? LTR24_RANGE_10 : LTR24_RANGE_2;
                    changed = true;
                } catch (...) {}
            }
            snprintf(key, sizeof(key), "ch%d_ac", i);
            if (req.has_param(key)) {
                std::string v = req.get_param_value(key);
                g_cfg.channels[i].ac = (v == "true" || v == "1") ? TRUE : FALSE;
                changed = true;
            }
            snprintf(key, sizeof(key), "ch%d_icp", i);
            if (req.has_param(key)) {
                std::string v = req.get_param_value(key);
                g_cfg.channels[i].icp = (v == "true" || v == "1") ? TRUE : FALSE;
                changed = true;
            }
        }

        int active = 0;
        for (int i = 0; i < 4; i++) if (g_cfg.channels[i].enable) active++;
        g_activeChannels = active;

        bool saved = SaveConfig("config.ini", g_cfg);

        if (saved) {
            printf("[CFG] Конфигурация сохранена — инициирую перезапуск\n");
            fflush(stdout);
            g_running = false;
            g_stopRequested = true;
            g_restartRequested = true;  
        }

        std::ostringstream json;
        json << "{\"ok\":" << (changed ? "true" : "false")
             << ",\"saved\":" << (saved ? "true" : "false")
             << ",\"active_channels\":" << g_activeChannels
             << ",\"note\":\"Изменения вступят в силу при следующем запуске сбора\"}";
        res.set_content(json.str(), "application/json");
    });

    // --- POST /buffer/clear ---
    svr.Post("/buffer/clear", [](const httplib::Request&, httplib::Response& res) {
        clearBuffer();
        res.set_content("{\"ok\":true}", "application/json");
    });

    // ============================================================
    /// === НОВОЕ === Эндпоинты управления ltrd
    // ============================================================

    // --- GET /api/ltrd/ip ---
    // Возвращает список добавленных IP-адресов крейтов с их конфигурацией.
    svr.Get("/api/ltrd/ip", [](const httplib::Request&, httplib::Response& res) {
        TLTR ltr;
        INT openRes = openControlConnection(&ltr);
        if (openRes != LTR_OK) {
            std::ostringstream err;
            err << "{\"error\":\"" << json_escape(LTR_GetErrorString(openRes)) << "\"}";
            res.status = 503;
            res.set_content(err.str(), "application/json");
            return;
        }

        DWORD entriesFound = 0;
        INT listRes = LTR_GetListOfIPCrates(&ltr, 0, 0, 0,
                                            &entriesFound, nullptr, nullptr);

        std::vector<TLTR_CRATE_IP_ENTRY> entries;
        DWORD returned = 0;

        if (listRes == LTR_OK && entriesFound > 0) {
            entries.resize(entriesFound);
            INT getRes = LTR_GetListOfIPCrates(&ltr, entriesFound, 0, 0,
                                               &entriesFound, &returned,
                                               entries.data());
            if (getRes != LTR_OK) {
                LTR_Close(&ltr);
                std::ostringstream err;
                err << "{\"error\":\"" << json_escape(LTR_GetErrorString(getRes)) << "\"}";
                res.status = 500;
                res.set_content(err.str(), "application/json");
                return;
            }
        }

        std::ostringstream arr;
        arr << "[";
        for (DWORD i = 0; i < returned; i++) {
            const auto& e = entries[i];
            if (i) arr << ",";

            std::string ipStr = dwordToIp(e.ip_addr);
            bool autoconn = (e.flags & LTR_CRATE_IP_FLAG_AUTOCONNECT) != 0;
            bool reconnect = (e.flags & LTR_CRATE_IP_FLAG_RECONNECT) != 0;

            arr << "{"
                << "\"ip\":\""        << ipStr << "\","
                << "\"ip_raw\":"      << e.ip_addr << ","
                << "\"csn\":\""       << json_escape(std::string((const char*)e.serial_number)) << "\","
                << "\"status\":"      << (int)e.status << ","
                << "\"flags\":"       << e.flags << ","
                << "\"autoconnect\":" << (autoconn ? "true" : "false") << ","
                << "\"reconnect\":"   << (reconnect ? "true" : "false")
                << "}";
        }
        arr << "]";

        LTR_Close(&ltr);

        std::ostringstream json;
        json << "{\"count\":" << returned
             << ",\"entries\":" << arr.str()
             << "}";
        res.set_content(json.str(), "application/json");
    });

    // --- POST /api/ltrd/ip ---
    // Параметры: ip=192.168.1.100, autoconnect=true, reconnect=true, permanent=true
    svr.Post("/api/ltrd/ip", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("ip")) {
            res.status = 400;
            res.set_content("{\"error\":\"ip required\"}", "application/json");
            return;
        }

        DWORD ip = 0;
        if (!parseIpToDword(req.get_param_value("ip"), &ip)) {
            res.status = 400;
            res.set_content("{\"error\":\"invalid ip format\"}", "application/json");
            return;
        }

        DWORD flags = 0;
        if (req.has_param("autoconnect") &&
            (req.get_param_value("autoconnect") == "true" ||
             req.get_param_value("autoconnect") == "1")) {
            flags |= LTR_CRATE_IP_FLAG_AUTOCONNECT;
        }
        if (req.has_param("reconnect") &&
            (req.get_param_value("reconnect") == "true" ||
             req.get_param_value("reconnect") == "1")) {
            flags |= LTR_CRATE_IP_FLAG_RECONNECT;
        }

        BOOL permanent = TRUE;
        if (req.has_param("permanent")) {
            std::string v = req.get_param_value("permanent");
            permanent = (v == "true" || v == "1") ? TRUE : FALSE;
        }

        TLTR ltr;
        INT openRes = openControlConnection(&ltr);
        if (openRes != LTR_OK) {
            std::ostringstream err;
            err << "{\"error\":\"" << json_escape(LTR_GetErrorString(openRes)) << "\"}";
            res.status = 503;
            res.set_content(err.str(), "application/json");
            return;
        }

        INT addRes = LTR_AddIPCrate(&ltr, ip, flags, permanent);
        LTR_Close(&ltr);

        std::ostringstream json;
        if (addRes == LTR_OK) {
            json << "{\"ok\":true,\"ip\":\"" << dwordToIp(ip) << "\"}";
        } else {
            res.status = 500;
            json << "{\"error\":\"" << json_escape(LTR_GetErrorString(addRes)) << "\"}";
        }
        res.set_content(json.str(), "application/json");
    });

    // --- DELETE /api/ltrd/ip?ip=192.168.1.100 ---
    svr.Delete("/api/ltrd/ip", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("ip")) {
            res.status = 400;
            res.set_content("{\"error\":\"ip required\"}", "application/json");
            return;
        }

        DWORD ip = 0;
        if (!parseIpToDword(req.get_param_value("ip"), &ip)) {
            res.status = 400;
            res.set_content("{\"error\":\"invalid ip format\"}", "application/json");
            return;
        }

        TLTR ltr;
        INT openRes = openControlConnection(&ltr);
        if (openRes != LTR_OK) {
            std::ostringstream err;
            err << "{\"error\":\"" << json_escape(LTR_GetErrorString(openRes)) << "\"}";
            res.status = 503;
            res.set_content(err.str(), "application/json");
            return;
        }

        BOOL permanent = TRUE;
        if (req.has_param("permanent")) {
            std::string v = req.get_param_value("permanent");
            permanent = (v == "true" || v == "1") ? TRUE : FALSE;
        }
        INT delRes = LTR_DeleteIPCrate(&ltr, ip, permanent);
        LTR_Close(&ltr);

        std::ostringstream json;
        if (delRes == LTR_OK) {
            json << "{\"ok\":true,\"ip\":\"" << dwordToIp(ip) << "\"}";
        } else {
            res.status = 500;
            json << "{\"error\":\"" << json_escape(LTR_GetErrorString(delRes)) << "\"}";
        }
        res.set_content(json.str(), "application/json");
    });

        // --- GET /api/ltrd/crates ---
    svr.Get("/api/ltrd/crates", [](const httplib::Request&, httplib::Response& res) {
        TLTR ltr;
        INT openRes = openControlConnection(&ltr);
        if (openRes != LTR_OK) {
            std::ostringstream err;
            err << "{\"error\":\"" << json_escape(LTR_GetErrorString(openRes)) << "\"}";
            res.status = 503;
            res.set_content(err.str(), "application/json");
            return;
        }

        // Буфер: CRATE_MAX строк по LTR_CRATE_SERIAL_SIZE байт
        std::vector<BYTE> csnBuf(CRATE_MAX * LTR_CRATE_SERIAL_SIZE, 0);

        INT getRes = LTR_GetCrates(&ltr, csnBuf.data());
        LTR_Close(&ltr);

        std::ostringstream arr;
        arr << "[";
        int count = 0;
        if (getRes == LTR_OK) {
            for (int i = 0; i < CRATE_MAX; i++) {
                const char* csn = (const char*)(csnBuf.data() + i * LTR_CRATE_SERIAL_SIZE);
                if (csn[0] == '\0') break;
                if (count) arr << ",";
                arr << "{\"csn\":\"" << json_escape(std::string(csn)) << "\"}";
                count++;
            }
        }
        arr << "]";

        std::ostringstream json;
        if (getRes != LTR_OK) {
            json << "{\"error\":\"" << json_escape(LTR_GetErrorString(getRes))
                 << "\",\"count\":0,\"crates\":[]}";
            res.status = 500;
        } else {
            json << "{\"count\":" << count
                 << ",\"crates\":" << arr.str() << "}";
        }
        res.set_content(json.str(), "application/json");
    });

    printf("HTTP-сервер запущен на http://127.0.0.1:9005\n");
    svr.listen("127.0.0.1", 9005);
}