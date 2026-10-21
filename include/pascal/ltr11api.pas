unit ltr11api;
interface
uses windows, SysUtils, ltrapi, ltrapitypes;
const
               LTR11_CLOCK                  =15000; // тактовая частота модуля в кГц */
               LTR11_MAX_CHANNEL            =32;    // Максимальное число физических каналов */
               LTR11_MAX_LCHANNEL           =128;   // Максимальное число логических каналов */
               LTR11_ADC_RANGEQNT           =4;     // количество входных диапазонов АЦП */
// Коды ошибок, возвращаемые функциями библиотеки */
               LTR11_ERR_INVALID_DESCR        =-1000; // указатель на описатель модуля равен NULL */
               LTR11_ERR_INVALID_ADCMODE      =-1001; // недопустимый режим запуска АЦП */
               LTR11_ERR_INVALID_ADCLCHQNT    =-1002; // недопустимое количество логических каналов */
               LTR11_ERR_INVALID_ADCRATE      =-1003; // недопустимое значение частоты дискретизации АЦП модуля*/
               LTR11_ERR_INVALID_ADCSTROBE    =-1004; // недопустимый источник тактовой частоты для АЦП */
               LTR11_ERR_GETFRAME             =-1005; // ошибка получения кадра данных с АЦП */
               LTR11_ERR_GETCFG               =-1006; // ошибка чтения конфигурации */
               LTR11_ERR_CFGDATA              =-1007; // ошибка при получении конфигурации модуля */
               LTR11_ERR_CFGSIGNATURE         =-1008; // неверное значение первого байта конфигурационной записи модуля */
               LTR11_ERR_CFGCRC               =-1009; // неверная контрольная сумма конфигурационной записи*/
               LTR11_ERR_INVALID_ARRPOINTER   =-1010; // указатель на массив равен NULL */
               LTR11_ERR_ADCDATA_CHNUM        =-1011; // неверный номер канала в массиве данных от АЦП */
               LTR11_ERR_INVALID_CRATESN      =-1012; // указатель на строку с серийным номером крейта равен NULL*/
               LTR11_ERR_INVALID_SLOTNUM      =-1013; // недопустимый номер слота в крейте */
               LTR11_ERR_NOACK                =-1014; // нет подтверждения от модуля */
               LTR11_ERR_MODULEID             =-1015; // попытка открытия модуля, отличного от LTR11 */
               LTR11_ERR_INVALIDACK           =-1016; // неверное подтверждение от модуля */
               LTR11_ERR_ADCDATA_SLOTNUM      =-1017; // неверный номер слота в данных от АЦП */
               LTR11_ERR_ADCDATA_CNT          =-1018; // неверный счетчик пакетов в данных от АЦП */
               LTR11_ERR_INVALID_STARTADCMODE =-1019; // неверный режим старта сбора данных */
// Режимы запуска АЦП */
               LTR11_ADCMODE_ACQ            =$00;  // сбор данных */
               LTR11_ADCMODE_TEST_U1P       =$04;  // подача тестового напряжения +U1 */
               LTR11_ADCMODE_TEST_U1N       =$05;  // подача тестового напряжения -U1 */
               LTR11_ADCMODE_TEST_U2N       =$06;  // подача тестового напряжения -U2 */
               LTR11_ADCMODE_TEST_U2P       =$07;  // подача тестового напряжения +U2 */
// Режим начала сбора данных модулем */
               LTR11_STARTADCMODE_INT       =0;     // внутренний старт =по команде хоста; */
               LTR11_STARTADCMODE_EXTRISE   =1;     // по фронту внешнего сигнала; */
               LTR11_STARTADCMODE_EXTFALL   =2;     // по спаду внешнего сигнала. */
// Источник тактирования АЦП */
               LTR11_INPMODE_EXTRISE        =0;     // запуск преобразования по фронту внешнего сигнала */
               LTR11_INPMODE_EXTFALL        =1;     // запуск преобразования по спаду внешнего сигнала */
               LTR11_INPMODE_INT            =2;     // внутренний запуск АЦП */
// Входные дипазоны каналов */
               LTR11_CHGANE_10000MV         =0;     // +-10 В (10000 мВ) */
               LTR11_CHGANE_2500MV          =1;     // +-2.5 В (2500 мВ) */
               LTR11_CHGANE_625MV           =2;     // +-0.625 В (625 мВ) */
               LTR11_CHGANE_156MV           =3;     // +-0.156 В (156 мВ) */
// Режимы работы каналов */
               LTR11_CHMODE_16CH            =0;     // 16-канальный */
               LTR11_CHMODE_32CH            =1;     // 32-канальный */

//#ifdef LTR11API_EXPORTS
//  #define LTR11API_DllExport(type) __declspec(dllexport) type APIENTRY
//#else
//  #define LTR11API_DllExport(type) __declspec(dllimport) type APIENTRY
//#endif

//================================================================================================*/
type

   LTR11_GainSet=packed record
        Offset:real;                      // смещение нуля */
        Gain  :real;                      // масштабный коэффициент */
   end;
   TINFO_LTR11=packed record              // информация о модуле */
        Name  :array[0..15]of char;         // название модуля (строка) */
        Serial:array[0..23]of char;         // серийный номер модуля (строка) */
        Ver   :word;                        // версия ПО модуля (младший байт - минорная,старший - мажорная
        Date:array[0..13]of char;           // дата создания ПО (строка) */
        CbrCoef:array[0..LTR11_ADC_RANGEQNT-1]of LTR11_GainSet ;      // калибровочные коэффициенты для каждого диапазона */
    end;


        ADC_SET=packed record
             divider:integer;               // делитель тактовой частоты модуля, значения:
                                            // 2..65535
                                            //
             prescaler:integer;             // пределитель тактовой частоты модуля:
                                            // 1, 8, 64, 256, 1024
                                            //
        end;


   TLTR11=packed record                     // информация о модуле LTR11 */
    size:integer;                           // размер структуры в байтах */
    Channel:TLTR;                           // описатель канала связи с модулем */
    StartADCMode:integer;                   // режим начала сбора данных:
                                            // LTR11_STARTADCMODE_INT     - внутренний старт (по
                                            //                              команде хоста);
                                            // LTR11_STARTADCMODE_EXTRISE - по фронту внешнего
                                            //                              сигнала;
                                            // LTR11_STARTADCMODE_EXTFALL - по спаду внешнего
                                            //                              сигнала.
                                            //
    InpMode:integer;                        // режим ввода данных с АЦП
                                            //  LTR11_INPMODE_INT     - внутренний запуск АЦП
                                            //                          (частота задается AdcRate)
                                            //  LTR11_INPMODE_EXTRISE - по фронту внешнего сигнала
                                            //  LTR11_INPMODE_EXTFALL - по спаду внешнего сигнала
                                            //
    LChQnt:integer;                         // число активных логических каналов (размер кадра) */
    LChTbl:array[0..LTR11_MAX_LCHANNEL-1]of byte;
                                            // управляющая таблица с активными логическими каналами
                                            // структура одного байта таблицы: MsbGGMMCCCCLsb
                                            //   GG   - входной диапазон:
                                            //          0 - +-10 В;
                                            //          1 - +-2.5 В;
                                            //          2 - +-0.625 В;
                                            //          3 - +-0.156В;
                                            //   MM   - режим:
                                            //          0 - 16-канальный, каналы 1-16;
                                            //          1 - измерение собственного напряжения
                                            //              смещения нуля;
                                            //          2 - 32-канальный, каналы 1-16;
                                            //          3 - 32-канальный, каналы 17-32;
                                            //   CCCC - номер физического канала:
                                            //          0 - канал 1 (17);
                                            //          . . .
                                            //          15 - канал 16 (32).
                                            //
    ADCMode:integer;                        // режим сбора данных или тип тестового режима */
    ADCRate:ADC_SET;                        // параметры для задания частоты дискретизации АЦП
                                            // частота рассчитывается по формуле:
                                            // F = LTR11_CLOCK/(prescaler*(divider+1)
                                            // ВНИМАНИЕ!!! Частота 400 кГц является особым случаем:
                                            // для ее установки пределитель и делитель должны иметь
                                            // следующие значения:
                                            //   prescaler = 1
                                            //   divider   = 36
                                            //
    ChRate:real;                            // частота одного канала в кГц (период кадра) при
                                            //внутреннем запуске АЦП
                                            //
    ModuleInfo:TINFO_LTR11;
    end;
//================================================================================================*/
    pTLTR11=^TLTR11;
//================================================================================================*/
Function LTR11_Close(module:pTLTR11):integer; stdcall;
Function LTR11_GetConfig(module:pTLTR11):integer;      stdcall;
Function LTR11_GetErrorString(err:integer):string; stdcall;
Function LTR11_GetFrame(module:pTLTR11; bufDWORD:Pointer):integer; stdcall;
Function LTR11_Init(module:pTLTR11):integer; stdcall;
Function LTR11_Open(module:pTLTR11; net_addrDWORD:Cardinal; net_portWORD:word; crate_snCHAR:Pointer;
    slot_numINT:integer):integer; stdcall;
Function LTR11_ProcessData(module:pTLTR11; srcDWORD:Pointer; destDOUBLE:Pointer; sizeINT:Pointer;
    calibrBOOL:boolean; voltBOOL:boolean):integer; stdcall;
Function LTR11_SetADC(module:pTLTR11):integer;   stdcall;
Function LTR11_Start(module:pTLTR11):integer; stdcall;
Function LTR11_Stop(module:pTLTR11):integer; stdcall;
//================================================================================================*/
implementation
      Function LTR11_Close(module:pTLTR11):integer; external 'ltr11api.dll';
      Function LTR11_GetConfig(module:pTLTR11):integer;external 'ltr11api.dll';
      Function LTR11_GetErrorString(err:integer):string;external 'ltr11api.dll';
      Function LTR11_GetFrame(module:pTLTR11; bufDWORD:Pointer):integer;external 'ltr11api.dll';
      Function LTR11_Init(module:pTLTR11):integer;external 'ltr11api.dll';
      Function LTR11_Open(module:pTLTR11; net_addrDWORD:Cardinal; net_portWORD:word; crate_snCHAR:Pointer;
                          slot_numINT:integer):integer;external 'ltr11api.dll';
      Function LTR11_ProcessData(module:pTLTR11; srcDWORD:Pointer; destDOUBLE:Pointer; sizeINT:Pointer;
                          calibrBOOL:boolean; voltBOOL:boolean):integer;external 'ltr11api.dll';
      Function LTR11_SetADC(module:pTLTR11):integer;external 'ltr11api.dll';
      Function LTR11_Start(module:pTLTR11):integer;external 'ltr11api.dll';
      Function LTR11_Stop(module:pTLTR11):integer;external 'ltr11api.dll';
end.
