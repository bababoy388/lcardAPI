/** @file ltr01api.h
    Файл содержит определение функций для получения дополнительного идентификатора
    дополнительных модулей системы LTR. Все дополнительные модули используют
    общий MID равный MID_LTR01. Узнать, какой дополнительный модуль реально установлен,
    можно с помощью расширенного идентификатора.

    В библиотеке введен как набор функций для установки соединения и последующиего
    получения расширенного ID (в первую очередь для использования в API
    самих дополнительных модулей), так и упрощенная функция, выполняющая все
    за один вызов (для удобного вызова из внешних программ при определении
    состава модулей).

    Библиотека не использует дополнительного типа для определения описателя
    модуля LTR01, а использует напрямую описатель соединения TLTR. Это сделано
    для более удобного встраивания в API дополнительных модулей. */

#ifndef LTR01API_H_
#define LTR01API_H_


#include "ltrapi.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

#ifdef _WIN32
    #ifdef LTR01API_EXPORTS
      #define LTR01API_DllExport(type) __declspec(dllexport) type APIENTRY
    #else
      #define LTR01API_DllExport(type) __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR01API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR01API_DllExport(type)  type
#endif

/** Группы дополнительных модулей */
typedef enum {
    LTR01_SUBID_GROUP_GENERAL = 0, /**< Общая группа, в которой идут модули без принадлежности
                                        к какой-либо группе или модули, разработанные до
                                        введения класификации на группы */
    LTR01_SUBID_GROUP_LTRK    = 1, /**< Модули семейства LTRK */
} e_LTR01_SUBID_GROUP;


/** Макрос для создания идентификатора по группе и номеру модуля */
#define LTR01_MAKE_SUBID(group, num)    ((((group) & 0x7) << 10) | ((num) & 0x3FF))
#define LTR01_MAKE_SUBID_GENERAL(num)   LTR01_MAKE_SUBID(LTR01_SUBID_GROUP_GENERAL, num)
#define LTR01_MAKE_SUBID_LTRK(num)      LTR01_MAKE_SUBID(LTR01_SUBID_GROUP_LTRK, num)

/** Максимальный размер имени дополнительного модуля */
#define LTR01_MODULE_NAME_SIZE          32

/** Коды расширенных идентификаторов, позволяющие определить, какой именно дополнительный
    модуль реально установлен */
typedef enum {
    LTR01_SUBID_INVALID = 0,
    LTR01_SUBID_LTRS511 = LTR01_MAKE_SUBID_GENERAL(1), /**< Идентификатор модуля LTRS511 */
    LTR01_SUBID_LTRS411 = LTR01_MAKE_SUBID_GENERAL(2), /**< Идентификатор модуля LTRS411 */
    LTR01_SUBID_LTRS412 = LTR01_MAKE_SUBID_GENERAL(3), /**< Идентификатор модуля LTRS412 */
    LTR01_SUBID_LTRT10  = LTR01_MAKE_SUBID_GENERAL(4), /**< Идентификатор модуля LTRT10 */
    LTR01_SUBID_LTRT13  = LTR01_MAKE_SUBID_GENERAL(5), /**< Идентификатор модуля LTRT13 */
    LTR01_SUBID_LTRK511 = LTR01_MAKE_SUBID_GENERAL(6), /**< Идентификатор модуля LTRK511 */
    LTR01_SUBID_LTRK416 = LTR01_MAKE_SUBID_GENERAL(7), /**< Идентификатор модуля LTRK416 */
    LTR01_SUBID_LTRK415 = LTR01_MAKE_SUBID_GENERAL(8), /**< Идентификатор модуля LTRK415 */
    LTR01_SUBID_LTRK71  = LTR01_MAKE_SUBID_GENERAL(9), /**< Идентификатор модуля LTRK71 */
} e_LTR01_SUBID;



/*============================================================================*/

/**  @brief Открытие соединения с модулем.

    Функция устанавливает соединение с одним из дополнительных модулей с MID равным 1.
    в соответствии с переданными параметрами, проверяет наличие модуля и при
    необходимости проверяет дополнительный идентификатор.

    После завершения работы необходимо закрыть соединение с помощью LTR01_Close().
 *
 *Установка соедининия
    Выполняет соединение, сброс модуля и проверку, что это именно LTR01.
    @param[in] hnd              Описатель для установки соединения с сервером,
                                предварительно проинициализированный с помощью LTR_Init()
    @param[in] ltrd_addr        IP-адрес машины, на которой запущена служба ltrd, в 32-битном
                                формате (описан в разделе "Формат задания IP-адресов"
                                @docref_ltrapi{руководства для библиотеки ltrapi}).
                                Если служба ltrd запущена на той же машине, что и программа,
                                вызывающая данную функцию, то в качестве адреса
                                можно передать LTRD_ADDR_DEFAULT.
    @param[in] ltrd_port        TCP-порт для подключения к службе ltrd. По умолчанию
                                используется LTRD_PORT_DEFAULT.
    @param[in] csn              Серийный номер крейта, в котором находится интересующий
                                модуль. Представляет собой оканчивающуюся нулем ASCII-строку.
                                Для соединения с первым найденным крейтом можно передать
                                пустую строку или нулевой указатель.
    @param[in] slot_num         Номер слота крейта, в котором установлен интересующий модуль.
                                Значение от LTR_CC_CHNUM_MODULE1 до LTR_CC_CHNUM_MODULE16.
    @param[in] subid            Если не равен #LTR01_SUBID_INVALID, то функция выполняет
                                проверку на соответствия дополнительного идентификатора
                                указанному в данном параметре
    @param[out] cpld_ver        Если не NULL, то в данную переменную сохраняется
                                версия CPLD, полученная в ответе на сброс модуля.
    @param[out] modification    Код модификации модуля (если поддерживается).
                                Возвращается только для subid отличного от
                                #LTR01_SUBID_INVALID.
    @return                     Код ошибки */
LTR01API_DllExport(INT) LTR01_Open(TLTR *hnd, DWORD ltrd_addr, WORD ltrd_port,
                                   const CHAR *csn, INT slot_num, WORD subid,
                                   BYTE *cpld_ver, BYTE *modification);

/** Закрытие ранее открытого с помощью LTR01_Open() соединения.
    @param[in] hnd              Описатель соединения с сервером
    @return                     Код ошибки */
LTR01API_DllExport(INT) LTR01_Close(TLTR *hnd);

/** Получение дополнительного ID для модуля LTR01, с которым была установлена
    до этого связь с помощью LTR01_Open().
    Дополнительный ID позволяет узнать, какой реально модуль дополнительный модуль
    установлен.
    @param[in] hnd              Описатель соединения с сервером.
    @param[out] subid           При успехе возвращается дополнительный ID-модуля
                                из #e_LTR01_SUBID.
    @param[out] modification    Код модификации модуля (если поддерживается).
    @return                     Код ошибки */
LTR01API_DllExport(INT) LTR01_GetModuleSubID(TLTR *hnd, WORD *subid, BYTE *modification);

/** Вспомогательная функция для получения дополнительного ID модуля за один вызов.
    Функция устанавливает соединение с модулем, получает ID модуля и закрывает
    установленное соединение.

    Аналогична последовательности вызовов LTR_Init(), LTR01_Open(),
    LTR01_GetModuleSubID() и LTR01_Close().

    @param[in] ltrd_addr        IP-адрес машины, на которой запущена служба ltrd, в 32-битном
                                формате (описан в разделе "Формат задания IP-адресов"
                                @docref_ltrapi{руководства для библиотеки ltrapi}).
                                Если служба ltrd запущена на той же машине, что и программа,
                                вызывающая данную функцию, то в качестве адреса
                                можно передать LTRD_ADDR_DEFAULT.
    @param[in] ltrd_port        TCP-порт для подключения к службе ltrd. По умолчанию
                                используется LTRD_PORT_DEFAULT.
    @param[in] csn              Серийный номер крейта, в котором находится интересующий
                                модуль. Представляет собой оканчивающуюся нулем ASCII-строку.
                                Для соединения с первым найденным крейтом можно передать
                                пустую строку или нулевой указатель.
    @param[in] slot_num         Номер слота крейта, в котором установлен интересующий модуль.
                                Значение от LTR_CC_CHNUM_MODULE1 до LTR_CC_CHNUM_MODULE16.
    @param[out] subid           При успехе возвращается дополнительный ID-модуля
                                из #e_LTR01_SUBID.
    @param[out] modification    Код модификации модуля (если поддерживается).
    @return                     Код ошибки */
LTR01API_DllExport(INT) LTR01_GetSubID(DWORD ltrd_addr, WORD ltrd_port, const CHAR *csn,
                                       INT slot_num, WORD *subid, BYTE *modification);

/** Получение названия модуля по его дополнительному идентификатору.
    Функция оставлена для совместимости, рекомендуется использовать LTR01_GetModuleNameEx().

    @param[in] subid            Дополнительный идентификатор из #e_LTR01_SUBID.
    @return                     Строка с именем модуля */
LTR01API_DllExport(LPCSTR) LTR01_GetModuleName(WORD subid);

/** Получение полного названия модуля по его дополнительному идентификатору.

    Функция получает полное название модуля с учетом модификации и принадлежности
    к группе. Если был передан идентификатор модуля, по которому библиотека не
    может сформировать название, то будет возвращена ошибка LTR_ERROR_UNKNOWN_MODULE_ID
    и результирующая строка останется без изменений.

    @param[in] subid           Дополнительный идентификатор модуля (может быть получен через LTR01_GetModuleSubID())
    @param[in] modification    Модификация модуля (может быть получена через LTR01_GetModuleSubID())
                                или оставлена равной 0, если нужно получить название без модификации.
    @param[out] module_name    Строка, в которую будет сохранено название типа модуля. Массив должен быть
                                достаточного размера для сохранения по крайней мере #LTR01_MODULE_NAME_SIZE
                                символов.

    @return                    Код ошибки. */
LTR01API_DllExport(INT) LTR01_GetModuleNameEx(WORD subid, BYTE modification, CHAR *module_name);

/*============================================================================*/

#ifdef __cplusplus
}                                          // only need to export C interface if
                                           // used by C++ source code
#endif

#endif                      /* #ifndef LTR11API_H_ */
