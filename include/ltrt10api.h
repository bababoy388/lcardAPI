/** @file ltrt10api.h
    Файл содержит определение функций, типов и констант для работы с
    наладочным модулем LTRT10.

    Модуль содержит DDS для генерации синусоидального сигнала заданной частоты
    и амплитуды и коммутатор на 4 канала. Амплитуда может иметь только
    одно из определенных значений.

    Для использования сперва заполняются настройки (TLTRT10_CONFIG) в описателе
    вручную или с помощью функций LTRT10_FillDDSFreq(),
    LTRT10_FillDDSOutAmpVolt()/LTRT10_FillDDSOutAmpCode(). После этого
    вызывается LTRT10_SetConfig() и снимается сброс LTRT10_SetDDSReset().

    Далее возможно изменение настроек при включенном DDS
    через изменение полей конфигурации и вызов LTRT10_SetConfig().
    */

#ifndef LTRT10API_H_
#define LTRT10API_H_


#include "ltr01api.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

#ifdef _WIN32
    #ifdef LTRT10API_EXPORTS
      #define LTRT10API_DllExport(type) __declspec(dllexport) type APIENTRY
    #else
      #define LTRT10API_DllExport(type) __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTRT10API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTRT10API_DllExport(type)  type
#endif


/** Размер строки с именем модуля в структуре #TINFO_LTRT10 */
#define LTRT10_NAME_SIZE        8
/** Размер строки с серийным номером модуля в структуре #TINFO_LTRT10 */
#define LTRT10_SERIAL_SIZE      16

/** Количество управляемых коммутаторов для подачи нужного сигнала */
#define LTRT10_SWITCH_CNT       4

/** Количество ступений усиления сигнала DDS */
#define LTRT10_DDS_GAIN_CNT     2

/** Количество возможныз выставляемых амплитуд сигнала DDS */
#define LTRT10_DDS_AMP_CNT      12

/** Частота клока DDS */
#define LTRT10_DDS_CLK_FREQ     60000000.

/** Делитель частоты клока в DDS */
#define LTRT10_DDS_FREQ_DIV     (268435456)

/** Максимальное значение поля FreqVal */
#define LTRT10_DDS_FREQ_VAL_MAX  (268435456-2)


/** Коды ошибок, специфичные для LTRT10. */
typedef enum {
    LTRT10_ERR_INVALID_SWITCH_POS           = -10400, /**< Задан неверный код состояния коммутатора*/
    LTRT10_ERR_INVALID_DDS_DIV              = -10401, /**< Задан неверный код коэффициента передачи выходного делителя сигнала DDS */
    LTRT10_ERR_INVALID_DDS_GAIN             = -10402, /**< Задан неверный код усиления для DDS */
    LTRT10_ERR_INVALID_FREQ_VAL             = -10403, /**< Неверно задан код частоты сигнала DDS */
    LTRT10_ERR_INVALID_DDS_AMP              = -10404, /**< Задан неверный код амплитуды сигнала DDS */
    LTRT10_ERR_GAIN2_EXCEED_GAIN1           = -10405  /**< Коэф. усиления второй ступени превышает коэф. первой ступени */
} e_LTRT10_ERROR_CODES;


/** Варианты состояния коммутатора для подачи нужного сигнала для определенного
    канала */
typedef enum {
    LTRT10_SWITCH_POS_ZERO         = 0, /**< Подача нуля */
    LTRT10_SWITCH_POS_DDS          = 1, /**< Внутренний DDS модуля */
    LTRT10_SWITCH_POS_DAC          = 2, /**< Подача сигнала с внешнего ЦАП */
    LTRT10_SWITCH_POS_ZSTATE       = 3  /**< Высокоимпедансное состояние */
} e_LTRT10_SWITCH_POS;

/** Константы для задания коэффициента передачи выходного делителя сигнала DDS */
typedef enum {
    LTRT10_DDS_DIV_1       = 0,
    LTRT10_DDS_DIV_0_75    = 1,
    LTRT10_DDS_DIV_0_5     = 2,
    LTRT10_DDS_DIV_0_25    = 3
} e_LTRT10_DDS_DIV;

/** Константы для задания коэффициента усиления для одной из ступений усиления
    сигнала DDS */
typedef enum {
    LTRT10_DDS_GAIN_4   = 0,
    LTRT10_DDS_GAIN_2   = 1,
    LTRT10_DDS_GAIN_1   = 2
} e_LTRT10_DDS_GAIN;

/** Константы для задания возможных амплитуд выходного сигнала через
    LTRT10_FillDDSOutAmpCode() */
typedef enum {
    LTRT10_DDS_OUT_AMP_9_6  = 0,
    LTRT10_DDS_OUT_AMP_7_2  = 1,
    LTRT10_DDS_OUT_AMP_4_8  = 2,
    LTRT10_DDS_OUT_AMP_3_6  = 3,
    LTRT10_DDS_OUT_AMP_2_4  = 4,
    LTRT10_DDS_OUT_AMP_1_8  = 5,
    LTRT10_DDS_OUT_AMP_1_2  = 6,
    LTRT10_DDS_OUT_AMP_0_9  = 7,
    LTRT10_DDS_OUT_AMP_0_6  = 8,
    LTRT10_DDS_OUT_AMP_0_45 = 9,
    LTRT10_DDS_OUT_AMP_0_3  = 10,
    LTRT10_DDS_OUT_AMP_0_15 = 11
} e_LTRT10_DDS_OUT_AMP;

#pragma pack (4)

/** Информация о модуле */
typedef struct {
    CHAR        Name[LTRT10_NAME_SIZE];      /**< Название модуля (оканчивающаяся нулем ASCII-строка) */
    CHAR        Serial[LTRT10_SERIAL_SIZE];  /**< Серийный номер модуля (оканчивающаяся нулем ASCII-строка) */
    BYTE        VerPLD;                      /**< Версия прошивки PLD  */
} TLTRT10_MODULE_INFO;


/** Настройки DDS */
typedef struct {
    /** Коэффициент деления выходного сигнала DDS. Значение из #e_LTRT10_DDS_DIV.
        Может заполняться через LTRT10_FillDDSOutAmpVolt()/LTRT10_FillDDSOutAmpCode() */
    WORD AmpDiv;
    /** Коэффициенты усиления для каждой ступени усиления.
        Каждый коэффициент - значение из #e_LTRT10_DDS_GAIN.
        Могут заполняться через LTRT10_FillDDSOutAmpVolt()/LTRT10_FillDDSOutAmpCode() */
    BYTE Gain[LTRT10_DDS_GAIN_CNT];
    /** Значение для задания частоты на выходе DDS.
        Частота fout = FreqVal*LTRT10_DDS_CLK_FREQ/LTRT10_DDS_FREQ_DIV.
        Может заполняться через LTRT10_FillDDSFreq()*/
    DWORD FreqVal;
} TLTRT10_DDS_CONFIG;


/** Настройки модуля. Заполняются перед вызовом LTRT10_SetConfig(). */
typedef struct {
    BYTE SwitchPos[LTRT10_SWITCH_CNT]; /**< Состояние коммутаторов для каждого канала */
    TLTRT10_DDS_CONFIG DDS; /**< Настройки DDS */
} TLTRT10_CONFIG;


/** Описатель модуля */
typedef struct {
    INT size;     /**< Размер структуры. Заполняется в LTRT10_Init(). */
    /** Структура, содержащая состояние соединения с сервером.
        Не используется напрямую пользователем. */
    TLTR Channel;
    /** Указатель на непрозрачную структуру с внутренними параметрами,
        используемыми исключительно библиотекой и недоступными для пользователя. */
    PVOID Internal;
    /** Настройки модуля, которые записываются по LTRT10_SetConfig() */
    TLTRT10_CONFIG Cfg;

    BYTE CurDSSFreqReg; /**< Признак, какой сейчас используется регистр DDS */
    BOOL DDSInReset; /**< Признак, находится ли DDS в сброшенном состоянии */
    double DDSFreq; /**< Установленная сейчас частота DSS. Обновляется после LTRT10_SetConfig() */
    double DDSAmp; /**< Установленная амплитуда выходного сигнала DDS. Обновляется после LTRT10_SetConfig() */

    TLTRT10_MODULE_INFO ModuleInfo;
} TLTRT10;

#pragma pack ()


/*============================================================================*/

LTRT10API_DllExport(INT) LTRT10_Init(TLTRT10 *hnd);
LTRT10API_DllExport(INT) LTRT10_Open(TLTRT10 *hnd, DWORD net_addr, WORD net_port,
                                     const CHAR *csn, WORD slot);
LTRT10API_DllExport(INT) LTRT10_Close(TLTRT10 *hnd);
LTRT10API_DllExport(INT) LTRT10_IsOpened(TLTRT10 *hnd);

/** Передача настроек DDS и коммутаторов в модуль. Можно выполнять
    многократно для смены параметов на лету.
   @param[in] hnd       Описатель
   @return              Код ошибки */
LTRT10API_DllExport(INT) LTRT10_SetConfig(TLTRT10 *hnd);

/** Установка или снятие сигнала сброса микросхемы DDS. После LTRT10_Open()
    DDS находится в сброшенном состоянии. Как правило LTRT10_SetDDSReset()
    вызывается после первой установки конфигурации с помощью LTRT10_SetConfig()
    для снятия сброса. Поле DDSInReset описателя показывает, находится ли
    микросхема DDS сейчас в состоянии сброса
   @param[in] hnd       Описатель
   @param[in] reset     Устанавливаемое состояние сигнала сброса DDS
                        (TRUE - сброс активен, FALSE - рабочий режим)
   @return              Код ошибки */
LTRT10API_DllExport(INT) LTRT10_SetDDSReset(TLTRT10 *hnd, BOOL reset);


/** Функция заполняет поле DDS.FreqVal конфигурации, чтобы результирующая частота
    сигнала DDS была наиболее близка к указанной.
    @param[in]  cfg       Указатель на структуру с конфигурацией
    @param[in]  freq      Устанавливаемая частота в Гц
    @param[out] set_freq  Если не NULL, то возвращается реальная частота,
                          соответствующая найденному значению DDS.FreqVal
    @return              Код ошибки */
LTRT10API_DllExport(INT) LTRT10_FillDDSFreq(TLTRT10_CONFIG *cfg, double freq, double* set_freq);

/** Функция заполняет поля DDS.AmpDiv и DDS.Gain конфигурации, чтобы результирующая
    амплитуда выходного сигнала DDS была наиболее близка к указанной.
    @param[in]  cfg       Указатель на структуру с конфигурацией
    @param[in]  value     Устанавливаемая амплитуда сигнала в Вольтах
    @param[out] set_value Если не NULL, то возвращается реальная амплитуда,
                          соответствующая найденным коэффициентам усиления.
    @return               Код ошибки */
LTRT10API_DllExport(INT) LTRT10_FillDDSOutAmpVolt(TLTRT10_CONFIG *cfg, double value, double* set_value);

/** Функция заполняет поля DDS.AmpDiv и DDS.Gain конфигурации по указанному
    коду амплитуды из #e_LTRT10_DDS_OUT_AMP.
    @param[in]  cfg       Указатель на структуру с конфигурацией
    @param[in]  code      Код амплитуды из #e_LTRT10_DDS_OUT_AMP
    @return               Код ошибки */
LTRT10API_DllExport(INT) LTRT10_FillDDSOutAmpCode(TLTRT10_CONFIG *cfg, DWORD code);

/***************************************************************************//**
   @brief Получение сообщения об ошибке.

   Функция возвращает строку, соответствующую переданному коду ошибки, в кодировке
   CP1251 для Windows или UTF-8 для Linux. Функция может обработать как ошибки
   из ltrt10api, так и общие коды ошибок из ltrapi.

   @param[in] err       Код ошибки
   @return              Указатель на строку, содержащую сообщение об ошибке.
 ******************************************************************************/
LTRT10API_DllExport(LPCSTR) LTRT10_GetErrorString(INT err);



#ifdef __cplusplus
}                                          // only need to export C interface if
                                           // used by C++ source code
#endif

#endif                      /* #ifndef LTR11API_H_ */
