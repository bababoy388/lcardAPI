unit ltr51api;
interface
uses windows, SysUtils, ltrapitypes, ltrapidefine, ltrapi;
const

        RISING_EDGE                             =0; 
        FALLING_EDGE                            =1;
// Коды ошибок
        LTR51_NO_ERR                            =0;
        LTR51_ERR_WRONG_MODULE_DESCR				    =-5001;
        LTR51_ERR_CANT_OPEN                     =-5002;
        LTR51_ERR_CANT_LOAD_ALTERA              =-5003;
        LTR51_ERR_INVALID_CRATE_SN 			        =-5004;
        LTR51_ERR_INVALID_SLOT_NUM					    =-5005;
        LTR51_ERR_CANT_SEND_COMMAND 				    =-5006;
        LTR51_ERR_CANT_RESET_MODULE				      =-5007;
        LTR51_ERR_MODULE_NO_RESPONCE				    =-5008;
        LTR51_ERR_CANT_OPEN_MODULE					    =-5009;
        LTR51_ERR_PARITY_TO_MODULE              =-5010;
        LTR51_ERR_PARITY_FROM_MODULE            =-5011;
        LTR51_ERR_ALTERA_TEST_FAILED				    =-5012;
        LTR51_ERR_CANT_START_DATA_AQC				    =-5013;
        LTR51_ERR_CANT_STOP_DATA_AQC				    =-5014;
        LTR51_ERR_CANT_SET_FS                   =-5015;
        LTR51_ERR_CANT_SET_BASE                 =-5016;
        LTR51_ERR_CANT_SET_EDGE_MODE            =-5017;
        LTR51_ERR_CANT_SET_THRESHOLD            =-5018;
        LTR51_WRONG_DATA							          =-5019;
        LTR51_ERR_WRONG_HIGH_THRESOLD_SETTINGS  =-5020;
        LTR51_ERR_WRONG_LOW_THRESOLD_SETTINGS	  =-5021;
        LTR51_ERR_WRONG_FPGA_FILE					      =-5022;
        LTR51_ERR_CANT_READ_ID_REC 				      =-5023;
        LTR51_ERR_WRONG_ID_REC					        =-5024;
        LTR51_ERR_WRONG_FS_SETTINGS	         	  =-5025;
        LTR51_ERR_WRONG_BASE_SETTINGS	          =-5026;
        LTR51_ERR_CANT_WRITE_EEPROM					    =-5027;
        LTR51_ERR_CANT_READ_EEPROM					    =-5028;
        LTR51_ERR_WRONG_EEPROM_ADDR 				    =-5029;
        LTR51_ERR_WRONG_THRESHOLD_VALUES 		    =-5030;
        LTR51_ERR_ERROR_OVERFLOW	              =-5031;
        LTR51_ERR_MODULE_WRONG_ACQ_TIME_SETTINGS=-5032;
        LTR51_ERR_NOT_ENOUGH_POINTS					    =-5033;
        LTR51_ERR_WRONG_SRC_SIZE					      =-5034;

        TIMEOUT_CMD_SEND							          =4000;
        TIMEOUT_CMD_RECIEVE							        =6000;


// Структура описания модуля
type

TINFO_LTR51=record
	Name:array[0..15]of char;
	Serial:array[0..23]of char;
	FirmwareVersion:array[0..7]of char;  // Версия прошивки AVR
  FirmwareDate:array[0..15]of char;    // Дата создания данной версии прошивки AVR
  FPGA_Version:array[0..7]of char;     // Версия прошивки ПЛИС
end;

PTINFO_LTR51=^TINFO_LTR51;

TLTR51=record
   size:integer;              // размер структуры
   Channel:TLTR;
   ChannelsEna:WORD;          // Маска доступных каналов (показывает, какие субмодули подкл.)
   SetUserPars:integer;	      // Указывает, задаются ли Fs и Base пользователем
   LChQnt:integer;            // Количество логических каналов
   LChTbl:array[0..15]of cardinal;       // Таблица логических каналов
   Fs:double;                 // Частота выборки сэмплов
   Base:word;                 // Делитель частоты измерения
   F_Base:double;			        // Частота измерений F_Base=Fs/Base
   AcqTime:integer;           // Время сбора в миллисекундах
   TbaseQnt:integer;		      // Количество периодов измерений, необходимое для обеспечения указанного интревала измерения
   ModuleInfo:TINFO_LTR51;
end;
pTLTR51=^TLTR51;              // Структура описания модуля

Function  LTR51_Init            (hnd:pTLTR51):integer;   stdcall;
Function  LTR51_Open            (hnd:pTLTR51; net_addrDWORD:Cardinal; net_port:WORD; crate_snCHAR:POINTER; slot_num:integer; ttf_nameCHAR:PChar):integer;stdcall;
Function  LTR51_IsOpened        (hnd:pTLTR51):integer;stdcall;
Function  LTR51_Close           (hnd:pTLTR51):integer;stdcall;
Function  LTR51_GetErrorString  (Error_Code:integer):Pointer; stdcall;
Function  LTR51_WriteEEPROM     (hnd:pTLTR51; Address:integer; val:byte):integer;stdcall;
Function  LTR51_ReadEEPROM      (hnd:pTLTR51; Address:integer; valBYTE:Pointer):integer; stdcall;
Function  LTR51_CreateLChannel  (PhysChannel:integer; HighThresholdDOUBLE:Pointer; LowThresholdDOUBLE:Pointer; ThresholdRange:integer; EdgeMode:integer):Cardinal;stdcall;
Function  LTR51_Config          (hnd:pTLTR51):integer;stdcall;
Function  LTR51_Start           (hnd:pTLTR51):integer;stdcall;
Function  LTR51_Stop            (hnd:pTLTR51):integer;stdcall;
Function  LTR51_Recv            (hnd:pTLTR51; dataDWORD:Pointer; tmarkDWORD:Pointer; size:Cardinal; timeout:Cardinal):integer;stdcall;
Function  LTR51_ProcessData     (hnd:pTLTR51; srcDWORD:Pointer; destDWORD:Pointer; FrequencyDOUBLE:Pointer; sizeDWORD:pointer):integer;stdcall;
Function  LTR51_GetThresholdVals(hnd:pTLTR51; LChNumber:integer; HighThresholdDOUBLE:Pointer; LowThresholdDOUBLE:pointer; ThresholdRange:integer):integer;stdcall;
Function  LTR51_CalcTimeOut     (hnd:pTLTR51; n:integer):Cardinal;stdcall;
Function  LTR51_EvaluateFrequencies   (hnd:pTLTR51):integer;stdcall;

implementation

  Function  LTR51_Init            (hnd:pTLTR51):integer;external 'ltr51api.dll';
  Function  LTR51_Open            (hnd:pTLTR51; net_addrDWORD:Cardinal; net_port:WORD; crate_snCHAR:POINTER; slot_num:integer; ttf_nameCHAR:PChar):integer;external 'ltr51api.dll';
  Function  LTR51_IsOpened        (hnd:pTLTR51):integer;external 'ltr51api.dll';
  Function  LTR51_Close           (hnd:pTLTR51):integer;external 'ltr51api.dll';
  Function  LTR51_GetErrorString  (Error_Code:integer):Pointer;external 'ltr51api.dll';
  Function  LTR51_WriteEEPROM     (hnd:pTLTR51; Address:integer; val:byte):integer;external 'ltr51api.dll';
  Function  LTR51_ReadEEPROM      (hnd:pTLTR51; Address:integer; valBYTE:Pointer):integer;external 'ltr51api.dll';
  Function  LTR51_CreateLChannel  (PhysChannel:integer; HighThresholdDOUBLE:Pointer; LowThresholdDOUBLE:Pointer; ThresholdRange:integer; EdgeMode:integer):Cardinal;external 'ltr51api.dll';
  Function  LTR51_Config          (hnd:pTLTR51):integer;external 'ltr51api.dll';
  Function  LTR51_Start           (hnd:pTLTR51):integer;external 'ltr51api.dll';
  Function  LTR51_Stop            (hnd:pTLTR51):integer;external 'ltr51api.dll';
  Function  LTR51_Recv            (hnd:pTLTR51; dataDWORD:Pointer; tmarkDWORD:Pointer; size:Cardinal; timeout:Cardinal):integer;external 'ltr51api.dll';
  Function  LTR51_ProcessData     (hnd:pTLTR51; srcDWORD:Pointer; destDWORD:Pointer; FrequencyDOUBLE:Pointer; sizeDWORD:pointer):integer;external 'ltr51api.dll';
  Function  LTR51_GetThresholdVals(hnd:pTLTR51; LChNumber:integer; HighThresholdDOUBLE:Pointer; LowThresholdDOUBLE:pointer; ThresholdRange:integer):integer;external 'ltr51api.dll';
  Function  LTR51_CalcTimeOut     (hnd:pTLTR51; n:integer):Cardinal;external 'ltr51api.dll';
  Function  LTR51_EvaluateFrequencies   (hnd:pTLTR51):integer;external 'ltr51api.dll';

end.















