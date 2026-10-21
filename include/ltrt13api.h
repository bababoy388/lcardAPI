/** @file LTRT13API.h
    Файл содержит определение функций, типов и констант для работы с
    наладочным модулем LTRT13.

    */

#ifndef LTRT13API_H_
#define LTRT13API_H_


#include "ltr01api.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

#ifdef _WIN32
    #ifdef LTRT13API_EXPORTS
      #define LTRT13API_DllExport(type) __declspec(dllexport) type APIENTRY
    #else
      #define LTRT13API_DllExport(type) __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTRT13API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTRT13API_DllExport(type)  type
#endif


/** Размер строки с именем модуля в структуре #TLTRT13_MODULE_INFO */
#define LTRT13_NAME_SIZE        16
/** Размер строки с серийным номером модуля в структуре #TLTRT13_MODULE_INFO */
#define LTRT13_SERIAL_SIZE      24

/** Номинальное значение сопротивления шунта (используется по умолчанию) */
#define LTRT13_RVAL_NOM      99.909


#define LTRT13_EXT_SYNC_FREQ_DIV_MIN   3
#define LTRT13_EXT_SYNC_FREQ_DIV_MAX   255

#define LTRT13_EXT_SYNC_FREQ_MAX       400000

/** Коды ошибок, специфичные для LTRT13. */
typedef enum {
    LTRT13_ERR_MARK_GEN_ENABLED         = -11000,  /**< Операция недопустима при разрешенной генерации меток */
    LTRT13_ERR_EXT_SYNC_FREQ_DIV        = -11001,  /**< Неверное значение делителя частоты для сигнала SYNC */
} e_LTRT13_ERROR_CODES;

#pragma pack (4)

/** Информация о модуле */
typedef struct {
    CHAR        Name[LTRT13_NAME_SIZE];      /**< Название модуля (оканчивающаяся нулем ASCII-строка) */
    CHAR        Serial[LTRT13_SERIAL_SIZE];  /**< Серийный номер модуля (оканчивающаяся нулем ASCII-строка) */
    BYTE        VerPLD;                      /**< Версия прошивки PLD  */
    LONGLONG    CbrTime;                     /**< Время записи калибровочного значения шунта - 64-бит Unixtime */
    double      RValue;                      /**< Значение сопротивления шунта */
    DWORD       Reserved[8];                 /**< Резерв */
} TLTRT13_MODULE_INFO;


typedef struct {
    DWORD ExtSyncFreqDiv;
} TLTRT13_CONFIG;

typedef struct {
    BOOLEAN MarkGenEn; /**< Резрешена ли генерация синхрометок (остальные команды не могут быть выполнены) */
    BOOLEAN ExtSyncGenEn; /**< Разрешена ли генерация частоты внешней синхронизации тестируемого модуля */
    BOOLEAN ExtStartLevel; /** Текущий установленный уровень выхода START */
    DWORD  ExtSyncFreqDiv;
    double ExtSyncFreq;
} TLTRT13_STATE;



/** Описатель модуля */
typedef struct {
    INT size;     /**< Размер структуры. Заполняется в LTRT13_Init(). */
    /** Структура, содержащая состояние соединения с сервером.
        Не используется напрямую пользователем. */
    TLTR Channel;
    /** Указатель на непрозрачную структуру с внутренними параметрами,
        используемыми исключительно библиотекой и недоступными для пользователя. */
    PVOID Internal;

    /** Настройки модуля. Заполняются пользователем перед вызовом LTR12_SetADC(). */
    TLTRT13_CONFIG Cfg;
    /** Состояние модуля и рассчитанные параметры. Поля изменяются функциями
        библиотеки. Пользовательской программой могут использоваться
        только для чтения. */
    TLTRT13_STATE State;
    /** Информация о модуле. */
    TLTRT13_MODULE_INFO ModuleInfo;
} TLTRT13;

#pragma pack ()

/*============================================================================*/

LTRT13API_DllExport(INT) LTRT13_Init(TLTRT13 *hnd);
LTRT13API_DllExport(INT) LTRT13_Open(TLTRT13 *hnd, DWORD net_addr, WORD net_port,
                                     const CHAR *csn, WORD slot);
LTRT13API_DllExport(INT) LTRT13_Close(TLTRT13 *hnd);
LTRT13API_DllExport(INT) LTRT13_IsOpened(TLTRT13 *hnd);


LTRT13API_DllExport(INT) LTRT13_FindExtSyncFreqParams(double adcFreq, DWORD *div, double *resultAdcFreq);
LTRT13API_DllExport(INT) LTRT13_FillExtSyncFreqParams(TLTRT13 *hnd, double adcFreq, double *resultAdcFreq);

LTRT13API_DllExport(INT) LTRT13_SetExtStartLevel(TLTRT13 *hnd, BOOLEAN lvl);
LTRT13API_DllExport(INT) LTRT13_SetExtSyncGenEnabled(TLTRT13 *hnd, BOOLEAN en);
LTRT13API_DllExport(INT) LTRT13_SetMarksGenEnabled(TLTRT13 *hnd, BOOLEAN en);

/***************************************************************************//**
   @brief Получение сообщения об ошибке.

   Функция возвращает строку, соответствующую переданному коду ошибки, в кодировке
   CP1251 для Windows или UTF-8 для Linux. Функция может обработать как ошибки
   из LTRT13API, так и общие коды ошибок из ltrapi.

   @param[in] err       Код ошибки
   @return              Указатель на строку, содержащую сообщение об ошибке.
 ******************************************************************************/
LTRT13API_DllExport(LPCSTR) LTRT13_GetErrorString(INT err);


LTRT13API_DllExport(INT) LTRT13_GetConfig(TLTRT13 *hnd);

#ifdef __cplusplus
}                                          // only need to export C interface if
                                           // used by C++ source code
#endif

#endif                      /* #ifndef LTR11API_H_ */
