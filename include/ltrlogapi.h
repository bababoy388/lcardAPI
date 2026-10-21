#ifndef __LTRLOGAPI_H_
#define __LTRLOGAPI_H_

#include "ltrapi.h"
#include <stddef.h>


#ifdef _WIN32
    #ifdef LTRLOGAPI_EXPORTS
        #define LTRLOGAPI_DllExport(type)    __declspec(dllexport) type APIENTRY
    #else
        #define LTRLOGAPI_DllExport(type)    __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTRLOGAPI_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTRLOGAPI_DllExport(type) type
#endif


#define LTRLOG_MSG_SIGN   0xA54C



#pragma pack (4)
typedef struct {
    INT    Size;
    TLTR   Channel;
    PVOID  Internal;
} TLTRLOG;


/** —труктура, описывающа€ переданное сообщение журнала */
typedef struct {
    WORD Sign; /**< ѕризнак сообщени€. ¬сегда равен #LTRLOG_MSG_SIGN */
    WORD Size; /**< –азмер всего сообщени€ с заголовком */
    INT  Err;  /**<  од ошибки (имеет значение только дл€ уровн€ ошибок) */
    LARGE_INTEGER Time; /**< ¬рем€ возникновени€ сообщени€ в сервере */
    BYTE Lvl; /**< ”ровень сообщени€ */
    BYTE Reserved[15]; /**< –езерв */
    char Msg[1]; /**< “екст сообщени€ (переменной длины) */
} TLTRLOG_MSG;

/** –азмер заголовка сообщени€ (фиксированной части) */
#define LTRLOG_MSG_HDR_SIZE (offsetof(TLTRLOG_MSG, Msg))

#pragma pack ()

#ifdef __cplusplus
extern "C" {
#endif
  
  
LTRLOGAPI_DllExport(INT) LTRLOG_Init(TLTRLOG *hnd);
LTRLOGAPI_DllExport(INT) LTRLOG_Open(TLTRLOG *hnd, DWORD net_addr, WORD net_port);
LTRLOGAPI_DllExport(INT) LTRLOG_Close(TLTRLOG *hnd);
LTRLOGAPI_DllExport(INT) LTRLOG_IsOpened(TLTRLOG *hnd);
LTRLOGAPI_DllExport(INT) LTRLOG_Shutdown(TLTRLOG *hnd);

/***************************************************************************//**
 * ѕрием сообщени€ от сервера. ¬озвращает управление как только будет прин€то
 * одно полное сообщение от сервера или истечет таймаут.
 *
 * ¬ случае приема целового сообщени€ возвращает в msg указатель на созданную
 * структуру сообщени€, которую затем следует освободить с помощью LTRLOG_FreeMsg().
 * ≈сли за заданный таймаут не прин€то ни одного полного сообщени€, то в msg
 * возвращаетс€ нулевой указатель.
 *
 * @param [in]  hnd      ќписатель соединени€
 * @param [out] msg      ¬ данной переменной возвращаетс€ указатель на созданную
 *                       структуру с прин€тым сообщением или NULL, если сообщение
 *                       не было прин€то.
 * @param [in]  tout     “аймаут на врем€ приема сообщени€ в мс
 * @return                од ошибки
 ******************************************************************************/
LTRLOGAPI_DllExport(INT) LTRLOG_GetNextMsg(TLTRLOG *hnd, TLTRLOG_MSG **msg, DWORD tout);

/***************************************************************************//**
 * ќсвобождение пам€ти структуры сообщени€, возвращанной LTRLOG_GetNextMsg().
 * ƒолжна вызыватьс€ всегда при получении нового сообщени€ после его обработки.
 *
 * @param[in]  msg      ”казатель на созданную структуру сообщени€
 * @return               од ошибки
 ******************************************************************************/
LTRLOGAPI_DllExport(INT) LTRLOG_FreeMsg(TLTRLOG_MSG *msg);

#ifdef __cplusplus
}
#endif


#endif
