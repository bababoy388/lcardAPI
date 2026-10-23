#include "config.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// ============================================================
// Утилиты
// ============================================================
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

static bool parseBool(const std::string& v) {
    std::string s = toLower(trim(v));
    return (s == "true" || s == "1" || s == "yes" || s == "on");
}

// ============================================================
// Преобразования
// ============================================================
int AppConfig::getFreqCode() const {
    switch (freq_hz) {
        case 610:    return LTR24_FREQ_610;     // Точная частота: 610.3515625 Гц
        case 915:    return LTR24_FREQ_915;     // Точная частота: 915.52734375 Гц
        case 1220:   return LTR24_FREQ_1K2;     // Точная частота: 1220.703125 Гц
        case 1831:   return LTR24_FREQ_1K8;     // Точная частота: 1831.0546875 Гц
        case 2441:   return LTR24_FREQ_2K4;     // Точная частота: 2441.40625 Гц
        case 3662:   return LTR24_FREQ_3K6;     // Точная частота: 3662.109375 Гц
        case 4883:   return LTR24_FREQ_4K8;     // Точная частота: 4882.8125 Гц
        case 7324:   return LTR24_FREQ_7K3;     // Точная частота: 7324.21875 Гц
        case 9766:   return LTR24_FREQ_9K7;     // Точная частота: 9765.625 Гц
        case 14648:  return LTR24_FREQ_14K;     // Точная частота: 14648.4375 Гц
        case 19531:  return LTR24_FREQ_19K;     // Точная частота: 19531.25 Гц
        case 29297:  return LTR24_FREQ_29K;     // Точная частота: 29296.875 Гц 
        case 39062:  return LTR24_FREQ_39K;     // Точная частота: 39062.5 Гц
        case 58594:  return LTR24_FREQ_58K;     // Точная частота: 58593.75 Гц
        case 78125:  return LTR24_FREQ_78K;     // Точная частота: 78125.0 Гц 
        case 117188: return LTR24_FREQ_117K;    // Точная частота: 117187.5 Гц
        default:     return LTR24_FREQ_29K;     
    }
}

int AppConfig::getFormatCode() const {
    return (format == 20) ? LTR24_FORMAT_20 : LTR24_FORMAT_24;
}

int AppConfig::getSlotCode() const {
    switch (slot) {
        case 1: return LTR_CC_CHNUM_MODULE1;
        case 2: return LTR_CC_CHNUM_MODULE2;
        case 3: return LTR_CC_CHNUM_MODULE3;
        case 4: return LTR_CC_CHNUM_MODULE4;
        default: return LTR_CC_CHNUM_MODULE1;
    }
}

// ============================================================
// Загрузка
// ============================================================
bool LoadConfig(const std::string& path, AppConfig& cfg) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Не удалось открыть файл конфига: " << path << "\n";
        return false;
    }

    std::string section;
    std::string line;

    while (std::getline(f, line)) {
        size_t comment = line.find(';');
        if (comment != std::string::npos) line = line.substr(0, comment);
        line = trim(line);
        if (line.empty()) continue;

        if (line.front() == '[' && line.back() == ']') {
            section = toLower(line.substr(1, line.size() - 2));
            continue;
        }

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key   = toLower(trim(line.substr(0, eq)));
        std::string value = trim(line.substr(eq + 1));

        if (section == "device") {
            if (key == "csn")  cfg.csn = value;
            else if (key == "slot") cfg.slot = std::stoi(value);
        }
        else if (section == "adc") {
            if (key == "freq_hz") cfg.freq_hz = std::stoi(value);
            else if (key == "format") cfg.format = std::stoi(value);
            else if (key == "block_size") cfg.block_size = std::stoi(value);
        }
        else if (section.rfind("channel", 0) == 0) {
            int ch = std::stoi(section.substr(7));
            if (ch < 0 || ch > 3) continue;
            if (key == "enable")      cfg.channels[ch].enable = parseBool(value) ? TRUE : FALSE;
            else if (key == "range") {
                int r = std::stoi(value);
                cfg.channels[ch].range = (r == 10) ? LTR24_RANGE_10 : LTR24_RANGE_2;
            }
            else if (key == "ac")     cfg.channels[ch].ac  = parseBool(value) ? TRUE : FALSE;
            else if (key == "icp")    cfg.channels[ch].icp = parseBool(value) ? TRUE : FALSE;
        }
    }
    return true;
}

// ============================================================
// Сохранение
// ============================================================
bool SaveConfig(const std::string& path, const AppConfig& cfg) {
    std::ofstream f(path);
    if (!f.is_open()) return false;

    f << "; Автоматически сохранено\n\n";

    f << "[Device]\n";
    f << "csn          = " << cfg.csn << "\n";
    f << "slot         = " << cfg.slot << "\n\n";

    f << "[ADC]\n";
    f << "freq_hz      = " << cfg.freq_hz << "\n";
    f << "format       = " << cfg.format << "\n";
    f << "block_size   = " << cfg.block_size << "\n\n";

    for (int i = 0; i < 4; i++) {
        f << "[Channel" << i << "]\n";
        f << "enable       = " << (cfg.channels[i].enable ? "true" : "false") << "\n";
        f << "range        = " << (cfg.channels[i].range == LTR24_RANGE_10 ? 10 : 2) << "\n";
        f << "ac           = " << (cfg.channels[i].ac ? "true" : "false") << "\n";
        f << "icp          = " << (cfg.channels[i].icp ? "true" : "false") << "\n\n";
    }
    return true;
}

// ============================================================
// Вывод
// ============================================================
void PrintConfig(const AppConfig& cfg) {
    std::cout << "===== Конфигурация =====\n";
    std::cout << "CSN:        '" << cfg.csn << "'\n";
    std::cout << "Slot:       " << cfg.slot << "\n";
    std::cout << "Freq:       " << cfg.freq_hz << " Гц\n";
    std::cout << "Format:     " << cfg.format << " бит\n";
    std::cout << "BlockSize:  " << cfg.block_size << " слов\n";
    for (int i = 0; i < 4; i++) {
        const auto& c = cfg.channels[i];
        std::cout << "CH" << i << ": " << (c.enable ? "ВКЛ" : "выкл")
                  << " ±" << (c.range == LTR24_RANGE_10 ? 10 : 2) << "В";
        if (c.ac) std::cout << " (AC)";
        std::cout << "\n";
    }
    std::cout << "========================\n";
}