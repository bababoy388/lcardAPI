unit ltr43api;
interface
uses windows, SysUtils, ltrapitypes, ltrapidefine, ltrapi;
const
// Коды ошибок
        LTR43_NO_ERR                          =0;
        LTR43_ERR_WRONG_MODULE_DESCR				  =-4001;
        LTR43_ERR_CANT_OPEN                   =-4002;
        LTR43_ERR_INVALID_CRATE_SN 			      =-4003;
        LTR43_ERR_INVALID_SLOT_NUM					  =-4004;
        LTR43_ERR_CANT_SEND_COMMAND 				  =-4005;
        LTR43_ERR_CANT_RESET_MODULE				    =-4006;
        LTR43_ERR_MODULE_NO_RESPONCE				  =-4007;
        LTR43_ERR_CANT_SEND_DATA					    =-4008;
        LTR43_ERR_CANT_CONFIG                 =-4009;
        LTR43_ERR_CANT_RS485_CONFIG           =-4010;
        LTR43_ERR_CANT_LAUNCH_SEC_MARK				=-4011;
        LTR43_ERR_CANT_STOP_SEC_MARK				  =-4012;
        LTR43_ERR_CANT_LAUNCH_START_MARK			=-4013;
        LTR43_ERR_CANT_STOP_RS485RCV				  =-4014;
        LTR43_ERR_RS485_CANT_SEND_BYTE			  =-4015;
        LTR43_ERR_RS485_FRAME_ERR_RCV			    =-4016;
        LTR43_ERR_RS485_PARITY_ERR_RCV			  =-4017;
        LTR43_ERR_WRONG_IO_GROUPS_CONF			  =-4018;
        LTR43_ERR_RS485_WRONG_BAUDRATE			  =-4019;
        LTR43_ERR_RS485_WRONG_FRAME_SIZE			=-4020;
        LTR43_ERR_RS485_WRONG_PARITY_CONF			=-4021;
        LTR43_ERR_RS485_WRONG_STOPBIT_CONF		=-4022;
        LTR43_ERR_DATA_TRANSMISSON_ERROR			=-4023;
        LTR43_ERR_RS485_CONFIRM_TIMEOUT       =-4024;
        LTR43_ERR_RS485_SEND_TIMEOUT       		=-4025;
        LTR43_ERR_LESS_WORDS_RECEIVED         =-4026;
        LTR43_ERR_PARITY_TO_MODULE            =-4027;
        LTR43_ERR_PARITY_FROM_MODULE          =-4028;
        LTR43_ERR_WRONG_IO_LINES_CONF         =-4029;
        LTR43_ERR_WRONG_SECOND_MARK_CONF			=-4030;
        LTR43_ERR_WRONG_START_MARK_CONF				=-4031;
        LTR43_ERR_CANT_READ_DATA 					    =-4032;
        LTR43_ERR_RS485_CANT_SEND_PACK				=-4033;
        LTR43_ERR_RS485_CANT_CONFIGURE        =-4034;
        LTR43_ERR_CANT_WRITE_EEPROM					  =-4035;
        LTR43_ERR_CANT_READ_EEPROM					  =-4036;
        LTR43_ERR_WRONG_EEPROM_ADDR 				  =-4037;
        LTR43_ERR_RS485_WRONG_PACK_SIZE 			=-4038;
        LTR43_ERR_RS485_WRONG_OUT_TIMEOUT 		=-4039;
        LTR43_ERR_RS485_WRONG_IN_TIMEOUT 			=-4040;
        LTR43_ERR_CANT_READ_CONF_REC				  =-4041;
        LTR43_ERR_WRONG_CONF_REC              =-4042;
        LTR43_ERR_RS485_CANT_STOP_TST_RCV     =-4043;
        LTR43_ERR_CANT_START_STREAM_READ      =-4044;
        LTR43_ERR_CANT_STOP_STREAM_READ       =-4045;
        LTR43_ERR_WRONG_IO_DATA						    =-4046;
        LTR43_ERR_WRONG_STREAM_READ_FREQ_SETTINGS=-4047;
        LTR43_ERR_ERROR_OVERFLOW					    =-4048;


type
// Структура описания модуля
TINFO_LTR43=record
	    Name  :array[0..15]of char;
	    Serial:array[0..23]of char;
	    FirmwareVersion:array[0..7]of char;// Версия БИОСа
	    FirmwareDate   :array[0..15]of char;  // Дата создания данной версии БИОСа
end;

pTINFO_LTR43 = ^TINFO_LTR43;

TIO_Ports = record
 		Port1:integer;	   // направление линий ввода/вывода группы 1
		Port2:integer;	   // направление линий ввода/вывода группы 2
		Port3:integer;    // направление линий ввода/вывода группы 3
		Port4:integer;	   // направление линий ввода/вывода группы 4
end;



TRS485 = record
		FrameSize:integer;	  // Кол-во бит в кадре
		Baud:integer;		  // Скорость обмена в бодах
		StopBit:integer;	  // Кол-во стоп-бит
		Parity:integer;		  // Включение бита четности
		SendTimeoutMultiplier:integer; // Множитель таймаута отправки
		ReceiveTimeoutMultiplier:integer; // Множитель таймаута приема подтверждения
end;

TMarks=record
		SecondMark_Mode:integer; // Режим меток. 0 - внутр., 1-внутр.+выход, 2-внешн
		StartMark_Mode:integer; //
end;

TLTR43=record
	  size:integer;   // размер структуры
	  Channel:TLTR;
	  StreamReadRate:double;
    IO_Ports:TIO_Ports;
	  RS485:TRS485; // Структура для конфигурации RS485
	  Marks:TMarks;  // Структура для работы с временными метками
	  ModuleInfo:TINFO_LTR43;
end;
pTLTR43=^TLTR43;// Структура описания модуля
//pERRSTING=^string;
  Function  LTR43_Init                  (module:pTLTR43):Integer; stdcall;
  Function  LTR43_IsOpened              (module:pTLTR43):Integer; stdcall;
	Function  LTR43_Open                  (module:pTLTR43; net_addr:Cardinal;net_port:WORD; crate_snCHAR:Pointer; slot_num:integer):Integer; stdcall;
	Function  LTR43_Close                 (module:pTLTR43):Integer; stdcall;
	Function  LTR43_WritePort             (module:pTLTR43; OutputData:Cardinal):Integer; stdcall;
	Function  LTR43_WriteArray            (module:pTLTR43; OutputArrayDWORD:Pointer; ArraySize:byte):Integer; stdcall;
	Function  LTR43_ReadPort              (module:pTLTR43; InputDataDWORD:Pointer):Integer; stdcall;
	Function  LTR43_StartStreamRead       (module:pTLTR43):Integer; stdcall;
	Function  LTR43_StopStreamRead        (module:pTLTR43):Integer; stdcall;
	Function  LTR43_Recv                  (module:pTLTR43; dataDWORD:Pointer;tmarkDWORD:Pointer;size:Cardinal;timeout:Cardinal):Integer; stdcall;
	Function  LTR43_ProcessData           (module:pTLTR43; srcDWORD:Pointer;destDWORD:Pointer; sizeDWORD:Pointer):Integer; stdcall;
	Function  LTR43_Config                (module:pTLTR43):Integer; stdcall;
	Function  LTR43_StartSecondMark       (module:pTLTR43):Integer; stdcall;
	Function  LTR43_StopSecondMark        (module:pTLTR43):Integer; stdcall;
	Function  LTR43_GetErrorString        (Error_Code:integer):Pointer; stdcall;
	Function  LTR43_MakeStartMark         (module:pTLTR43):Integer; stdcall;
	Function  LTR43_RS485_Exchange        (module:pTLTR43; PackToSendSHORT:Pointer; ReceivedPack:Pointer; OutPackSize:integer; InPackSize:integer):Integer; stdcall;
	Function  LTR43_WriteEEPROM           (module:pTLTR43; Address:integer;val:byte):Integer; stdcall;
	Function  LTR43_ReadEEPROM            (module:pTLTR43; Address:integer;valBYTE:Pointer):Integer; stdcall;
	Function  LTR43_RS485_TestReceiveByte (module:pTLTR43; OutBytesQnt:integer;InBytesQnt:integer):Integer; stdcall;
	Function  LTR43_RS485_TestStopReceive (module:pTLTR43):Integer; stdcall;
implementation
  Function  LTR43_Init                  (module:pTLTR43):Integer; external 'ltr43api.dll';
  Function  LTR43_IsOpened              (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_Open                  (module:pTLTR43; net_addr:Cardinal;net_port:WORD; crate_snCHAR:Pointer; slot_num:integer):Integer; external 'ltr43api.dll';
	Function  LTR43_Close                 (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_WritePort             (module:pTLTR43; OutputData:Cardinal):Integer; external 'ltr43api.dll';
	Function  LTR43_WriteArray            (module:pTLTR43; OutputArrayDWORD:Pointer; ArraySize:byte):Integer; external 'ltr43api.dll';
	Function  LTR43_ReadPort              (module:pTLTR43; InputDataDWORD:Pointer):Integer; external 'ltr43api.dll';
	Function  LTR43_StartStreamRead       (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_StopStreamRead        (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_Recv                  (module:pTLTR43; dataDWORD:Pointer;tmarkDWORD:Pointer;size:Cardinal;timeout:Cardinal):Integer; external 'ltr43api.dll';
	Function  LTR43_ProcessData           (module:pTLTR43; srcDWORD:Pointer;destDWORD:Pointer; sizeDWORD:Pointer):Integer; external 'ltr43api.dll';
	Function  LTR43_Config                (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_StartSecondMark       (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_StopSecondMark        (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_GetErrorString        (Error_Code:integer):Pointer; external 'ltr43api.dll';
	Function  LTR43_MakeStartMark         (module:pTLTR43):Integer; external 'ltr43api.dll';
	Function  LTR43_RS485_Exchange        (module:pTLTR43; PackToSendSHORT:Pointer; ReceivedPack:Pointer; OutPackSize:integer; InPackSize:integer):Integer; external 'ltr43api.dll';
	Function  LTR43_WriteEEPROM           (module:pTLTR43; Address:integer;val:byte):Integer; external 'ltr43api.dll';
	Function  LTR43_ReadEEPROM            (module:pTLTR43; Address:integer;valBYTE:Pointer):Integer; external 'ltr43api.dll';
	Function  LTR43_RS485_TestReceiveByte (module:pTLTR43; OutBytesQnt:integer;InBytesQnt:integer):Integer; external 'ltr43api.dll';
	Function  LTR43_RS485_TestStopReceive (module:pTLTR43):Integer; external 'ltr43api.dll';

end.



















