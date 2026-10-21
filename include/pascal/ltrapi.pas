unit ltrapi;
interface
uses windows, SysUtils, ltrapitypes, ltrapidefine;

const
          LTR_OK                    =  0;     //*Выполнено без ошибок.*/
          LTR_ERROR_UNKNOWN         = -1;     //*Неизвестная ошибка.*/
          LTR_ERROR_PARAMETRS       = -2;     //*Ошибка входных параметров.*/
          LTR_ERROR_MEMORY_ALLOC    = -3;     //*Ошибка динамического выделения памяти.*/
          LTR_ERROR_OPEN_CHANNEL    = -4;     //*Ошибка открытия канала обмена с сервером.*/
          LTR_ERROR_OPEN_SOCKET     = -5;     //*Ошибка открытия сокета.*/
          LTR_ERROR_CHANNEL_CLOSED  = -6;     //*Ошибка. Канал обмена с сервером не создан.*/
          LTR_ERROR_SEND            = -7;     //*Ошибка отправления данных.*/
          LTR_ERROR_RECV            = -8;     //*Ошибка приема данных.*/
          LTR_ERROR_EXECUTE         = -9;     //*Ошибка обмена с крейт-контроллером.*/
          LTR_WARNING_MODULE_IN_USE = -10;    //*Канал обмена с сервером создан в текущей программе
                                                //и в какой - то еще*/
          LTR_ERROR_NOT_CTRL_CHANNEL= -11;    //* Номер канала для этой операции должен быть CC_CONTROL */                                                
type
// Значения для управления ножками процессора, доступными для пользовательского программирования
en_LTR_UserIoCfg=(LTR_USERIO_DIGIN1=1,// ножка является входом и подключена к DIGIN1
                  LTR_USERIO_DIGIN2   = 2,    // ножка является входом и подключена к DIGIN2
                  LTR_USERIO_DIGOUT   = 0,    // ножка является выходом (подключение см. en_LTR_DigOutCfg)
                  LTR_USERIO_DEFAULT  = LTR_USERIO_DIGOUT);
// Значения для управления выходами DIGOUTx
en_LTR_DigOutCfg=(
    LTR_DIGOUT_CONST0   = $00, // постоянный уровень логического "0"
    LTR_DIGOUT_CONST1   = $01, // постоянный уровень логической "1"
    LTR_DIGOUT_USERIO0  = $02, // выход подключен к ножке userio0 (PF1 в рев. 0, PF1 в рев. 1)
    LTR_DIGOUT_USERIO1  = $03, // выход подключен к ножке userio1 (PG13)
    LTR_DIGOUT_DIGIN1   = $04, // выход подключен ко входу DIGIN1
    LTR_DIGOUT_DIGIN2   = $05, // выход подключен ко входу DIGIN2
    LTR_DIGOUT_START    = $06, // на выход подаются метки "СТАРТ"
    LTR_DIGOUT_SECOND   = $07, // на выход подаются метки "СЕКУНДА"
    LTR_DIGOUT_DEFAULT  = LTR_DIGOUT_CONST0
    );
// Значения для управления метками "СТАРТ" и "СЕКУНДА"
en_LTR_MarkMode=(
    LTR_MARK_OFF                = $00, // метка отключена
    LTR_MARK_EXT_DIGIN1_RISE    = $01, // метка по фронту DIGIN1
    LTR_MARK_EXT_DIGIN1_FALL    = $02, // метка по спаду DIGIN1
    LTR_MARK_EXT_DIGIN2_RISE    = $03, // метка по фронту DIGIN2
    LTR_MARK_EXT_DIGIN2_FALL    = $04, // метка по спаду DIGIN2
    LTR_MARK_INTERNAL           = $05  // внутренняя генерация метки
    );

TLTR=record
     saddr:Cardinal;                     // сетевой адрес сервера
	   sport:word;                         // сетевой порт сервера
	   csn:SERNUMtext;                     // серийный номер крейта
	   cc:WORD;                            // номер канала крейта
	   flags:Cardinal;                     // флаги состояния канала
	   tmark:Cardinal;                     // последняя принятая метка времени
	   internal:Pointer;                   // указатель на канал
end;
pTLTR = ^TLTR;
TLTR_CONFIG=record
     userio:array[0..3]of WORD;
     digout:array[0..1]of WORD;
     digout_en:WORD;
end;

// Функции для внутреннего применения
Function LTR__GenericCtlFunc(
    	TLTR:Pointer;                       // дескриптор LTR
    	request_buf:Pointer;                // буфер с запросом
    	request_size:Cardinal;              // длина запроса (в байтах)
    	reply_buf:Pointer;                  // буфер для ответа (или NULL)
    	reply_size:Cardinal;                // длина ответа (в байтах)
    	ack_error_code:integer;             // какая ошибка, если ack не GOOD (0 = не использовать ack)
    	timeout:Cardinal                   // таймаут, мс
    ):integer; stdcall;

// Функции общего назначения



Function LTR_Init(ltr:pTLTR):integer;  stdcall;
Function LTR_Open(ltr:pTLTR):integer;  stdcall;
Function LTR_Close(ltr:pTLTR):integer;  stdcall;
Function LTR_IsOpened(ltr:pTLTR):integer;  stdcall;
Function LTR_Recv(ltr:pTLTR; dataDWORD:Pointer; tmarkDWORD:POINTER; size:Cardinal; timeout:Cardinal):integer;  stdcall;
Function LTR_Send(ltr:pTLTR; dataDWORD:Pointer; size:Cardinal; timeout:Cardinal):integer;  stdcall;
Function LTR_GetCrates(ltr:pTLTR; csnBYTE:Pointer):integer;  stdcall;
Function LTR_GetCrateModules(ltr:pTLTR; midWORD:Pointer):integer;  stdcall;
Function LTR_SetServerProcessPriority(ltr:pTLTR; Priority:Cardinal):integer;  stdcall;
Function LTR_GetCrateRawData(ltr:pTLTR; dataDWORD:Pointer; tmarkDWORD:Pointer; size:Cardinal; timeout:Cardinal):integer;  stdcall;
Function LTR_GetErrorString(error:integer):string;  stdcall;

Function    LTR_GetCrateInfo(ltr:pTLTR; CrateInfoTCRATE_INFO:Pointer):integer; stdcall;
Function    LTR_Config(ltr:pTLTR; confTLTR_CONFIG:Pointer):integer; stdcall;
Function    LTR_StartSecondMark(ltr:pTLTR; mode:en_LTR_MarkMode):integer; stdcall;
Function    LTR_StopSecondMark(ltr:pTLTR):integer; stdcall;
Function    LTR_MakeStartMark(ltr:pTLTR; mode:en_LTR_MarkMode):integer; stdcall;
Function    LTR_GetListOfIPCrates(ltr:pTLTR; max_entries:DWORD; ip_net:DWORD; ip_mask:DWORD; entries_foundDWORD:Pointer; entries_returnedDWORD:Pointer; info_array:TIPCRATE_ENTRY):integer; stdcall;
Function    LTR_AddIPCrate(ltr:pTLTR; ip_addr:DWORD; flags:DWORD; permanent:BOOL):integer; stdcall;
Function    LTR_DeleteIPCrate(ltr:pTLTR; ip_addr:DWORD; permanent:BOOL):integer; stdcall;
Function    LTR_ConnectIPCrate(ltr:pTLTR; ip_addr:DWORD):integer; stdcall;
Function    LTR_DisconnectIPCrate(ltr:pTLTR; ip_addr:DWORD):integer; stdcall;
Function    LTR_ConnectAllAutoIPCrates(ltr:pTLTR):integer; stdcall;
Function    LTR_DisconnectAllIPCrates(ltr:pTLTR):integer; stdcall;
Function    LTR_SetIPCrateFlags(ltr:pTLTR; ip_addr:DWORD; new_flags:DWORD; permanent:BOOL):integer; stdcall;
Function    LTR_GetIPCrateDiscoveryMode(ltr:pTLTR; enabledBOOL:Pointer; autoconnectBOOL:Pointer):integer; stdcall;
Function    LTR_SetIPCrateDiscoveryMode(ltr:pTLTR; enabled:BOOL; autoconnect:BOOL; permanent:BOOL):integer; stdcall;
Function    LTR_GetLogLevel(ltr:pTLTR; levelINT:Pointer):integer; stdcall;
Function    LTR_SetLogLevel(ltr:pTLTR; level:integer;  permanent:BOOL):integer; stdcall;
Function    LTR_ServerRestart(ltr:pTLTR):integer; stdcall;
Function    LTR_ServerShutdown(ltr:pTLTR):integer; stdcall;
Function    LTR_SetTimeout(ltr:pTLTR; ms:Cardinal):integer; stdcall;
Function    LTR_GetServerProcessPriority(ltr:pTLTR; Priority:Pointer):integer; stdcall;
Function    LTR_GetServerVersion(ltr:pTLTR; version:Pointer):integer; stdcall;


Implementation
  Function LTR__GenericCtlFunc(TLTR:Pointer;request_buf:Pointer;request_size:Cardinal;reply_buf:Pointer;reply_size:Cardinal;ack_error_code:integer;timeout:Cardinal):integer;external 'ltrapi.dll';
  Function LTR_Init(ltr:pTLTR):integer;external 'ltrapi.dll';
  Function LTR_Open(ltr:pTLTR):integer;external 'ltrapi.dll';
  Function LTR_Close(ltr:pTLTR):integer;external 'ltrapi.dll';
  Function LTR_IsOpened(ltr:pTLTR):integer;external 'ltrapi.dll';
  Function LTR_Recv(ltr:pTLTR; dataDWORD:Pointer; tmarkDWORD:POINTER; size:Cardinal; timeout:Cardinal):integer;external 'ltrapi.dll';
  Function LTR_Send(ltr:pTLTR; dataDWORD:Pointer; size:Cardinal; timeout:Cardinal):integer;external 'ltrapi.dll';
  Function LTR_GetCrates(ltr:pTLTR; csnBYTE:Pointer):integer;external 'ltrapi.dll';
  Function LTR_GetCrateModules(ltr:pTLTR; midWORD:Pointer):integer;external 'ltrapi.dll';
  Function LTR_SetServerProcessPriority(ltr:pTLTR; Priority:Cardinal):integer;external 'ltrapi.dll';
  Function LTR_GetCrateRawData(ltr:pTLTR; dataDWORD:Pointer; tmarkDWORD:Pointer; size:Cardinal; timeout:Cardinal):integer;external 'ltrapi.dll';
  Function LTR_GetErrorString(error:integer):string; external 'ltrapi.dll';

  Function LTR_GetCrateInfo(ltr:pTLTR; CrateInfoTCRATE_INFO:Pointer):integer; external 'ltrapi.dll';
  Function LTR_Config(ltr:pTLTR; confTLTR_CONFIG:Pointer):integer; external 'ltrapi.dll';
  Function LTR_StartSecondMark(ltr:pTLTR; mode:en_LTR_MarkMode):integer; external 'ltrapi.dll';
  Function LTR_StopSecondMark(ltr:pTLTR):integer; external 'ltrapi.dll';
  Function LTR_MakeStartMark(ltr:pTLTR; mode:en_LTR_MarkMode):integer; external 'ltrapi.dll';
  Function LTR_GetListOfIPCrates(ltr:pTLTR; max_entries:DWORD; ip_net:DWORD; ip_mask:DWORD; entries_foundDWORD:Pointer; entries_returnedDWORD:Pointer; info_array:TIPCRATE_ENTRY):integer; external 'ltrapi.dll';
  Function LTR_AddIPCrate(ltr:pTLTR; ip_addr:DWORD; flags:DWORD; permanent:BOOL):integer; external 'ltrapi.dll';
  Function LTR_DeleteIPCrate(ltr:pTLTR; ip_addr:DWORD; permanent:BOOL):integer; external 'ltrapi.dll';
  Function LTR_ConnectIPCrate(ltr:pTLTR; ip_addr:DWORD):integer; external 'ltrapi.dll';
  Function LTR_DisconnectIPCrate(ltr:pTLTR; ip_addr:DWORD):integer; external 'ltrapi.dll';
  Function LTR_ConnectAllAutoIPCrates(ltr:pTLTR):integer; external 'ltrapi.dll';
  Function LTR_DisconnectAllIPCrates(ltr:pTLTR):integer; external 'ltrapi.dll';
  Function LTR_SetIPCrateFlags(ltr:pTLTR; ip_addr:DWORD; new_flags:DWORD; permanent:BOOL):integer; external 'ltrapi.dll';
  Function LTR_GetIPCrateDiscoveryMode(ltr:pTLTR; enabledBOOL:Pointer; autoconnectBOOL:Pointer):integer; external 'ltrapi.dll';
  Function LTR_SetIPCrateDiscoveryMode(ltr:pTLTR; enabled:BOOL; autoconnect:BOOL; permanent:BOOL):integer; external 'ltrapi.dll';
  Function LTR_GetLogLevel(ltr:pTLTR; levelINT:Pointer):integer; external 'ltrapi.dll';
  Function LTR_SetLogLevel(ltr:pTLTR; level:integer;  permanent:BOOL):integer; external 'ltrapi.dll';
  Function LTR_ServerRestart(ltr:pTLTR):integer; external 'ltrapi.dll';
  Function LTR_ServerShutdown(ltr:pTLTR):integer; external 'ltrapi.dll';
  Function LTR_SetTimeout(ltr:pTLTR; ms:Cardinal):integer; external 'ltrapi.dll';
  Function LTR_GetServerProcessPriority(ltr:pTLTR; Priority:Pointer):integer; external 'ltrapi.dll';
  Function LTR_GetServerVersion(ltr:pTLTR; version:Pointer):integer; external 'ltrapi.dll';

end.


