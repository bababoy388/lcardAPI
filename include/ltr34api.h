
#ifndef __LTR34API__
#define __LTR34API__

#ifdef _WIN32
    #ifdef LTR34API_EXPORTS
        #define LTR34API_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTR34API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR34API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR34API_DllExport(type) type
#endif


#include "ltrapi.h"

    // константы
#define LTR34_ERROR_SEND_DATA              (-3001)
#define LTR34_ERROR_RECV_DATA              (-3002)
#define LTR34_ERROR_RESET_MODULE           (-3003)
#define LTR34_ERROR_NOT_LTR34              (-3004)
#define LTR34_ERROR_CRATE_BUF_OWF          (-3005)
#define LTR34_ERROR_PARITY                 (-3006)
#define LTR34_ERROR_OVERFLOW               (-3007)
#define LTR34_ERROR_INDEX                  (-3008)
    //
#define LTR34_ERROR                        (-3009)
#define LTR34_ERROR_EXCHANGE               (-3010)
#define LTR34_ERROR_FORMAT                 (-3011)
#define LTR34_ERROR_PARAMETERS             (-3012)
#define LTR34_ERROR_ANSWER                 (-3013)
#define LTR34_ERROR_WRONG_FLASH_CRC        (-3014)
#define LTR34_ERROR_CANT_WRITE_FLASH       (-3015)
#define LTR34_ERROR_CANT_READ_FLASH        (-3016)
#define LTR34_ERROR_CANT_WRITE_SERIAL_NUM  (-3017)
#define LTR34_ERROR_CANT_READ_SERIAL_NUM   (-3018)
#define LTR34_ERROR_CANT_WRITE_FPGA_VER    (-3019)
#define LTR34_ERROR_CANT_READ_FPGA_VER     (-3020)
#define LTR34_ERROR_CANT_WRITE_CALIBR_VER  (-3021)
#define LTR34_ERROR_CANT_READ_CALIBR_VER   (-3022)
#define LTR34_ERROR_CANT_STOP              (-3023)
#define LTR34_ERROR_SEND_CMD               (-3024)
#define LTR34_ERROR_CANT_WRITE_MODULE_NAME (-3025)
#define LTR34_ERROR_CANT_WRITE_MAX_CH_QNT  (-3026)
#define LTR34_ERROR_CHANNEL_NOT_OPENED     (-3027)
#define LTR34_ERROR_WRONG_LCH_CONF         (-3028)



#define LTR34_MAX_BUFFER_SIZE         2097151
#define LTR34_EEPROM_SIZE                2048
#define LTR34_USER_EEPROM_SIZE           1024
#define LTR34_DAC_NUMBER_MAX                8

#define LTR34_DAC_CHANNEL_CNT       8


/* Размеры полей информации о модуле */
#define LTR34_SERIAL_SIZE                                    24
#define LTR34_MODULE_NAME_SIZE                               16
#define LTR34_FPGA_VER_SIZE                                  8
#define LTR34_CALIBR_VER_SIZE                                8
/** Максимальное значение для делителя частоты */
#define LTR34_FREQ_DIV_MAX                                   60

/* Режимы подтверждений */
#define LTR34_ACKTYPE_ECHO           (1)  /** Подтверждения на каждое слово */
#define LTR34_ACKTYPE_STATUS         (0)  /** Выдача статуса на 1024 слова */



#pragma pack(4)

typedef struct {
    float FactoryCalibrOffset[2*LTR34_DAC_CHANNEL_CNT];
    float FactoryCalibrScale[2*LTR34_DAC_CHANNEL_CNT];
} TLTR34_DAC_CHANNEL_CALIBRATION;

typedef struct {
    CHAR Name[LTR34_MODULE_NAME_SIZE];
    CHAR Serial[LTR34_SERIAL_SIZE];
    CHAR FPGA_Version[LTR34_FPGA_VER_SIZE];
    CHAR CalibrVersion[LTR34_CALIBR_VER_SIZE];
    BYTE MaxChannelQnt;
} TLTR34_MODULE_INFO;


//**** конфигурация модуля
typedef struct {
    INT size;                    /** Размер структуры */
    TLTR Channel;                /** описатель канала связи с модулем */
    DWORD LChTbl[LTR34_DAC_CHANNEL_CNT];   /** Таблица логических каналов */
    //**** настройки модуля
    BYTE FrequencyDivisor;       /** делитель частоты дискретизации 0..60 (31.25..500 кГц) */
    BYTE ChannelQnt;             /** количество каналов: 1, 2, 4 или 8 */
    BOOLEAN UseClb;
    BOOLEAN AcknowledgeType;     /**< тип подтверждения:
                                    LTR34_ACKTYPE_ECHO - высылать подтверждение
                                            каждого слова
                                    LTR34_ACKTYPE_STATUS - высылать состояние
                                            буфера на каждые 1024 слова */
    BOOLEAN ExternalStart;       /** внешний старт (TRUE - внешний старт,
                                                   FALSE - внутренний */
    BOOLEAN RingMode;            /** режим кольца  (TRUE - режим кольца,
                                                   FALSE - потоковый режим) */
    BOOLEAN BufferFull;          /** статус - признак переполнения буфера */
    BOOLEAN BufferEmpty;         /** статус - признак пустого буфера ("голодание")*/
    BOOLEAN DACRunning;          /** статус - запущена ли генерация */
    float FrequencyDAC;          /** статус - частота - на которую настроен
                                              ЦАП в текущей конфигурации */
    TLTR34_DAC_CHANNEL_CALIBRATION DacCalibration; /** калибровочные коэффициенты */
    TLTR34_MODULE_INFO ModuleInfo;   /** информация о модуле */
} TLTR34,*PTLTR34;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

// Инициализация полей структуры TLTR34
LTR34API_DllExport (INT) LTR34_Init (TLTR34 *module);

// Установление связи с модулем LTR34.
// Модулю посылается пакет STOP+RESET и ожидается подтверждение сброса.
LTR34API_DllExport (INT) LTR34_Open (TLTR34 *module, DWORD net_addr, WORD net_port,
                                     const CHAR *crate_sn, WORD slot_num);
// Разрыв связи с модулем.
LTR34API_DllExport (INT) LTR34_Close (TLTR34 *module);

// Определение сотояния канала связи с модулем.
LTR34API_DllExport (INT) LTR34_IsOpened (TLTR34 *module);

/*
Функция для приема данных от модуля.
1) при работе цап в режиме ECHO - все данные, генерируемые
модулем, сервер обрабатвает и отдает клиенту в том виде
в котором принял. Таким образом, в режиме ECHO клиент
должен всегда откачивать данные от модуля.
2) При работе цап в режиме PERIOD - все данные, генерируемые
модулем, сервер обрабатвает и "проглатывает".
*/
LTR34API_DllExport (INT) LTR34_Recv (TLTR34 *module, DWORD *data, DWORD *tstamp,
                                     DWORD size, DWORD timeout);

// Формирование логического канала
LTR34API_DllExport (DWORD) LTR34_CreateLChannel(BYTE PhysChannel, BOOLEAN ScaleFlag);

/*
функция для отправки данных модулю
1) В ltr-сервере реализовано управление потоком данных
поступающих на модуль LTR34 от клиента. Поэтому при
работе с ЦАП в потоковом режиме (RingMode=0)
данные можно высылать в любых количествах, не заботясь
о переполнении буфера в LTR34. Однако, следует
позаботиться о правильном подборе таймаута, т.к.
при достаточном заполнении буфера в модуле
прием данных сервером от клиента будет производиться со скоростью
пропорцианальной частоте дискретизации ЦАП, т.е. при низких частотах
дискретизации и достаточно большом количестве отсылаемых данных
может потребоваться достаточно большое значение таймаута.

2) Управление потоком модуля LTR34 строится на подсчете количества
отправленных сэмплов и принятых подтверждений, что в некоторых
случаях может приводить к блокировке потока в модуль (Например,
на LTR34 уже было отправлено достаточное количество данных, но
ЦАП еще не стартовал и подтверждения от модуля еще не поступают.
В этом случае сервер прикратит прием данных от клиента).
Для сброса механизма управления потоком следует выслать модулю
команду RESET (при этом будут сброшены внутренние счетчики в ltr-сервере)
или произвести переподключение к модулю (вызвать LTR34_Close() и LTR34_Open())
*/
LTR34API_DllExport (INT) LTR34_Send (TLTR34 *module, const DWORD *data,
                                     DWORD size, DWORD timeout);

LTR34API_DllExport (INT) LTR34_ProcessData(TLTR34 *module, const double *src,
                                           DWORD *dest, DWORD size, BOOLEAN volt);

// Запись регистра CONFIG
LTR34API_DllExport (INT) LTR34_Config(TLTR34 *module);

// Запуск ЦАП.
LTR34API_DllExport (INT) LTR34_DACStart(TLTR34 *module);

// Останов ЦАП.
LTR34API_DllExport (INT) LTR34_DACStop(TLTR34 *module);

/***********************************************************************//**
   @brief Запрос на остнов генерации ЦАП

  Функция посылает команду на останов генерации ЦАП без ожидания подтверждения
  о завершении генерации. После этой команды нужно будет вызвать
  LTR34_DACStopResponseWait() для получения ответа.

  @param module Описатель модуля LTR34
  @return Код ошибки
  ************************************************************************/
LTR34API_DllExport (INT) LTR34_DACStopRequest(TLTR34 *module);
/***********************************************************************//**
   @brief Ожидание ответа на останов ЦАП

  Функция ожидает получение ответа о заверении останова генерации. Должна
  вызываться только после LTR34_DACStopRequest().

  Модуль высылает ответ только после того, как выдаст на ЦАП все записанные
  до этого точки в потоковом режиме, что может занять относительно большое
  время, поэтому в функции явно введена возможность задать таймаут на ожидение.

  Если функции возвращает код ошибки LTR_ERROR_NO_CMD_RESPONSE, то подтверждение
  останова не было получено. Можно после этого либо заново вызвать данную
  функцию для дальнейшего ожидания, либо сбросить модуль.

  @param module Описатель модуля LTR34
  @return Код ошибки
  ************************************************************************/
LTR34API_DllExport (INT) LTR34_DACStopResponseWait(TLTR34 *module, DWORD tout);

LTR34API_DllExport (INT) LTR34_Reset(TLTR34 *module);


LTR34API_DllExport (INT) LTR34_GetCalibrCoeffs(TLTR34 *module);
LTR34API_DllExport (INT) LTR34_WriteCalibrCoeffs(TLTR34 *module);

LTR34API_DllExport (INT) LTR34_ReadFlash(TLTR34 *module, BYTE *data,
                                         WORD size, WORD Address);

// функции вспомагательного характера
LTR34API_DllExport (LPCSTR) LTR34_GetErrorString(INT error);

// Проверка целостности ППЗУ
LTR34API_DllExport (INT) LTR34_TestEEPROM(TLTR34 *module);


/** @brief Подбор значения делителя частоты ЦАП

   Функция по заданному значению частоты генерации ЦАП (на канал) подбирает
   значения делителя, чтобы результирующая частота была наиболее близка
   к заданной.

   @param[in] dacFreq         Требуемая частота генерации ЦАП в Гц.
   @param[in] ChannelQnt      Количество разрешенных каналов в модуле
   @param[out] devisor        В данной переменной сохраняется найденное значение
                              делителя, соответствующее ближайшей частоте.
                              Может быть NULL, если не требуется.
   @param[out] resultAdcFreq  В данной переменной сохраняется найденное реальное
                              значение частоты генерации ЦАП в Гц.
                              Может быть NULL, если не требуется.
   @return                    Код ошибки */
LTR34API_DllExport(INT) LTR34_FindDacFreqDivisor(double dacFreq, BYTE ChannelQnt, BYTE *devisor, double *resultDacFreq);

#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    /** @cond obsoleted */
    #define LTR34_DACStopRequst     LTR34_DACStopRequest
    typedef TLTR34_DAC_CHANNEL_CALIBRATION DAC_CHANNEL_CALIBRATION;
    typedef TLTR34_MODULE_INFO TINFO_LTR34,*PTINFO_LTR34;
    typedef TLTR34_DAC_CHANNEL_CALIBRATION LTR34_DAC_CHANNEL_CALIBRATION;
    /** @endcond */
#endif
#ifdef __cplusplus
}
#endif

#endif

