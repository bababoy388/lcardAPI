unit ltr22api;

interface

uses windows, SysUtils;

const
        LTR_OK=0;
        LTR22_ADC_NUMBERS=4;
        LTR22_ADC_CHANNELS=LTR22_ADC_NUMBERS;
        LTR22_RANGE_NUMBER=6;
        LTR22_RANGE_OVERFLOW=7;

type

TLTR = packed record
        saddr:Cardinal;                       // сетевой адрес сервера
        sport:word;                       // сетевой порт сервера
        csn:array [0..15] of byte;                     // серийный номер крейта
        cc:word;                          // номер канала крейта
        flags:Cardinal;                       // флаги состояния канала
        tmark:Cardinal;                       // последняя принятая метка времени
        internal:Pointer;                   // указатель на канал
end;

DAC_CHANNEL_CALIBRATION=packed record
	FactoryCalibOffset:array [0..LTR22_RANGE_NUMBER] of real;
	FactoryCalibScale:array [0..LTR22_RANGE_NUMBER] of real;

	UserCalibOffset:array [0..LTR22_RANGE_NUMBER] of real;
	UserCalibScale:array [0..LTR22_RANGE_NUMBER] of real;
end;

TDESCRIPTION_MODULE=packed record                        //                                                          //
  CompanyName:array[0..16] of byte;                                  //
  DeviceName:array[0..16] of byte;                                   // название изделия
  SerialNumber:array[0..16] of byte;                                 // серийный номер изделия
  Revision:byte;                                         // ревизия изделия
  Comment:array[0..256] of byte;                          //
end;

TDESCRIPTION_CPU=packed record                        //                                                          //
  Active:byte;                                      // флаг достоверности остальных полей структуры
  Name:array[0..16] of byte;                                        // название
  ClockRate:double;
  FirmwareVersion:Cardinal;                          //
  Comment:array[0..256] of byte;                          //
end;                                     //

// описание модуля (не реализованно)
TDESCRIPTION_LTR22=packed record                   //                                   //
  Description:TDESCRIPTION_MODULE;    //
  Cpu:TDESCRIPTION_CPU;       //
end;               //


// структура, задающая режим работы АЦП для модуля E-440
TLTR22 = packed record
   //**** служебная информация
  ltr:TLTR;							// структура описывающая модуль в крейте – описание в ltrapi.pdf
  //**** настройки модуля
  // настройки модуля
  Fdiv_rg:byte;						// дивайзер частоты клоков 1..15
  Adc384:bool;						// дополнительный дивайзер частоты сэмплов true =3 false =4
  AC_DC_State:bool;					// состояние true =AC+DC false=AC
  MeasureADCZero:bool;				// измерение Zero true - включено false - выключено
  DataReadingProcessed:bool;		// состояние считывания АЦП true-АЦП считывается false - нет
  ADCChannelRange:array [0..LTR22_ADC_NUMBERS] of Byte;// предел имзерений АЦП по каналам 0 - 1В 1 - 0.3В 2 - 0.1В 3 - 0.03В 4 - 10В 5 - 3В

  ChannelEnabled:array [0..LTR22_ADC_NUMBERS] of bool;		// Состояние каналов, включен - true выключен - false

  SyncType:Byte;		// Тип синхронизации 0 - внутренний старт по сигналу Go
						//1 - фазировка
						//2 -  внешний старт
						//3 -  резервировано
  SyncMaster:bool;            // true - модуль генерит сигнал, false - модуль принимает синхросигнал

  ADCCalibration:array [1..LTR22_ADC_NUMBERS] of DAC_CHANNEL_CALIBRATION;
  ModuleDescription:TDESCRIPTION_LTR22;
end;
pTLTR22 = ^TLTR22;



Function LTR22_Init(module:pTLTR22) : Integer; stdcall;
Function LTR22_Open(module:pTLTR22; saddr:Cardinal; sport:Cardinal;csnArrayByte:Pointer;cc:short) : Integer; stdcall;
Function LTR22_Close(module:pTLTR22) : Integer; stdcall;
Function LTR22_IsOpened(module:pTLTR22) : Integer; stdcall;
Function LTR22_GetConfig(module:pTLTR22) : Integer; stdcall;
Function LTR22_SetConfig(module:pTLTR22) : Integer; stdcall;
Function LTR22_StartADC(module:pTLTR22; WaitSync:bool) : Integer; stdcall;
Function LTR22_StopADC(module:pTLTR22) : Integer; stdcall;
Function LTR22_ClearBuffer(module:pTLTR22; wait_response:bool) : Integer; stdcall;
Function LTR22_SetSyncPriority(module:pTLTR22; SyncMaster:bool) : Integer; stdcall;
Function LTR22_SyncPhaze(module:pTLTR22; timeout:Cardinal) : Integer; stdcall;
Function LTR22_SwitchADCStartStop(module:pTLTR22;Value:bool) : Integer; stdcall;
Function LTR22_SwitchMeasureADCZero(module:pTLTR22;Value:bool) : Integer; stdcall;
Function LTR22_SetFreq(module:pTLTR22;adc384:bool;Freq_dv:Byte) : Integer; stdcall;
Function LTR22_SetADCRange(module:pTLTR22;ADCChannel:Byte;ADCChannelRange:Byte) : Integer; stdcall;
Function LTR22_SetADCChannel(module:pTLTR22;ADCChannel:Byte;EnableADC:bool) : Integer; stdcall;
Function LTR22_GetCalibrovka(module:pTLTR22) : Integer; stdcall;
Function LTR22_Recv(module:pTLTR22; dataCardinalArray:Pointer; tstampCardinal:Pointer; size:Cardinal; timeout:Cardinal) : Integer; stdcall;
Function LTR22_GetModuleDescription(module:pTLTR22) : Integer; stdcall;
Function LTR22_ProcessData(module:pTLTR22; dataSourceCardinalArray:Pointer; dataDestinationDouble:Pointer;
                Size:Cardinal;calibrMainPset:bool;calibrExtraVolts:bool;OverflowFlagsByteArray:Pointer) : Integer; stdcall;

Function LTR22_ReadAVREEPROM(module:pTLTR22; dataByteArray:Pointer; address:short;size:short) : Integer; stdcall;
Function LTR22_WriteAVREEPROM(module:pTLTR22; dataByteArray:Pointer;address:short;size:short) : Integer; stdcall;

Function LTR22_GetErrorString(Index:Integer) : string; stdcall;

implementation
        Function LTR22_Init(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_Open(module:pTLTR22; saddr:Cardinal; sport:Cardinal;csnArrayByte:Pointer;cc:short) : Integer; external 'ltr22api.dll';
        Function LTR22_Close(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_IsOpened(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_GetConfig(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_SetConfig(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_StartADC(module:pTLTR22; WaitSync:bool) : Integer; external 'ltr22api.dll';
        Function LTR22_StopADC(module:pTLTR22) : Integer;external 'ltr22api.dll';
        Function LTR22_ClearBuffer(module:pTLTR22; wait_response:bool) : Integer; external 'ltr22api.dll';
        Function LTR22_SetSyncPriority(module:pTLTR22; SyncMaster:bool) : Integer; external 'ltr22api.dll';
        Function LTR22_SyncPhaze(module:pTLTR22; timeout:Cardinal) : Integer; external 'ltr22api.dll';
        Function LTR22_SwitchADCStartStop(module:pTLTR22;Value:bool) : Integer; external 'ltr22api.dll';
        Function LTR22_SwitchMeasureADCZero(module:pTLTR22;Value:bool) : Integer; external 'ltr22api.dll';
        Function LTR22_SetFreq(module:pTLTR22;adc384:bool;Freq_dv:Byte) : Integer; external 'ltr22api.dll';
        Function LTR22_SetADCRange(module:pTLTR22;ADCChannel:Byte;ADCChannelRange:Byte) : Integer; external 'ltr22api.dll';
        Function LTR22_SetADCChannel(module:pTLTR22;ADCChannel:Byte;EnableADC:bool) : Integer; external 'ltr22api.dll';
        Function LTR22_GetCalibrovka(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_Recv(module:pTLTR22; dataCardinalArray:Pointer; tstampCardinal:Pointer; size:Cardinal; timeout:Cardinal) : Integer; external 'ltr22api.dll';
        Function LTR22_GetModuleDescription(module:pTLTR22) : Integer; external 'ltr22api.dll';
        Function LTR22_ProcessData(module:pTLTR22; dataSourceCardinalArray:Pointer; dataDestinationDouble:Pointer;
                        Size:Cardinal;calibrMainPset:bool;calibrExtraVolts:bool;OverflowFlagsByteArray:Pointer) : Integer; external 'ltr22api.dll';

        Function LTR22_ReadAVREEPROM(module:pTLTR22; dataByteArray:Pointer; address:short;size:short) : Integer; external 'ltr22api.dll';
        Function LTR22_WriteAVREEPROM(module:pTLTR22; dataByteArray:Pointer;address:short;size:short) : Integer; external 'ltr22api.dll';

        Function LTR22_GetErrorString(Index:Integer) : string; external 'ltr22api.dll';
end.
