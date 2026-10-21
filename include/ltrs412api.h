#ifndef LTRS412API_H_
#define LTRS412API_H_


#include "ltr01api.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

#ifdef _WIN32
    #ifdef LTRS412API_EXPORTS
      #define LTRS412API_DllExport(type) __declspec(dllexport) type APIENTRY
    #else
      #define LTRS412API_DllExport(type) __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTRS412API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTRS412API_DllExport(type)  type
#endif


/** Размер строки с именем модуля в структуре #TLTRS412_MODULE_INFO */
#define LTRS412_NAME_SIZE        16
/** Размер строки с серийным номером модуля в структуре #TLTRS412_MODULE_INFO */
#define LTRS412_SERIAL_SIZE      24
/** Количество выходов реле в модуле */
#define LTRS412_RELAY_CNT        16

#pragma pack (4)

/** Информация о модуле */
typedef struct {
    CHAR        Name[LTRS412_NAME_SIZE];      /**< Название модуля (оканчивающаяся нулем ASCII-строка) */
    CHAR        Serial[LTRS412_SERIAL_SIZE];  /**< Серийный номер модуля (оканчивающаяся нулем ASCII-строка) */
    BYTE        VerPLD;                      /**< Версия прошивки PLD  */
    DWORD       Reserved[11];                 /**< Резерв */
} TLTRS412_MODULE_INFO;

/** Параметры состояния модуля */
typedef struct {
    WORD  RelayStates; /**< Текущее состояние выходов реле. Устанавливается
                            библиотекой при успешном вызове LTRS412_SetRelayStates() */
    DWORD Reserved[7]; /**< Резерв */
} TLTRS412_STATE;



/** Описатель модуля */
typedef struct {
    INT size;     /**< Размер структуры. Заполняется в LTRS412_Init(). */
    /** Структура, содержащая состояние соединения с сервером.
        Не используется напрямую пользователем. */
    TLTR Channel;
    /** Указатель на непрозрачную структуру с внутренними параметрами,
        используемыми исключительно библиотекой и недоступными для пользователя. */
    PVOID Internal;    
    DWORD Reserved[16]; /**< Резерв */
    /** Состояние модуля и рассчитанные параметры. Поля изменяются функциями
        библиотеки. Пользовательской программой могут использоваться
        только для чтения. */
    TLTRS412_STATE State;
    TLTRS412_MODULE_INFO ModuleInfo; /**< Информация о модуле */
} TLTRS412;

#pragma pack ()

/*============================================================================*/

LTRS412API_DllExport(INT) LTRS412_Init(TLTRS412 *hnd);
LTRS412API_DllExport(INT) LTRS412_Open(TLTRS412 *hnd, DWORD net_addr, WORD net_port,
                                       const CHAR *csn, WORD slot);
LTRS412API_DllExport(INT) LTRS412_Close(TLTRS412 *hnd);
LTRS412API_DllExport(INT) LTRS412_IsOpened(TLTRS412 *hnd);


/***************************************************************************//**
   @brief Установка состояний выходов реле.

   Функция устанавливает состояние всех #LTRS412_RELAY_CNT выходов реле модуля.
   Каждому реле соответствует один бит в записываемом слове, начиная с младшего.
   Если он установлен в 1, то реле включено, иначе - выключено.

   @param[in] hnd       Описатель модуля
   @param[in] states    Слово с состоянием выходов реле.
                        Действительны только младшие #LTRS412_RELAY_CNT бит.
   @return              Код ошибки.
 ******************************************************************************/
LTRS412API_DllExport(INT) LTRS412_SetRelayStates(TLTRS412 *hnd, DWORD states);

/***************************************************************************//**
   @brief Получение сообщения об ошибке.

   Функция возвращает строку, соответствующую переданному коду ошибки, в кодировке
   CP1251 для Windows или UTF-8 для Linux. Функция может обработать как ошибки
   из LTRS412API, так и общие коды ошибок из ltrapi.

   @param[in] err       Код ошибки
   @return              Указатель на строку, содержащую сообщение об ошибке.
 ******************************************************************************/
LTRS412API_DllExport(LPCSTR) LTRS412_GetErrorString(INT err);


#ifdef __cplusplus
}                                          // only need to export C interface if
                                           // used by C++ source code
#endif

#endif                      /* #ifndef LTR11API_H_ */
