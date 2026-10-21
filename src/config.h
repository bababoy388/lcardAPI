#pragma once
#include <string>

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
#else
  #include "linux_compat.h"
#endif

#include "ltrapi.h"
#include "ltr24api.h"

// ============================================================
// Конфигурация одного канала
// ============================================================
struct ChannelConfig {
    BOOL  enable = FALSE;
    BYTE  range  = LTR24_RANGE_2;   // LTR24_RANGE_2 или LTR24_RANGE_10
    BOOL  ac     = FALSE;
    BOOL  icp    = FALSE;
};

// ============================================================
// Общая конфигурация
// ============================================================
struct AppConfig {
    // [Device]
    std::string csn      = "";
    int         slot     = 1;

    // [ADC]
    int freq_hz          = 14000;
    int format           = 24;

    // [Channel0..3]
    ChannelConfig channels[4];

    // [Cycle]
    double record_time   = 5.0;
    double sleep_time    = 5.0;

    // [Output]
    std::string output_file   = "data.csv";
    int         block_size    = 512;
    bool        write_to_file = true;

    // Вспомогательные преобразования
    int getFreqCode()   const;
    int getFormatCode() const;
    int getSlotCode()   const;
};

// ============================================================
// Загрузка / сохранение / вывод
// ============================================================
bool LoadConfig(const std::string& path, AppConfig& cfg);
bool SaveConfig(const std::string& path, const AppConfig& cfg);
void PrintConfig(const AppConfig& cfg);