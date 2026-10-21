#ifndef __LTR212API__H
#define __LTR212API__H
#include "ltrapi.h"


// версия библиотеки
#define     LTR212_VERSION_MAJOR        (0x1)
#define     LTR212_VERSION_MINOR        (0x2)
#define     LTR212_VERSION_BUILD        (0x0)
#define     LTR212_VERSION_REVISION     (0x0)

#ifdef _WIN32
    #ifdef LTR212API_EXPORTS
        #define LTR212API_DllExport(type)   __declspec(dllexport) type APIENTRY
    #else
        #define LTR212API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR212API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR212API_DllExport(type)  type
#endif

// Коды ошибок. Описание см. ф-ю LRT212_GetErrorString()
#define LTR212_ERR_WRONG_ADC_SETTINGS                               (-2017)
#define LTR212_ERR_WRONG_VCH_SETTINGS                               (-2018)
#define LTR212_ERR_CANT_CALIBRATE                                   (-2020)
#define LTR212_ERR_INVALID_ACQ_MODE                                 (-2022)
#define LTR212_ERR_INV_ADC_DATA                                     (-2026)
#define LTR212_ERR_FIRMWARE_TEST_NOT_PASSED                         (-2027)
#define LTR212_ERR_CANT_USE_FABRIC_AND_USER_CALIBR_SYM              (-2032)
#define LTR212_ERR_WRONG_BIOS_FILE                                  (-2034)
#define LTR212_ERR_CANT_USE_CALIBR_MODE                             (-2035)
#define LTR212_ERR_CANT_SET_BRIDGE_CONNECTIONS                      (-2047)
#define LTR212_ERR_QB_RESISTORS_IN_ALL_CHANNELS_MUST_BE_EQUAL       (-2049)
#define LTR212_ERR_INVALID_VCH_CNT                                  (-2051)
#define LTR212_ERR_FILTER_FILE_OPEN                                 (-2052)
#define LTR212_ERR_FILTER_FILE_READ                                 (-2053)
#define LTR212_ERR_FILTER_FILE_FORMAT                               (-2054)
#define LTR212_ERR_FILTER_ORDER                                     (-2055)
#define LTR212_ERR_UNSUPPORTED_MODULE_TYPE                          (-2056)
#define LTR212_ERR_CANT_USE_FILTER                                  (-2057)
#define LTR212_ERR_CANT_USE_CHNUM                                   (-2058)
#define LTR212_ERR_INVALID_CHNUM                                    (-2059)
#define LTR212_ERR_INVALID_SCALE                                    (-2060)
#define LTR212_ERR_INVALID_FIR_DECIMATION                           (-2061)

#define LTR212_LCH_CNT_MAX     8  /* Макс. число логических. каналов */
#define LTR212_CHANNEL_CNT     8


#define LTR212_FIR_ORDER_MAX 255 /* Максимальное значение порядка КИХ-фильтра */
#define LTR212_FIR_ORDER_MIN 3   /* Минимальное значение порядка КИХ-фильтра */

#define LTR212_IIR_ORDER     5 /* необходимое значение порядка для БИХ-фильтра */





// модификации модуля
enum LTR212_MODULE_TYPES {
    LTR212_OLD,     // старый модуль с поддержкой полно- и полу-мостовых подключений
    LTR212_M_1,     // новый модуль с поддержкой полно-,  полу- и четверть-мостовых подключений
    LTR212_M_2      // новый модуль с поддержкой полно- и полу-мостовых подключений
};


// типы возможных мостов
enum LTR212_BRIDGE_TYPES {
    LTR212_FULL_OR_HALF_BRIDGE,
    LTR212_QUARTER_BRIDGE_WITH_200_Ohm,
    LTR212_QUARTER_BRIDGE_WITH_350_Ohm,
    LTR212_QUARTER_BRIDGE_WITH_CUSTOM_Ohm,
    LTR212_UNBALANCED_QUARTER_BRIDGE_WITH_200_Ohm,
    LTR212_UNBALANCED_QUARTER_BRIDGE_WITH_350_Ohm,
    LTR212_UNBALANCED_QUARTER_BRIDGE_WITH_CUSTOM_Ohm
};

// режимы сбора данных (AcqMode)
enum LTR212_ACQ_MODE {
    LTR212_FOUR_CHANNELS_WITH_MEDIUM_RESOLUTION =0,
    LTR212_FOUR_CHANNELS_WITH_HIGH_RESOLUTION   =1,
    LTR212_EIGHT_CHANNELS_WITH_HIGH_RESOLUTION  =2
};

// значения опорного напряжения
enum LTR212_REF_VAL {
    LTR212_REF_2_5V = 0,  //2.5 В
    LTR212_REF_5V   = 1   //5   В
};

// диапазоны канало
enum LTR212_SCALE {
    LTR212_SCALE_B_10 = 0, /* диапазон -10мВ/+10мВ */
    LTR212_SCALE_B_20 = 1, /* диапазон -20мВ/+20мВ */
    LTR212_SCALE_B_40 = 2, /* диапазон -40мВ/+40мВ */
    LTR212_SCALE_B_80 = 3, /* диапазон -80мВ/+80мВ */
    LTR212_SCALE_U_10 = 4, /* диапазон -10мВ/+10мВ */
    LTR212_SCALE_U_20 = 5, /* диапазон -20мВ/+20мВ */
    LTR212_SCALE_U_40 = 6, /* диапазон -40мВ/+40мВ */
    LTR212_SCALE_U_80 = 7 /* диапазон -80мВ/+80мВ */
};

// режимы калибровки
enum LTR212_CALIBR_MODE {
    LTR212_CALIBR_MODE_INT_ZERO             = 0,
    LTR212_CALIBR_MODE_INT_SCALE            = 1,
    LTR212_CALIBR_MODE_INT_FULL             = 2,
    LTR212_CALIBR_MODE_EXT_ZERO             = 3,
    LTR212_CALIBR_MODE_EXT_SCALE            = 4,
    LTR212_CALIBR_MODE_EXT_ZERO_INT_SCALE   = 5,
    LTR212_CALIBR_MODE_EXT_FULL_2ND_STAGE   = 6, /* вторая стадия внешней калибровки */
    LTR212_CALIBR_MODE_EXT_ZERO_SAVE_SCALE  = 7  /* внешний ноль с сохранением до этого полученных коэф. масштаба */
};

/******************** Определение структуры описания модуля *******************/
#pragma pack(4) 


typedef union _LTR212_DLL_VERSION_ {
    struct {
        BYTE    Revision;
        BYTE    Build;
        BYTE    Minor;
        BYTE    Major;
    };
    DWORD Value;
}   LTR212_DLL_VERSION;

typedef struct {
    CHAR Name[15]; // название используемого модуля
    BYTE Type;   // тип используемого модуля: LTR212 (LTR212M-3), LTR212M-1 или LTR212M-2
    CHAR Serial[24]; // серийный номер используемого модуля
    CHAR BiosVersion[8];// Версия БИОСа
    CHAR BiosDate[16];  // Дата создания данной версии БИОСа
} TLTR212_MODULE_INFO;

typedef struct {
    INT size;
    TLTR Channel;
    INT AcqMode; // Режим сбора данных
    INT UseClb;  // Флаг использования калибровочных коэфф-тов
    INT UseFabricClb;// Флаг использования заводских калибровочных коэфф-тов
    INT LChQnt;     // Кол-во используемых виртуальных каналов
    INT LChTbl[LTR212_LCH_CNT_MAX];  //Таблица виртуальных каналов
    INT REF;         // Флаг высокого опорного напряжения
    INT AC;         // Флаг знакопеременного опорного напряжения
    double Fs;     // Частота дискретизации АЦП
  
    struct {
        INT IIR;         // Флаг использования БИХ-фильтра
        INT FIR;         // Флаг использования КИХ-фильтра
        INT Decimation;  // Значение коэффициента децимации для КИХ-фильтра
        INT TAP;         // Порядок КИХ-фильтра
        CHAR IIR_Name[512+1]; // Полный путь к файлу с коэфф-ми программного БИХ-фильтра
        CHAR FIR_Name[512+1]; // Полный путь к файлу с коэфф-ми программного КИХ-фильтра
    } filter;   // Структура, хранящая данные о прогр. фильтрах и их коэфф-ты.
  
    TLTR212_MODULE_INFO ModuleInfo;

    WORD CRC_PM; // для служебного пользования
    WORD CRC_Flash_Eval; // для служебного пользования
    WORD CRC_Flash_Read;   // для служебного пользования
} TLTR212, *PTLTR212; // структура описания модуля
 
typedef struct {
    DWORD Offset  [LTR212_LCH_CNT_MAX];
    DWORD Scale   [LTR212_LCH_CNT_MAX];
    BYTE DAC_Value[LTR212_LCH_CNT_MAX];
} TLTR212_USER_TARE_COEFS;
 

typedef struct {
    double fs;
    BYTE   decimation;
    BYTE   taps;
    SHORT  koeff[LTR212_FIR_ORDER_MAX];
} TLTR212_FILTER; // Структура, используеамая при загрузке фильтра
#pragma pack()
         
                                  
// Функции api-библиотеки. Описание см.в реализации
#ifdef __cplusplus
extern "C" { 
#endif

// Доступные пользователю
LTR212API_DllExport (void) LTR212_GetDllVersion(LTR212_DLL_VERSION * const DllVersion);
LTR212API_DllExport (INT) LTR212_Init(PTLTR212 hnd);
LTR212API_DllExport (INT) LTR212_IsOpened(PTLTR212 hnd);
LTR212API_DllExport (INT) LTR212_Open(PTLTR212 hnd, DWORD net_addr, WORD net_port,
                                      const CHAR *crate_sn, INT slot_num, const CHAR *biosname);
LTR212API_DllExport (INT) LTR212_Close(PTLTR212 hnd);
LTR212API_DllExport (INT) LTR212_CreateLChannel(INT PhysChannel, INT Scale);
LTR212API_DllExport (INT) LTR212_CreateLChannel2(DWORD PhysChannel, DWORD Scale, DWORD BridgeType);
LTR212API_DllExport (INT) LTR212_SetADC(PTLTR212 hnd);
LTR212API_DllExport (INT) LTR212_SetADCExpFlt(PTLTR212 hnd, const TLTR212_FILTER *firFlt, const TLTR212_FILTER *iirFlt);
LTR212API_DllExport (INT) LTR212_Start(PTLTR212 hnd); 
LTR212API_DllExport (INT) LTR212_Stop(PTLTR212 hnd); 
LTR212API_DllExport (INT) LTR212_Recv(PTLTR212 hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout);
LTR212API_DllExport (INT) LTR212_ProcessData(PTLTR212 hnd, const DWORD *src, double *dest,
                                             DWORD *size, BOOL volt);
LTR212API_DllExport (LPCSTR) LTR212_GetErrorString(INT Error_Code); 
LTR212API_DllExport (INT) LTR212_Calibrate(PTLTR212 hnd, BYTE *LChannel_Mask, INT mode, INT reset); 
LTR212API_DllExport (INT) LTR212_CalcFS(PTLTR212 hnd, double *fsBase, double *fs);
LTR212API_DllExport (INT) LTR212_TestEEPROM(PTLTR212 hnd);  
LTR212API_DllExport (DWORD) LTR212_CalcTimeOut(PTLTR212 hnd, DWORD n);

// Вспомогательные функции
LTR212API_DllExport (INT) LTR212_ProcessDataTest(PTLTR212 hnd, const DWORD *src,
                                                 double *dest, DWORD *size, BOOL volt,DWORD *bad_num);
LTR212API_DllExport (INT) LTR212_ReadFilter(CHAR *fname, TLTR212_FILTER *filter);

//Тестовые функции, чтение и запись пользовательских коэффициэнтов (в формате кодека)
//Этот формат не соответствует типу DOUBLE
LTR212API_DllExport (INT) LTR212_ReadUSR_Clb (PTLTR212 hnd, TLTR212_USER_TARE_COEFS *coefs);
LTR212API_DllExport (INT) LTR212_WriteUSR_Clb(PTLTR212 hnd, const TLTR212_USER_TARE_COEFS *coefs);
LTR212API_DllExport (INT) LTR212_ReadCodec_Clb(PTLTR212 hnd, DWORD ch_mask, TLTR212_USER_TARE_COEFS *coefs);
LTR212API_DllExport (INT) LTR212_Load_Clb(PTLTR212 hnd);



#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    /* определения для совместимости со старым заголовком */
    #define MAX_212_CH                    LTR212_LCH_CNT_MAX
    #define MAXTAPS                       LTR212_FIR_ORDER_MAX
    #define MINTAPS                       LTR212_FIR_ORDER_MIN

    typedef TLTR212_MODULE_INFO     TINFO_LTR212, *PTINFO_LTR212;
    typedef TLTR212_USER_TARE_COEFS TLTR212_Usr_Clb;
    typedef TLTR212_FILTER          ltr212filter;
#endif

#ifdef __cplusplus 
}
#endif

#endif

