#ifndef LTR41API_H_
#define LTR41API_H_

#include "ltrapi.h"

#ifdef _WIN32
    #ifdef LTR41API_EXPORTS
        #define LTR41API_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTR41API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR41API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR41API_DllExport(type) type
#endif



#define LTR41_ERR_WRONG_SECOND_MARK_CONF              (-7015)
#define LTR41_ERR_WRONG_START_MARK_CONF               (-7016)
#define LTR41_ERR_CANT_READ_DATA                      (-7017)
#define LTR41_ERR_WRONG_IO_DATA                       (-7025)
#define LTR41_ERR_WRONG_STREAM_READ_FREQ_SETTINGS     (-7026)


#define LTR41_EEPROM_SIZE                       (512)

#define LTR41_MARK_MODE_INTERNAL         0
#define LTR41_MARK_MODE_MASTER           1
#define LTR41_MARK_MODE_EXTERNAL         2

#define LTR41_STREAM_READ_RATE_MIN       100.
#define LTR41_STREAM_READ_RATE_MAX       100000.


#ifdef __cplusplus
extern "C" {
#endif

/* Структура описания модуля */
#pragma pack(4) 
typedef struct {
    CHAR Name[16];
    CHAR Serial[24];
    CHAR FirmwareVersion[8];// Версия БИОСа
    CHAR FirmwareDate[16];  // Дата создания данной версии БИОСа
} TLTR41_MODULE_INFO;


typedef struct {
    INT size;   // размер структуры
    TLTR Channel;
    double StreamReadRate;
    struct {
        INT SecondMark_Mode; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
        INT StartMark_Mode; //
    } Marks;  // Структура для работы с временными метками
    
    TLTR41_MODULE_INFO ModuleInfo;
} TLTR41, *PTLTR41; // Структура описания модуля

#pragma pack()

LTR41API_DllExport (INT) LTR41_Init(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_Open(PTLTR41 hnd, INT net_addr, WORD net_port,
                                    const CHAR *crate_sn, INT slot_num);
LTR41API_DllExport (INT) LTR41_IsOpened(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_Close(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_Config(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_GetConfig(TLTR41 *hnd);
LTR41API_DllExport (INT) LTR41_ReadPort(PTLTR41 hnd, WORD *InputData);
LTR41API_DllExport (INT) LTR41_StartStreamRead(PTLTR41 hnd); 
LTR41API_DllExport (INT) LTR41_StopStreamRead(PTLTR41 hnd); 
LTR41API_DllExport (INT) LTR41_Recv(PTLTR41 hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout); 
LTR41API_DllExport (INT) LTR41_ProcessData(PTLTR41 hnd, const DWORD *src, WORD *dest, DWORD *size);
LTR41API_DllExport (INT) LTR41_CalcStreamReadFreq(double freq, double *resultFreq);
LTR41API_DllExport (INT) LTR41_StartSecondMark(PTLTR41 hnd);  
LTR41API_DllExport (INT) LTR41_StopSecondMark(PTLTR41 hnd);  
LTR41API_DllExport (INT) LTR41_MakeStartMark(PTLTR41 hnd);
LTR41API_DllExport (LPCSTR) LTR41_GetErrorString(INT Error_Code);
LTR41API_DllExport (INT) LTR41_WriteEEPROM(PTLTR41 hnd, INT Address, BYTE val); 
LTR41API_DllExport (INT) LTR41_ReadEEPROM(PTLTR41 hnd, INT Address, BYTE *val); 

/***************************************************************************//**
  @brief Установка ширины импульса метки СТАРТ

  Данная функция позволяет задать время импульса, генерируемого модулем на
  выходе при генерации метки старт, если разрешена трансляция метки СТАРТ на
  выход (режим #LTR41_MARK_MODE_MASTER).
  По-умолчанию время импульса составляет порядка 200нс, что может быть
  недостаточно для запуска других устройств от данного импульса. Данная функция
  позволяет установить большее время импульса.
  Данная функция доступна только в прошивке, начиная с версии 2.0.
  Кроме того для генерации длинного импульса не должен быть запущен потоковый
  ввод данных (при запущенном время импульса соответствует времени без установки)

  @param[in] hnd        Описатель модуля
  @param[in] time_mks   Время импульса в мкс. Если 0 --- то используется вариант
                        по-умолчанию (~200нс).
  @return               Код ошибки
 ******************************************************************************/
LTR41API_DllExport (INT) LTR41_SetStartMarkPulseTime(TLTR41 *hnd, DWORD time_mks);


#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    /** @cond obsoleted */
    typedef TLTR41_MODULE_INFO TINFO_LTR41,*PTINFO_LTR41;
    /** @endcond */
#endif

#ifdef __cplusplus
    }
#endif

#endif
















