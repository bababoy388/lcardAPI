//---------------------------------------------------------------------------
// boot loader programming part
//---------------------------------------------------------------------------
#ifndef __LTRBOOTAPI_H__
#define __LTRBOOTAPI_H__
#include "ltr010api.h"


#ifdef _WIN32
    #ifdef LTRBOOTAPI_EXPORTS
        #define LTRBOOTAPI_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTRBOOTAPI_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTRBOOTAPI_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTRBOOTAPI_DllExport(type)   type
#endif


#ifdef __cplusplus
extern "C" {
#endif
// коды ошибок
#define LTRBOOT_OK                     (LTR_OK)     /*Выполнено без ошибок.*/
#define LTRBOOT_ERROR_GET_ARRAY        (-300l)      /*Ошибка выполнения команды GET_ARRAY.*/
#define LTRBOOT_ERROR_PUT_ARRAY        (-301l)      /*Ошибка выполнения команды PUT_ARRAY.*/
#define LTRBOOT_ERROR_CALL_APPL        (-302l)      /*Ошибка выполнения команды CALL_APPLICATION.*/
#define LTRBOOT_ERROR_GET_DESCRIPTION  (-303l)      /*Ошибка выполнения команды GET_DESCRIPTION.*/
#define LTRBOOT_ERROR_PUT_DESCRIPTION  (-304l)      /*Ошибка выполнения команды PUT_DESCRIPTION.*/
//-----------------------------------------------------------------------------
LTRBOOTAPI_DllExport(INT) LTRBOOT_Init(TLTR010 *module);
LTRBOOTAPI_DllExport(INT) LTRBOOT_Open(TLTR010 *module, DWORD saddr, WORD sport, const CHAR *csn);
LTRBOOTAPI_DllExport(INT) LTRBOOT_Close(TLTR010 *module);
LTRBOOTAPI_DllExport(INT) LTRBOOT_GetArray(TLTR010 *module, BYTE *buf, DWORD size, DWORD address);
LTRBOOTAPI_DllExport(INT) LTRBOOT_PutArray(TLTR010 *module, const BYTE *buf, DWORD size, DWORD address);
LTRBOOTAPI_DllExport(INT) LTRBOOT_GetDescription(TLTR010 *module, TDESCRIPTION_LTR010 *description);
LTRBOOTAPI_DllExport(INT) LTRBOOT_SetDescription(TLTR010 *module, const TDESCRIPTION_LTR010 *description);
LTRBOOTAPI_DllExport(INT) LTRBOOT_CallApplication(TLTR010 *module, DWORD address);
//
LTRBOOTAPI_DllExport(LPCSTR) LTRBOOT_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif

