#include "ltrapi.h"

#ifndef LTR22API_H_
#define LTR22API_H_

#ifdef _WIN32
    #ifdef LTR22API_EXPORTS
        #define LTR22API_DllExport(type)    __declspec(dllexport) type APIENTRY
        #define LTR22API_Export             __declspec(dllexport)
    #else
        #define LTR22API_DllExport(type)    __declspec(dllimport) type APIENTRY
        #define LTR22API_Export             __declspec(dllimport)
    #endif
#elif defined __GNUC__
    #define LTR22API_DllExport(type) __attribute__ ((visibility("default"))) type
    #define LTR22API_Export __attribute__ ((visibility("default")))
#else
    #define LTR22API_DllExport(type)    type
    #define LTR22API_Export
#endif

#define LTR22_RANGE_CNT   6
#define LTR22_CHANNEL_CNT 4
#define LTR22_ADC_FREQ_CNT  25

// коды диапазонов АЦП
typedef  enum {
    LTR22_ADC_RANGE_1       = 0,
    LTR22_ADC_RANGE_0_3     = 1,
    LTR22_ADC_RANGE_0_1     = 2,
    LTR22_ADC_RANGE_0_03    = 3,
    LTR22_ADC_RANGE_10      = 4,
    LTR22_ADC_RANGE_3       = 5
} e_LTR22_RANGES;


/** @brief Флаги обработки данных

    Флаги, управляющие работой функции LTR22_ProcessDataEx() */
typedef enum {
    /** Признак, что нужно выполнить калибровку, применив к данным калибровочные
     *  коэффициенты, как описано в разделе @ref sect_gen_cbr. */
    LTR22_PROC_FLAG_CALIBR          = 0x00000001,
    /** Признак, что нужно перевести коды АЦП в Вольты. */
    LTR22_PROC_FLAG_VOLT            = 0x00000002,
    /** Признак, что данные должны сохраняться в порядке, соответствующем
     * функци LTR22_ProcessData() --- сперва все отсчеты первого разрешенного
     * канала, затем второго и т.д.
     * Если флаг не указан, то по умолчанию функция сохраняет данные в порядке,
     * аналогичном функциям ProcessData() дрегих модулей --- сперва первый отсчет
     * каждого разрешенного канала, затем второй и т.д. */
    LTR22_PROC_FLAG_PER_CH_ORDER    = 0x01000000
} e_LTR22_PROC_FLAGS;


// возможные варианты ошибок
#define LTR22_ERROR_SEND_DATA                              (-6000)
#define LTR22_ERROR_RECV_DATA                              (-6001)
#define LTR22_ERROR_NOT_LTR22                              (-6002)
#define LTR22_ERROR_OVERFLOW                               (-6003)
#define LTR22_ERROR_ADC_RUNNING                            (-6004)
#define LTR22_ERROR_MODULE_INTERFACE                       (-6005)
#define LTR22_ERROR_INVALID_FREQ_DIV                       (-6006)
#define LTR22_ERROR_INVALID_TEST_HARD_INTERFACE            (-6007)
#define LTR22_ERROR_INVALID_DATA_RANGE_FOR_THIS_CHANNEL    (-6008)
#define LTR22_ERROR_INVALID_DATA_COUNTER                   (-6009)
#define LTR22_ERROR_PRERARE_TO_WRITE                       (-6010)
#define LTR22_ERROR_WRITE_AVR_MEMORY                       (-6011)
#define LTR22_ERROR_READ_AVR_MEMORY                        (-6012)
#define LTR22_ERROR_PARAMETERS                             (-6013)
#define LTR22_ERROR_CLEAR_BUFFER_TOUT                      (-6014)
#define LTR22_ERROR_SYNC_FHAZE_NOT_STARTED                 (-6015)
#define LTR22_ERROR_INVALID_CH_NUMBER                      (-6016)
#define LTR22_ERROR_AVR_MEMORY_COMPARE                     (-6017)


#pragma pack(4)

typedef struct {
    TLTR_DESCRIPTION_MODULE Description;    // описание модуля
    TLTR_DESCRIPTION_CPU CPU;                // описание AVR
} TINFO_LTR22, *PTINFO_LTR22, TLTR22_INFO;


typedef struct  {
    float FactoryCalibOffset[LTR22_RANGE_CNT];
    float FactoryCalibScale[LTR22_RANGE_CNT];

    float UserCalibOffset[LTR22_RANGE_CNT];
    float UserCalibScale[LTR22_RANGE_CNT];
} TLTR22_ADC_CHANNEL_CALIBRATION;


/** конфигурация модуля */
typedef struct {
  //**** служебная информация       //
  INT size;                            // размер структуры TLTR22 1036 байт
  TLTR Channel;                        // служебная структура для работы с крейтом через сервер

  // настройки модуля
  BYTE Fdiv_rg;                        // делитель частоты клоков 1..15
  BOOLEAN Adc384;                      // дополнительный делитель частоты сэмплов true =3 false =2
  BOOLEAN AC_DC_State;                 // состояние true =AC+DC false=AC
  BOOLEAN MeasureADCZero;              // измерение собственного нуля (true - включено false - выключено)
  BOOLEAN DataReadingProcessed;        // состояние считывания АЦП true-АЦП считывается false - нет
  BYTE    ADCChannelRange[LTR22_CHANNEL_CNT];// предел имзерений АЦП по каналам 0 - 1В 1 - 0.3В 2 - 0.1В 3 - 0.03В 4 - 10В 5 - 3В

  BOOLEAN ChannelEnabled[LTR22_CHANNEL_CNT];        // Состояние каналов, включен - true выключен - false

  INT FreqDiscretizationIndex;            // частота дискретизации, выставленная сейчас 0..24 - в зависимости от частоты
                                        // из массива LTR22_DISK_FREQ_ARRAY
   
  BYTE SyncType;        // Тип синхронизации 0 - внутренний старт по сигналу Go
                        //1 - фазировка модуля
                        //2 - внешний старт
                        //3 - резервировано
  BOOLEAN SyncMaster;   // true - модуль генерит сигнал, false - модуль принимает синхросигнал

  TINFO_LTR22 ModuleInfo;
  TLTR22_ADC_CHANNEL_CALIBRATION ADCCalibration[LTR22_CHANNEL_CNT][LTR22_ADC_FREQ_CNT];

} TLTR22,*PTLTR22;  

#pragma pack()


/*
-----------------------------------------------------------------------------------------
 декларирование функций 
*/
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

LTR22API_Export extern const INT LTR22_DISK_FREQ_ARRAY[LTR22_ADC_FREQ_CNT];


LTR22API_DllExport(INT) LTR22_Init(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_Close(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_Open(TLTR22 *module, DWORD saddr, WORD sport, const CHAR *csn, WORD cc);
LTR22API_DllExport(INT) LTR22_IsOpened(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_GetConfig(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_SetConfig(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_ClearBuffer(TLTR22 *module, BOOLEAN wait_response);
LTR22API_DllExport(INT) LTR22_StartADC(TLTR22 *module, BOOLEAN WaitSync);
LTR22API_DllExport(INT) LTR22_StopADC(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_SetSyncPriority(TLTR22 *module, BOOLEAN SyncMaster);
LTR22API_DllExport(INT) LTR22_SyncPhaze(TLTR22 *module, DWORD timeout);
LTR22API_DllExport(INT) LTR22_SyncPhazeStart(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_SyncPhazeWaitDone(TLTR22 *module, DWORD timeout);
LTR22API_DllExport(INT) LTR22_SetFreq(TLTR22 *module, BOOLEAN adc384, BYTE Freq_dv);
LTR22API_DllExport(INT) LTR22_SwitchMeasureADCZero(TLTR22 *module, BOOLEAN SetMeasure);
LTR22API_DllExport(INT) LTR22_SwitchACDCState(TLTR22 *module, BOOLEAN ACDCState);
LTR22API_DllExport(INT) LTR22_SetADCRange(TLTR22 *module, BYTE ADCChannel, BYTE ADCChannelRange);
LTR22API_DllExport(INT) LTR22_SetADCChannel(TLTR22 *module, BYTE ADCChannel, BOOLEAN EnableADC);
LTR22API_DllExport(INT) LTR22_GetCalibrCoeffs(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_Recv(TLTR22 *module, DWORD *data, DWORD *tstamp, DWORD size, DWORD timeout);
LTR22API_DllExport(INT) LTR22_GetModuleDescription(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_ProcessData(TLTR22 *module, const DWORD *src_data, double *dst_data,
                                         DWORD size, BOOLEAN calibrMainPset, BOOLEAN calibrExtraVolts, BYTE * OverflowFlags);
LTR22API_DllExport(INT) LTR22_ProcessDataEx(TLTR22 *module, const DWORD *src_data, double *dst_data,
                                            INT *size, DWORD flags, BYTE * OverflowFlags, void *reserved);
LTR22API_DllExport(INT) LTR22_ReadAVREEPROM(TLTR22 *module, BYTE *Data, DWORD BeginAddress, DWORD size);
LTR22API_DllExport(INT) LTR22_WriteAVREEPROM(TLTR22 *module, const BYTE *Data, DWORD BeginAddress, DWORD size);
LTR22API_DllExport(LPCSTR) LTR22_GetErrorString(INT ErrorCode);

/*
тестовые функции
*/
LTR22API_DllExport(INT) LTR22_TestHardwareInterface(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_GetADCData(TLTR22 *module, double * Data, DWORD Size, DWORD time, 
                                        BOOLEAN calibrMainPset, BOOLEAN calibrExtraVolts);
LTR22API_DllExport(INT) LTR22_ReopenModule(TLTR22 *module);
LTR22API_DllExport(INT) LTR22_ReadAVRFirmware(TLTR22 *module, BYTE *Data, DWORD size, DWORD BeginPage, DWORD PageNumbers);
LTR22API_DllExport(INT) LTR22_WriteAVRFirmvare(TLTR22 *module, BYTE *Data, DWORD size,
                                               BOOLEAN WriteCalibrAndDescription, BOOLEAN ProgrammAVR);



LTR22API_DllExport(INT) LTR22_FindAdcFreqParams(double adcFreq, BYTE *freqDiv, BOOLEAN *adc384, INT *adcFreqIndex, double *resultAdcFreq);

LTR22API_DllExport(double) LTR22_CalcAdcFreq(BYTE freqDiv, BOOLEAN adc384);

#ifndef LTRAPI_DISABLE_COMPAT_DEFS
    #define LTR22_ADC_NUMBERS           LTR22_CHANNEL_CNT
    #define LTR22_ADC_CHANNELS          LTR22_CHANNEL_CNT
    #define LTR22_RANGE_NUMBER          LTR22_RANGE_CNT
    #define LTR22_MAX_DISC_FREQ_NUMBER  LTR22_ADC_FREQ_CNT


    #define LTR22_ReadAVRBroaching  LTR22_ReadAVRFirmware
    #define LTR22_WriteBroaching    LTR22_WriteAVRFirmvare
    #define LTR22_GetCalibrovka     LTR22_GetCalibrCoeffs
    typedef TLTR22_ADC_CHANNEL_CALIBRATION ADC_CHANNEL_CALIBRATION;
#endif

#ifdef __cplusplus
}
#endif

#endif
