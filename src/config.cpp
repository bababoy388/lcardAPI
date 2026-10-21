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
        case 610:    return LTR24_FREQ_610;      // 610.3515625 Гц
        case 915:    return LTR24_FREQ_915;      // 915.52734375 Гц
        case 1220:   return LTR24_FREQ_1K2;      // 1.220703125 кГц
        case 1831:   return LTR24_FREQ_1K8;      // 1.8310546875 кГц
        case 2441:   return LTR24_FREQ_2K4;      // 2.44140625 кГц
        case 3662:   return LTR24_FREQ_3K6;      // 3.662109375 кГц
        case 4883:   return LTR24_FREQ_4K8;      // 4.8828125 кГц
        case 7324:   return LTR24_FREQ_7K3;      // 7.32421875 кГц
        case 9766:   return LTR24_FREQ_9K7;      // 9.765625 кГц
        case 14648:  return LTR24_FREQ_14K;      // 14.6484375 кГц
        case 19531:  return LTR24_FREQ_19K;      // 19.53125 кГц
        case 29297:  return LTR24_FREQ_29K;      // 29.296875 кГц
        case 39062:  return LTR24_FREQ_39K;      // 39.0625 кГц
        case 58594:  return LTR24_FREQ_58K;      // 58.59375 кГц
        case 78125:  return LTR24_FREQ_78K;      // 78.125 кГц
        case 117188: return LTR24_FREQ_117K;     // 117.1875 кГц
        default:
            printf("[CFG] getFreqCode: НЕИЗВЕСТНАЯ freq_hz=%d → default 14K\n", freq_hz);
            fflush(stdout);
            return LTR24_FREQ_14K;
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
        }
        else if (section == "cycle") {
            if (key == "record_time") cfg.record_time = std::stod(value);
            else if (key == "sleep_time") cfg.sleep_time = std::stod(value);
        }
        else if (section == "output") {
            if (key == "file")            cfg.output_file = value;
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
    f << "format       = " << cfg.format << "\n\n";

    for (int i = 0; i < 4; i++) {
        f << "[Channel" << i << "]\n";
        f << "enable       = " << (cfg.channels[i].enable ? "true" : "false") << "\n";
        f << "range        = " << (cfg.channels[i].range == LTR24_RANGE_10 ? 10 : 2) << "\n";
        f << "ac           = " << (cfg.channels[i].ac ? "true" : "false") << "\n";
        f << "icp          = " << (cfg.channels[i].icp ? "true" : "false") << "\n\n";
    }

    f << "[Cycle]\n";
    f << "record_time  = " << cfg.record_time << "\n";
    f << "sleep_time   = " << cfg.sleep_time << "\n\n";

    f << "[Output]\n";
    f << "file         = " << cfg.output_file << "\n";
    f << "block_size   = " << cfg.block_size << "\n";
    return true;
}

// ============================================================
// Вывод
// ============================================================
void PrintConfig(const AppConfig& cfg) {
    std::cout << "===== Конфигурация =====\n";
    std::cout << "CSN:        '" << cfg.csn << "'\n";
    std::cout << "Slot:       " << cfg.slot << "\n";
    std::cout << "Freqq:       " << cfg.freq_hz << " Гц\n";
    std::cout << "Format:     " << cfg.format << " бит\n";
    for (int i = 0; i < 4; i++) {
        const auto& c = cfg.channels[i];
        std::cout << "CH" << i << ": enable=" << (c.enable ? "да" : "нет")
                  << " range=±" << (c.range == LTR24_RANGE_10 ? 10 : 2) << " В"
                  << " ac=" << (c.ac ? "да" : "нет")
                  << " icp=" << (c.icp ? "да" : "нет") << "\n";
    }
    std::cout << "Record:     " << cfg.record_time << " с\n";
    std::cout << "Sleep:      " << cfg.sleep_time << " с\n";
    std::cout << "Output:     " << cfg.output_file << "\n";
    std::cout << "BlockSize:  " << cfg.block_size << "\n";
    std::cout << "========================\n";
}