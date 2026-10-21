//-----------------------------------------------------------------------------
// crate controller ltr010
//-----------------------------------------------------------------------------
#ifndef __LTR010API_H__
#define __LTR010API_H__

#include "ltrapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef LTR010API_EXPORTS
        #define LTR010API_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTR010API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR010API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR010API_DllExport(type)   type
#endif


// коды ошибок
#define LTR010_OK                     (LTR_OK)    /*Выполнено без ошибок.*/
#define LTR010_ERROR_GET_ARRAY        (-320l)     /*Ошибка выполнения команды GET_ARRAY.*/
#define LTR010_ERROR_PUT_ARRAY        (-321l)     /*Ошибка выполнения команды PUT_ARRAY.*/
#define LTR010_ERROR_GET_MODULE_NAME  (-322l)     /*Ошибка выполнения команды GET_MODULE_NAME.*/
#define LTR010_ERROR_GET_MODULE_DESCR (-323l)     /*Ошибка выполнения команды GET_MODULE_DESCRIPTOR.*/
#define LTR010_ERROR_INIT_FPGA        (-324l)     /*Ошибка выполнения команды INIT_FPGA.*/
#define LTR010_ERROR_RESET_FPGA       (-325l)     /*Ошибка выполнения команды RESET_FPGA.*/
#define LTR010_ERROR_INIT_DMAC        (-326l)     /*Ошибка выполнения команды INIT_DMAC.*/
#define LTR010_ERROR_LOAD_FPGA        (-327l)     /*Ошибка выполнения команды LOAD_FPGA.*/
#define LTR010_ERROR_OPEN_FILE        (-328l)     /*Ошибка открытия файла.*/
#define LTR010_ERROR_GET_INFO_FPGA    (-329l)     /*Ошибка выполнения команды GET_INFO_FPGA.*/


#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    #define LTR_ERROR_LOAD_FPGA           LTR010_ERROR_LOAD_FPGA
#endif
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef struct {
    TLTR_DESCRIPTION_MODULE     Module;
    TLTR_DESCRIPTION_CPU        Cpu;
    TLTR_DESCRIPTION_FPGA       Fpga;
    TLTR_DESCRIPTION_INTERFACE  Interface;
} TDESCRIPTION_LTR010;
//
typedef struct
{
    TLTR ltr;
}
TLTR010;
#pragma pack()
//-----------------------------------------------------------------------------
LTR010API_DllExport(INT) LTR010_Init(TLTR010 *module);
LTR010API_DllExport(INT) LTR010_Open(TLTR010 *module, DWORD saddr, WORD sport, const CHAR *csn);
LTR010API_DllExport(INT) LTR010_Close(TLTR010 *module);
LTR010API_DllExport(INT) LTR010_GetArray(TLTR010 *module, BYTE *buf, DWORD size, DWORD address);
LTR010API_DllExport(INT) LTR010_PutArray(TLTR010 *module, const BYTE *buf, DWORD size, DWORD address);
LTR010API_DllExport(INT) LTR010_GetDescription(TLTR010 *module, TDESCRIPTION_LTR010 *description);
LTR010API_DllExport(INT) LTR010_LoadFPGA(TLTR010 *module, const CHAR *fname, BYTE rdma, BYTE wdma);
//
LTR010API_DllExport(LPCSTR) LTR010_GetErrorString(INT error);
#ifdef __cplusplus
}
#endif
#endif

