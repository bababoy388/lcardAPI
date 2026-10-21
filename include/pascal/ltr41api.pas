unit ltr41api;
interface
uses windows, SysUtils, ltrapitypes, ltrapidefine, ltrapi;
const
// Коды ошибок
         LTR41_NO_ERR                         =0;
         LTR41_ERR_WRONG_MODULE_DESCR				  =-7001;
         LTR41_ERR_CANT_OPEN                  =-7002;
         LTR41_ERR_INVALID_CRATE_SN 			    =-7003;
         LTR41_ERR_INVALID_SLOT_NUM					  =-7004;
         LTR41_ERR_CANT_SEND_COMMAND 				  =-7005;
         LTR41_ERR_CANT_RESET_MODULE				  =-7006;
         LTR41_ERR_MODULE_NO_RESPONCE				  =-7007;
         LTR41_ERR_CANT_CONFIG                =-7008;
         LTR41_ERR_CANT_LAUNCH_SEC_MARK				=-7009;
         LTR41_ERR_CANT_STOP_SEC_MARK				  =-7010;
         LTR41_ERR_CANT_LAUNCH_START_MARK			=-7011;
         LTR41_ERR_LESS_WORDS_RECEIVED        =-7012;
         LTR41_ERR_PARITY_TO_MODULE           =-7013;
         LTR41_ERR_PARITY_FROM_MODULE         =-7014;
         LTR41_ERR_WRONG_SECOND_MARK_CONF			=-7015;
         LTR41_ERR_WRONG_START_MARK_CONF			=-7016;
         LTR41_ERR_CANT_READ_DATA 					  =-7017;
         LTR41_ERR_CANT_WRITE_EEPROM					=-7018;
         LTR41_ERR_CANT_READ_EEPROM					  =-7019;
         LTR41_ERR_WRONG_EEPROM_ADDR 				  =-7020;
         LTR41_ERR_CANT_READ_CONF_REC				  =-7021;
         LTR41_ERR_WRONG_CONF_REC             =-7022;
         LTR41_ERR_CANT_START_STREAM_READ     =-7023;
         LTR41_ERR_CANT_STOP_STREAM_READ      =-7024;
         LTR41_ERR_WRONG_IO_DATA						  =-7025;
    LTR41_ERR_WRONG_STREAM_READ_FREQ_SETTINGS =-7026;
         LTR41_ERR_ERROR_OVERFLOW					    =-7027;

 // Таймауты приема и передачи команд
         TIMEOUT_CMD_SEND							        =4000;
         TIMEOUT_CMD_RECIEVE							    =6000;


type

// Структура описания модуля
TINFO_LTR41=packed record
	    Name  :array[0..15]of char;
	    Serial:array[0..23]of char;
	    FirmwareVersion:array[0..7]of char;// Версия БИОСа
	    FirmwareDate   :array[0..15]of char;  // Дата создания данной версии БИОСа
end;

pTINFO_LTR41 = ^TINFO_LTR41;

TMarks=record
		SecondMark_Mode:integer; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
		StartMark_Mode:integer; //
end;

TLTR41=packed record
	  size:integer;   // размер структуры
	  Channel:TLTR;
	  StreamReadRate:double;
    Marks:TMarks;  // Структура для работы с временными метками
	  ModuleInfo:TINFO_LTR41;
end;

pTLTR41=^TLTR41;// Структура описания модуля




  Function  LTR41_Init            (module:pTLTR41):Integer; stdcall;
  Function  LTR41_Open            (module:pTLTR41; net_addr:Cardinal;net_port:WORD; crate_snCHAR:Pointer; slot_num:integer):Integer; stdcall;
  Function  LTR41_IsOpened        (module:pTLTR41):Integer; stdcall;
  Function  LTR41_Close           (module:pTLTR41):Integer; stdcall;
  Function  LTR41_ReadPort        (module:pTLTR41; InputDataDWORD:Pointer):Integer; stdcall;
  Function  LTR41_StartStreamRead (module:pTLTR41):Integer; stdcall;
  Function  LTR41_StopStreamRead  (module:pTLTR41):Integer; stdcall;
  Function  LTR41_Recv            (module:pTLTR41; dataDWORD:Pointer;tmarkDWORD:Pointer;size:Cardinal;timeout:Cardinal):Integer; stdcall;
  Function  LTR41_ProcessData     (module:pTLTR41; srcDWORD:Pointer;destDWORD:Pointer; sizeDWORD:Pointer):Integer; stdcall;
  Function  LTR41_Config          (module:pTLTR41):Integer; stdcall;
  Function  LTR41_StartSecondMark (module:pTLTR41):Integer; stdcall;
  Function  LTR41_StopSecondMark  (module:pTLTR41):Integer; stdcall;
  Function  LTR41_GetErrorString  (Error_Code:integer):Pointer; stdcall;
  Function  LTR41_MakeStartMark   (module:pTLTR41):Integer; stdcall;
  Function  LTR41_WriteEEPROM     (module:pTLTR41; Address:integer;val:byte):Integer; stdcall;
  Function  LTR41_ReadEEPROM      (module:pTLTR41; Address:integer;valBYTE:Pointer):Integer; stdcall;
implementation
 Function  LTR41_Init            (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_Open            (module:pTLTR41; net_addr:Cardinal;net_port:WORD; crate_snCHAR:Pointer; slot_num:integer):Integer; external 'ltr41api.dll';
  Function  LTR41_IsOpened        (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_Close           (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_ReadPort        (module:pTLTR41; InputDataDWORD:Pointer):Integer; external 'ltr41api.dll';
  Function  LTR41_StartStreamRead (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_StopStreamRead  (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_Recv            (module:pTLTR41; dataDWORD:Pointer;tmarkDWORD:Pointer;size:Cardinal;timeout:Cardinal):Integer; external 'ltr41api.dll';
  Function  LTR41_ProcessData     (module:pTLTR41; srcDWORD:Pointer;destDWORD:Pointer; sizeDWORD:Pointer):Integer; external 'ltr41api.dll';
  Function  LTR41_Config          (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_StartSecondMark (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_StopSecondMark  (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_GetErrorString  (Error_Code:integer):Pointer; external 'ltr41api.dll';
  Function  LTR41_MakeStartMark   (module:pTLTR41):Integer; external 'ltr41api.dll';
  Function  LTR41_WriteEEPROM     (module:pTLTR41; Address:integer;val:byte):Integer;  external 'ltr41api.dll';
  Function  LTR41_ReadEEPROM      (module:pTLTR41; Address:integer;valBYTE:Pointer):Integer; external 'ltr41api.dll';

end.








#pragma pack()

LTR41API_DllExport (INT) LTR41_Init(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_Open(PTLTR41 hnd, INT net_addr, WORD net_port, CHAR *crate_sn, INT slot_num);
LTR41API_DllExport (INT) LTR41_IsOpened(PTLTR41 hnd);       
LTR41API_DllExport (INT) LTR41_Close(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_ReadPort(PTLTR41 hnd, WORD *InputData);
LTR41API_DllExport (INT) LTR41_StartStreamRead(PTLTR41 hnd); 
LTR41API_DllExport (INT) LTR41_StopStreamRead(PTLTR41 hnd); 
LTR41API_DllExport (INT) LTR41_Recv(PTLTR41 hnd, DWORD *data, DWORD *tmark, DWORD size, DWORD timeout); 
LTR41API_DllExport (INT) LTR41_ProcessData(PTLTR41 hnd, DWORD *src, WORD *dest, DWORD *size);   
LTR41API_DllExport (INT) LTR41_Config(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_StartSecondMark(PTLTR41 hnd);  
LTR41API_DllExport (INT) LTR41_StopSecondMark(PTLTR41 hnd);  
LTR41API_DllExport (LPCSTR) LTR41_GetErrorString(INT Error_Code); 
LTR41API_DllExport (INT) LTR41_MakeStartMark(PTLTR41 hnd);
LTR41API_DllExport (INT) LTR41_WriteEEPROM(PTLTR41 hnd, INT Address, BYTE val); 
LTR41API_DllExport (INT) LTR41_ReadEEPROM(PTLTR41 hnd, INT Address, BYTE *val); 

 #ifdef __cplusplus 
 }
 
#endif
























