#pragma once
#include <string>
#include <atomic>
#include <cstdio>
#include "config.h"

enum CyclePhase {
    PHASE_IDLE      = 0,
    PHASE_RECORDING = 1,
    PHASE_SLEEPING  = 2
};

extern std::atomic<bool>      g_running;
extern std::atomic<bool>      g_stopRequested;
extern std::atomic<int>       g_phase;
extern std::atomic<long long> g_phaseStartNs;
extern std::atomic<long long> g_recordStartEpochNs; // Время первого сэмпла (нс)

extern std::atomic<bool>      g_configDirty;
extern std::atomic<bool>      g_restartRequested;

extern std::string            g_lastError;
extern AppConfig              g_cfg;
extern int                    g_activeChannels;

std::string json_escape(const std::string& s);
std::string cp1251_to_utf8(const std::string& src);
std::string formatTimestampNs(long long ns);
void        setLastError(const std::string& err);
std::string getLastError();