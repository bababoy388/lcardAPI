#ifndef LTEDSAPI_LCARD_H
#define LTEDSAPI_LCARD_H

#include "ltedsapi_base.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

#pragma pack(4)

/***************************************************************************//**
  @addtogroup lcard_base_types Типы и определения, специфичные для производителя "Л Кард"
  @{
  *****************************************************************************/


/** Размер в битах идентификатора шаблона компании ООО "Л Кард" */
#define LTEDS_LCARD_TEMPLATE_ID_BITSIZE 8

/** Идентификатор производителя, соответствующий компании ООО "Л Кард" */
#define LTEDS_LCARD_MANUFACTURER_ID   2614


/** @brief Идентификаторы моделей преобразователей компании "Л Кард" в TEDS

   Данное поле определяет, какому типу преобразователя "Л Кард"
   соответствуют данные TEDS.
   В случае многоканальных преобразователей (как LPW25-U-2-230) введены отдельные
   идентификаторы модели для каждого канала для возможности определения,
   какой именно канал преобразователя используется. */
typedef enum {
    /** Первый канал преобразователя LPW25-U-2-230 */
    LTEDS_LCARD_MODEL_ID_LPW25_U_2_230_CH1 = 2500,
    /** Второй канал преобразователя LPW25-U-2-230 */
    LTEDS_LCARD_MODEL_ID_LPW25_U_2_230_CH2 = 2501,
    /** Преобразователь LPW25-I-1-5-1 */
    LTEDS_LCARD_MODEL_ID_LPW25_I_1_5_1     = 2502,
    /** Преобразователь LPW25-I-1-5-2 */
    LTEDS_LCARD_MODEL_ID_LPW25_I_1_5_2     = 2503,
} e_LTEDS_LCARD_MODEL_ID;


/** Идентификаторы шаблонов производителя "Л Кард" */
typedef enum {
    /** Таблица зависимости выходного сопротивления датчика от тока питания
        датчика (раздел @ref lcard_tmpl_curimp). */
    LTEDS_LCARD_TEMPLATE_ID_CUR_IMP_TABLE = 100,
    /** Таблица с фазо частотной характеристикой модуля. Указывает задержку
        фазы относительно линейной для разных частот (раздел @ref lcard_tmpl_phafreqtbl). */
    LTEDS_LCARD_TEMPLATE_ID_PHASE_FREQ_TABLE = 101
} e_LTEDS_LCARD_TEMPLATE_ID;


/***************************************************************************//**
    @brief Проверка, относится ли данный идентификатор производителя к компании "Л Кард".

    Функция принимает значение кода ManufacturerID из TEDS данных и проверяет,
    относится ли данный идентификатор к компании "Л Кард".

    @brief manid Идендификтор производителя, который нужно проверить.
    @return Функция возвращает TRUE, если идентификатор относится к компании "Л Кард",
            иначе --- FALSE.
*******************************************************************************/
LTEDSAPI_DllExport(BOOL) LTEDS_IsLCardManufacturerID(WORD manid);

/** @} */




/***************************************************************************//**
    @addtogroup lcard_tmpl_curimp Шаблон Current Impendance Table (1)
    @{
*******************************************************************************/

/** Максимальное количество элементов в таблице соответствия между
    током питания и выходным сопротивлением */
#define LTEDS_LCARD_INFO_CUR_IMP_DATASET_MAX  127

/** @brief Соответствие сопротивления току

    Структура задает соответствие между определенным током питания преобразователя
    и его выходным сопротивлением. */
typedef struct {
    double SourceCurrent; /**< Значение источника тока в Амперах */
    double OutputImpedance; /**< Значение выходного сопротивления в Омах */
} TLTEDS_LCARD_INFO_CUR_IMP_DATASET;


/** @brief Информация зависимости выходного сопротивления датчика от тока питания
        датчика

   Структура содержит информацию из шаблона "Л Кард" с идентификатором
   #LTEDS_LCARD_TEMPLATE_ID_CUR_IMP_TABLE.

    В структуре содержатся значения выходного сопротивления преобразователей
    для различных значений тока питания преобразователя.

    Для получения используется функция LTEDS_GetLCardTmpltInfoCurImpTable(). */
typedef struct {
    BYTE PrecFormat; /**< Тип формата (поддерживается только 0) */
    BYTE DataSetCount; /**< Количество точек соответствия сопротивления и тока питания. */
    /** Набор точек соответствия сопротивления и тока питания. Действительны первые
       [DataSetCount](@ref TLTEDS_LCARD_INFO_TMPLT_CUR_IMP::DataSetCount) элементов. */
    TLTEDS_LCARD_INFO_CUR_IMP_DATASET DataSets[LTEDS_LCARD_INFO_CUR_IMP_DATASET_MAX];
} TLTEDS_LCARD_INFO_TMPLT_CUR_IMP;


/***************************************************************************//**
    @brief Извлечение таблицы соответствия сопротивления току из данных TEDS

    Функция позволяет получить из TEDS информацию, соответствующую шаблону
    "Л Кард" с идентификатором #LTEDS_LCARD_TEMPLATE_ID_CUR_IMP_TABLE.

    Информация возвращается в виде структуры типа #TLTEDS_LCARD_INFO_TMPLT_CUR_IMP.

    @param[in]   ctx      Контекст разбора данных TEDS.
    @param[out]  info     Структура, в которой возвращается полученная информация.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_GetLCardTmpltInfoCurImpTable(
        TLTEDS_DECODE_CONTEXT *ctx, TLTEDS_LCARD_INFO_TMPLT_CUR_IMP *info);

/***************************************************************************//**
    @brief Запись таблицы соответствия сопротивления току в данные TEDS

    Функция позволяет закодировать в TEDS информацию, соответствующую шаблону
    "Л Кард" с идентификатором #LTEDS_LCARD_TEMPLATE_ID_CUR_IMP_TABLE.

    @param[in]   ctx      Контекст кодирования данных TEDS.
    @param[in]   info     Структура, содержащая записываемую информацию.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_PutLCardTmpltInfoCurImpTable(
        TLTEDS_ENCODE_CONTEXT *ctx, const TLTEDS_LCARD_INFO_TMPLT_CUR_IMP *info);

/** @} */




/***************************************************************************//**
    @addtogroup lcard_tmpl_phafreqtbl Шаблон Phase Frequency Table (2)
    @{
*******************************************************************************/

/** Максимальное количество элементов в таблице фазо частотной характеристики */
#define LTEDS_LCARD_INFO_PHASE_FREQ_DATASET_MAX 127


/** @brief Соответствие смещения фазы частоте

    Структура задает смещение фазы, вносимое преобразователем, для заданного
    значения частоты сигнала. */
typedef struct {
    double Frequency; /**< Частота сигнала в Гц */
    double PhaseShift; /**< Сдвиг фазы в градусах */
} TLTEDS_LCARD_INFO_PHASE_FREQ_DATASET;


/** @brief Информация о фазо частотной характеристике преобразователя

   Структура содержит информацию из шаблона "Л Кард" с идентификатором
   #LTEDS_LCARD_TEMPLATE_ID_PHASE_FREQ_TABLE.

    В структуре представлен набор точек, определяющих сдвиг фазы, вносимый
    преобразователем, для указанных значений частот сигнала.

    Для получения используется функция LTEDS_GetLCardTmpltInfoPhaseFreqTable(). */
typedef struct {
    BYTE DataSetCount; /**< Количество точек соответствия частоты и фазы. */
    /** Набор точек соответствия частоты и фазы. Действительны первые
       [DataSetCount](@ref TLTEDS_LCARD_INFO_TMPLT_PHASE_FREQ::DataSetCount) элементов. */
    TLTEDS_LCARD_INFO_PHASE_FREQ_DATASET DataSets[LTEDS_LCARD_INFO_PHASE_FREQ_DATASET_MAX];
} TLTEDS_LCARD_INFO_TMPLT_PHASE_FREQ;

/***************************************************************************//**
    @brief Извлечение таблицы с фазо частотной характеристикой из данных TEDS

    Функция позволяет получить из TEDS информацию, соответствующую шаблону
    "Л Кард" с идентификатором #LTEDS_LCARD_TEMPLATE_ID_PHASE_FREQ_TABLE.

    Информация возвращается в виде структуры типа #TLTEDS_LCARD_INFO_TMPLT_PHASE_FREQ.

    @param[in]   ctx      Контекст разбора данных TEDS.
    @param[out]  info     Структура, в которой возвращается полученная информация.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_GetLCardTmpltInfoPhaseFreqTable(
        TLTEDS_DECODE_CONTEXT *ctx, TLTEDS_LCARD_INFO_TMPLT_PHASE_FREQ *info);


/***************************************************************************//**
    @brief Запись таблицы с фазо частотной характеристикой в данные TEDS

    Функция позволяет закодировать в TEDS информацию, соответствующую шаблону
    "Л Кард" с идентификатором #LTEDS_LCARD_TEMPLATE_ID_PHASE_FREQ_TABLE.

    @param[in]   ctx      Контекст кодирования данных TEDS.
    @param[in]   info     Структура, содержащая записываемую информацию.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_PutLCardTmpltInfoPhaseFreqTable(
        TLTEDS_ENCODE_CONTEXT *ctx, const TLTEDS_LCARD_INFO_TMPLT_PHASE_FREQ *info);


/** @} */

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif // LTEDSAPI_LCARD_H
