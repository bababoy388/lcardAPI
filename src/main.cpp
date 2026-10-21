#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <conio.h>
#else
  #include "linux_compat.h"
#endif

#include <cstdio>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <string>

#include "config.h"
#include "server.h"
#include "ltrapi.h"
#include "ltrapidefine.h"
#include "ltr24api.h"

// ============================================================
// Поток сбора (циклический режим с переоткрытием модуля
// при изменении конфигурации)
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

    // ------------------------------------------------------------
    // Открытие модуля + настройка АЦП
    // ------------------------------------------------------------
    auto openAndConfigure = [&]() -> bool {
        LTR24_Init(&ltr24);

        INT res = LTR24_Open(&ltr24,
                             LTRD_ADDR_DEFAULT,
                             LTRD_PORT_DEFAULT,
                             g_cfg.csn.c_str(),
                             g_cfg.getSlotCode());
        if (res != LTR_OK) {
            printf("[ACQ] Ошибка LTR24_Open: %s\n", LTR24_GetErrorString(res));
            fflush(stdout);
            setLastError(LTR24_GetErrorString(res));
            return false;
        }
        printf("[ACQ] Модуль открыт\n"); fflush(stdout);

        for (int i = 0; i < 4; i++) {
            ltr24.ChannelMode[i].Enable  = g_cfg.channels[i].enable;
            ltr24.ChannelMode[i].Range   = g_cfg.channels[i].range;
            ltr24.ChannelMode[i].AC      = g_cfg.channels[i].ac;
            ltr24.ChannelMode[i].ICPMode = g_cfg.channels[i].icp;
        }
        ltr24.ADCFreqCode = g_cfg.getFreqCode();
        ltr24.DataFmt     = g_cfg.getFormatCode();

        printf("[ACQ] Настраиваю АЦП: freq_hz=%d, code=%d, format=%d\n",
               g_cfg.freq_hz, g_cfg.getFreqCode(), g_cfg.getFormatCode());
        fflush(stdout);

        res = LTR24_SetADC(&ltr24);
        if (res != LTR_OK) {
            printf("[ACQ] Ошибка LTR24_SetADC: %s\n", LTR24_GetErrorString(res));
            fflush(stdout);
            setLastError(LTR24_GetErrorString(res));
            LTR24_Close(&ltr24);
            return false;
        }
        printf("[ACQ] АЦП настроен\n"); fflush(stdout);
        return true;
    };

    auto recalcActiveChannels = [&]() {
        g_activeChannels = 0;
        for (int i = 0; i < 4; i++) {
            if (g_cfg.channels[i].enable) g_activeChannels++;
        }
    };

    // ------------------------------------------------------------
    // Первичная инициализация
    // ------------------------------------------------------------
    recalcActiveChannels();
    if (g_activeChannels == 0) {
        printf("[ACQ] Нет активных каналов, поток завершён\n"); fflush(stdout);
        return;
    }
    if (!openAndConfigure()) {
        printf("[ACQ] Модуль не открыт, поток завершён\n"); fflush(stdout);
        return;
    }

    blockSize = g_cfg.block_size;
    recv_buf.assign(blockSize, 0);
    tmark.assign(blockSize, 0);
    data.assign(blockSize, 0.0);

    setPhase(PHASE_IDLE);

    // ============================================================
    // Главный цикл
    // ============================================================
    while (!g_stopRequested.load()) {

        // ─────────── Ожидание (не запущено) ───────────
        if (!g_running.load()) {
            if (adcStarted) { LTR24_Stop(&ltr24); adcStarted = false; }
            if (isDataFileOpen()) closeDataFile();

            if (g_configDirty.load() && !g_restartRequested.load()) {
                printf("[ACQ] Конфиг изменился — переоткрываю модуль\n");
                fflush(stdout);

                LTR24_Close(&ltr24);

                if (!LoadConfig("config.ini", g_cfg)) {
                    printf("[ACQ] Ошибка перечитывания config.ini\n");
                    fflush(stdout);
                }
                PrintConfig(g_cfg);

                recalcActiveChannels();

                if (g_activeChannels == 0) {
                    printf("[ACQ] Нет активных каналов после смены конфига\n");
                    fflush(stdout);
                    g_configDirty = false;
                    Sleep(500);
                    continue;
                }

                blockSize = g_cfg.block_size;
                recv_buf.assign(blockSize, 0);
                tmark.assign(blockSize, 0);
                data.assign(blockSize, 0.0);

                if (!openAndConfigure()) {
                    printf("[ACQ] Не удалось переоткрыть модуль\n");
                    fflush(stdout);
                    g_configDirty = false;
                    Sleep(500);
                    continue;
                }

                g_configDirty = false;
                printf("[ACQ] Конфиг применён: freq_hz=%d, каналов=%d\n",
                       g_cfg.freq_hz, g_activeChannels);
                fflush(stdout);
            }

            setPhase(PHASE_IDLE);
            Sleep(50);
            continue;
        }

        // ─────────── Старт АЦП ───────────
        if (!adcStarted) {
            INT res = LTR24_Start(&ltr24);
            if (res != LTR_OK) {
                printf("[ACQ] Ошибка LTR24_Start: %s\n", LTR24_GetErrorString(res));
                fflush(stdout);
                setLastError(LTR24_GetErrorString(res));
                g_running = false;
                continue;
            }
            adcStarted = true;
            printf("[ACQ] АЦП запущен\n"); fflush(stdout);
        }

        // ─────────── Новый файл ───────────
        if (g_cfg.write_to_file && !isDataFileOpen()) {
            g_recordStartEpochNs = nowEpochNs();
            if (!openDataFile()) {
                printf("[ACQ] Не удалось открыть файл\n"); fflush(stdout);
                g_running = false;
                continue;
            }
            printf("[ACQ] Файл открыт: %s\n", g_currentFileName.c_str());
            fflush(stdout);
        }

        setPhase(PHASE_RECORDING);
        printf("[ACQ] Начало записи (%.1f с)\n", g_cfg.record_time); fflush(stdout);

        // ─────────── Цикл записи ───────────
        const double fs = (double)g_cfg.freq_hz;

        while (g_running.load() && !g_stopRequested.load()) {
            double elapsed = (double)(nowSteadyNs() - g_phaseStartNs.load()) / 1e9;
            if (elapsed >= g_cfg.record_time) break;

            if (g_configDirty.load()) {
                printf("[ACQ] Обнаружено изменение конфига — прерываю запись\n");
                fflush(stdout);
                break;
            }

            INT res = LTR24_Recv(&ltr24, recv_buf.data(), tmark.data(),
                                 blockSize, 200);
            if (res < 0) {
                printf("[ACQ] Ошибка LTR24_Recv: %s\n", LTR24_GetErrorString(res));
                fflush(stdout);
                setLastError(LTR24_GetErrorString(res));
                g_running = false;
                break;
            }
            if (res == 0) continue;

            INT data_size = res;
            res = LTR24_ProcessData(&ltr24, recv_buf.data(), data.data(),
                                    &data_size, LTR24_PROC_FLAG_VOLT, NULL);
            if (res != LTR_OK) {
                setLastError(LTR24_GetErrorString(res));
                continue;
            }

            int points = data_size / g_activeChannels;
            long long startIdx = totalSamples();

            addSamples(data.data(), points, g_activeChannels, fs);
            if (g_cfg.write_to_file) {
                writeBlockToFile(data.data(), points, g_activeChannels, fs, startIdx);
            }
        }

        // ─────────── Конец записи ───────────
        if (adcStarted) { LTR24_Stop(&ltr24); adcStarted = false; }
        if (isDataFileOpen()) closeDataFile();

        if (!g_running.load()) continue;

        // ─────────── Фаза сна ───────────
        setPhase(PHASE_SLEEPING);
        printf("[ACQ] Сон (%.1f с)\n", g_cfg.sleep_time); fflush(stdout);

        while (g_running.load() && !g_stopRequested.load()) {
            double s = (double)(nowSteadyNs() - g_phaseStartNs.load()) / 1e9;
            if (s >= g_cfg.sleep_time) break;
            Sleep(50);
        }
    }

    // ─────────── Завершение ───────────
    if (adcStarted) LTR24_Stop(&ltr24);
    if (isDataFileOpen()) closeDataFile();
    LTR24_Close(&ltr24);
    g_phase = PHASE_IDLE;
    printf("[ACQ] Поток сбора завершён\n"); fflush(stdout);
}

// ============================================================
// Перезапуск текущего процесса
// ============================================================
static void restartSelf() {
    printf("\n[RESTART] Подготовка к перезапуску...\n"); fflush(stdout);
    Sleep(500);

#ifdef _WIN32
    char exePath[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    char workDir[MAX_PATH] = { 0 };
    GetCurrentDirectoryA(MAX_PATH, workDir);

    std::string cmd = "cmd /c start \"L-Card LTR24\" /D \"";
    cmd += workDir;
    cmd += "\" \"";
    cmd += exePath;
    cmd += "\"";

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, workDir, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        printf("[RESTART] OK\n");
    }
#else
    char exePath[1024] = { 0 };
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len <= 0) { printf("[RESTART] нет пути\n"); return; }
    exePath[len] = '\0';

    char workDir[1024] = { 0 };
    getcwd(workDir, sizeof(workDir));

    printf("[RESTART] exe = %s\n", exePath);
    printf("[RESTART] cwd = %s\n", workDir);

    pid_t pid = fork();
    if (pid == 0) {
        setsid();
        chdir(workDir);
        execl(exePath, exePath, (char*)NULL);
        _exit(1);
    } else if (pid > 0) {
        printf("[RESTART] PID=%d\n", pid);
    }
#endif
    fflush(stdout);
}

// ============================================================
// main
// ============================================================
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // --- Загрузка конфига ---
    if (!LoadConfig("config.ini", g_cfg)) {
        printf("Ошибка загрузки config.ini\n");
        return -1;
    }
    PrintConfig(g_cfg);

    // --- Подсчёт активных каналов ---
    g_activeChannels = 0;
    for (int i = 0; i < 4; i++) {
        if (g_cfg.channels[i].enable) g_activeChannels++;
    }
    if (g_activeChannels == 0) {
        printf("Не включено ни одного канала!\n");
        return -1;
    }

    // --- Запуск потоков ---
    printf("\nЗапуск потоков...\n");
    std::thread acqThread(acquisitionThread);
    std::thread httpThread(runHttpServer);
    httpThread.detach();

    printf("Нажмите 'q' для выхода (или отправьте SIGTERM).\n\n");
    while (true) {
        if (g_stopRequested.load()) break;
        
        // На Linux, если нет терминала (работает как служба), не слушаем клавиатуру
        #ifdef _WIN32
        if (_kbhit() && _getch() == 'q') break;
        #else
        if (isatty(STDIN_FILENO) && _kbhit() && _getch() == 'q') break;
        #endif
        
        Sleep(100);
    }
    

    // --- Остановка ---
    printf("\nОстановка...\n");
    g_running = false;
    g_stopRequested = true;
    acqThread.join();

    // --- Перезапуск, если было сохранение конфига ---
    if (g_restartRequested.load()) {
        restartSelf();
    }

    printf("Завершение.\n");
    return 0;
}