#ifndef LTRD_PROTOCOL_DEFS_H
#define LTRD_PROTOCOL_DEFS_H


/* команды крейт-контроллера */
#define LTR010DATA                (0x0000l)
#define LTR010USERDATA            (0x4000l)
#define LTR010CMD                 (0x8000l)
#define LTR010CRATE_CMD           (0xC000l)
#define LTR010_CMD_DATA_MASK      (0xC000l)

#define LTR010CMD_STOP            (LTR010CMD|0x00)
#define LTR010CMD_PROGR           (LTR010CMD|0x40)
#define LTR010CMD_RESET           (LTR010CMD|0x80)
#define LTR010CMD_INSTR           (LTR010CMD|0xC0)
#define LTR010_SLOT_MASK          (0x0Fl<<8)


#define LTR_SIG_ESC_MASK          (0x0000FFF0)        /* маска esc-последовательности
                                                       * сигнала сервер->клиент
                                                       */
#define LTR_SIG_ESC_VAL           (0x0000FFF0)        /* значение esc-последовательности
                                                       * сигнала сервер->клиент
                                                       */
#define LTR_SIG_MAX_DWORDS        0x0F
#define LTR_SIG_LEN(n)            ((n) & 0x0F)
#define LTR_SIG_TYPE_MASK         (0xFFFF0000)
/* Если (data_word & LTR_SIG_ESC_MASK) == LTR_SIG_ESC_VAL, то изъять из потока
 * LTR_SIG_LEN(data_word) слов и обработать их отдельно.
 */
#define LTR_SIG_TYPE_LEGACY       (0xFFFF0000)
#define LTR_SIG_TYPE_TSTAMP_EX    (0xFFFE0000)

#define LTR_SIGNAL(type, len)     (LTR_SIG_ESC_VAL | ((type) & LTR_SIG_TYPE_MASK) | LTR_SIG_LEN(len))
/* Готовые значения ESC-слов сигналов */
#define LTR_SIGNAL_RBUF_OVF       LTR_SIGNAL(LTR_SIG_TYPE_LEGACY, 0)
#define LTR_SIGNAL_TSTAMP         LTR_SIGNAL(LTR_SIG_TYPE_LEGACY, 1)



/* команды клиент-сервер */
#define CONTROL_COMMAND_SIZE                        8
#define CONTROL_COMMAND_START                       0xFFFFFFFF
#define CONTROL_COMMAND_BASE_MASK_                  0xFFFFFF00
#define CONTROL_COMMAND_BASE_                       0xABCDEF00

#define CONTROL_COMMAND_INIT_CONNECTION             (CONTROL_COMMAND_BASE_ + 0x00)
#define CONTROL_COMMAND_SET_SERVER_PROCESS_PRIORITY (CONTROL_COMMAND_BASE_ + 0x01) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_GET_CRATE_TYPE              (CONTROL_COMMAND_BASE_ + 0x02)
#define CONTROL_COMMAND_GET_SERVER_PROCESS_PRIORITY (CONTROL_COMMAND_BASE_ + 0x03) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_PUT_ARRAY                   (CONTROL_COMMAND_BASE_ + 0x80)
#define CONTROL_COMMAND_GET_ARRAY                   (CONTROL_COMMAND_BASE_ + 0x81)
#define CONTROL_COMMAND_GET_DESCRIPTION             (CONTROL_COMMAND_BASE_ + 0x82)
#define CONTROL_COMMAND_BOOT_LOADER_GET_DESCRIPTION (CONTROL_COMMAND_BASE_ + 0x83)
#define CONTROL_COMMAND_BOOT_LOADER_SET_DESCRIPTION (CONTROL_COMMAND_BASE_ + 0x84)
#define CONTROL_COMMAND_GET_CRATES                  (CONTROL_COMMAND_BASE_ + 0x87) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_GET_MODULES                 (CONTROL_COMMAND_BASE_ + 0x88)
#define CONTROL_COMMAND_GET_CRATE_RAW_DATA          (CONTROL_COMMAND_BASE_ + 0x89)
#define CONTROL_COMMAND_GET_CRATE_RAW_DATA_SIZE     (CONTROL_COMMAND_BASE_ + 0x8A)
#define CONTROL_COMMAND_LOAD_FPGA                   (CONTROL_COMMAND_BASE_ + 0x8B)
#define CONTROL_COMMAND_IP_GET_LIST                 (CONTROL_COMMAND_BASE_ + 0x91) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_ADD_LIST_ENTRY           (CONTROL_COMMAND_BASE_ + 0x92) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_DELETE_LIST_ENTRY        (CONTROL_COMMAND_BASE_ + 0x93) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_CONNECT                  (CONTROL_COMMAND_BASE_ + 0x94) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_DISCONNECT               (CONTROL_COMMAND_BASE_ + 0x95) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_CONNECT_ALL_AUTO         (CONTROL_COMMAND_BASE_ + 0x96) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_DISCONNECT_ALL           (CONTROL_COMMAND_BASE_ + 0x97) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_SET_FLAGS                (CONTROL_COMMAND_BASE_ + 0x98) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_GET_DISCOVERY_MODE       (CONTROL_COMMAND_BASE_ + 0x99) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_IP_SET_DISCOVERY_MODE       (CONTROL_COMMAND_BASE_ + 0x9A) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_GET_SERVER_VERSION          (CONTROL_COMMAND_BASE_ + 0xAA) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_GET_LOG_LEVEL               (CONTROL_COMMAND_BASE_ + 0xF0) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_SET_LOG_LEVEL               (CONTROL_COMMAND_BASE_ + 0xF1) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_RESTART_SERVER              (CONTROL_COMMAND_BASE_ + 0xF2) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_SHUTDOWN_SERVER             (CONTROL_COMMAND_BASE_ + 0xF3) /*SERVER_CONTROL*/
#define CONTROL_COMMAND_BOOT_LOADER_PUT_ARRAY       (CONTROL_COMMAND_BASE_ + 0xFD)
#define CONTROL_COMMAND_BOOT_LOADER_GET_ARRAY       (CONTROL_COMMAND_BASE_ + 0xFE)
#define CONTROL_COMMAND_CALL_APPLICATION            (CONTROL_COMMAND_BASE_ + 0xFF)
/*#define CONTROL_COMMAND_START_ALL_CRATE_MODULES     (CONTROL_COMMAND_BASE_ + 0x90)*/
#define LTR_CLIENT_CMD_EX_BASE                     (CONTROL_COMMAND_BASE_ + 0x321100)
#define LTR_CLIENT_CMD_EX_MSK                      (0xFFFF)
#define LTR_CLIENT_CMD_EX_HDR_SIZE                 (LTR_CLIENT_CMD_SIZE + 8)
#define LTR_CLIENT_CMD_GET_CRATE_STAT              (LTR_CLIENT_CMD_EX_BASE + 0)
#define LTR_CLIENT_CMD_GET_MODULE_STAT             (LTR_CLIENT_CMD_EX_BASE + 1)
#define LTR_CLIENT_CMD_GET_CRATE_DESCR             (LTR_CLIENT_CMD_EX_BASE + 2)
#define LTR_CLIENT_CMD_RESET_MODULE                (LTR_CLIENT_CMD_EX_BASE + 3)
#define LTR_CLIENT_CMD_SET_SERVER_PARAM            (LTR_CLIENT_CMD_EX_BASE + 4)
#define LTR_CLIENT_CMD_GET_SERVER_PARAM            (LTR_CLIENT_CMD_EX_BASE + 5)
#define LTR_CLIENT_CMD_GET_CRATES_WITH_INFO        (LTR_CLIENT_CMD_EX_BASE + 6)
#define LTR_CLIENT_CMD_SEND_CUSTOM_IOCTL_REQ       (LTR_CLIENT_CMD_EX_BASE + 7)


/* Отклики на команды сервера */
#define LTR_CLIENT_ACK_MODULE_IN_USE           0xABCDEFDD
#define LTR_CLIENT_ACK_GOOD                    0xABCDEFEE
#define LTR_CLIENT_ACK_BAD                     0xABCDEFFF
#define LTR_CLIENT_ACK_ERR_INVALID_CMD         0xABCDEF01
#define LTR_CLIENT_ACK_ERR_CRATE_NOT_FOUND     0xABCDEF02
#define LTR_CLIENT_ACK_ERR_EMPTY_SLOT          0xABCDEF03
#define LTR_CLIENT_ACK_ERR_UNSUP_FOR_SRV_CTL   0xABCDEF04
#define LTR_CLIENT_ACK_ERR_INVALID_PARAMS      0xABCDEF05
#define LTR_CLIENT_ACK_ERR_INVALID_IP_ENTRY    0xABCDEF06
#define LTR_CLIENT_ACK_ERR_INVALID_CRATE_SLOT  0xABCDEF07

#define CONTROL_ACKNOWLEDGE_SIZE                    4
#define CONTROL_ACKNOWLEDGE_MODULE_IN_USE           LTR_CLIENT_ACK_MODULE_IN_USE
#define CONTROL_ACKNOWLEDGE_GOOD                    LTR_CLIENT_ACK_GOOD
#define CONTROL_ACKNOWLEDGE_BAD                     LTR_CLIENT_ACK_BAD



/* распределение адресного пространства LTR010 */
#define SEL_AVR_DM                              0x82000000
#define SEL_AVR_PM                              0x83000000
#define SEL_DMA_TEST_FLAG                       0x84000000
#define SEL_FLASH_BUFFER                        0x85000000
#define SEL_FPGA_DATA                           0x86000000
#define SEL_FPGA_FLAGS                          0x87000000
#define SEL_FLASH_DSP                           0x88000000
#define SEL_FLASH_STATUS                        0x89000000
#define SEL_SDRAM                               0x8A000000
#define SEL_SRAM                                0x8B000000
#define SEL_DSP                                 0x90000000
#define SEL_DSP_SPI                             0x91000000
#define SEL_DSP_SPI_BOOT                        0x92000000
#define SEL_DSP_MEM                             0x93000000
#define SEL_SLOTS                               0x95000000


#define LTR010_FIRMWARE_SIZE                    (0x3000)
#define LTR010_BOOT_LOADER_SIZE                 (0x1000)
#define LTR010_FIRMWARE_VERSION_ADDRESS         (SEL_AVR_PM|0x2FF0)
#define LTR010_BOOT_LOADER_VERSION_ADDRESS      (SEL_AVR_PM|0x3FF0)
#define LTR010_MODULE_DESCRIPTOR_ADDRESS        (SEL_AVR_PM|0x3000)
#define LTR010_FIRMWARE_START_ADDRESS           (SEL_AVR_PM|0x0000)
#define LTR010_BOOT_LOADER_START_ADDRESS        (SEL_AVR_PM|0x3C00)

#define EP1K100_SIZE                            (164*1024+100)
#define EP1K50_SIZE                             ( 96*1024+100)
#define EP1K30_SIZE                             ( 58*1024+100)
#define EP1K10_SIZE                             ( 22*1024+100)
#define FPGA_INFO_SIZE                          250


#endif // LTRD_PROTCOL_DEFS_H
