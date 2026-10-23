#include "server.h"
#ifdef _WIN32
  #include <conio.h>
#endif
#include <thread>
#include <chrono>
#include <vector>
#include <cstdio>
#include "ltrapi.h"
#include "ltrapidefine.h"
#include "ltr24api.h"

// ============================================================
// Поток непрерывного сбора
// ============================================================
static void acquisitionThread() {
    printf("[ACQ] Поток сбора запущен\n"); fflush(stdout);

    TLTR24 ltr24;
    LTR24_Init(&ltr24);

    bool adcStarted = false;
    int blockSize = g_cfg.block_size;
    std::vector<DWORD>  recv_buf(blockSize);
    std::vector<DWORD>  tmark(blockSize);
    std::vector<double> data(blockSize);

    auto nowEpochNs = []() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    };

    // Функция запуска/настройки АЦП
    auto startADC = [&]() -> bool {
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

        res = LTR24_Start(&ltr24);
        if (res != LTR_OK) {
            printf("[ACQ] Ошибка LTR24_Start: %s\n", LTR24_GetErrorString(res)); fflush(stdout);
            setLastError(LTR24_GetErrorString(res));
            LTR24_Close(&ltr24);
            return false;
        }
        
        return true;
    };

    // Бесконечный цикл
    while (!g_stopRequested.load()) {
        
        // Если сбор остановлен пользователем
        if (!g_running.load()) {
            if (adcStarted) { 
                LTR24_Stop(&ltr24); 
                LTR24_Close(&ltr24); 
                adcStarted = false; 
            }
            
            // Проверка на изменение конфига
            if (g_configDirty.load() && !g_restartRequested.load()) {
                LTR24_Init(&ltr24);
                if (LoadConfig("config.ini", g_cfg)) {
                    g_activeChannels = 0;
                    for (int i = 0; i < 4; i++) if (g_cfg.channels[i].enable) g_activeChannels++;
                    
                    blockSize = g_cfg.block_size;
                    recv_buf.assign(blockSize, 0);
                    tmark.assign(blockSize, 0);
                    data.assign(blockSize, 0.0);
                }
                g_configDirty = false;
            }
            
            g_phase = PHASE_IDLE;
            Sleep(50);
            continue;
        }

        // Запуск АЦП по команде START
        if (!adcStarted) {
            printf("[ACQ] Запуск АЦП (непрерывный режим)...\n"); fflush(stdout);
            if (!startADC()) {
                g_running = false;
                continue;
            }
            g_recordStartEpochNs = nowEpochNs(); // Фиксируем точку отсчета времени
            adcStarted = true;
            g_phase = PHASE_RECORDING;
        }

        // Чтение данных (без таймеров и пауз)
        INT res = LTR24_Recv(&ltr24, recv_buf.data(), tmark.data(), blockSize, 200);
        
        if (res < 0) {
            printf("[ACQ] Ошибка LTR24_Recv: %s\n", LTR24_GetErrorString(res)); fflush(stdout);
            setLastError(LTR24_GetErrorString(res));
            g_running = false;
            continue;
        }
        if (res == 0) continue; // Таймаут приема, пробуем снова

        INT data_size = res;
        res = LTR24_ProcessData(&ltr24, recv_buf.data(), data.data(), &data_size, LTR24_PROC_FLAG_VOLT, NULL);
        if (res != LTR_OK) continue;

        // Передача в кольцевой буфер
        int points = data_size / g_activeChannels;
        addSamples(data.data(), points, g_activeChannels, (double)g_cfg.freq_hz);
    }

    // Завершение
    if (adcStarted) { LTR24_Stop(&ltr24); LTR24_Close(&ltr24); }
    printf("[ACQ] Поток сбора завершён\n"); fflush(stdout);
}

// ============================================================
// main
// ============================================================
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (!LoadConfig("config.ini", g_cfg)) {
        printf("Ошибка загрузки config.ini\n");
        return -1;
    }
    PrintConfig(g_cfg);

    g_activeChannels = 0;
    for (int i = 0; i < 4; i++) {
        if (g_cfg.channels[i].enable) g_activeChannels++;
    }
    if (g_activeChannels == 0) {
        printf("Не включено ни одного канала!\n");
        return -1;
    }

    printf("\nЗапуск потоков...\n");
    std::thread acqThread(acquisitionThread);
    std::thread httpThread(runHttpServer);
    std::thread wsThread(startWebSocketServer, 9006);
    
    httpThread.detach();
    wsThread.detach();

    printf("Нажмите 'q' для выхода.\n\n");
    while (true) {
        if (g_stopRequested.load()) break;
        
        #ifdef _WIN32
        if (_kbhit() && _getch() == 'q') break;
        #else
        if (isatty(STDIN_FILENO) && _kbhit() && _getch() == 'q') break;
        #endif
        
        Sleep(100);
    }

    printf("\nОстановка...\n");
    g_running = false;
    g_stopRequested = true;
    acqThread.join();

    if (g_restartRequested.load()) {
        // Логика перезапуска (если используется)
    }

    printf("Завершение.\n");
    return 0;
}