#include "server.h"
#ifdef _WIN32
  #include <conio.h>
#endif
#include <thread>
#include <chrono>
#include <cstdio>
#include "ltrapi.h"
#include "ltrapidefine.h"
#include "ltr24api.h"

static void acquisitionThread() {
    printf("[ACQ] Поток сбора запущен\n"); fflush(stdout);

    TLTR24 ltr24;
    LTR24_Init(&ltr24);

    bool adcStarted = false;
    int blockSize = g_cfg.block_size;
    std::vector<DWORD>  recv_buf(blockSize);
    std::vector<DWORD>  tmark(blockSize);
    std::vector<double> data(blockSize);

    auto nowSteadyNs = []() {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    };
    auto nowEpochNs = []() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    };
    auto setPhase = [&](int p) {
        g_phase = p;
        g_phaseStartNs = nowSteadyNs();
    };

    auto openAndConfigure = [&]() -> bool {
        LTR24_Init(&ltr24);
        INT res = LTR24_Open(&ltr24, LTRD_ADDR_DEFAULT, LTRD_PORT_DEFAULT,
                             g_cfg.csn.c_str(), g_cfg.getSlotCode());
        if (res != LTR_OK) {
            printf("[ACQ] Ошибка LTR24_Open: %s\n", LTR24_GetErrorString(res)); fflush(stdout);
            setLastError(LTR24_GetErrorString(res));
            return false;
        }

        for (int i = 0; i < 4; i++) {
            ltr24.ChannelMode[i].Enable  = g_cfg.channels[i].enable;
            ltr24.ChannelMode[i].Range   = g_cfg.channels[i].range;
            ltr24.ChannelMode[i].AC      = g_cfg.channels[i].ac;
            ltr24.ChannelMode[i].ICPMode = g_cfg.channels[i].icp;
        }
        ltr24.ADCFreqCode = g_cfg.getFreqCode();
        ltr24.DataFmt     = g_cfg.getFormatCode();

        res = LTR24_SetADC(&ltr24);
        if (res != LTR_OK) {
            printf("[ACQ] Ошибка LTR24_SetADC: %s\n", LTR24_GetErrorString(res)); fflush(stdout);
            setLastError(LTR24_GetErrorString(res));
            LTR24_Close(&ltr24);
            return false;
        }
        return true;
    };

    auto recalcActiveChannels = [&]() {
        g_activeChannels = 0;
        for (int i = 0; i < 4; i++) if (g_cfg.channels[i].enable) g_activeChannels++;
    };

    recalcActiveChannels();
    if (g_activeChannels == 0 || !openAndConfigure()) return;

    setPhase(PHASE_IDLE);

    while (!g_stopRequested.load()) {
        if (!g_running.load()) {
            if (adcStarted) { LTR24_Stop(&ltr24); adcStarted = false; }
            if (g_configDirty.load() && !g_restartRequested.load()) {
                LTR24_Close(&ltr24);
                LoadConfig("config.ini", g_cfg);
                recalcActiveChannels();
                if (!openAndConfigure()) { g_configDirty = false; Sleep(500); continue; }
                g_configDirty = false;
            }
            setPhase(PHASE_IDLE);
            Sleep(50);
            continue;
        }

        if (!adcStarted) {
            INT res = LTR24_Start(&ltr24);
            if (res != LTR_OK) { setLastError(LTR24_GetErrorString(res)); g_running = false; continue; }
            adcStarted = true;
            // Фиксируем время старта для всей сессии
            g_recordStartEpochNs = nowEpochNs(); 
            setPhase(PHASE_RECORDING);
            printf("[ACQ] Начало сбора данных\n"); fflush(stdout);
        }

        INT res = LTR24_Recv(&ltr24, recv_buf.data(), tmark.data(), blockSize, 200);
        if (res < 0) {
            setLastError(LTR24_GetErrorString(res));
            g_running = false;
            continue;
        }
        if (res == 0) continue;

        INT data_size = res;
        res = LTR24_ProcessData(&ltr24, recv_buf.data(), data.data(),
                                &data_size, LTR24_PROC_FLAG_VOLT, NULL);
        if (res != LTR_OK) continue;

        int points = data_size / g_activeChannels;
        // Пишем только в буфер (он сам отправит в WS)
        addSamples(data.data(), points, g_activeChannels, (double)g_cfg.freq_hz);
    }

    if (adcStarted) LTR24_Stop(&ltr24);
    LTR24_Close(&ltr24);
    printf("[ACQ] Поток сбора завершён\n"); fflush(stdout);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    if (!LoadConfig("config.ini", g_cfg)) return -1;
    g_activeChannels = 0;
    for (int i = 0; i < 4; i++) if (g_cfg.channels[i].enable) g_activeChannels++;
    if (g_activeChannels == 0) return -1;

    std::thread acqThread(acquisitionThread);
    std::thread httpThread(runHttpServer);
    std::thread wsThread(startWebSocketServer, 9006);
    
    httpThread.detach();
    wsThread.detach();

    printf("Нажмите 'q' для выхода.\n");
    while (true) {
        if (g_stopRequested.load()) break;
        #ifdef _WIN32
        if (_kbhit() && _getch() == 'q') break;
        #else
        if (isatty(STDIN_FILENO) && _kbhit() && _getch() == 'q') break;
        #endif
        Sleep(100);
    }

    g_running = false;
    g_stopRequested = true;
    acqThread.join();
    return 0;
}