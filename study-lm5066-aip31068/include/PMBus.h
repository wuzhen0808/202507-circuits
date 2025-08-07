#ifndef __PMBUS_COMMAND_H__
#define __PMBUS_COMMAND_H__
#include <stdint.h>
#include "a8/util.h"
#define READ_CMD_ABUSE -1
#define TRANS_FAIL -2
#define AVAILABLE_FAIL -3
#define AVAILABLE_FAIL_2 -4

namespace a9
{
    using a8::util::Buffer;
    using a8::util::StringUtil;

    using Formater = void (*)(Buffer<char> &, uint8_t, String &);

    class PMBus
    {
    public:
        enum Direction
        {
            SEND = 0xF0,
            READ = 0x01,
            WRITE = 0x02,

        };

        struct Command
        {
            uint8_t code;
            uint8_t direction; // READ, WRITE, SEND
            uint8_t len;
            bool isBlock;
            Formater formater;
        };

        static void formatAscii(Buffer<char> &buffer, uint8_t len, String &str)
        {
            int aLen = buffer[0];
            for (int i = 0; i < aLen; i++)
            {
                char ch = buffer[i + 1];
                if (ch == '\0')
                {
                    break;
                }
                str.append(ch);
            }
        }

        static void formatHex(Buffer<char> &buffer, uint8_t len, String &str)
        {
            str.append(StringUtil::toHexString(buffer.buffer(), len));
        }

        static constexpr Formater nullFormater = 0;
        static constexpr Formater emptyFormater = [](Buffer<char> &buffer, uint8_t len, String &str) -> void {

        };

        static constexpr Command COMMANDS[] = {
            {0x01, READ | WRITE, 1, false, formatHex}, // OPERATION Retrieves or stores the operation status R/W 1 80h
            {0x03, SEND, 0, false, nullFormater},      // CLEAR_FAULTS Clears the status registers and re-arms the black box registers for updating Send byte 0
            {0x19, READ, 1, false, formatHex},         // CAPABILITY Retrieves the device capability R 1 B0h
            {0x43, READ | WRITE, 2, false, formatHex}, // VOUT_UV_WARN_LIMIT Retrieves or stores output undervoltage warn limit threshold R/W 2 0000h
            {0x4F, READ | WRITE, 2, false, formatHex}, // OT_FAULT_LIMIT Retrieves or stores over temperature fault limit threshold R/W 2/0960h(150°C)
            {0x51, READ | WRITE, 2, false, formatHex}, // OT_WARN_LIMIT Retrieves or stores over temperature warn limit threshold R/W 2/07D0h (125°C)
            {0x57, READ | WRITE, 2, false, formatHex}, // VIN_OV_WARN_LIMIT Retrieves or stores input overvoltage warn limit threshold R/W 2 0FFFh
            {0x58, READ | WRITE, 2, false, formatHex}, // VIN_UV_WARN_LIMIT Retrieves or stores input undervoltage warn limit threshold R/W 2 0000h
            {0x78, READ, 1, false, formatHex},         // STATUS_BYTE Retrieves information about the parts operating status R 1 49h
            {0x79, READ, 2, false, formatHex},         // STATUS_WORD Retrieves information about the parts operating status R 2 3849h
            {0x7A, READ, 1, false, formatHex},         // STATUS_VOUT Retrieves information about output voltage status R 1 00h
            {0x7C, READ, 1, false, formatHex},         // STATUS_INPUT Retrieves information about input status R 1 10h
            {0x7D, READ, 1, false, formatHex},         // STATUS_TEMPERATURE Retrieves information about temperature status R 1 00h
            {0x7E, READ, 1, false, formatHex},         // STATUS_CML Retrieves information about communications status R 1 00h
            {0x80, READ, 1, false, formatHex},         // STATUS_MFR_SPECIFIC Retrieves information about circuit breaker and MOSFET shorted status R 1 10h
            {0x88, READ, 2, false, formatHex},         // READ_VIN Retrieves input voltage measurement R 2 0000h
            {0x8B, READ, 2, false, formatHex},         // READ_VOUT Retrieves output voltage measurement R 2 0000h
            {0x8D, READ, 2, false, formatHex},         // READ_TEMPERATURE_1 Retrieves temperature measurement R 2 0190h
            {0x99, READ, 3, true, formatAscii},        // MFR_ID Retrieves manufacturer ID in ASCII characters (NSC) R 3/4Eh/53h/43h
            {0x9A, READ, 8, true, formatAscii},        // MFR_MODEL Retrieves part number in ASCII characters. (LM5066) R 8/4Ch/4Dh/35h/30h/36h/36h/0h/0h
            {0x9B, READ, 2, false, formatHex},         // MFR_REVISION Retrieves part revision letter or number in ASCII (for example, AA) R 2/41h/41h/
            {0xD0, READ, 2, false, formatHex},         // MFR_SPECIFIC_00/READ_VAUX Retrieves auxiliary voltage measurement R 2 0000h
            {0xD1, READ, 2, false, formatHex},         // MFR_SPECIFIC_01/MFR_READ_IIN Retrieves input current measurement R 2 0000h
            {0xD2, READ, 2, false, formatHex},         // MFR_SPECIFIC_02/MFR_READ_PIN Retrieves input power measurement R 2 0000h
            {0xD3, READ | WRITE, 2, false, formatHex}, // MFR_SPECIFIC_03/MFR_IIN_OC_WARN_LIMIT Retrieves or stores input current limit warn threshold R/W 2 0FFFh
            {0xD4, READ | WRITE, 2, false, formatHex}, // MFR_SPECIFIC_04/MFR_PIN_OP_WARN_LIMIT Retrieves or stores input power limit warn threshold R/W 2 0FFFh
            {0xD5, READ, 2, false, formatHex},         // MFR_SPECIFIC_05/READ_PIN_PEAK Retrieves measured peak input power measurement R 2 0000h
            {0xD6, SEND, 0, false, nullFormater},      // MFR_SPECIFIC_06/CLEAR_PIN_PEAK Resets the contents of the peak input power register to 0 Send byte 0
            {0xD7, READ | WRITE, 1, false, formatHex}, // MFR_SPECIFIC_07/GATE_MASK Allows the user to disable MOSFET gate shutdown for various fault conditions R/W 1 0000h
            {0xD8, READ | WRITE, 2, false, formatHex}, // MFR_SPECIFIC_08/ALERT_MASK Retrieves or stores user SMBA fault mask R/W 2 0820h
            {0xD9, READ | WRITE, 1, false, formatHex}, // MFR_SPECIFIC_09/DEVICE_SETUP Retrieves or stores information about number of retry attempts R/W 1 0000h
            {0xDA, READ, 12, true, formatAscii},       // MFR_SPECIFIC_10/BLOCK_READ Retrieves most recent diagnostic and telemetry information in a single transaction R 12/ 08E0h/0000h/0000h/0000h/0000h/0000h
            {0xDB, READ | WRITE, 1, false, formatHex}, // MFR_SPECIFIC_11/SAMPLES_FOR_AVG Exponent value AVGN for number of samples to be averaged (N = 2 AVGN), range = 00h to 0Ch R/W 1 00h
            {0xDC, READ, 2, false, formatHex},         // MFR_SPECIFIC_12/READ_AVG_VIN Retrieves averaged input voltage measurement R 2 0000h
            {0xDD, READ, 2, false, formatHex},         // MFR_SPECIFIC_13/READ_AVG_VOUT Retrieves averaged output voltage measurement R 2 0000h
            {0xDE, READ, 2, false, formatHex},         // MFR_SPECIFIC_14/READ_AVG_IIN Retrieves averaged input current measurement R 2 0000h
            {0xDF, READ, 2, false, formatHex},         // MFR_SPECIFIC_15/READ_AVG_PIN Retrieves averaged input power measurement R 2 0000h
            {0xE0, READ, 12, true, formatAscii},       // MFR_SPECIFIC_16/BLACK_BOX_READ Captures diagnostic and telemetry information, which are latched when the first SMBA event occurs after faults are cleared R 12/08E0h/0000h/0000h/0000h/0000h/0000h
            {0xE1, READ, 2, false, formatHex},         // MFR_SPECIFIC_17/DIAGNOSTIC_WORD_READ Manufacturer-specific parallel of the STATUS_WORD to convey all FAULT/WARN data in a single transaction R 2 08E0h
            {0xE2, READ, 12, true, formatAscii},       // MFR_SPECIFIC_18/AVG_BLOCK_READ Retrieves most recent average telemetry and diagnostic information in a single transaction R 12/08E0h/0000h/0000h/0000h/0000h/0000h
        };

        static void getCommands(uint8_t direction, Buffer<Command> &buf)
        {
            for (const auto &cmd : COMMANDS)
            {
                if (cmd.direction & direction)
                {
                    buf.append(cmd);
                }
            }
        };

    public:
        int read(uint8_t address, Command &cmd, Buffer<char> &buffer)
        {
            if (!(cmd.direction | READ))
            {
                return READ_CMD_ABUSE; // Invalid command for reading
            }

            Wire.beginTransmission(address);
            Wire.write(cmd.code);                         // 发送命令
            uint8_t result = Wire.endTransmission(false); // false = 不发送 STOP，使用 Repeated Start

            if (result != 0)
            {
                return TRANS_FAIL; // 发送失败
            }

            uint8_t rawLen = cmd.len;
            if (cmd.isBlock)
            {
                rawLen = cmd.len + 1; // 如果是块读取，实际长度需要加1
            }

            Wire.requestFrom(address, rawLen);
            int availableBytes = Wire.available();

            if (availableBytes < rawLen)
            {
                return AVAILABLE_FAIL_2; // 没有可用数据
            }

            for (int i = 0; i < rawLen; i++)
            {
                char ch = Wire.read(); // 读取数据
                buffer.add(ch);        // 添加到缓冲区
            }

            return rawLen; // 返回读取的字节数
        }
    };

}

#endif
