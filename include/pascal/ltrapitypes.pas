unit ltrapitypes;
interface
uses windows, SysUtils;

const
      COMMENT_LENGTH =256;
      ADC_CALIBRATION_NUMBER =256;
      DAC_CALIBRATION_NUMBER =256;
type
   // char16=array[0..15]of char;
    SERNUMtext=array[0..15]of char;

    TCRATE_INFO=record
      CrateType:byte;
      CrateInterface:byte;
    end;


    TDESCRIPTION_MODULE=record
      CompanyName:array[1..16]of char;                     //
      DeviceName:array[1..16]of char;                      // название изделия
      SerialNumber:array[1..16]of char;                    // серийный номер изделия
      Revision:byte;                                       // ревизия изделия
      Comment:array[0..256-1]of char;           //
    end;

// описание процессора и програмного обеспечения
    TDESCRIPTION_CPU=record
      Active:byte;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;                            // название
      ClockRate:double;                                    //
      FirmwareVersion:Cardinal;                            //
      Comment:array[0..COMMENT_LENGTH-1]of char;           //
    end;
// описание плис
    TDESCRIPTION_FPGA=record
      Active:byte;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;                            // название
      ClockRate:double;                                    //
      FirmwareVersion:Cardinal;                            //
      Comment:array[0..COMMENT_LENGTH-1]of char;           //
    end;
// описание ацп
    TDESCRIPTION_ADC=record
      Active:byte;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;                            // название
      Calibration:array[0..ADC_CALIBRATION_NUMBER-1]of double;// корректировочные коэффициенты
      Comment:array[0..COMMENT_LENGTH-1]of char;           //
    end;

    TDESCRIPTION_DAC =record
      Active:byte;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;                            // название
      Calibration:array[0..ADC_CALIBRATION_NUMBER-1]of double;// корректировочные коэффициенты
      Comment:array[0..COMMENT_LENGTH-1]of char;           //
    end;
// описание h-мезанинов
    TDESCRIPTION_MEZZANINE=record
      Active:byte;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;
      SerialNumber:array[0..15]of char;                    // серийный номер изделия
      Revision:Byte;                                       // ревизия изделия
      Calibration:array[0..3]of double;                    // корректировочные коэффициенты
      Comment:array[0..COMMENT_LENGTH-1]of char;           // комментарий
    end;
// описание цифрового вв
    TDESCRIPTION_DIGITAL_IO=record
      Active:byte;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;                            // название ???????
      InChannels:word;                                     // число каналов
      OutChannels:word;                                    // число каналов
      Comment:array[0..COMMENT_LENGTH-1]of char;           // комментарий
    end;
// описание интерфейсных модулей
    TDESCRIPTION_INTERFACE=record
      Active:BYTE;                                         // флаг достоверности остальных полей структуры
      Name:array[0..15]of char;                            // название
      Comment:array[0..COMMENT_LENGTH-1]of char;           //
    end;
  // элемент списка IP-крейтов
  TIPCRATE_ENTRY=record
    ip_addr:DWORD;                                          // IP адрес (host-endian)
    flags:DWORD;                                            // флаги режимов (CRATE_IP_FLAG_...)
    serial_number:array[0..15]of CHAR;                                 // серийный номер (если крейт подключен)
    is_dynamic:byte;                                        // 0 = задан пользователем, 1 = найден автоматически
    status:byte;                                            // состояние (CRATE_IP_STATUS_...)
  end;



implementation
end.
