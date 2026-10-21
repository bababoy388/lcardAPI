#pragma once
#ifndef _WIN32

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

// BOOL / TRUE / FALSE для config.h
typedef int BOOL;
#define TRUE  1
#define FALSE 0

// Sleep(ms)
inline void Sleep(unsigned int ms) { usleep(ms * 1000); }

// _kbhit / _getch
inline bool _kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) { ungetc(ch, stdin); return true; }
    return false;
}

inline char _getch() { return (char)getchar(); }

// Консольные функции Windows — заглушки
#define CP_UTF8 65001
inline int SetConsoleOutputCP(int) { return 0; }
inline int SetConsoleCP(int) { return 0; }

// _stat → stat
#define _stat stat

#endif // !_WIN32