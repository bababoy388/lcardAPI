/** \mainpage
 *
 */
 
#ifndef LTR032API_H
#define LTR032API_H

#include "ltrapitypes.h"


#ifdef _WIN32
    #ifdef LTR032API_EXPORTS
        #define LTR032API_DllExport(type)    __declspec(dllexport) type APIENTRY
    #else
        #define LTR032API_DllExport(type)    __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR032API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR032API_DllExport(type) type
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \cond */
/** Версия библиотеки для работы с крейт-контроллером LTR032. */
#define LTR032_CURR_VERS_MAJOR      1
#define LTR032_CURR_VERS_MINOR      0
#define LTR032_CURR_VERS_BUILD      0
#define LTR032_CURR_VERS_CODE       0x01000000UL
/** \endcond */

/** \defgroup namur_default Значения токов по умолчанию для порогов NAMUR
 * @{
 */
/** Значение тока по умолчанию для нижнего порога NAMUR */
#define LTR032_NAMUR_DEFAULT_LOWER  1.3146F
/** Значение тока по умолчанию для верхнего порога NAMUR */
#define LTR032_NAMUR_DEFAULT_UPPER  1.9466F
/** @} */

/** \brief Коды ошибок
 *  \anchor ltr032_errors
 */
enum {
    /** Некорретный тип крейт-контроллера */
    LTR032_ERR_INVAL_CRATE_TYPE         = -10304,
    /** Команда отклонена крейт-контроллером */
    LTR032_ERR_CMD_REJECTED             = -10305,
    /** Неверно задан режим работы выхода M1S OUT */
    LTR032_ERR_INVALID_M1S_OUT_MODE     = -10306,
    /** Неверно заданы пороговые уровни NAMUR */
    LTR032_ERR_INVALID_NAMUR_LEVELS     = -10307,
    /** Неверно задан индекс термодатчика */
    LTR032_ERR_INVALID_THERM_SENS_IND   = -10308
};

/** \brief Индексы температурных сенсоров
 *  \anchor ltr032_temp_sens
 */
enum {
    /** Нижний температурный сенсор */
    LTR032_TEMP_SENS_LOWER,
    /** Верхний температурный сенсор */
    LTR032_TEMP_SENS_UPPER
};

/** \brief Режимы работы выхода M1S_OUT (выхода секундной метки, оптореле)
 *  \anchor ltr032_m1sout_modes
 */
enum {
    /** Оптореле разомкнуто */
    LTR032_M1SOUT_MODE_OPEN=0,
    /** Оптореле замкнуто */
    LTR032_M1SOUT_MODE_CLOSE=1,
    /** Оптореле замыкается на 100 мс в момент смены секунды */
    LTR032_M1SOUT_MODE_CLOSE_PULSE=2,
    /** Оптореле размыкается на 100 мс в момент смены секунды */
    LTR032_M1SOUT_MODE_OPEN_PULSE = 4
};

/** Описатель интерфейса с крейт-контроллером. */
struct ltr032;

/** Возвращает текущую версию библиотеки. */
LTR032API_DllExport(DWORD) ltr032_get_version(void);

/** \brief Создает экземпляр контекста библиотеки
 *  
 *  Полученный контекст передается в большинство функций библиотеки.
 *  В нем хранится текущее состояние модуля, настройки, канал связи и прочее.
 *  
 *  После завершения использования контекста, его необходимо уничтожить
 *  с помощью функции \ref ltr032_exit.
 *  \return Экземпляр контекста библиотеки или NULL, если недостаточно памяти
 */
LTR032API_DllExport(struct ltr032 *) ltr032_init(void);

/** \brief Уничтожает контекст библиотеки
 *  
 *  Освобождает ресурсы, выделенные для хранения контекста библиотеки.
 *  Если канал связи с крейт-контроллером не был закрыт, то он корректно
 *  закрывается. Дальнейшее использование данного контекста библиотеки
 *  недопустимо.
 *  \param ltr032 Контекст
 */
LTR032API_DllExport(void) ltr032_exit(struct ltr032 *ltr032);

/** \brief Открывает канал связи с крейт-контроллером
 *
 *  Соединение осуществляется через LTR сервер, запущенный на хосте с
 *  IP адресом \a ip_addr и слушающий TCP-порт \a port. Конкретный крейт
 *  выбирается по серийному номеру \a serial.
 *
 *  Если \a serial равен NULL или
 *  пустой строке, тогда открывается первый крейт-контроллер в списке LTR
 *  сервера. В качестве IP адреса и номера порта можно использовать константы
 *  SADDR_DEFAULT и SPORT_DEFAULT, что соответствует локальной машине и порту
 *  по умолчанию. В IP адресе байты идут от старшего к младшему
 *  (например, 127.0.0.1 соответствует 0x7F000001UL).
 *
 *  После завершения работы с крейт-контроллером необходимо закрыть канал связи
 *  с помощью функции \ref ltr032_close.
 *  \param ltr032 Контекст
 *  \param ip_addr IP адрес хоста с LTR сервером
 *  \param port TCP порт, прослушиваемый LTR сервером
 *  \param serial Серийный номер крейт-контроллера
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_open(struct ltr032 *ltr032, DWORD ip_addr, WORD port,
                              const char *serial);

/** \brief Проверяет, открыт ли канал связи с крейт-контроллером
 *  \param ltr032 Контекст
 *  \return 0, если канал связи закрыт, или не 0, если открыт
 */
LTR032API_DllExport(INT) ltr032_is_open(struct ltr032 *ltr032);

/** \brief Закрывает канал связи с крейт-контроллером
 *  \param ltr032 Контекст
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_close(struct ltr032 *ltr032);

/** \brief Устанавливает режим работы выхода M1S_OUT
 *
 *  Код режима работы выбирается с помощью констант \ref ltr032_m1sout_modes
 *  "LTR032_M1SOUT_MODE_*".
 *  \param ltr032 Контекст
 *  \param mode Код режим работы
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_set_m1sout_mode(struct ltr032 *ltr032, BYTE mode);

/** \brief Считывает режим работы выхода M1S_OUT
 *
 *  Возвращает текущий режим работы выхода M1S_OUT. Доступные режимы работы
 *  определяются константами \ref ltr032_m1sout_modes "LTR032_M1SOUT_MODE_*".
 *  \param ltr032 Контекст
 *  \param mode Указатель на код режима работы
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_get_m1sout_mode(struct ltr032 *ltr032, BYTE *mode);

/** \brief Изменяет пороговые значения NAMUR для входа START_IN
 *
 *  Значения пороговых уровней указываются в миллиамперах (мА). Можно
 *  использовать константы значений уровней по умолчанию \ref namur_default
 *  "LTR032_NAMUR_DEFAULT_*".
 *  \param ltr032 Контекст
 *  \param lower Значение тока нижнего порога
 *  \param upper Значение тока верхнего порога
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_set_startin_namur(struct ltr032 *ltr032, float lower, float upper);

/** \brief Считывает пороговые значения NAMUR для входа START_IN
 *
 *  Значения пороговых уровней измеряются в миллиамперах (мА).
 *  \param ltr032 Контекст
 *  \param lower Указатель на значение тока нижнего порога
 *  \param upper Указатель на значение тока верхнего порога
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_get_startin_namur(struct ltr032 *ltr032, float *lower, float *upper);

/** \brief Считывает текущую температуру с термодатчика
 *  
 *  Термодатчик выбирается с помощью своего индекса \a sens. Индексы
 *  термодатчиков определяются константами \ref ltr032_temp_sens
 *  "LTR032_TEMP_SENS_*". Температура измеряется в градусах по Цельсию.
 *  \param ltr032 Контекст
 *  \param sens Индекс термодатчика
 *  \param temp Указатель на значение текущей температуры
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_get_temperature(struct ltr032 *ltr032, BYTE sens, float *temp);

/** \brief Считывает текущее значение напряжения питания
 *  
 *  Напряжение питания измеряется в вольтах (В).
 *  \param ltr032 Контекст
 *  \param volt Указатель на значение напряжения питания
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_get_supply_voltage(struct ltr032 *ltr032, float *volt);

/** \brief Устанавливает текущее время в крейт-контроллере
 *
 *  Для представления времени испльзуется 64-битный
 *  <a href=http://ru.wikipedia.org/wiki/UNIX-%D0%B2%D1%80%D0%B5%D0%BC%D1%8F>
 *  POSIX формат</a>.
 *  \param ltr032 Контекст
 *  \param ptime Значение времени
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_set_time(struct ltr032 *ltr032, ULONGLONG ptime);

/** \brief Считывает текущее время в крейт-контроллере
 *
 *  Для представления времени испльзуется 64-битный
 *  <a href=http://ru.wikipedia.org/wiki/UNIX-%D0%B2%D1%80%D0%B5%D0%BC%D1%8F>
 *  POSIX формат</a>.
 *  \param ltr032 Контекст
 *  \param ptime Указатель на значение времени
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_get_time(struct ltr032 *ltr032, ULONGLONG *ptime);

/** \brief Возвращает текстовое описание последней ошибки
 *  \param ltr032 Контекст
 *  \return Указатель на текстовое описание или NULL в случае ошибки
 */
LTR032API_DllExport(const char *) ltr032_get_error_string(struct ltr032 *ltr032);

/** \brief Возвращает указатель на старый интерфейс
 *
 *  Старый интерфейс -- интерфейс библиотеки LTRAPI, структура TLTR,
 *  представляющая собой канал связи с крейт-контроллером через LTR сервер.
 *
 *  Изменять структуру и использовать ее в функциях LTRAPI нежелательно и может
 *  привести к ошибкам.
 *  \param ltr032 Контекст
 *  \param ltr Указатель на структуру TLTR, которая будет проинициализирована
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_get_legacy_interface(struct ltr032 *ltr032, void *ltr);

/** \brief Выполняет полный аппаратный сброс крейт-контроллера
 *
 *  Аппаратный сброс приводит к перезагрузке крейт-контроллера,
 *  переинициализации всех систем, проверке целостности прошивки.
 *  Соединение с крейт-контроллером обрывается с ошибкой.
 *  \param ltr032 Контекст
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_reset(struct ltr032 *ltr032);

/** \brief Выполняет переход из основной прошивки в загрузчик
 *
 *  Переход осуществляется через полный аппаратный сброс крейт-контроллера с
 *  флагом запуска загрузчика. Соединение с крейт-контроллером обрывается с
 *  ошибкой.
 *  \param ltr032 Контекст
 *  \return 0 или код ошибки в случае ошибки
 */
LTR032API_DllExport(INT) ltr032_start_boot(struct ltr032 *ltr032);

#ifdef __cplusplus
}
#endif

#endif
