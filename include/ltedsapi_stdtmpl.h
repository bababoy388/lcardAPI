#ifndef LTEDSAPI_STDTMPL_H
#define LTEDSAPI_STDTMPL_H

#include "ltedsapi_base.h"

#ifdef __cplusplus
extern "C" {                                 // only need to export C interface if
                                             // used by C++ source code
#endif

#pragma pack(4)

/***************************************************************************//**
  @addtogroup std_tmpl_general Типы данных для представления общей информации
                               из стандартных шаблонов
  @{
  *****************************************************************************/

/** @brief Базовая информация о выполнении калибровки

   Структура содержит информацию о последней проведенной калибровке
   и периоде между их проведениями.
   Входит в большинство стандартных шаблонов, описывающих датчик. */
typedef struct {
    TLTEDS_INFO_DATE Date; /**< Дата проведения калибровки */
    WORD PeriodDays;   /**< Период между проведением калибровки в днях */
    CHAR  Initials[4];  /**< Инициалы проводящего калибровку. 3 заглавные английские 
                                                   буквы с завершающим нулевым символом */
} TLTEDS_INFO_BASE_CAL;

/** @brief Измеряемая физическая величина.

  Перечисление используются в нескольких стандартных шаблонах для указания,
  какую физическую величину измеряет датчик, а также в какой размерности
  заданы и предпочтительно отображать ее значения */
typedef  enum {
    LTEDS_PHYSMEAS_TEMP_K           = 0, /**< Температура в Кельвинах */
    LTEDS_PHYSMEAS_TEMP_CELSIUS     = 1, /**< Температура в градусах Цельсия (K - 273.15) */
    LTEDS_PHYSMEAS_STRAIN           = 2, /**< Деформация, натяжение (м/м) */
    LTEDS_PHYSMEAS_MICROSTRAIN      = 3, /**< Микродеформация, микронатяжение */
    LTEDS_PHYSMEAS_FORCE_WEIGHT_N   = 4, /**< Сила или вес в Ньютонах (Н) */
    LTEDS_PHYSMEAS_FORCE_WEIGHT_LB  = 5, /**< Сила или вес в фунтах (4.44822 Ньютонов) */
    LTEDS_PHYSMEAS_FORCE_WEIGHT_KGF = 6, /**< Сила или вес в кгс (килограмм-сила) */
    LTEDS_PHYSMEAS_ACCEL_M_S2       = 7, /**< Ускорение в \f$ \textup{м}/\textup{с}^2 \f$ */
    LTEDS_PHYSMEAS_ACCEL_G          = 8, /**< Ускорение в g (9.80665 \f$\textup{м}/\textup{с}^2\f$) */
    LTEDS_PHYSMEAS_TORQUE_NM_RADIAN = 9, /**< Крутящий момент в \f$ \textup{Н}*\textup{м}/\textup{рад} \f$ */
    LTEDS_PHYSMEAS_TORQUE_NM        = 10,/**< Крутящий момент в \f$ \textup{Н}*\textup{м} \f$
                                             (без указания радиан в аббревиатуре) */
    LTEDS_PHYSMEAS_TORQUE_OZ_IN     = 11,/**< Крутящий момент в "oz-in" (0.00706155 Н*м) */
    LTEDS_PHYSMEAS_PRESSURE_PASCAL  = 12,/**< Давление в Паскалях */
    LTEDS_PHYSMEAS_PRESSURE_PSI     = 13,/**< Давление в фунтах на квадратный
                                              дюйм (6894.757 Паскалей) */
    LTEDS_PHYSMEAS_MASS_KG          = 14,/**< Масса в килограммах */
    LTEDS_PHYSMEAS_MASS_G           = 15,/**< Масса в граммах */
    LTEDS_PHYSMEAS_DISTANCE_M       = 16,/**< Дистанция в метрах */
    LTEDS_PHYSMEAS_DISTANCE_MM      = 17,/**< Дистанция в миллиметрах */
    LTEDS_PHYSMEAS_DISTANCE_INCHES  = 18,/**< Дистанция в дюймах (0.0254 метра) */
    LTEDS_PHYSMEAS_VELOCITY_M_S     = 19,/**< Скорость в метрах в секунду */
    LTEDS_PHYSMEAS_VELOCITY_MPH     = 20,/**< Скорость в милях в час (0.44704 м/с) */
    LTEDS_PHYSMEAS_VELOCITY_FPS     = 21,/**< Скорость в футах в секунду (0.3048 м/с) */
    LTEDS_PHYSMEAS_ANGPOS_RADIAN    = 22,/**< Угол в радианах */
    LTEDS_PHYSMEAS_ANGPOS_DEGREES   = 23,/**< Угол в градусах (0.0174533 радиан) */
    LTEDS_PHYSMEAS_ROT_VEL_RADIAN_S = 24,/**< Угловая скорость в радианах в секунду */
    LTEDS_PHYSMEAS_ROT_VEL_RPM      = 25,/**< Угловая скорость в оборотах в минуту
                                           (0.104720 радиан в секунду) */
    LTEDS_PHYSMEAS_FREQ_HZ          = 26,/**< Частота в Гц */
    LTEDS_PHYSMEAS_CONCENT_KG_M3    = 27,/**< Плотность в \f$ \textup{кг}/\textup{м}^3 \f$ */
    LTEDS_PHYSMEAS_CONCENT_UNIT_G_L = 28,/**< Плотность в \f$ \textup{г}/\textup{л}^3 \f$
                                            (эквивалентно \f$ \textup{кг}/\textup{м}^3\f$)*/
    LTEDS_PHYSMEAS_CONCENT_MOLE_M3  = 29,/**< Молярная концентрация в \f$ \textup{моль}/\textup{м}^3 \f$ */
    LTEDS_PHYSMEAS_CONCENT_MOLE_L   = 30,/**< Моль на литр (\f$1000 *\textup{моль}/\textup{м}^3 \f$) */
    LTEDS_PHYSMEAS_VOL_CONCENT_M3_M3= 31,/**< Молярная объемная концентрация в
                                            \f$ \textup{м}^3/\textup{м}^3 \f$ */
    LTEDS_PHYSMEAS_VOL_CONCENT_L_L  = 32,/**< Молярная объемная концентрация в литрах
                                          на литр */
    LTEDS_PHYSMEAS_MASS_FLOW_KG_S   = 33,/**< Массовый расход в кг/с */
    LTEDS_PHYSMEAS_VOL_FLOW_M3_S    = 34,/**< Объемный расход в \f$ \textup{м}^3/\textup{c} \f$ */
    LTEDS_PHYSMEAS_VOL_FLOW_M3_HR   = 35,/**< Объемный расход в \f$ \textup{м}^3\f$ в час */
    LTEDS_PHYSMEAS_VOL_FLOW_GPM     = 36,/**< Объемный расход в галлонах в минуту
                                          = \f$ 6.30902e^{-5} * \textup{м}^3/\textup{c}\f$)*/
    LTEDS_PHYSMEAS_VOL_FLOW_CFM     = 37,/**< Объемный расход в кубических футах в минуту
                                          = \f$ 4.71947e^{-4} * \textup{м}^3/\textup{c}\f$)*/
    LTEDS_PHYSMEAS_VOL_FLOW_L_M     = 38,/**< Объемный расход в литрах в минуту
                                          = \f$ 1.66667e^{-5} * \textup{м}^3/\textup{c}\f$)*/
    LTEDS_PHYSMEAS_REL_HUMIDITY     = 39,/**< Относительная влажность в
                                          \f$ (\textup{кг}/\textup{м}^3)/(\textup{кг}/\textup{м}^3)\f$)*/
    LTEDS_PHYSMEAS_RATIO_PERCENT    = 40,/**< Безразмерное отношение в процентах */
    LTEDS_PHYSMEAS_VOLTAGE          = 41,/**< Напряжение в Вольтах */
    LTEDS_PHYSMEAS_RMS_VOLTAGE      = 42,/**< RMS напряжения в Вольтах */
    LTEDS_PHYSMEAS_CURRENT          = 43,/**< Сила тока в Амперах */
    LTEDS_PHYSMEAS_RMS_CURRENT      = 44,/**< RMS силы тока в Амперах  */
    LTEDS_PHYSMEAS_POWER_WATTS      = 45,/**< Мощность в Ваттах  */
}  e_LTEDS_INFO_PHYSICAL_MEASURAND;

/** @brief Тип электрического интерфейса преобразователя

   Данное перечисление определяет общий тип сигналов внешнего электрического интерфейса
   преобразователя (без учета внутреннего устройства).
*/
typedef enum {
    LTEDS_INFO_ELECSIGTYPE_VOLTAGE_SENSOR    = 0, /**< Датчик с выходом по напряжению */
    LTEDS_INFO_ELECSIGTYPE_CURRENT_SENSOR    = 1, /**< Датчик с выходом по току */
    LTEDS_INFO_ELECSIGTYPE_RESISTANCE_SENSOR = 2, /**< Датчик с выходом в виде сопротивления */
    LTEDS_INFO_ELECSIGTYPE_BRIDGE_SENSOR     = 3, /**< Датчик  с выходом в виде моста */
    LTEDS_INFO_ELECSIGTYPE_LVDT_SENSOR       = 4, /**< LVDT датчик */
    LTEDS_INFO_ELECSIGTYPE_POTENT_VDIV_SENSOR= 5, /**< Потенциометрический делитель напряжения */
    LTEDS_INFO_ELECSIGTYPE_PULSE_SENSOR      = 6, /**< Датчик с импульсным выходом */
    LTEDS_INFO_ELECSIGTYPE_VOLTAGE_ACTUATOR  = 7, /**< Источник воздействия, управляемый напряжением */
    LTEDS_INFO_ELECSIGTYPE_CURRENT_ACTUATOR  = 8, /**< Источник воздействия, управляемый током */
    LTEDS_INFO_ELECSIGTYPE_PULSE_ACTUATOR    = 9, /**< Источник воздействия, управляемый импульсами */
} e_LTEDS_INFO_ELECSIGTYPE;

/** Знак (полярность) выходного сигнала относительно входного */
typedef enum {
    LTEDS_INFO_OUTSIG_SIGN_POSITIVE = 0, /**< Выходной сигнал положительной полярности
                                              (в фазе с входным) */
    LTEDS_INFO_OUTSIG_SIGN_NEGATIVE = 1, /**< Выходной сигнал отрицательной полярности
                                              (в противофазе с входным) */
} e_LTEDS_INFO_OUTSIG_SIGN;

/** Ось измерения датчика */
typedef  enum {
    LTEDS_INFO_SENS_DIRECTION_X = 0, /**< Ось X */
    LTEDS_INFO_SENS_DIRECTION_Y = 1, /**< Ось Y */
    LTEDS_INFO_SENS_DIRECTION_Z = 2, /**< Ось Z */
    LTEDS_INFO_SENS_DIRECTION_NA = 3, /**< Нет данных */
} e_LTEDS_INFO_SENS_DIRECTION;

/** Метод отображения величин физического сигнала (PV) на значения электрического сигнала (EV) */
typedef enum {
    LTEDS_INFO_MAPMETH_LINEAR       = 0, /**< Линейное отображение: \f$ PV = m*EV + b \f$ */
    LTEDS_INFO_MAPMETH_INVERSE_1    = 1, /**< Инверсное линейное отображение типа 1:
                                           \f$ PV = m/(EV + b) \f$ */
    LTEDS_INFO_MAPMETH_INVERSE_2    = 2, /**< Инверсное линейное отображение типа 2:
                                           \f$ PV = b + m/EV \f$ */
    LTEDS_INFO_MAPMETH_INVERSE_3    = 3, /**< Инверсное линейное отображение типа 3:
                                           \f$ PV = 1/(b + m/EV)\f$ */
    LTEDS_INFO_MAPMETH_THERMOCOUPLE = 4, /**< Отображение для термопары */
    LTEDS_INFO_MAPMETH_THERMISTOR   = 5, /**< Отображение для термистора */
    LTEDS_INFO_MAPMETH_RTD          = 6, /**< Отображение для термосопротивления */
    LTEDS_INFO_MAPMETH_BRIDGE       = 7, /**< Отображение для моста (с использованием
                                              свойств тензодатчика) */
} e_LTEDS_INFO_MAPMETH;


/** @brief Информация о диапазоне физических величин, измеряемых датчиком

    Структура содержит информацию, какую физическую величину измеряет датчик,
    а также пределы рабочего диапазона измеряемых величин.
    Для датчиков с линейной характеристикой данный диапазон вместе с диапазоном
    электрических величин (#TLTEDS_INFO_ELECTRICAL_RANGE) может определять
    коэффициент преобразования (чувствительность) датчика.

    Входит в состав многих стандартных шаблонов. */
typedef struct {
    BYTE   Measurand; /**< Измеряемая физическая величина и единицы ее измерения.
                           Значение из #e_LTEDS_INFO_PHYSICAL_MEASURAND. */
    double MinValue; /**< Минимальное измеряемое значение в единицах, определяемых
                          [Units](@ref TLTEDS_INFO_PHYSMEAS_RANGE::Measurand). */
    double MaxValue; /**< Максимальное измеряемое значение в единицах, определяемых
                          [Units](@ref TLTEDS_INFO_PHYSMEAS_RANGE::Measurand). */
} TLTEDS_INFO_PHYSMEAS_RANGE;

/** @brief Информация о диапазоне электрических значений на выходе датчика

    Структура определяет пределы электрических выходных сигналов датчика.
    Для датчиков с линейной характеристикой данный диапазон вместе с диапазоном
    физических величин (#TLTEDS_INFO_PHYSMEAS_RANGE) может определять
    коэффициент преобразования (чувствительность) датчика.

    Входит в состав многих стандартных шаблонов. */
typedef struct {
    BYTE PrecType; /**< Данное поле определяет, каким способом и с какой точностью
                       было закодировано значение в TEDS. Набор кодов зависит от
                       типа шаблона. */
    double MinValue; /**< Минимальное значение выходного электрического сигнала. */
    double MaxValue; /**< Максимальное значение выходного электрического сигнала. */
} TLTEDS_INFO_ELECTRICAL_RANGE;

/** Информация о  уровнях напряжении возбуждения или питания датчика */
typedef struct {
    double NominalValue;  /**< Номинальное значение в Вольтах */
    double MinValue; /**< Минимальное значение в Вольтах */
    double MaxValue; /**< Максимально значение в Вольтах */
} TLTEDS_INFO_EXCITATION_LEVELS;


/** @brief Тип напряжения питания или возбуждения.

     Перечисление определяет тип напряжения питания или возбуждения и
     определяет, как интерпретировать заданные уровни, например в структуре
     #TLTEDS_INFO_EXCITATION_LEVELS. */
typedef  enum {
    /** Питание постоянным однополярным уровнем. */
    LTEDS_INFO_EXCITETYPE_DC         = 0,
    /** Питание постоянным двуполярным уровнем, т.е. требуется подача  положительного
     *  и отрицательного постоянного уровня заданной величины. */
    LTEDS_INFO_REXCITETYPE_BIPOLAR_DC = 1,
    /** Переменное питание. Уровни указываются в RMS. */
    LTEDS_INFO_EXCITETYPE_AC         = 2,
} e_LTEDS_INFO_EXCITE_TYPE;

/** @} */


/***************************************************************************//**
    @addtogroup std_tmpl_25 Шаблон Accelerometer/Force transducer (25)
    @{
*******************************************************************************/

/** Тип преобразователя для шаблона акселерометра/датчика силы */
typedef enum {
    LTEDS_INFO_ACCF_TRANS_TYPE_ACC = 0, /**< Акселерометр */
    LTEDS_INFO_ACCF_TRANS_TYPE_FORCE = 1, /**< Датчик силы */
} e_LTEDS_INFO_ACCF_TRANS_TYPE;

/** Настройки чувствительности по умолчанию */
typedef enum {
    LTEDS_INFO_ACCF_DEFSENS_NO = 0, /**< Пассивный режим */
    LTEDS_INFO_ACCF_DEFSENS_LOW = 1, /**< Низкая чувствительность */
    LTEDS_INFO_ACCF_DEFSENS_HIGH = 2 /**< Высокая чувствительность */
} e_LTEDS_INFO_ACCF_DEFSENS;


/** Параметры одного варианта чувствительности акселерометра/датчика силы */
typedef struct {
    double Sens; /**< Чувствительность преобразователя */
    double Fhp; /**< Частота среза ФВЧ по уровню -3dB, определяющего звено передаточной
                     характеристики как

                     \f$ H(f, F_{hp}) = 1 + \frac{i*f}{F_{hp}} \f$. */
} TLTEDS_INFO_ACCF_SENS_HP;

/** Параметры чувствительности акселерометра/датчика силы при программном управлении */
typedef struct {
    BYTE DefaultSens; /**< Значение установленной чувствительности по умолканию.
                           Значение из #e_LTEDS_INFO_ACCF_DEFSENS. */
    BOOLEAN SupportPassiveMode; /**< Поддерживает ли датчик пассивный режим,
                                     в котором он не передает выходной сигнал,
                                     что позволяет подключить несколько
                                     мультиплексированных датчиков на один канал. */
    TLTEDS_INFO_ACCF_SENS_HP Low; /**< Параметры при выбранной низкой чувствительности. */
    TLTEDS_INFO_ACCF_SENS_HP High; /**< Параметры при выбранной высокой чувствительности. */
} TLTEDS_INFO_ACCF_PROG_SENS;

/** Параметры измерения физической величины, специфичные для датчика силы */
typedef struct {
    double Stiffness; /**< Жесткость в Ньютонах на метр. */
    double MassBelow; /**< Масса ниже чувствительного элемента в граммах. */
    double PhaseCorrection; /**< Коррекция фазы в градусах, необходимая для компенсации сдвига
                                 на опорной частоте. Используется только
                                 при поддержке программного управления чувствительностью. */
} TLTEDS_INFO_ACCF_FORCE;


/** Параметры измерения физической величины акселерометром/датчиком силы */
typedef struct {
    BYTE TransducerType; /**< Тип преобразователя из #e_LTEDS_INFO_ACCF_TRANS_TYPE */
    BOOLEAN HasProgrSens; /**< Признак, поддерживает ли датчик программное управление чувствительностью */
    union {
        TLTEDS_INFO_ACCF_SENS_HP SensInfo; /**< Фиксированные параметры чувствительности
                                                (если @fieldref{TLTEDS_INFO_ACCF_MEAS,HasProgrSens} имеет значение "ложь") */
        TLTEDS_INFO_ACCF_PROG_SENS ProgrSensInfo; /**< Параметры чувствительности при программном управлении
                                                  (если @fieldref{TLTEDS_INFO_ACCF_MEAS,HasProgrSens}  имеет значение "истина") */
    };
    TLTEDS_INFO_ACCF_FORCE Force; /**< Параметры, специфичные для датчика силы
                                       (для акселерометра не имеют значения) */
    BYTE SensDirection; /**< Ось измерения датчика. Значение из #e_LTEDS_INFO_SENS_DIRECTION */
    double Weight; /**< Вес датчика в граммах */
} TLTEDS_INFO_ACCF_MEAS;


/** Параметры электрического интерфейса акселерометра/датчика силы */
typedef struct {
    BYTE SigType; /**< Тип сигналов электрического интерфейса. Всегда равно
                       #LTEDS_INFO_ELECSIGTYPE_VOLTAGE_SENSOR для данного шаблона */
    BOOLEAN AcCoupling; /**< Признак, требуется ли отсечка постоянной составляющей
                             при измерении выходного сигнала.
                             Всегда истина для данного шаблона. */
    BYTE MapMeth; /**< Метод отображения физической величины на электрическую.
                       Всегда равен #LTEDS_INFO_MAPMETH_LINEAR для данного шаблона. */
    BYTE Sign; /**< Полярность выходного сигнала. Значение из #e_LTEDS_INFO_OUTSIG_SIGN */
} TLTEDS_INFO_ACCF_ELEC;

/** Параметры передаточной функции акселерометра/датчика силы */
typedef struct {
    BOOLEAN IsSpecified; /**< Определена ли переходная характеристика в шаблоне.
                              Если ложь, то остальные поля не имеют значения */
    double Flp; /**< Частота среза ФВЧ по уроню -3dB, определяющего звено передаточной
                     характеристики как

                     \f$ H(f, F_{lp}) = \frac{1}{1 + \frac{i*f}{F_{lp}}} \f$. */
    double Fpres; /**< Резонансная частота для комплексного полюса, определяющего
                       звено передаточной характеристики как

                       \f$ H(f, F_{pres}, Q_p) = \frac{1}{1 + \frac{i*f}{Q_p * F_{pres}} + (\frac{i*f}{F_{pres}})^2} \f$.

                       Значение \f$ Q_p \f$ берется из параметра @fieldref{TLTEDS_INFO_ACCF_TF,Qp}.*/
    double Qp; /**< Добротность для комплексного полюса. Используется в формуле из
                    описания параметра @fieldref{TLTEDS_INFO_ACCF_TF,Fpres}.*/
    double AmpSlope; /**< Относительный наклон АЧХ в процентах на декаду.
                          Падение описывается по формуле
                          \f$ H(f,a,F_{ref}) = (\frac{i*f}{F_{ref}})^{\frac{a}{ln(10)}}\f$,
                          где данное поле задает значение a. */
    double TempCoef; /**< Температурный коэффициент, описывающий влияние
                          температуры на передаточную характеристику по формуле

                          \f$ H(T) = (1 + TempCoef * (T - RefTemp)) \f$ */
} TLTEDS_INFO_ACCF_TF;

/** Информация о калибровке акселерометра/датчика силы */
typedef struct {
    double RefFreq; /**< Значение частоты в Гц, на которой измерена чувствительность датчика
                         и относительно которой может задаваться передаточная характеристика */
    double RefTemp; /**< Значение температуры в градусах Цельсия, при которой измерена
                         чувствительность датчика и относительно которой может задаваться
                         температурный коэффициент изменения */
    TLTEDS_INFO_BASE_CAL Base; /**< Базовая информация о времени проведения калибровки */
} TLTEDS_INFO_ACCF_CALINFO;


/** @brief Информация о акселерометре/датчике силы

    Структура содержит всю информацию о датчике, которая соответствует
    стандартному шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_ACCFORCE. */
typedef struct {
    TLTEDS_INFO_ACCF_MEAS Meas; /**< Свойства измерения физической величины */
    TLTEDS_INFO_ACCF_ELEC ElecSigOut; /**< Свойства электрического интерфейса */
    TLTEDS_INFO_ACCF_TF TransfFunc; /**< Свойства передаточной функция */
    TLTEDS_INFO_ACCF_CALINFO CalInfo; /**< Информация о калибровке */
    WORD MeasLocationID; /**< Пользовательский идентификатор места измерения. */
} TLTEDS_INFO_TMPLT_ACCFORCE;




/***************************************************************************//**
    @brief Извлечение информации о акселерометре/датчике силы из данных TEDS

    Функция позволяет получить из TEDS информацию, соответствующую стандартному
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_ACCFORCE.

    Информация возвращается в виде структуры типа #TLTEDS_INFO_TMPLT_ACCFORCE.

    @param[in]   ctx      Контекст разбора данных TEDS.
    @param[out]  info     Структура, в которой возвращается полученная информация.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_GetTmpltInfoAccForce(TLTEDS_DECODE_CONTEXT *ctx,
                                                 TLTEDS_INFO_TMPLT_ACCFORCE *info);

/***************************************************************************//**
    @brief Запись информации о акселерометре/датчике силы в данные TEDS

    Функция позволяет закодировать в TEDS информацию, соответствующую стандартному
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_ACCFORCE.

    @param[in]   ctx      Контекст кодирования данных TEDS.
    @param[in]   info     Структура, содержащая записываемую информации.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_PutTmpltInfoAccForce(TLTEDS_ENCODE_CONTEXT *ctx,
                                       const TLTEDS_INFO_TMPLT_ACCFORCE *info);


/** @} */

/***************************************************************************//**
    @addtogroup std_tmpl_hlvout Шаблон High Level Voltage (30)
    @{
*******************************************************************************/

/** Способ кодирования диапазона электрической величины датчика с выходом по напряжению */
typedef enum {
    LTEDS_INFO_HLV_ELEC_PREC_STD_0_10V = 0, /**< Стандартное фиксированное значение
                                                 пределов 0-10 Вольт */
    LTEDS_INFO_HLV_ELEC_PREC_STD_PM10V = 1, /**< Стандартное фиксированное значение
                                                 пределов \f$\pm10\f$ Вольт. */
    LTEDS_INFO_HLV_ELEC_PREC_20MV      = 2, /**< Значение закодировано в виде
                                                 целого числа с шагом 20 мВ. */
    LTEDS_INFO_HLV_ELEC_PREC_FULL      = 3, /**< Значение закодировано как
                                                 вещественное число. */
} e_LTEDS_INFO_HLV_ELEC_PRECTYPE;



/** Информация об электрических характеристиках выходного интерфейса датчика
    с выходом по напряжению */
typedef struct {   
    BYTE SigType; /**< Тип сигналов электрического интерфейса. Всегда равно
                       #LTEDS_INFO_ELECSIGTYPE_VOLTAGE_SENSOR для данного шаблона */
    TLTEDS_INFO_ELECTRICAL_RANGE Range; /**< Диапазон выходного электрического сигнала. */    
    BYTE MapMeth; /**< Метод отображения физической величины на электрическую.
                       Всегда равен #LTEDS_INFO_MAPMETH_LINEAR для данного шаблона. */
    BOOLEAN AcCoupling; /**< Признак, требуется ли отсечка постоянной составляющей
                             при измерении выходного сигнала. */
    double OutputImpedance; /**< Выходное сопротивление датчика при номинальной
                               частоте и питании в Омах. */
    double ResponseTime; /**< Время отклика в секундах.
                              Определяет частоту, с которой может
                              меняться выходной сигнал.
                              Свойство предназначено для описания полосы выходного
                              сигнала и не обязательно соответствует времени задержки
                              изменения электрического сигнала относительно
                              физического. */
} TLTEDS_INFO_HLV_ELECTRICAL;



/** Информация о питании датчика с выходом по напряжению */
typedef struct {
    BOOLEAN IsRequired; /**< Признак, требуется ли датчику отдельное питание.
                             Если нет, то остальные поля не имеют значения. */
    BYTE   Type;        /**< Тип требуемого питания. Значение из #e_LTEDS_INFO_EXCITE_TYPE. */
    TLTEDS_INFO_EXCITATION_LEVELS Levels; /**< Уровни напряжения питания датчика. */
    double CurrentDraw; /**< Требуемая сила тока (максимальный потребляемый ток)
                             для источника питания при номинальном уровне и
                             нормальных условиях. */
} TLTEDS_INFO_HLV_POWSUPPLY;

/** @brief Информация о датчике с выходом по напряжению

    Структура содержит всю информацию о датчике, которая соответствует
    стандартному шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_HL_VOLTAGE_OUTPUT. */
typedef struct {
    TLTEDS_INFO_PHYSMEAS_RANGE Meas; /**< Измеряемая физическая величина и рабочий диапазон. */
    TLTEDS_INFO_HLV_ELECTRICAL ElecSigOut; /**< Параметры выходного электрического сигнала. */
    TLTEDS_INFO_HLV_POWSUPPLY  PowerSupply; /**< Параметры напряжения питания (если требуется). */
    TLTEDS_INFO_BASE_CAL       CalInfo; /**< Информация о проведении калибровки. */
    WORD MeasLocationID; /**< Пользовательский идентификатор места измерения. */
} TLTEDS_INFO_TMPLT_HLVOUT;



/***************************************************************************//**
    @brief Извлечение информации о датчике с выходом по напряжению из данных TEDS

    Функция позволяет получить из TEDS информацию, соответствующую стандартному
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_HL_VOLTAGE_OUTPUT.

    Информация возвращается в виде структуры типа #TLTEDS_INFO_TMPLT_HLVOUT.

    @param[in]   ctx      Контекст разбора данных TEDS.
    @param[out]  info     Структура, в которой возвращается полученная информация.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_GetTmpltInfoHLVOut(TLTEDS_DECODE_CONTEXT *ctx,
                                                 TLTEDS_INFO_TMPLT_HLVOUT *info);

/***************************************************************************//**
    @brief Запись информации о датчике с выходом по напряжению в данные TEDS

    Функция позволяет закодировать в TEDS информацию, соответствующую стандартному
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_HL_VOLTAGE_OUTPUT.

    @param[in]   ctx      Контекст кодирования данных TEDS.
    @param[in]   info     Структура, содержащая записываемую информации.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_PutTmpltInfoHLVOut(TLTEDS_ENCODE_CONTEXT *ctx,
                                       const TLTEDS_INFO_TMPLT_HLVOUT *info);
/** @} */




/***************************************************************************//**
    @addtogroup std_tmpl_bridge Шаблон Bridge Sensors (33)
    @{
*******************************************************************************/

/** Тип моста */
typedef enum {
    LTEDS_INFO_BRIDGE_TYPE_QUARTER = 0, /**< Четвертьмост */
    LTEDS_INFO_BRIDGE_TYPE_HALF    = 1, /**< Полумост */
    LTEDS_INFO_BRIDGE_TYPE_FULL    = 2  /**< Полный мост */
} e_LTEDS_INFO_BRIDGE_TYPE;


/** Способ кодирования диапазона электрической величины для шаблона мостового датчика */
typedef enum {
    LTEDS_INFO_BRIDGE_ELEC_PREC_11BIT = 0, /**< От -1 до +1 с шагом  0.001 */
    LTEDS_INFO_BRIDGE_ELEC_PREC_19BIT = 1, /**< От -6.55E-3 до 6.55E-3 с шагом 25E-9 */
    LTEDS_INFO_BRIDGE_ELEC_PREC_FULL  = 2, /**< Напряжение закодировано числом с плавающей точкой */
} e_LTEDS_INFO_BRIDGE_ELEC_PRECTYPE;


/** Информация о электрических характеристиках выходного интерфейса датчика
    с выходом в виде моста */
typedef struct {
    BYTE SigType; /**< Тип сигналов электрического интерфейса. Всегда равно
                       #LTEDS_INFO_ELECSIGTYPE_BRIDGE_SENSOR для данного шаблона */
    TLTEDS_INFO_ELECTRICAL_RANGE Range; /**< Диапазон выходного электрического сигнала. */
    BYTE MapMeth; /**< Метод отображения физической величины на электрическую.
                       Всегда равен #LTEDS_INFO_MAPMETH_LINEAR для данного шаблона. */
    BYTE BridgeType; /**< Тип моста. Значение из #e_LTEDS_INFO_BRIDGE_TYPE. */
    double BridgeImpedance; /**< Выходное сопротивление  каждого элемента моста в Омах. */
    double ResponseTime; /**< Время отклика в секундах.
                              Значение аалогично одноименному полю из [TLTEDS_INFO_HLV_ELECTRICAL]
                              (@ref TLTEDS_INFO_HLV_ELECTRICAL::ResponseTime). */
} TLTEDS_INFO_BRIDGE_ELECTRICAL;




/** @brief Информация шаблона мостового датчика.

   Структура содержит информацию из шаблона с идентификатором
   #LTEDS_STD_TEMPLATE_ID_BRIDGE_SENSOR, описывающего свойства мостового датчика.
   
   Для получения используется функция LTEDS_GetTmpltInfoBridge(). */
typedef struct {
    TLTEDS_INFO_PHYSMEAS_RANGE    Meas; /**< Информация о измеряемой физической величине. */
    TLTEDS_INFO_BRIDGE_ELECTRICAL ElecSigOut; /**< Информация о выходном электрическом интерфейсе датчика. */
    TLTEDS_INFO_EXCITATION_LEVELS ExcitationLevels; /**< Информация о напряжении возбуждения (питания) моста. */
    TLTEDS_INFO_BASE_CAL CalInfo; /**< Информация о проведении калибровки. */
    WORD MeasLocationID; /**< Пользовательский идентификатор места измерения. */
} TLTEDS_INFO_TMPLT_BRIDGE;



/***************************************************************************//**
    @brief Извлечение информации о мостовом датчике из TEDS данных

    Функция позволяет получить из TEDS информацию, которая соответствует
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_BRIDGE_SENSOR и описывает свойства
    датчика с выходом в виде моста сопротивлений.

    Информация возвращается в виде структуры типа #TLTEDS_INFO_TMPLT_BRIDGE.

    @param[in]   ctx      Контекст разбора данных TEDS.
    @param[out]  info     Структура, в которой возвращается полученная информация.
    @return               Код ошибки.
    ****************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_GetTmpltInfoBridge(TLTEDS_DECODE_CONTEXT *ctx,
                                                TLTEDS_INFO_TMPLT_BRIDGE *info);


/***************************************************************************//**
    @brief Запись информации о мостовом датчике в данные TEDS

    Функция позволяет закодировать в TEDS информацию, соответствующую
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_BRIDGE_SENSOR.

    @param[in]   ctx      Контекст кодирования данных TEDS.
    @param[in]   info     Структура, содержащая записываемую информацию.
    @return               Код ошибки.
    ****************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_PutTmpltInfoBridge(TLTEDS_ENCODE_CONTEXT *ctx,
                                                 const TLTEDS_INFO_TMPLT_BRIDGE *info);
/** @} */




/***************************************************************************//**
    @addtogroup std_tmpl_caltbl Шаблон Calibration Table (40)
    @{
*******************************************************************************/

/** Максимальное количество элементов в таблице калибровки */
#define LTEDS_INFO_CALTABLE_DATASET_MAX 127

/** Домен параметра для задания точек калибровки */
typedef enum {
    LTEDS_INFO_CAL_DOMAIN_ELECTRICAL = 0, /**< Значение электрической величины */
    LTEDS_INFO_CAL_DOMAIN_PHYSICAL   = 1  /**< Значение физической величины */
} e_LTEDS_CAL_DOMAIN;


/** @brief Точка из таблицы калибровки

    Структура, содержащая набор из двух значений, определяющих отклонение
    калибровочного значения от номинального в указанной точке. */
typedef struct {
    double DomainValue; /**< Значение величины (физической или электрической, в
                            зависимости от выбранного домена), для которой указано
                            калибровочное значение. Задается в процентах от
                            полного диапазона. */
    double RangeDeviation; /**< Отклонение от номинальной линейной передаточной
                            функции в процентах от шкалы. */
} TLTEDS_INFO_CALTABLE_DATASET;


/** @brief Информация шаблона таблицы калибровки

   Структура содержит информацию из шаблона с идентификатором
   #LTEDS_STD_TEMPLATE_ID_CAL_TABLE.

    В структуре представлены данные калибровочной таблицы, описывающей передаточную
    функцию преобразования между электрическими и физическими величинами.
    Каждая точка таблицы указывает для определенного значения величины отклонение
    от номинальной линейной характеристики в процентах шкалы.

    Для получения используется функция LTEDS_GetTmpltInfoCalTable(). */
typedef struct {
    BYTE Domain; /**< Домен, определяющий какая величина используется для задания
                      точек, где производилась калибровка. Значение из
                      #e_LTEDS_CAL_DOMAIN. */
    BYTE DataSetCount; /**< Количество точек калибровки. Определяет количество
                            действительных элементов в массиве
                            [DataSets](@ref TLTEDS_INFO_TMPLT_CAL_TABLE::DataSets). */
    /** Набор точек калибровки. Действительны первые
       [DataSetCount](@ref TLTEDS_INFO_TMPLT_CAL_TABLE::DataSetCount) элементов. */
    TLTEDS_INFO_CALTABLE_DATASET DataSets[LTEDS_INFO_CALTABLE_DATASET_MAX];
} TLTEDS_INFO_TMPLT_CAL_TABLE;



/***************************************************************************//**
    @brief Извлечение  калибровочной таблицы из данных TEDS

    Функция позволяет получить из TEDS информацию, соответствующую стандартному
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_CAL_TABLE.

    Информация возвращается в виде структуры типа #TLTEDS_INFO_TMPLT_CAL_TABLE
    @param[in]   ctx      Контекст разбора данных TEDS.
    @param[out]  info     Структура, в которой возвращается полученная информация.
    @return               Код ошибки.
********************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_GetTmpltInfoCalTable(TLTEDS_DECODE_CONTEXT *ctx,
                                                   TLTEDS_INFO_TMPLT_CAL_TABLE *info);

/***************************************************************************//**
    @brief Запись информации о мостовом датчике в данные TEDS

    Функция позволяет закодировать в TEDS информацию, соответствующую
    шаблону с идентификатором #LTEDS_STD_TEMPLATE_ID_CAL_TABLE.

    @param[in]   ctx      Контекст кодирования данных TEDS.
    @param[in]   info     Структура, содержащая записываемую информацию.
    @return               Код ошибки.
    ****************************************************************************/
LTEDSAPI_DllExport(INT) LTEDS_PutTmpltInfoCalTable(TLTEDS_ENCODE_CONTEXT *ctx,
                                                   const TLTEDS_INFO_TMPLT_CAL_TABLE *info);
/** @} */

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif // LTEDSAPI_STDTMPL_H
