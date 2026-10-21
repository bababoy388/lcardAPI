#ifndef LTRAPITYPES_H_
#define LTRAPITYPES_H_


#include "lwintypes.h"
#include "ltrapidefine.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @cond obsolete */


#define LTR_DESCR_NAME_LEN              (16)
#define LTR_DESCR_SERIAL_LEN            (16)
#define LTR_DESCR_COMMENT_LEN           (256)
#define LTR_DESCR_MODULE_COMMENT_LEN    (LTR_DESCR_COMMENT_LEN-2)
#define LTR_DESCR_ADC_CALIB_CNT         (256)
#define LTR_DESCR_DAC_CALIB_CNT         (256)
#define LTR_DESCR_MEZ_CALIB_CNT         (4)


#pragma pack(4)


typedef struct {
    INT code;
    LPCSTR message;
} TLTR_ERROR_STRING_DEF;


/* описание модуля */
typedef struct {
    BYTE CompanyName[LTR_DESCR_NAME_LEN];
    BYTE DeviceName[LTR_DESCR_NAME_LEN];                   /* название изделия */
    BYTE SerialNumber[LTR_DESCR_SERIAL_LEN];               /* серийный номер изделия */
    BYTE Revision;                                         /* ревизия изделия */
    BYTE Comment[LTR_DESCR_COMMENT_LEN - 2];
    BYTE VerPLD;                                           /* версия CPLD (если присутствует) */
    BYTE VerPLDIsValid;                                      /* признак действительности версии CPLD */
} TLTR_DESCRIPTION_MODULE;
/* описание процессора и програмного обеспечения */
typedef struct {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                                         /* название */
    double ClockRate;
    DWORD FirmwareVersion;
    BYTE Comment[LTR_DESCR_COMMENT_LEN];
} TLTR_DESCRIPTION_CPU;
/* описание ПЛИС */
typedef struct {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                                         /* название */
    double ClockRate;
    DWORD FirmwareVersion;
    BYTE Comment[LTR_DESCR_COMMENT_LEN];
} TLTR_DESCRIPTION_FPGA;
/* описание АЦП */
typedef struct {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                                         /* название */
    double Calibration[LTR_DESCR_ADC_CALIB_CNT];            /* корректировочные коэффициенты */
    BYTE Comment[LTR_DESCR_COMMENT_LEN];
} TLTR_DESCRIPTION_ADC;
/* описание ЦАП */
typedef struct {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                                         /* название */
    double Calibration[LTR_DESCR_DAC_CALIB_CNT];            /* корректировочные коэффициенты */
    BYTE Comment[LTR_DESCR_COMMENT_LEN];
} TLTR_DESCRIPTION_DAC ;
/* описание h-мезонинов */
typedef struct {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                         /* название изделия */
    BYTE SerialNumber[LTR_DESCR_SERIAL_LEN];               /* серийный номер изделия */
    BYTE Revision;                                         /* ревизия изделия */
    double Calibration[LTR_DESCR_MEZ_CALIB_CNT];           /* корректировочные коэффициенты */
    BYTE Comment[LTR_DESCR_COMMENT_LEN];                   /* комментарий */
} TLTR_DESCRIPTION_MEZZANINE;
/* описание цифрового в/в */
typedef struct {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                                         /* название */
    WORD InChannels;                                       /* число каналов */
    WORD OutChannels;                                      /* число каналов */
    BYTE Comment[LTR_DESCR_COMMENT_LEN];
} TLTR_DESCRIPTION_DIGITAL_IO;
/* описание интерфейсных модулей */
typedef struct _DESCRIPTION_INTERFACE_ {
    BYTE Active;                                           /* флаг достоверности остальных полей
                                                            * структуры
                                                            */
    BYTE Name[LTR_DESCR_NAME_LEN];                                         /* название */
    BYTE Comment[LTR_DESCR_COMMENT_LEN];
} TLTR_DESCRIPTION_INTERFACE;




/** @endcond */

/***************************************************************************//**
  @addtogroup type_list
  @{
  *****************************************************************************/

/** @brief Информация о типе и интерфейсе подключения крейта.

    Данная структура заполняется функцией LTR_GetCrateInfo() и содержит информацию
    о типе крейта и интерфейсе его подключения.
 */
typedef struct {
    BYTE CrateType;                      /**< Тип крейта --- значение из #en_LTR_CrateTypes */
    BYTE CrateInterface;                 /**< Интерфейс подключения крейта ---
                                              значение из #en_LTR_CrateIface */
} TLTR_CRATE_INFO;


/** @brief Запись с IP-адресом крейта

   Данная структура содержит информацию о записи об IP-адресе крейта, сохраненную
   в настройках службы ltrd, а также состояние подключения крейта по интерфейсу
   Ethernet (TCP/IP), соответствующего данной записи */
typedef struct {
    DWORD ip_addr;                             /**< IP-адрес крейта. Формат аналогичен полю
                                                    saddr в #TLTR */
    DWORD flags;                               /**< Набор флагов, связанных с данной записью,
                                                    из #en_LTR_CrateIpFlags */
    CHAR serial_number[LTR_CRATE_SERIAL_SIZE]; /**< Если крейт подключен, то
                                                    в данном поле содержится серийный номер
                                                    подключенного крейта. Этот
                                                    номер можно использовать для
                                                    открытия соединения с крейтом.
                                                    Для остальных значений status данное
                                                    поле содержит пустую строку, т.к.
                                                    серийный номер неизвестен */
    BYTE is_dynamic;      /**< Резервное поле. Всегда равно 0 */
    BYTE status;          /**< Состояние подключения крейта, соответствующего
                               данной записи. Одно значение из #en_LTR_CrateIpStatus */
} TLTR_CRATE_IP_ENTRY;

/** @brief Статистика крейта

    Структура содержит информацию о состоянии крейта и параметры статистики
    работы с крейтом, которую ведет служба ltrd.
    Данная статистика может быть получена через управляющее соединение с помощью
    функции LTR_GetCrateStatistic().
    Статистика ведется с момента установления связи между ltrd и крейтом. */
typedef struct {
    DWORD size;                             /**< Размер всех действительных полей
                                                 структуры, включая само поле size */
    DWORD flags;                            /**< Флаги --- резерв */
    WORD crate_type;                        /**< Тип крейта из #en_LTR_CrateTypes */
    WORD crate_intf;                        /**< Интерфейс, по которому подключен
                                                 крейт из #en_LTR_CrateIface */
    WORD crate_state;                       /**< Резерв */
    WORD crate_mode;                        /**< Режим работы крейта из #en_LTR_CrateMode */
    ULONGLONG con_time;                     /**< Время установления соединения службы
                                                 с крейтом (формат unixtime) */
    WORD res[11];                           /**< Резерв */
    WORD modules_cnt;                       /**< Количество слотов в данном типе крейта */
    WORD mids[LTR_MODULES_PER_CRATE_MAX];   /**< ID-модулей для всех слотов крейта */
    WORD res2[3*LTR_MODULES_PER_CRATE_MAX]; /**< Резерв */
    WORD ctl_clients_cnt;                   /**< Количество клиентов, подключенных по
                                                 управляющему каналу к крейту */
    WORD total_mod_clients_cnt;             /**< Количество клиентов, подключенных ко
                                                 всем модулям крейта */
    DWORD res3[11];                         /**< Резерв */

    ULONGLONG wrd_sent;                     /**< Общее количество слов, переданных в
                                                 крейт (крейту и всем его модулям) */
    ULONGLONG wrd_recv;                     /**< Общее количество слов, принятое от
                                                 крейта (от самого крейта и всех его модулей) */
    double bw_send;                         /**< Текущая скорость передачи слов в крейт (слов/с) */
    double bw_recv;                         /**< Текущая скорость приема слов из крейта (слов/c) */
    ULONGLONG crate_wrd_recv;               /**< Количество принятых слов
                                                 непосредственно от крейта */
    ULONGLONG internal_rbuf_miss;            /**< Количество потерянных буферов в крейте
                                                  из-за внутреннего переполнения */
    DWORD internal_rbuf_ovfls;               /**< Количество переполнений внутреннго
                                                   буфера крейта */
    DWORD rbuf_ovfls;                        /**< Количество переполнений буфера
                                                  прима данных от модулей в службе
                                                  ltrd для модулей крейта (суммарное
                                                  по всем модулям) */
    DWORD total_start_marks;                 /**< Количество принятых меток "Старт" как от крейта,
                                                  так и от модулей */
    DWORD total_sec_marks;                   /**< Количество принятых секундных меток как от крейта,
                                                  так и от модулей */
    DWORD crate_start_marks;                 /**< Количество принятых меток "Старт"
                                                  непосредственно от крейта */
    DWORD crate_sec_marks;                   /**< Количество принятых секундных меток
                                                  непосредственно от крейта */
    ULONGLONG crate_unixtime;                /**< Последнее значение расширенной
                                                  секундной метки (формат unixtime),
                                                  если поддерживается крейтом */
    DWORD therm_mask;                        /**< Маска действительных показаний термометров
                                                  (если не поддерживается --- 0) */
    float therm_vals[LTR_CRATE_THERM_MAX_CNT]; /**< значение показаний термометров крейта. действительны
                                                    только если соответствующий бит в therm_mask в 1 */
    DWORD res4[19];                          /**< Резерв */
} TLTR_CRATE_STATISTIC;

/** @brief Статистика модуля

    Структура содержит информацию о состоянии модуля и параметры статистики
    работы с данным модулем, которую ведет служба ltrd.
    Данная статистика может быть получена через управляющее соединение с помощью
    функции LTR_GetModuleStatistic().
    Статистика ведется с момента обнаружения модуля и сбрасывается вместе со
    сбросом модуля через LTR_ResetModule() */
typedef struct {
    DWORD size;                             /**< Размер всех действительных полей
                                                 структуры, включая само поле size */
    WORD client_cnt;                        /**< Количество клиентов, установивших соединение
                                                 с модулем */
    WORD mid;                               /**< Идентификатор модуля из #en_LTR_MIDs */
    DWORD flags;                            /**< Набор флагов, описывающих особенности
                                                 модуля, из #en_LTR_ModuleDescrFlags */
    CHAR  name[LTR_MODULE_NAME_SIZE];       /**< Строка с названием модуля (возможно с модификациями,
                                                  если ltrd способен их определить) */
    DWORD res[5];                           /**< Резерв */
    ULONGLONG wrd_sent;                     /**< Количество слов, переданных модулю */
    ULONGLONG wrd_recv;                     /**< Количество слов, принятых от модуля */
    double bw_send;                         /**< Текущая скорость передачи слов в модуль (слов/с) */
    double bw_recv;                         /**< Текущая скорость приема слов из модуля (слов/с) */
    ULONGLONG wrd_sent_to_client;           /**< Количество слов, переданных клиенту */
    ULONGLONG wrd_recv_from_client;         /**< Количество слов, принятых от клиента */
    ULONGLONG wrd_recv_drop;                /**< Количество отброшенных слов из-за переполнения
                                                 буфера на прием в службе ltrd */
    DWORD rbuf_ovfls;                       /**< Количество переполнений буфера на прием
                                                 в службе ltrd */
    DWORD send_srvbuf_size;                 /**< Размер буфера в ltrd для модуля на передачу */
    DWORD recv_srvbuf_size;                 /**< Размер буфера в ltrd для модуля на прием */
    DWORD send_srvbuf_full;                 /**< На сколько слов заполнен буфер на передачу */
    DWORD recv_srvbuf_full;                 /**< На сколько слов заполнен буфер на прием */
    DWORD send_srvbuf_full_max;             /**< На сколько слов был максимально заполнен буфер на передачу */
    DWORD recv_srvbuf_full_max;             /**< На сколько слов был максимально заполнен буфер на прием */
    DWORD res2[17];                         /**< Резерв */
    DWORD start_mark;                       /**< Количество принятых меток "СТАРТ" от модуля */
    DWORD sec_mark;                         /**< Количество принятых секундных меток от модуля */
    /* Поля, описывающие состояние внутренней аппаратной очереди модуля.
     * Действительны, только если есть флаг
     */
    DWORD hard_send_fifo_size;              /**< Размер аппаратной очереди внутри модуля. Данное поле, а также
                                                 все остальные поля hard_send_... действительны только
                                                 для модулей вывода с наличием контролируемой ltrd очереди
                                                 в модуле (для этих модулей также установлен соответствующий
                                                 флаг в поле flags) */
    DWORD hard_send_fifo_unack_words;       /**< Заполненность аппаратной очереди (количество отправленных,
                                                 но не подтвержденных слов) */
    DWORD hard_send_fifo_underrun;          /**< Количество "голоданий" очереди (очередь пуста
                                                 при попытке вывода значения из нее) с момента
                                                 последнего сброса модуля */
    DWORD hard_send_fifo_overrun;           /**< Количество переполнений очереди с момента
                                                 последнего сброса модуля*/
    DWORD hard_send_fifo_internal;          /**< Внутренне состояние аппаратной очереди */
    DWORD res3[25];                         /**< Резерв */
} TLTR_MODULE_STATISTIC;

/** @brief Информация о крейте и его прошивке

    Структура содержит информацию о крейте, включая все версии, связанные с крейтом.
    Большинство полей представлены в виде окончивающейся нулевым символом строки.

    Данная информация может быть получена Данная статистика может быть получена
      через управляющее соединение с помощью LTR_GetCrateDescr().

    */
typedef struct {
    DWORD size;                                            /**< Размер всех действительных полей
                                                                структуры, включая само поле size */
    char devname[LTR_CRATE_DEVNAME_SIZE];                  /**< Название крейта */
    char serial[LTR_CRATE_SERIAL_SIZE];                    /**< Серийный номер */
    char soft_ver[LTR_CRATE_SOFTVER_SIZE];                 /**< Версия прошивки */
    char brd_revision[LTR_CRATE_REVISION_SIZE];            /**< Ревизия платы */
    char brd_opts[LTR_CRATE_BOARD_OPTIONS_SIZE];           /**< Опции платы */
    char bootloader_ver[LTR_CRATE_BOOTVER_SIZE];           /**< Версия загрузчика */
    char cpu_type[LTR_CRATE_CPUTYPE_SIZE];                 /**< Тип микроконтроллера */
    char fpga_name[LTR_CRATE_FPGA_NAME_SIZE];              /**< Название ПЛИС в крейте */
    char fpga_version[LTR_CRATE_FPGA_VERSION_SIZE];        /**< Версия прошивки ПЛИС */
    char crate_type_name[LTR_CRATE_TYPE_NAME];             /**< Строка с типом крейта */
    char spec_info[LTR_CRATE_SPECINFO_SIZE];               /**< Резерв */
    BYTE protocol_ver_major;                               /**< Версия протокола между
                                                                ltrd и крейтом (мажорная) */
    BYTE protocol_ver_minor;                               /**< Версия протокола между
                                                                ltrd и крейтом (минорная) */
    /** @cond kd_extension */
    BYTE slots_config_ver_major;                           /**< Версия протокола для сохранения
                                                                настроек модуля (мажорная) */
    BYTE slots_config_ver_minor;                           /**< Версия протокола для сохранения
                                                                настроек модуля (минорная) */
    /** @endcond */
} TLTR_CRATE_DESCR;

#pragma pack()


/** @cond kd_extension */
typedef INT TLTR_CARD_START_MODE;
/** @endcond */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /*#ifndef LTRAPITYPES_H_*/
