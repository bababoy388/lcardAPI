unit ltr212api;
interface
uses windows, SysUtils, ltrapi, ltrapitypes;
const
        LTR_OK=0;
        LTR22_ADC_NUMBERS=4;
        LTR22_ADC_CHANNELS=LTR22_ADC_NUMBERS;
        LTR22_RANGE_NUMBER=6;
        LTR22_RANGE_OVERFLOW=7;
 // Коды ошибок. Описание см. ф-ю LRT212_GetErrorString()
        LTR212_NO_ERR=0;
        LTR212_ERR_INVALID_DESCR=               -2001;
        LTR212_ERR_INVALID_CRATE_SN=            -2002;
        LTR212_ERR_INVALID_SLOT_NUM=            -2003;
        LTR212_ERR_CANT_INIT=                   -2004;
        LTR212_ERR_CANT_OPEN_CHANNEL=           -2005;
        LTR212_ERR_CANT_CLOSE=                  -2006;
        LTR212_ERR_CANT_LOAD_BIOS=              -2007;
        LTR212_ERR_CANT_SEND_COMMAND=           -2008;
        LTR212_ERR_CANT_READ_EEPROM=            -2009;
        LTR212_ERR_CANT_WRITE_EEPROM=           -2010;
        LTR212_ERR_CANT_LOAD_IIR=               -2011;
        LTR212_ERR_CANT_LOAD_FIR=               -2012;
        LTR212_ERR_CANT_RESET_CODECS=           -2013;
        LTR212_ERR_CANT_SELECT_CODEC=           -2014;
        LTR212_ERR_CANT_WRITE_REG=              -2015;
        LTR212_ERR_CANT_READ_REG=               -2016;
        LTR212_ERR_WRONG_ADC_SETTINGS=          -2017;
        LTR212_ERR_WRONG_VCH_SETTINGS=          -2018;
        LTR212_ERR_CANT_SET_ADC=                -2019;
        LTR212_ERR_CANT_CALIBRATE=              -2020;
        LTR212_ERR_CANT_START_ADC=              -2021;
        LTR212_ERR_INVALID_ACQ_MODE=            -2022;
        LTR212_ERR_CANT_GET_DATA=               -2023;
        LTR212_ERR_CANT_MANAGE_FILTERS=         -2024;
        LTR212_ERR_CANT_STOP=                   -2025;
        LTR212_ERR_CANT_GET_FRAME=              -2026;
        LTR212_ERR_INV_ADC_DATA	=               -2026;
        LTR212_ERR_TEST_NOT_PASSED=             -2027;
        LTR212_ERR_CANT_WRITE_SERIAL_NUM =      -2028;
        LTR212_ERR_CANT_RESET_MODULE =          -2029;
        LTR212_ERR_MODULE_NO_RESPONCE =         -2030;
        LTR212_ERR_WRONG_FLASH_CRC=             -2031;
        LTR212_ERR_CANT_USE_FABRIC_AND_USER_CALIBR_SYM=-2032;
        LTR212_ERR_CANT_START_INTERFACE_TEST=   -2033;
        LTR212_ERR_WRONG_BIOS_FILE=             -2034;
        LTR212_ERR_CANT_USE_CALIBR_MODE	=       -2035;
        LTR212_ERR_PARITY_ERROR   =             -2036;
        LTR212_ERR_CANT_LOAD_CLB_COEFFS =       -2037;
        LTR212_ERR_CANT_LOAD_FABRIC_CLB_COEFFS =-2038;
        LTR212_ERR_CANT_GET_VER=                -2039;
        LTR212_ERR_CANT_GET_DATE=               -2040;
        LTR212_ERR_WRONG_SN	=                   -2041;
        LTR212_ERR_CANT_EVAL_DAC=               -2042;
        LTR212_ERR_ERROR_OVERFLOW	=             -2043;

        MAXTAPS=                      255;		// Максимальное значение порядка КИХ-фильтра
        MINTAPS=                      3;	   	// Минимальное значение порядка КИХ-фильтра
        ACQ_MODE_MEDIUM_PRECISION =   0;
        ACQ_MODE_HIGH_PRECISION    =  1;
        ACQ_MODE_8CH_HIGH_PRECISION=  2;
        TIMEOUT_CMD_SEND					=	  4000;
        TIMEOUT_CMD_RECIEVE				=	  6000;
        MAX_212_CH= 8;
 //******** Определение структуры описания модуля *************/
type
TINFO_LTR212=packed record
        Name:       array[0..15]of char;
        Serial:     array[0..23]of char;
        BiosVersion:array[0..7]of char; // Версия БИОСа
        BiosDate:   array[0..16]of char;// Дата создания данной версии БИОСа
end;

  Tfilter= record
           IIR:integer;         // Флаг использования БИХ-фильтра
           FIR:integer;         // Флаг использования КИХ-фильтра
           Decimation:integer;  // Значение коэффициента децимации для КИХ-фильтра
           TAP:integer;		 // Порядок КИХ-фильтра
           IIR_Name:array[0..512]of char; // Полный путь к файлу с коэфф-ми программного БИХ-фильтра
           FIR_Name:array[0..512]of char; // Полный путь к файлу с коэфф-ми программного КИХ-фильтра
   end;
TLTR212_Usr_Clb=record
	Offset   :array[0..MAX_212_CH-1]of DWORD;
	Scale    :array[0..MAX_212_CH-1]of DWORD;
	DAC_Value:array[0..MAX_212_CH-1]of byte;
end;

 ////  *PTINFO_LTR212;!!!!!!!!!!!!
TLTR212=packed record
  size:integer;
  Channel:TLTR;
  AcqMode:integer; // Режим сбора данных
  UseClb:integer;  // Флаг использования калибровочных коэфф-тов
  UseFabricClb:integer;// Флаг использования заводских калибровочных коэфф-тов
  LChQnt:integer;	 // Кол-во используемых виртуальных каналов
  LChTbl:array[0..7]of integer;  //Таблица виртуальных каналов
  REF:integer;		 // Флаг высокого опорного напряжения
  AC:integer;		 // Флаг знакопеременного опорного напряжения
  Fs:real;     // Частота дискретизации АЦП


  filter:Tfilter;

  ModuleInfo:TINFO_LTR212;
  CRC_PM:word; // для служебного пользования
  CRC_Flash_Eval:word; // для служебного пользования
  CRC_Flash_Read:word;   // для служебного пользования
end;
ltr212filter=packed record
           fs:double;
           decimation:byte;
           taps:byte;
           koeff:array[0..MAXTAPS]of Smallint;
end;
pTLTR212 = ^TLTR212;

// Доступные пользователю
Function LTR212_Init(module:pTLTR212):Integer; stdcall;
Function LTR212_IsOpened(module:pTLTR212):Integer;stdcall;
Function LTR212_Open(module:pTLTR212; net_addr:Cardinal; net_port:WORD; crate_snCHAR:Pointer; slot_num:integer; biosnameCHAR:Pointer):Integer;stdcall;
Function LTR212_Close(module:pTLTR212):Integer;stdcall;
Function LTR212_CreateLChannel(PhysChannel:integer; Scale:integer):Integer;stdcall;
Function LTR212_SetADC(module:pTLTR212):Integer;stdcall;
Function LTR212_Start(module:pTLTR212):Integer;stdcall;
Function LTR212_Stop(module:pTLTR212):Integer;stdcall;
Function LTR212_Recv(module:pTLTR212; dataDWORD:Pointer; tmarkDWORD:Pointer; size:Cardinal; timeout:Cardinal):Integer;stdcall;
Function LTR212_ProcessData(module:pTLTR212; srcDWORD:Pointer;  destDOUBLE:Pointer; sizeDWORD:Pointer; volt:Boolean):Integer;stdcall;
Function LTR212_GetErrorString(Error_Code:integer):Integer;stdcall;
Function LTR212_Calibrate(module:pTLTR212; LChannel_MaskBYTE:pointer; mode:integer; reset:integer):Integer;stdcall;
Function LTR212_CalcFS(module:pTLTR212; fsBaseDOUBLE:pointer; fs:pointer):Integer;stdcall;
Function LTR212_TestEEPROM(module:pTLTR212):Integer;stdcall;
// Вспомогательные функции
Function LTR212_ProcessDataTest(module:pTLTR212; srcDWORD:pointer;  destDOUBLE:Pointer; sizeDWORD:pointer; volt:boolean; bad_numDWORD:pointer):Integer;stdcall;
Function LTR212_ReadFilter(fnameCHAR:Pointer; filter_ltr212filter:Pointer):Integer;stdcall;
Function LTR212_WriteSerialNumber(module:pTLTR212; snCHAR:pointer; Code:WORD):Integer;stdcall;
Function LTR212_TestInterfaceStart(module:pTLTR212; PackDelay:integer):Integer;stdcall;
Function LTR212_CalcTimeOut(module:pTLTR212; n:Cardinal):Integer;stdcall;

Function LTR212_ReadUSR_Clb (module:pTLTR212; CALLIBR:pointer):Integer;stdcall;
Function LTR212_WriteUSR_Clb(module:pTLTR212; CALLIBR:pointer):Integer;stdcall;


implementation
      Function LTR212_Init(module:pTLTR212):Integer; external 'ltr212api.dll';
      Function LTR212_IsOpened(module:pTLTR212):Integer;external 'ltr212api.dll';
      Function LTR212_Open(module:pTLTR212; net_addr:Cardinal; net_port:WORD; crate_snCHAR:Pointer; slot_num:integer; biosnameCHAR:Pointer):Integer;external 'ltr212api.dll';
      Function LTR212_Close(module:pTLTR212):Integer;external 'ltr212api.dll';
      Function LTR212_CreateLChannel(PhysChannel:integer; Scale:integer):Integer;external 'ltr212api.dll';
      Function LTR212_SetADC(module:pTLTR212):Integer;external 'ltr212api.dll';
      Function LTR212_Start(module:pTLTR212):Integer;external 'ltr212api.dll';
      Function LTR212_Stop(module:pTLTR212):Integer;external 'ltr212api.dll';
      Function LTR212_Recv(module:pTLTR212; dataDWORD:Pointer; tmarkDWORD:Pointer; size:Cardinal; timeout:Cardinal):Integer;external 'ltr212api.dll';
      Function LTR212_ProcessData(module:pTLTR212; srcDWORD:Pointer;  destDOUBLE:Pointer; sizeDWORD:Pointer; volt:Boolean):Integer;external 'ltr212api.dll';
      Function LTR212_GetErrorString(Error_Code:integer):Integer;external 'ltr212api.dll';
      Function LTR212_Calibrate(module:pTLTR212; LChannel_MaskBYTE:pointer; mode:integer; reset:integer):Integer;external 'ltr212api.dll';
      Function LTR212_CalcFS(module:pTLTR212; fsBaseDOUBLE:pointer; fs:pointer):Integer;external 'ltr212api.dll';
      Function LTR212_TestEEPROM(module:pTLTR212):Integer;external 'ltr212api.dll';
      // Вспомогательные функции
      Function LTR212_ProcessDataTest(module:pTLTR212; srcDWORD:pointer;  destDOUBLE:Pointer; sizeDWORD:pointer; volt:boolean; bad_numDWORD:pointer):Integer;external 'ltr212api.dll';
      Function LTR212_ReadFilter(fnameCHAR:Pointer; filter_ltr212filter:Pointer):Integer;external 'ltr212api.dll';
      Function LTR212_WriteSerialNumber(module:pTLTR212; snCHAR:pointer; Code:WORD):Integer;external 'ltr212api.dll';
      Function LTR212_TestInterfaceStart(module:pTLTR212; PackDelay:integer):Integer;external 'ltr212api.dll';
      Function LTR212_CalcTimeOut(module:pTLTR212; n:Cardinal):Integer;external 'ltr212api.dll';

      Function LTR212_ReadUSR_Clb (module:pTLTR212; CALLIBR:pointer):Integer;external 'ltr212api.dll';
      Function LTR212_WriteUSR_Clb(module:pTLTR212; CALLIBR:pointer):Integer;external 'ltr212api.dll';
end.
