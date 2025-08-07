#ifndef __PMBUS_COMMAND_H__
#define __PMBUS_COMMAND_H__
#include <stdint.h>
#include <cmath>

#include "a8/util.h"
#define READ_CMD_ABUSE -1
#define TRANS_FAIL -2
#define AVAILABLE_FAIL -3
#define AVAILABLE_FAIL_2 -4

namespace a9
{
    using a8::util::Buffer;
    using a8::util::Format;
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

        class DataType
        {
        public:
            bool isFloat = false;
            bool isString = false;
            virtual void getAsFloat(Buffer<char> &buf, int len, float &fValue) {};
            virtual void getAsString(Buffer<char> &buf, int len, String &str) {};
        };

        struct Command
        {
            uint8_t code;
            uint8_t direction; // READ, WRITE, SEND
            uint8_t len;
            bool isBlock;
            DataType *dataType;
        };

        class AsciiDataType : public DataType
        {
        public:
            AsciiDataType()
            {
                this->isString = true;
            }
            void getAsString(Buffer<char> &buffer, int len, String &str) override
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
        };

        class HexDataType : public DataType
        {
        public:
            HexDataType()
            {
                this->isString = true;
            }
            void getAsString(Buffer<char> &buffer, int len, String &str) override
            {
                str.append(StringUtil::toHexString(buffer.buffer(), len));
            }
        };
        class DirectDataType : public DataType
        {
            uint16_t m;
            int16_t b;
            int16_t R;
            String unit;

        public:
            DirectDataType(uint16_t m, int16_t b, int16_t R, String unit = "")
                : m(m), b(b), R(R), unit(unit)
            {
                this->isFloat = true;
            }
            void getAsFloat(Buffer<char> &buf, int len, float &fValue) override
            {
                uint16_t value0 = buf[0];
                if (len == 2)
                {
                    value0 = buf[1] << 8 | buf[0];
                }

            
                fValue = ((float)value0 * pow<float, int>(10.0f, -R) - b) / (float)m;
            }

            void getAsString(Buffer<char> &buf, int len, String &str) override
            {
                float fValue;
                getAsFloat(buf, len, fValue);
                str.append(fValue);
                str.append(unit);
            }
        };

    private:
        AsciiDataType asciiDataType;
        HexDataType hexDataType;
        DirectDataType D_4587_1200_2_V = DirectDataType(4587, -1200, -2, "V");          // READ_VIN, READ_AVG_VIN,VIN_OV_WARN_LIMIT,VIN_UV_WARN_LIMIT
        DirectDataType D_4587_2400_2_V = DirectDataType(4587, -2400, -2, "V");          // READ_VOUT, READ_AVG_VOUT/VOUT_UV_WARN_LIMIT
        DirectDataType D_13793_0_1_V = DirectDataType(13793, 0, -1, "V");               // READ_VAUX
        DirectDataType D_10753_1200_2_A_CL_VDD = DirectDataType(10753, -1200, -2, "A"); // READ_IIN, READ_AVG_IIN(1)/MFR_IIN_OC_WARN_LIMIT
        DirectDataType D_5405_600_2_A_CL_GND = DirectDataType(5405, -600, -2, "A");     // READ_IN, READ_AVG_IN(1)/MFR_IIN_OC_WARN_LIMIT
        DirectDataType D_1204_6000_3_A_CL_VDD = DirectDataType(1204, -6000, -3, "W");   // READ_PIN, READ_AVG_PIN(1)/READ_PIN_PEAK/MFR_PIN_OP_WARN_LIMIT
        DirectDataType D_605_8000_3_A_CL_GND = DirectDataType(605, -8000, -3, "W");     // READ_PIN, READ_AVG_PIN(1)/READ_PIN_PEAK/MFR_PIN_OP_WARN_LIMIT
        DirectDataType D_16000_0_3_C = DirectDataType(16000, 0, -3, "°C");              //

        Buffer<Command *> commands;
        void add(uint8_t code, uint8_t direction,
                 uint8_t len,
                 bool isBlock, DataType *dataType = 0)
        {
            if (dataType == 0)
            {
                dataType = &hexDataType; // 默认使用十六进制数据类型
            }
            commands.add(new Command{code, direction, len, isBlock, dataType});
        }

    public:
        void getCommands(uint8_t direction, Buffer<Command *> &buf)
        {
            for (int i = 0; i < commands.len(); i++)
            {
                Command *cmd = commands.get(i, 0);
                if (cmd->direction & direction)
                {
                    buf.append(cmd);
                }
            }
        }

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

    public:
        PMBus()
        {
            add(0x01, READ | WRITE, 1, false);                         // OPERATION Retrieves or stores the operation status R/W 1 80h
            add(0x03, SEND, 0, false);                                 // CLEAR_FAULTS Clears the status registers and re-arms the black box registers for updating Send byte 0
            add(0x19, READ, 1, false);                                 // CAPABILITY Retrieves the device capability R 1 B0h
            add(0x43, READ | WRITE, 2, false);                         // VOUT_UV_WARN_LIMIT Retrieves or stores output undervoltage warn limit threshold R/W 2 0000h
            add(0x4F, READ | WRITE, 2, false);                         // OT_FAULT_LIMIT Retrieves or stores over temperature fault limit threshold R/W 2/0960h(150°C)
            add(0x51, READ | WRITE, 2, false);                         // OT_WARN_LIMIT Retrieves or stores over temperature warn limit threshold R/W 2/07D0h (125°C)
            add(0x57, READ | WRITE, 2, false);                         // VIN_OV_WARN_LIMIT Retrieves or stores input overvoltage warn limit threshold R/W 2 0FFFh
            add(0x58, READ | WRITE, 2, false);                         // VIN_UV_WARN_LIMIT Retrieves or stores input undervoltage warn limit threshold R/W 2 0000h
            add(0x78, READ, 1, false);                                 // STATUS_BYTE Retrieves information about the parts operating status R 1 49h
            add(0x79, READ, 2, false);                                 // STATUS_WORD Retrieves information about the parts operating status R 2 3849h
            add(0x7A, READ, 1, false);                                 // STATUS_VOUT Retrieves information about output voltage status R 1 00h
            add(0x7C, READ, 1, false);                                 // STATUS_INPUT Retrieves information about input status R 1 10h
            add(0x7D, READ, 1, false);                                 // STATUS_TEMPERATURE Retrieves information about temperature status R 1 00h
            add(0x7E, READ, 1, false);                                 // STATUS_CML Retrieves information about communications status R 1 00h
            add(0x80, READ, 1, false);                                 // STATUS_MFR_SPECIFIC Retrieves information about circuit breaker and MOSFET shorted status R 1 10h
            add(0x88, READ, 2, false, &D_4587_1200_2_V);               // READ_VIN Retrieves input voltage measurement R 2 0000h
            add(0x8B, READ, 2, false, &D_4587_2400_2_V);               // READ_VOUT Retrieves output voltage measurement R 2 0000h
            add(0x8D, READ, 2, false, &D_16000_0_3_C);                 // READ_TEMPERATURE_1 Retrieves temperature measurement R 2 0190h
            add(0x99, READ, 3, true, &asciiDataType);                  // MFR_ID Retrieves manufacturer ID in ASCII characters (NSC) R 3/4Eh/53h/43h
            add(0x9A, READ, 8, true, &asciiDataType);                  // MFR_MODEL Retrieves part number in ASCII characters. (LM5066) R 8/4Ch/4Dh/35h/30h/36h/36h/0h/0h
            add(0x9B, READ, 2, false);                                 // MFR_REVISION Retrieves part revision letter or number in ASCII (for example, AA) R 2/41h/41h/
            add(0xD0, READ, 2, false, &D_13793_0_1_V);                 // MFR_SPECIFIC_00/READ_VAUX Retrieves auxiliary voltage measurement R 2 0000h
            add(0xD1, READ, 2, false, &D_5405_600_2_A_CL_GND);         // MFR_SPECIFIC_01/MFR_READ_IIN Retrieves input current measurement R 2 0000h
            add(0xD2, READ, 2, false);                                 // MFR_SPECIFIC_02/MFR_READ_PIN Retrieves input power measurement R 2 0000h
            add(0xD3, READ | WRITE, 2, false, &D_5405_600_2_A_CL_GND); // MFR_SPECIFIC_03/MFR_IIN_OC_WARN_LIMIT Retrieves or stores input current limit warn threshold R/W 2 0FFFh
            add(0xD4, READ | WRITE, 2, false, &D_605_8000_3_A_CL_GND); // MFR_SPECIFIC_04/MFR_PIN_OP_WARN_LIMIT Retrieves or stores input power limit warn threshold R/W 2 0FFFh
            add(0xD5, READ, 2, false, &D_605_8000_3_A_CL_GND);         // MFR_SPECIFIC_05/READ_PIN_PEAK Retrieves measured peak input power measurement R 2 0000h
            add(0xD6, SEND, 0, false);                                 // MFR_SPECIFIC_06/CLEAR_PIN_PEAK Resets the contents of the peak input power register to 0 Send byte 0
            add(0xD7, READ | WRITE, 1, false);                         // MFR_SPECIFIC_07/GATE_MASK Allows the user to disable MOSFET gate shutdown for various fault conditions R/W 1 0000h
            add(0xD8, READ | WRITE, 2, false);                         // MFR_SPECIFIC_08/ALERT_MASK Retrieves or stores user SMBA fault mask R/W 2 0820h
            add(0xD9, READ | WRITE, 1, false);                         // MFR_SPECIFIC_09/DEVICE_SETUP Retrieves or stores information about number of retry attempts R/W 1 0000h
            add(0xDA, READ, 12, true, &asciiDataType);                 // MFR_SPECIFIC_10/BLOCK_READ Retrieves most recent diagnostic and telemetry information in a single transaction R 12/ 08E0h/0000h/0000h/0000h/0000h/0000h
            add(0xDB, READ | WRITE, 1, false);                         // MFR_SPECIFIC_11/SAMPLES_FOR_AVG Exponent value AVGN for number of samples to be averaged (N = 2 AVGN), range = 00h to 0Ch R/W 1 00h
            add(0xDC, READ, 2, false, &D_4587_1200_2_V);               // MFR_SPECIFIC_12/READ_AVG_VIN Retrieves averaged input voltage measurement R 2 0000h
            add(0xDD, READ, 2, false, &D_4587_2400_2_V);               // MFR_SPECIFIC_13/READ_AVG_VOUT Retrieves averaged output voltage measurement R 2 0000h
            add(0xDE, READ, 2, false, &D_5405_600_2_A_CL_GND);         // MFR_SPECIFIC_14/READ_AVG_IIN Retrieves averaged input current measurement R 2 0000h
            add(0xDF, READ, 2, false, &D_605_8000_3_A_CL_GND);         // MFR_SPECIFIC_15/READ_AVG_PIN Retrieves averaged input power measurement R 2 0000h
            add(0xE0, READ, 12, true, &asciiDataType);                 // MFR_SPECIFIC_16/BLACK_BOX_READ Captures diagnostic and telemetry information, which are latched when the first SMBA event occurs after faults are cleared R 12/08E0h/0000h/0000h/0000h/0000h/0000h
            add(0xE1, READ, 2, false);                                 // MFR_SPECIFIC_17/DIAGNOSTIC_WORD_READ Manufacturer-specific parallel of the STATUS_WORD to convey all FAULT/WARN data in a single transaction R 2 08E0h
            add(0xE2, READ, 12, true, &asciiDataType);                 // MFR_SPECIFIC_18/AVG_BLOCK_READ Retrieves most recent average telemetry and diagnostic information in a single transaction R 12/08E0h/0000h/0000h/0000h/0000h/0000h
        }
    };

}

#endif
