unit ltr42api;
interface
uses windows, SysUtils, ltrapitypes, ltrapidefine, ltrapi;
const
// Коды ошибок
        LTR42_NO_ERR                          =0;
        LTR42_ERR_WRONG_MODULE_DESCR				  =-8001;
        LTR42_ERR_CANT_OPEN                   =-8002;
        LTR42_ERR_INVALID_CRATE_SN 			      =-8003;
        LTR42_ERR_INVALID_SLOT_NUM					  =-8004;
        LTR42_ERR_CANT_SEND_COMMAND 				  =-8005;
        LTR42_ERR_CANT_RESET_MODULE				    =-8006;
        LTR42_ERR_MODULE_NO_RESPONCE				  =-8007;
        LTR42_ERR_CANT_SEND_DATA					    =-8008;
        LTR42_ERR_CANT_CONFIG                 =-8009;
        LTR42_ERR_CANT_LAUNCH_SEC_MARK				=-8010;
        LTR42_ERR_CANT_STOP_SEC_MARK				  =-8011;
        LTR42_ERR_CANT_LAUNCH_START_MARK			=-8012;
        LTR42_ERR_DATA_TRANSMISSON_ERROR			=-8013;
        LTR42_ERR_LESS_WORDS_RECEIVED         =-8014;
        LTR42_ERR_PARITY_TO_MODULE            =-8015;
        LTR42_ERR_PARITY_FROM_MODULE          =-8016;
        LTR42_ERR_WRONG_SECOND_MARK_CONF			=-8017;
        LTR42_ERR_WRONG_START_MARK_CONF				=-8018;
        LTR42_ERR_CANT_READ_DATA 					    =-8019;
        LTR42_ERR_CANT_WRITE_EEPROM					  =-8020;
        LTR42_ERR_CANT_READ_EEPROM					  =-8021;
        LTR42_ERR_WRONG_EEPROM_ADDR 				  =-8022;
        LTR42_ERR_CANT_READ_CONF_REC				  =-8023;
        LTR42_ERR_WRONG_CONF_REC              =-8024;


type
// Структура описания модуля
TINFO_LTR42=packed record
	    Name  :array[0..15]of char;
	    Serial:array[0..23]of char;
	    FirmwareVersion:array[0..7]of char;// Версия БИОСа
	    FirmwareDate   :array[0..15]of char;  // Дата создания данной версии БИОСа
end;
pTINFO_LTR42 = ^TINFO_LTR42;

TMarks=record
		SecondMark_Mode:integer; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
		StartMark_Mode:integer; //
end;

TLTR42=packed record
    Channel:TLTR;
	  size:integer;   // размер структуры
    AckEna:boolean;
    Marks:TMarks;  // Структура для работы с временными метками
	  ModuleInfo:TINFO_LTR42;
end;
pTLTR42=^TLTR42;// Структура описания модуля



	Function  LTR42_Init            (module:pTLTR42):Integer; stdcall;
	Function  LTR42_Open            (module:pTLTR42; net_addr:Cardinal;net_port:WORD; crate_snCHAR:Pointer; slot_num:integer):Integer; stdcall;
	Function  LTR42_Close           (module:pTLTR42):Integer; stdcall;
	Function  LTR42_WritePort       (module:pTLTR42;OutputData:Cardinal):Integer; stdcall;
	Function  LTR42_WriteArray      (module:pTLTR42; OutputArrayDWORD:Pointer; ArraySize:byte):Integer; stdcall;
	Function  LTR42_Config          (module:pTLTR42):Integer; stdcall;
	Function  LTR42_StartSecondMark (module:pTLTR42):Integer; stdcall;
	Function  LTR42_StopSecondMark  (module:pTLTR42):Integer; stdcall;
	Function  LTR42_GetErrorString  (Error_Code:integer):Pointer; stdcall;
	Function  LTR42_MakeStartMark   (module:pTLTR42):Integer; stdcall;
	Function  LTR42_WriteEEPROM     (module:pTLTR42; Address:integer;val:byte):Integer; stdcall;
	Function  LTR42_ReadEEPROM      (module:pTLTR42;Address:integer;valBYTE:Pointer):Integer; stdcall;
	Function  LTR42_IsOpened        (module:pTLTR42):Integer; stdcall;
implementation
	Function  LTR42_Init            (module:pTLTR42):Integer;  external 'ltr42api.dll';
	Function  LTR42_Open            (module:pTLTR42; net_addr:Cardinal;net_port:WORD; crate_snCHAR:Pointer; slot_num:integer):Integer;  external 'ltr42api.dll';
	Function  LTR42_Close           (module:pTLTR42):Integer;  external 'ltr42api.dll';
	Function  LTR42_WritePort       (module:pTLTR42;OutputData:Cardinal):Integer;  external 'ltr42api.dll';
	Function  LTR42_WriteArray      (module:pTLTR42; OutputArrayDWORD:Pointer; ArraySize:byte):Integer;  external 'ltr42api.dll';
	Function  LTR42_Config          (module:pTLTR42):Integer;  external 'ltr42api.dll';
	Function  LTR42_StartSecondMark (module:pTLTR42):Integer;  external 'ltr42api.dll';
	Function  LTR42_StopSecondMark  (module:pTLTR42):Integer;  external 'ltr42api.dll';
	Function  LTR42_GetErrorString  (Error_Code:integer):Pointer;  external 'ltr42api.dll';
	Function  LTR42_MakeStartMark   (module:pTLTR42):Integer;  external 'ltr42api.dll';
	Function  LTR42_WriteEEPROM     (module:pTLTR42; Address:integer;val:byte):Integer;  external 'ltr42api.dll';
	Function  LTR42_ReadEEPROM      (module:pTLTR42;Address:integer;valBYTE:Pointer):Integer; external 'ltr42api.dll';
	Function  LTR42_IsOpened        (module:pTLTR42):Integer;  external 'ltr42api.dll';
end.
