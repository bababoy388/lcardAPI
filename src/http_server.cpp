#include "server.h"
#include "httplib.h"
#include "ltrapi.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>

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

void runHttpServer() {
    httplib::Server svr;

    svr.Get("/status", [](const httplib::Request&, httplib::Response& res) {
        std::ostringstream json;
        json << "{"
             << "\"running\":" << (g_running.load() ? "true" : "false") << ","
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

    // GET /data/all (Весь кольцевой буфер)
    svr.Get("/data/all", [](const httplib::Request&, httplib::Response& res) {
        long long total = totalSamples();
        if (total == 0) { res.set_content("", "application/octet-stream"); return; }

        long long count = std::min((long long)MAX_SAMPLES, total);
        size_t head = g_write_idx.load();
        size_t startIdx = (head - count) & BUF_MASK;

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
        
        TLTR ltr;
        if (openControlConnection(&ltr) == LTR_OK) {
            LTR_AddIPCrate(&ltr, ip, LTR_CRATE_IP_FLAG_AUTOCONNECT | LTR_CRATE_IP_FLAG_RECONNECT, TRUE);
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

    printf("HTTP-сервер запущен на http://127.0.0.1:9005\n");
    svr.listen("127.0.0.1", 9005);
}