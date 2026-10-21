//-----------------------------------------------------------------------------
// crate controller ltr010
//-----------------------------------------------------------------------------
#ifndef __LTR030API_H__
#define __LTR030API_H__

#include "ltrapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef LTR030API_EXPORTS
        #define LTR030API_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTR030API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR030API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR030API_DllExport(type)   type
#endif


// коды ошибок
typedef enum {
    LTR030_ERR_UNSUPORTED_CRATE_TYPE = -400, /* Данный тип крейта не поддерживается библиотекой */
    LTR030_ERR_FIRM_VERIFY           = -401, /* Ошибка проверки правильности записи прошивки */
    LTR030_ERR_FIRM_SIZE             = -402  /* Неверный размер прошивки */
} e_LTR030_ERROR_CODES;

typedef enum {
    LTR030_LOAD_STAGE_WRITE  = 1,
    LTR030_LOAD_STAGE_VERIFY = 2
} e_LTR030_LOAD_STAGES;

//-----------------------------------------------------------------------------
#pragma pack(4)

typedef struct {
    WORD Size;
    WORD Interface;
    DWORD IpAddr;
    DWORD Gateway;
    DWORD IpMask;
    BYTE  UserMac[6];
} TLTR030_CONFIG;


typedef struct {
    INT Size;
    TLTR Channel;
    LPVOID Internal;
    TLTR_CRATE_INFO Info;
} TLTR030;
#pragma pack()



/** Тип для callback-функции на прогресс записи прошивки */
typedef void (*TLTR030_LOAD_PROGR_CB)(DWORD stage, DWORD done_size, DWORD full_size, void* cb_data);


//-----------------------------------------------------------------------------
LTR030API_DllExport(INT) LTR030_Init(TLTR030 *hnd);
LTR030API_DllExport(INT) LTR030_Open(TLTR030 *hnd, DWORD saddr, WORD sport, WORD iface, const CHAR *csn);
LTR030API_DllExport(INT) LTR030_Close(TLTR030 *hnd);
LTR030API_DllExport(INT) LTR030_SetInterfaceUsb(TLTR030 *hnd);
LTR030API_DllExport(INT) LTR030_SetInterfaceTcp(TLTR030 *hnd, DWORD ip_addr, DWORD ip_mask, DWORD gate);
LTR030API_DllExport(INT) LTR030_FlashWrite(TLTR030 *hnd, DWORD addr, const BYTE *data, DWORD size);
LTR030API_DllExport(INT) LTR030_FlashRead(TLTR030 *hnd, DWORD addr, BYTE *data, DWORD size);
LTR030API_DllExport(INT) LTR030_LoadDspFirmware(TLTR030 *hnd, const char *file, TLTR030_LOAD_PROGR_CB cb, void *cb_data);
LTR030API_DllExport(INT) LTR030_LoadFpgaFirmware(TLTR030 *hnd, const char *file, TLTR030_LOAD_PROGR_CB cb, void *cb_data);
LTR030API_DllExport(INT) LTR030_GetConfig(TLTR030 *hnd, TLTR030_CONFIG *cfg);
LTR030API_DllExport(INT) LTR030_SetConfig(TLTR030 *hnd, const TLTR030_CONFIG *cfg);
LTR030API_DllExport(INT) LTR030_GetFactoryMac(TLTR030 *hnd, BYTE *mac);
LTR030API_DllExport(INT) LTR030_CrateReset(TLTR030 *hnd);

//
LTR030API_DllExport(LPCSTR) LTR030_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif

