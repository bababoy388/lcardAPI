#ifndef LTR27API_H_
#define LTR27API_H_

#ifdef _WIN32
    #ifdef LTR27API_EXPORTS
        #define LTR27API_DllExport(type)   __declspec(dllexport)  type APIENTRY
    #else
        #define LTR27API_DllExport(type)   __declspec(dllimport) type APIENTRY
    #endif
#elif defined __GNUC__
    #define LTR27API_DllExport(type) __attribute__ ((visibility("default"))) type
#else
    #define LTR27API_DllExport(type)   type
#endif


#include "ltrapi.h"


#ifdef __cplusplus
extern "C" {
#endif

/*================================================================================================*/

/* Количество мезанинов в модуле */
#define LTR27_MEZZANINE_CNT                 8
/* Количество каналов в мезонине */
#define LTR27_MEZZANINE_CHANNEL_CNT         2
/* Общее кол-во каналов в модуле */
#define LTR27_CHANNEL_CNT                   (LTR27_MEZZANINE_CHANNEL_CNT * LTR27_MEZZANINE_CNT)
/* Количество калибровочных коэффициентов на канал */
#define LTR27_CHANNEL_CBR_COEF_CNT          2
/* Общее кол-во калибровочных коэффициентов в модуле */
#define LTR27_MEZZANINE_CBR_COEF_CNT        (LTR27_MEZZANINE_CHANNEL_CNT * LTR27_CHANNEL_CBR_COEF_CNT)
/* Количество коэффициентов перевода в величину в мезанине (одна пара на оба канала) */
#define LTR27_MEZZANINE_CONV_COEF_CNT       LTR27_CHANNEL_CBR_COEF_CNT



#define LTR27_DATA_CORRECTION               (1 << 0)
#define LTR27_DATA_VALUE                    (1 << 1)
/* макросы для функций */
#define LTR27_MODULE_DESCRIPTION            (1 << 0)
#define LTR27_MEZZANINE1_DESCRIPTION        (1 << 1)
#define LTR27_MEZZANINE2_DESCRIPTION        (1 << 2)
#define LTR27_MEZZANINE3_DESCRIPTION        (1 << 3)
#define LTR27_MEZZANINE4_DESCRIPTION        (1 << 4)
#define LTR27_MEZZANINE5_DESCRIPTION        (1 << 5)
#define LTR27_MEZZANINE6_DESCRIPTION        (1 << 6)
#define LTR27_MEZZANINE7_DESCRIPTION        (1 << 7)
#define LTR27_MEZZANINE8_DESCRIPTION        (1 << 8)
#define LTR27_ALL_MEZZANINE_DESCRIPTION     (LTR27_MEZZANINE1_DESCRIPTION | \
    LTR27_MEZZANINE2_DESCRIPTION | LTR27_MEZZANINE3_DESCRIPTION | LTR27_MEZZANINE4_DESCRIPTION | \
    LTR27_MEZZANINE5_DESCRIPTION | LTR27_MEZZANINE6_DESCRIPTION | LTR27_MEZZANINE7_DESCRIPTION | \
    LTR27_MEZZANINE8_DESCRIPTION)
#define LTR27_ALL_DESCRIPTION               (LTR27_MODULE_DESCRIPTION | \
    LTR27_ALL_MEZZANINE_DESCRIPTION)

typedef enum {
    LTR27_PROC_FLAG_CALIBR     = 0x00000001,
    LTR27_PROC_FLAG_CONV_VALUE = 0x00000002
} e_LTR27_PROC_FLAGS;

/*================================================================================================*/
#pragma pack(4)
/* конфигурация модуля */
/* описание модуля */
typedef struct {
  TLTR_DESCRIPTION_MODULE Module;
  TLTR_DESCRIPTION_CPU Cpu;
  TLTR_DESCRIPTION_MEZZANINE Mezzanine[LTR27_MEZZANINE_CNT];
} TLTR27_MODULE_INFO;



typedef struct {
  /* служебная информация */
  INT size;
  TLTR Channel;
  BYTE subchannel;
  /* настройки модуля */
  BYTE  FrequencyDivisor;                            /* делитель частоты дискретизации 0..255 (1000..4 Гц) */
  struct {
      CHAR Name[16];                                     /* название субмодуля */
      CHAR Unit[16];                                     /* измеряемая субмодулем физ. величина */
      double ConvCoeff[LTR27_MEZZANINE_CONV_COEF_CNT];   /* масштаб и смещение для пересчета кода в физ. величину */
      double CalibrCoeff[LTR27_MEZZANINE_CBR_COEF_CNT];  /* калибровочные коэффициенты */
  } Mezzanine[LTR27_MEZZANINE_CNT]; /* установленные мезонины */

  TLTR27_MODULE_INFO ModuleInfo;
} TLTR27;
#pragma pack()

/*================================================================================================*/
LTR27API_DllExport(INT) LTR27_Init(TLTR27 *h_ltr27);

LTR27API_DllExport(INT) LTR27_Open(TLTR27 *h_ltr27, DWORD net_addr, WORD net_port,
    const CHAR *crate_sn, WORD slot);
LTR27API_DllExport(INT) LTR27_OpenEx(TLTR27 *h_ltr27, DWORD net_addr, WORD net_port,
    const CHAR *crate_sn, WORD slot, DWORD in_flags, DWORD *out_flags);
LTR27API_DllExport(INT) LTR27_IsOpened(TLTR27 *h_ltr27);
LTR27API_DllExport(INT) LTR27_Close(TLTR27 *h_ltr27);

LTR27API_DllExport(INT) LTR27_GetConfig(TLTR27 *h_ltr27);
LTR27API_DllExport(INT) LTR27_GetDescription(TLTR27 *h_ltr27, WORD flags);
LTR27API_DllExport(INT) LTR27_SetConfig(TLTR27 *h_ltr27);
#ifdef LTRAPI_USE_KD_STORESLOTS
LTR27API_DllExport(INT) LTR27_StoreConfig(TLTR27 *h_ltr27, TLTR_CARD_START_MODE start_mode);
#endif

LTR27API_DllExport(INT) LTR27_ADCStart(TLTR27 *h_ltr27);
LTR27API_DllExport(INT) LTR27_ADCStop(TLTR27 *h_ltr27);

LTR27API_DllExport(INT) LTR27_Recv(TLTR27 *h_ltr27, DWORD *data, DWORD *tstamp, DWORD size,
    DWORD timeout);
LTR27API_DllExport(INT) LTR27_ProcessData(TLTR27 *h_ltr27, const DWORD *src_data, double *dst_data,
    DWORD *size, BOOL calibr, BOOL value);
LTR27API_DllExport(INT) LTR27_ProcessDataEx(TLTR27 *h_ltr27, const DWORD *src_data, double *dst_data,
                                            DWORD *size, DWORD flags, DWORD ch_mask, void *reserved);
LTR27API_DllExport(INT) LTR27_SearchFirstFrame(TLTR27 *h_ltr27, const DWORD *data, DWORD size,
    DWORD *frame_idx);

LTR27API_DllExport(INT) LTR27_Echo(TLTR27 *h_ltr27);
LTR27API_DllExport(INT) LTR27_WriteMezzanineDescr(TLTR27 *h_ltr27, BYTE mn);

LTR27API_DllExport(LPCSTR) LTR27_GetErrorString(INT error);



/** @brief Подбор значений делителя и пределителя АЦП

   Функция по заданному значению частоты АЦП подбирает значение делителя,
   чтобы результирующая частота была наиболее близка.

   @param[in] adcFreq         Требуемая частота в Гц.
   @param[out] divisor        Значение пределителя, соответствующее данной частоте
   @param[out] resultAdcFreq  Полученное реальное значение частоты АЦП (в Гц)
   @return                    Код ошибки */
LTR27API_DllExport(INT) LTR27_FindAdcFreqParams(double adcFreq, BYTE *divisor, double *resultAdcFreq);


#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    /** @cond obsoleted */
    #define LTR27_MEZZANINE_NUMBER              LTR27_MEZZANINE_CNT
    #define LTR27_MEZZANINES_CNT                LTR27_MEZZANINE_CNT
    #define LTR27_MAZZANINE_CHANNELS_CNT        LTR27_MEZZANINE_CHANNEL_CNT
    #define LTR27_CHANNELS_COUNT                LTR27_CHANNEL_CNT
    #define LTR27_CHANNELS_CNT                  LTR27_CHANNEL_CNT
    #define LTR27_CHANNEL_CBR_COEFS_CNT         LTR27_CHANNEL_CBR_COEF_CNT
    #define LTR27_MAZZANINE_CBR_COEFS_CNT       LTR27_MEZZANINE_CBR_COEF_CNT
    #define LTR27_MEZZANINE_CONV_COEFS_CNT      LTR27_MEZZANINE_CONV_COEF_CNT
    typedef TLTR27_MODULE_INFO TINFO_LTR27;
    /** @endcond */
#endif

#ifdef __cplusplus
}
#endif

#endif /*#ifndef LTR27API_H_*/
