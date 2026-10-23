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
    BYTE  range  = LTR24_RANGE_2;
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
    int freq_hz          = 29297;
    int format           = 24;
    int block_size       = 512;

    // [Channel0..3]
    ChannelConfig channels[4];

    // Вспомогательные преобразования
    int getFreqCode()   const;
    int getFormatCode() const;
    int getSlotCode()   const;
};

// Загрузка / сохранение / вывод
bool LoadConfig(const std::string& path, AppConfig& cfg);
bool SaveConfig(const std::string& path, const AppConfig& cfg);
void PrintConfig(const AppConfig& cfg);