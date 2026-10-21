#ifndef LWINTYPES_H__
#define LWINTYPES_H__

#include "ltrapi_config.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define LTRAPI_SLEEP_MS(ms) Sleep(ms)

#else
    #ifndef APIENTRY
        #define APIENTRY
    #endif

#ifdef LTRAPI_HAVE_STDINT_H
    #include <stdint.h>
#endif
#ifdef LTRAPI_HAVE_INTTYPES_H
    #include <inttypes.h>
#endif
    #include <errno.h>
    #include <unistd.h>
    #include <time.h>

    typedef uint8_t BYTE;
    typedef uint8_t BOOLEAN;
    typedef uint16_t WORD;
    typedef uint32_t DWORD;

    typedef int32_t INT;
    typedef uint32_t UINT;
    typedef int32_t LONG;
    typedef int16_t SHORT;
    typedef int BOOL;
    typedef int SOCKET;
    typedef BYTE* LPBYTE;
    typedef const BYTE* LPCBYTE;
    typedef char CHAR;
    typedef void* LPVOID;
    typedef const void* LPCVOID;
    typedef const char* LPCSTR;
    typedef DWORD* LPDWORD;
    typedef void* PVOID;
    typedef void* HINSTANCE;

#ifndef LTRAPI_HAVE_UINT64
    typedef struct {
        uint32_t l;
        uint32_t h;
    } uint64_t;
#endif

#ifndef LTRAPI_HAVE_INT64
    typedef struct {
        int32_t l;
        int32_t h;
    } int64_t;
#endif
    typedef uint64_t ULONGLONG;
    typedef int64_t  INT64;
    typedef int64_t  LONGLONG;

    typedef union _LARGE_INTEGER {
        struct {
            DWORD LowPart;
            LONG  HighPart;
        };
        struct {
            DWORD LowPart;
            LONG  HighPart;
        } u;
        LONGLONG QuadPart;
    } LARGE_INTEGER, *PLARGE_INTEGER;

    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1

    #ifndef TRUE
        #define TRUE    1
    #endif

    #ifndef FALSE
        #define FALSE   0
    #endif

#ifdef LTRAPI_HAVE_USLEEP
    #define LTRAPI_SLEEP_MS(ms) do {usleep(ms*1000);} while(0)
#else
    #define LTRAPI_SLEEP_MS(ms) do { \
        struct timespec time; \
        time.tv_sec = ms/1000; \
        time.tv_nsec = (ms%1000)*1000000UL; \
        for ( ; nanosleep(&time, &time)!=0; ) {} \
    } while(0)
#endif
#endif

#endif
