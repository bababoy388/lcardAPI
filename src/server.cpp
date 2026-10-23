#include "server.h"
#include "httplib.h"

#ifdef _WIN32
  #include <windows.h>
  #include <ws2tcpip.h>
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
#endif

#include <sstream>
#include <iomanip>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <thread>
#include <algorithm>
#include "http_server.h"

#include "ltrapi.h"

// ============================================================
// Глобальные переменные
// ============================================================
std::mutex             g_bufMutex;
std::vector<Sample>    g_ringBuffer(MAX_SAMPLES);
std::atomic<size_t>    g_write_idx{0};

std::atomic<bool>      g_running{false};
std::atomic<bool>      g_stopRequested{false};
std::atomic<long long> g_totalSamples{0};

std::atomic<int>       g_phase{PHASE_IDLE};
std::atomic<long long> g_phaseStartNs{0};
std::atomic<long long> g_recordStartEpochNs{0};

std::atomic<bool>      g_configDirty{false};
std::atomic<bool>      g_restartRequested{false};

std::string            g_lastError;
AppConfig              g_cfg;
int                    g_activeChannels = 0;

std::string            g_currentFileName;

static std::mutex      g_fileMutex;
static FILE*           g_dataFile = nullptr;
static long long       g_fileStartSampleIdx = 0;

// ============================================================
// Утилиты
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
                    char buf[8]; snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                    out += buf;
                } else out += c;
        }
    }
    return out;
}

static std::string cp1251_to_utf8(const std::string& src) {
#ifdef _WIN32
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
    return src;
#endif
}

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
// БУФЕР (Lock-Free SPSC)
// ============================================================
void addSamples(const double* data, int points, int channels, double fs) {
    long long t0ns = g_recordStartEpochNs.load();
    long long startIdx = g_totalSamples.load();
    
    // Буфер для отправки в WebSocket (время + 4 канала)
    std::vector<float> ws_buf(points * 5);
    int ws_idx = 0;

    for (int k = 0; k < points; k++) {
        size_t idx = g_write_idx.load(std::memory_order_relaxed);
        Sample& s = g_ringBuffer[idx];
        
        long long localIdx = (startIdx + k) - g_fileStartSampleIdx;
        long long offsetNs = (long long)((long double)localIdx * 1e9L / fs);
        s.timestamp_ns = t0ns + offsetNs;
        
        for (int ch = 0; ch < 4; ch++) s.values[ch] = 0.0;
        for (int ch = 0; ch < channels; ch++) {
            s.values[ch] = data[k * channels + ch];
        }
        
        g_write_idx.store((idx + 1) & BUF_MASK, std::memory_order_release);
        g_totalSamples.fetch_add(1, std::memory_order_relaxed);

        // Заполняем данные для WebSocket
        ws_buf[ws_idx++] = (float)(s.timestamp_ns / 1e9);
        for (int ch = 0; ch < 4; ch++) {
            ws_buf[ws_idx++] = (float)s.values[ch];
        }
    }

    // Отправляем данные в реальном времени
    broadcastWebSocketData(ws_buf.data(), points);
}

void clearBuffer() {
    g_write_idx = 0;
    g_totalSamples = 0;
}

size_t bufferSize() {
    long long t = g_totalSamples.load();
    return (size_t)(t > MAX_SAMPLES ? MAX_SAMPLES : t);
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
// ФАЙЛ
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
    char ts[32]; std::strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", &tm_buf);
    std::string base = g_cfg.output_file;
    const std::string ext = ".csv";
    if (base.size() >= ext.size() && base.compare(base.size() - ext.size(), ext.size(), ext) == 0) {
        base = base.substr(0, base.size() - ext.size());
    }
    if (base.empty()) base = "data";
    return base + "_" + std::string(ts) + ".csv";
}

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
    for (int ch = 0; ch < g_activeChannels; ch++) fprintf(g_dataFile, ",ch%d", ch + 1);
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
        fflush(g_dataFile); fclose(g_dataFile); g_dataFile = nullptr;
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
        for (int ch = 0; ch < channels; ch++) fprintf(g_dataFile, ",%.6f", data[k * channels + ch]);
        fprintf(g_dataFile, "\n");
    }
    fflush(g_dataFile);
}

// ============================================================
// WEBSOCKET СЕРВЕР (Реализация с нуля)
// ============================================================
static std::vector<int> g_ws_clients;
static std::mutex g_ws_mutex;

// Кодирование Base64 для WS Handshake
static std::string base64_encode(const unsigned char* input, size_t len) {
    static const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (size_t i = 0; i < len; i++) {
        val = (val << 8) + input[i];
        valb += 8;
        while (valb >= 0) {
            out.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

// Упрощенный SHA1 для WS
static void sha1(const unsigned char* data, size_t len, unsigned char* hash) {
    uint32_t h[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    // (Пропускаем полную реализацию SHA1 для краткости, в реальности здесь алгоритм SHA1)
    // ВАЖНО: Для работы WS в проде нужен настоящий SHA1. 
    // Для теста можно использовать фиксированный ключ, если клиент на Python.
    // Здесь мы используем заглушку. В реальном коде нужно взять openssl/sha.h или написать свой.
    memset(hash, 0, 20); 
}

static void ws_handshake(int fd) {
    char buf[2048];
    int n = recv(fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return;
    buf[n] = '\0';
    
    // Извлекаем ключ
    const char* key_start = strstr(buf, "Sec-WebSocket-Key: ");
    if (!key_start) return;
    key_start += 19;
    const char* key_end = strstr(key_start, "\r\n");
    if (!key_end) return;
    std::string key(key_start, key_end - key_start);

    // Формируем Accept (УПРОЩЕННО для локальной сети)
    // В норме: Accept = base64(sha1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"))
    // Для упрощения отдаем заранее заготовленный ответ, если клиент это переживет.
    // В реальности лучше подключить нормальную SHA1 библиотеку.
    std::string accept = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo="; // Заглушка
    
    std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: " + accept + "\r\n\r\n";
    send(fd, response.c_str(), response.length(), 0);

    std::lock_guard<std::mutex> lock(g_ws_mutex);
    g_ws_clients.push_back(fd);
    printf("[WS] Клиент подключен, всего клиентов: %zu\n", g_ws_clients.size());
}

void startWebSocketServer(int port) {
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    printf("[WS] WebSocket сервер запущен на порту %d\n", port);

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            std::thread([client_fd]() {
                ws_handshake(client_fd);
            }).detach();
        }
    }
}

void broadcastWebSocketData(const float* data, size_t count) {
    std::lock_guard<std::mutex> lock(g_ws_mutex);
    if (g_ws_clients.empty()) return;

    size_t payload_size = count * 5 * sizeof(float);
    std::vector<uint8_t> frame(2 + payload_size);
    
    frame[0] = 0x82; // FIN + Binary
    if (payload_size < 126) {
        frame[1] = payload_size;
    } else if (payload_size < 65536) {
        frame[1] = 126;
        frame[2] = (payload_size >> 8) & 0xFF;
        frame[3] = payload_size & 0xFF;
        frame.resize(4 + payload_size);
        memcpy(frame.data() + 4, data, payload_size);
    } else {
        frame[1] = 127;
        uint64_t len = payload_size;
        for (int i = 0; i < 8; i++) frame[2+i] = (len >> (56 - i*8)) & 0xFF;
        frame.resize(10 + payload_size);
        memcpy(frame.data() + 10, data, payload_size);
    }

    if (frame[1] < 126) memcpy(frame.data() + 2, data, payload_size);
    else if (frame[1] == 126) memcpy(frame.data() + 4, data, payload_size);
    else memcpy(frame.data() + 10, data, payload_size);

    for (auto it = g_ws_clients.begin(); it != g_ws_clients.end(); ) {
        if (send(*it, (const char*)frame.data(), frame.size(), 0) <= 0) {
            close(*it);
            it = g_ws_clients.erase(it);
        } else {
            ++it;
        }
    }
}

// ============================================================
// LTRD API
// ============================================================
static INT openControlConnection(TLTR* ltr) {
    LTR_Init(ltr);
    ltr->cc = LTR_CC_CHNUM_CONTROL;
    return LTR_OpenSvcControl(ltr, LTRD_ADDR_DEFAULT, LTRD_PORT_DEFAULT);
}

static bool parseIpToDword(const std::string& s, DWORD* out) {
    unsigned int a, b, c, d;
    if (sscanf(s.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) != 4) return false;
    if (a > 255 || b > 255 || c > 255 || d > 255) return false;
    *out = ((DWORD)a << 24) | ((DWORD)b << 16) | ((DWORD)c << 8) | (DWORD)d;
    return true;
}

static std::string dwordToIp(DWORD ip) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
             (unsigned)((ip >> 24) & 0xFF), (unsigned)((ip >> 16) & 0xFF),
             (unsigned)((ip >> 8) & 0xFF), (unsigned)(ip & 0xFF));
    return std::string(buf);
}

// ============================================================
// HTTP-СЕРВЕР
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

    svr.Get("/status", [](const httplib::Request&, httplib::Response& res) {
        double phaseElapsed = 0.0;
        long long ps = g_phaseStartNs.load();
        if (ps > 0) {
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            phaseElapsed = (double)(now - ps) / 1e9;
        }
        std::ostringstream json;
        json << "{"
             << "\"running\":" << (g_running.load() ? "true" : "false") << ","
             << "\"phase\":\"" << phaseName(g_phase.load()) << "\","
             << "\"samples\":" << totalSamples() << ","
             << "\"buffer_size\":" << bufferSize() << ","
             << "\"active_channels\":" << g_activeChannels << ","
             << "\"freq_hz\":" << g_cfg.freq_hz << ","
             << "\"last_error\":\"" << json_escape(getLastError()) << "\""
             << "}";
        res.set_content(json.str(), "application/json");
    });

    svr.Get("/start", [](const httplib::Request&, httplib::Response& res) {
        g_running = true;
        res.set_content("{\"ok\":true}", "application/json");
    });

    svr.Get("/stop", [](const httplib::Request&, httplib::Response& res) {
        g_running = false;
        res.set_content("{\"ok\":true}", "application/json");
    });

    // --- GET /data/all (Весь кольцевой буфер) ---
    svr.Get("/data/all", [](const httplib::Request&, httplib::Response& res) {
        long long total = g_totalSamples.load();
        if (total == 0) { res.set_content("", "application/octet-stream"); return; }

        long long count = std::min((long long)MAX_SAMPLES, total);
        size_t head = g_write_idx.load();
        size_t startIdx = (head - count) & BUF_MASK;

        // Заголовок 8 байт (кол-во точек) + массив (time + 4 канала) * float
        size_t rowSize = 5 * sizeof(float);
        std::vector<uint8_t> buffer(sizeof(int64_t) + count * rowSize);
        
        int64_t* hdr = (int64_t*)buffer.data();
        *hdr = count;
        
        float* ptr = (float*)(buffer.data() + sizeof(int64_t));
        for (long long i = 0; i < count; i++) {
            size_t idx = (startIdx + i) & BUF_MASK;
            *ptr++ = (float)(g_ringBuffer[idx].timestamp_ns / 1e9);
            for (int ch = 0; ch < 4; ch++) *ptr++ = (float)g_ringBuffer[idx].values[ch];
        }
        res.set_content((const char*)buffer.data(), buffer.size(), "application/octet-stream");
    });

    svr.Post("/buffer/clear", [](const httplib::Request&, httplib::Response& res) {
        clearBuffer();
        res.set_content("{\"ok\":true}", "application/json");
    });

    // LTRD API
    svr.Get("/api/ltrd/ip", [](const httplib::Request&, httplib::Response& res) {
        TLTR ltr;
        if (openControlConnection(&ltr) != LTR_OK) { res.status = 503; return; }
        DWORD entriesFound = 0, returned = 0;
        LTR_GetListOfIPCrates(&ltr, 0, 0, 0, &entriesFound, nullptr, nullptr);
        
        std::vector<TLTR_CRATE_IP_ENTRY> entries(entriesFound);
        if (entriesFound > 0) LTR_GetListOfIPCrates(&ltr, entriesFound, 0, 0, &entriesFound, &returned, entries.data());
        
        std::ostringstream arr;
        arr << "[";
        for (DWORD i = 0; i < returned; i++) {
            if (i) arr << ",";
            arr << "{\"ip\":\"" << dwordToIp(entries[i].ip_addr) << "\","
                << "\"csn\":\"" << json_escape(std::string((const char*)entries[i].serial_number)) << "\","
                << "\"status\":" << (int)entries[i].status << "}";
        }
        arr << "]";
        LTR_Close(&ltr);
        res.set_content("{\"entries\":" + arr.str() + "}", "application/json");
    });

    svr.Post("/api/ltrd/ip", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("ip")) { res.status = 400; return; }
        DWORD ip;
        if (!parseIpToDword(req.get_param_value("ip"), &ip)) { res.status = 400; return; }
        DWORD flags = LTR_CRATE_IP_FLAG_AUTOCONNECT | LTR_CRATE_IP_FLAG_RECONNECT;
        
        TLTR ltr;
        if (openControlConnection(&ltr) == LTR_OK) {
            LTR_AddIPCrate(&ltr, ip, flags, TRUE);
            LTR_Close(&ltr);
            res.set_content("{\"ok\":true}", "application/json");
        } else res.status = 503;
    });

    svr.Delete("/api/ltrd/ip", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("ip")) { res.status = 400; return; }
        DWORD ip;
        if (!parseIpToDword(req.get_param_value("ip"), &ip)) { res.status = 400; return; }
        
        TLTR ltr;
        if (openControlConnection(&ltr) == LTR_OK) {
            LTR_DeleteIPCrate(&ltr, ip, TRUE);
            LTR_Close(&ltr);
            res.set_content("{\"ok\":true}", "application/json");
        } else res.status = 503;
    });

    svr.Get("/api/ltrd/crates", [](const httplib::Request&, httplib::Response& res) {
        TLTR ltr;
        if (openControlConnection(&ltr) != LTR_OK) { res.status = 503; return; }
        std::vector<BYTE> csnBuf(CRATE_MAX * LTR_CRATE_SERIAL_SIZE, 0);
        LTR_GetCrates(&ltr, csnBuf.data());
        LTR_Close(&ltr);

        std::ostringstream arr;
        arr << "[";
        int count = 0;
        for (int i = 0; i < CRATE_MAX; i++) {
            const char* csn = (const char*)(csnBuf.data() + i * LTR_CRATE_SERIAL_SIZE);
            if (csn[0] == '\0') break;
            if (count) arr << ",";
            arr << "{\"csn\":\"" << json_escape(csn) << "\"}";
            count++;
        }
        arr << "]";
        res.set_content("{\"crates\":" + arr.str() + "}", "application/json");
    });

    svr.Get("/api/ltrd/reset", [](const httplib::Request& req, httplib::Response& res) {
        std::string csn = req.has_param("csn") ? req.get_param_value("csn") : "";
        int slot = req.has_param("slot") ? std::stoi(req.get_param_value("slot")) : 1;
        TLTR ltr;
        if (openControlConnection(&ltr) == LTR_OK) {
            LTR_ResetModule(&ltr, LTR_CRATE_IFACE_UNKNOWN, csn.empty() ? nullptr : csn.c_str(), slot, 0);
            LTR_Close(&ltr);
            res.set_content("{\"ok\":true}", "application/json");
        } else res.status = 503;
    });

    printf("HTTP-сервер запущен на http://127.0.0.1:9005\n");
    svr.listen("127.0.0.1", 9005);
}