#ifndef LTR43API_H_
#define LTR43API_H_

#include "ltrapi.h"

#ifdef _WIN32
    #ifdef LTR43API_EXPORTS
        #define LTR43API_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTR43API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR43API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR43API_DllExport(type)   type
#endif


// Коды ошибок
#define LTR43_ERR_RS485_FRAME_ERR_RCV                 (-4016)
#define LTR43_ERR_RS485_PARITY_ERR_RCV                (-4017)
#define LTR43_ERR_RS485_WRONG_BAUDRATE                (-4019)
#define LTR43_ERR_RS485_WRONG_FRAME_SIZE              (-4020)
#define LTR43_ERR_RS485_WRONG_PARITY_CONF             (-4021)
#define LTR43_ERR_RS485_WRONG_STOPBIT_CONF            (-4022)
#define LTR43_ERR_DATA_TRANSMISSON_ERROR              (-4023)
#define LTR43_ERR_RS485_CONFIRM_TIMEOUT               (-4024)
#define LTR43_ERR_RS485_SEND_TIMEOUT                  (-4025)
#define LTR43_ERR_WRONG_IO_LINES_CONF                 (-4029)
#define LTR43_ERR_WRONG_SECOND_MARK_CONF              (-4030)
#define LTR43_ERR_WRONG_START_MARK_CONF               (-4031)
#define LTR43_ERR_CANT_READ_DATA                      (-4032)
#define LTR43_ERR_RS485_WRONG_PACK_SIZE               (-4038)
#define LTR43_ERR_RS485_WRONG_OUT_TIMEOUT             (-4039)
#define LTR43_ERR_RS485_WRONG_IN_TIMEOUT              (-4040)
#define LTR43_ERR_WRONG_IO_DATA                       (-4046)
#define LTR43_ERR_WRONG_STREAM_READ_FREQ_SETTINGS     (-4047)



#define LTR43_EEPROM_SIZE                (512)


#define LTR43_MARK_MODE_INTERNAL         0
#define LTR43_MARK_MODE_MASTER           1
#define LTR43_MARK_MODE_EXTERNAL         2

#define LTR43_RS485_PARITY_NONE          0
#define LTR43_RS485_PARITY_EVEN          1
#define LTR43_RS485_PARITY_ODD           2

#define LTR43_PORT_DIR_IN                0
#define LTR43_PORT_DIR_OUT               1


/* флаг для LTR43_RS485_ExchangeEx(), указывающий, что нужно интервал между
   соседними байтами больше указанного таймаута является признаком конца кадра */
#define LTR43_RS485_FLAGS_USE_INTERVAL_TOUT  1


#define LTR43_STREAM_READ_RATE_MIN       100.
#define LTR43_STREAM_READ_RATE_MAX       100000.





#pragma pack(4) 

/* Информация о модуле */
typedef struct {
    CHAR Name[16]; //Название модуля "LTR43"
    CHAR Serial[24]; //Серийный номер модуля
    CHAR FirmwareVersion[8];// Версия прошивки AVR
    CHAR FirmwareDate[16];  // Дата создания прошивки AVR
} TLTR43_MODULE_INFO;

/* Структура описания модуля */
typedef struct {
    INT size;   // размер структуры
    TLTR Channel;
    double StreamReadRate;
    struct {
        INT Port1; // направление линий ввода/вывода группы 1 (IO1-IO8)
        INT Port2; // направление линий ввода/вывода группы 2 (IO9-IO16)
        INT Port3; // направление линий ввода/вывода группы 3 (IO17-IO24)
        INT Port4; // направление линий ввода/вывода группы 4 (IO25-IO32)
    } IO_Ports;

    struct {
        INT FrameSize;      // Кол-во бит в кадре
        INT Baud;          // Скорость обмена в бодах
        INT StopBit;      // Кол-во стоп-бит
        INT Parity;          // Включение бита четности
        INT SendTimeoutMultiplier; // Множитель таймаута отправки
        INT ReceiveTimeoutMultiplier; // Множитель таймаута приема подтверждения
    } RS485; // Структура для конфигурации RS485

    struct {
        INT SecondMark_Mode; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
        INT StartMark_Mode; //
    } Marks;  // Структура для работы с временными метками

    TLTR43_MODULE_INFO ModuleInfo;
} TLTR43, *PTLTR43; // Структура описания модуля

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

LTR43API_DllExport (INT) LTR43_Init(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_Open(TLTR43 *hnd, DWORD net_addr, WORD net_port,
                                    const CHAR *crate_sn, INT slot_num);
LTR43API_DllExport (INT) LTR43_IsOpened(PTLTR43 hnd);
LTR43API_DllExport (INT) LTR43_Close(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_Config(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_GetConfig(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_WritePort(TLTR43 *hnd, DWORD OutputData);
LTR43API_DllExport (INT) LTR43_WriteArray(TLTR43 *hnd, DWORD *OutputArray, BYTE ArraySize);
LTR43API_DllExport (INT) LTR43_ReadPort(TLTR43 *hnd, DWORD *InputData);
LTR43API_DllExport (INT) LTR43_WritePortRequest(TLTR43 *hnd, DWORD OutputData);
LTR43API_DllExport (INT) LTR43_WritePortResponseWait(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_ReadPortRequest(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_ReadPortResponseWait(TLTR43 *hnd, DWORD *InputData);

LTR43API_DllExport (INT) LTR43_StartStreamRead(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_StopStreamRead(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_Recv(TLTR43 *hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout);
LTR43API_DllExport (INT) LTR43_ProcessData(TLTR43 *hnd, const DWORD *src, DWORD *dest, DWORD *size);
LTR43API_DllExport (INT) LTR43_CalcStreamReadFreq(double freq, double *resultFreq);

LTR43API_DllExport (INT) LTR43_StartSecondMark(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_StopSecondMark(TLTR43 *hnd);
LTR43API_DllExport (INT) LTR43_MakeStartMark(TLTR43 *hnd);
LTR43API_DllExport (LPCSTR) LTR43_GetErrorString(INT Error_Code);

LTR43API_DllExport (INT) LTR43_RS485_SetResponseTout(PTLTR43 hnd, DWORD tout);
LTR43API_DllExport (INT) LTR43_RS485_SetIntervalTout(PTLTR43 hnd, DWORD tout);
LTR43API_DllExport (INT) LTR43_RS485_SetTxActiveInterval(PTLTR43 hnd, DWORD start_of_packet, DWORD end_of_packet);
LTR43API_DllExport (INT) LTR43_RS485_Exchange(TLTR43 *hnd, const SHORT *PackToSend,
                                              SHORT *ReceivedPack, INT OutPackSize, INT InPackSize);
LTR43API_DllExport (INT) LTR43_RS485_ExchangeEx(PTLTR43 hnd, const SHORT *PackToSend,
                                                SHORT *ReceivedPack, INT OutPackSize, INT InPackSize,
                                                DWORD flags, INT *ReceivedSize);

LTR43API_DllExport (INT) LTR43_RS485_TestReceiveByte(TLTR43 *hnd, INT OutBytesQnt,INT InBytesQnt);
LTR43API_DllExport (INT) LTR43_RS485_TestStopReceive(TLTR43 *hnd);

LTR43API_DllExport (INT) LTR43_WriteEEPROM(TLTR43 *hnd, INT Address, BYTE val);
LTR43API_DllExport (INT) LTR43_ReadEEPROM(TLTR43 *hnd, INT Address, BYTE *val);


/***************************************************************************//**
  @brief Установка ширины импульса метки СТАРТ

  Данная функция позволяет задать время импульса, генерируемого модулем на
  выходе при генерации метки старт, если разрешена трансляция метки СТАРТ на
  выход (режим #LTR43_MARK_MODE_MASTER).
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
LTR43API_DllExport (INT) LTR43_SetStartMarkPulseTime(TLTR43 *hnd, DWORD time_mks);

#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    /** @cond obsoleted */
    typedef TLTR43_MODULE_INFO TINFO_LTR43,*PTINFO_LTR43;
    /** @endcond */
#endif

#ifdef __cplusplus 
}
#endif

#endif
















