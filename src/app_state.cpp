#include "app_state.h"
#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif
#include <ctime>

std::atomic<bool>      g_running{false};
std::atomic<bool>      g_stopRequested{false};
std::atomic<int>       g_phase{PHASE_IDLE};
std::atomic<long long> g_phaseStartNs{0};
std::atomic<long long> g_recordStartEpochNs{0};

std::atomic<bool>      g_configDirty{false};
std::atomic<bool>      g_restartRequested{false};

std::string            g_lastError;
AppConfig              g_cfg;
int                    g_activeChannels = 0;

std::string json_escape(const std::string& s) {
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

std::string cp1251_to_utf8(const std::string& src) {
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

std::string formatTimestampNs(long long ns) {
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

void setLastError(const std::string& err) { g_lastError = cp1251_to_utf8(err); }
std::string getLastError() { return g_lastError; }