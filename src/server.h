#pragma once

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include "linux_compat.h"
#endif

#include <mutex>
#include <vector>
#include <atomic>
#include <string>
#include <chrono>
#include <cstdio>

#include "config.h"

// ============================================================
// Точка данных
// ============================================================
struct Sample {
    double time;
    double values[4];
};

// Фаза цикла
enum CyclePhase {
    PHASE_IDLE      = 0,
    PHASE_RECORDING = 1,
    PHASE_SLEEPING  = 2
};

// ============================================================
// Общее состояние
// ============================================================
extern std::mutex             g_bufMutex;
extern std::vector<Sample>    g_ringBuffer;
extern const size_t           RING_SIZE;

extern std::atomic<bool>      g_configDirty;
extern std::atomic<bool>      g_restartRequested;

extern std::atomic<bool>      g_running;
extern std::atomic<bool>      g_stopRequested;
extern std::atomic<long long> g_totalSamples;

extern std::atomic<int>       g_phase;
extern std::atomic<long long> g_phaseStartNs;

// Абсолютное время (Unix epoch, наносекунды) начала текущей записи
extern std::atomic<long long> g_recordStartEpochNs;

extern std::string            g_lastError;
extern AppConfig              g_cfg;
extern int                    g_activeChannels;

// Имя текущего открытого файла (пустое, если файл закрыт)
extern std::string            g_currentFileName;

// ============================================================
// Работа с буфером
// ============================================================
void        addSamples(const double* data, int points, int channels, double fs);
std::string getDataCSV(int n);
void        clearBuffer();
size_t      bufferSize();
long long   totalSamples();
void        setLastError(const std::string& err);
std::string getLastError();

// ============================================================
// Файл данных
// ============================================================
std::string generateDataFileName();
bool        openDataFile();
void        closeDataFile();
bool        isDataFileOpen();
void        writeBlockToFile(const double* data, int points, int channels,
                             double fs, long long startSampleIdx);

// ============================================================
// HTTP-сервер
// ============================================================
void runHttpServer();