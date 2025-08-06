#ifndef __PMBUS_COMMAND_H__
#define __PMBUS_COMMAND_H__
#include <stdint.h>
#include "a8/util.h"

namespace a9
{
    using a8::util::Buffer;
    class PMBus
    {
    public:
        enum Direction : uint8_t
        {
            READ = 0b10000000,  // 读操作
            WRITE = 0b01000000, // 写操作
            SEND = 0b00100000,  // 发送操作
            NONE = 0b00000000   // 空操作
        };

        struct Command
        {
            uint8_t code;
            uint8_t direction;
            uint8_t len;
        };

        static constexpr Command COMMANDS[] = {
            {0x01, READ | WRITE, 1}, // OPERATION Retrieves or stores the operation status R/W 1 80h
            {0x03, SEND, 0},         // CLEAR_FAULTS Clears the status registers and re-arms the black box registers for updating Send byte 0
            {0x19, READ, 1},         // CAPABILITY Retrieves the device capability R 1 B0h
            {0x43, READ | WRITE, 2}, // VOUT_UV_WARN_LIMIT Retrieves or stores output undervoltage warn limit threshold R/W 2 0000h
            {0x4F, READ | WRITE, 2}, // OT_FAULT_LIMIT Retrieves or stores over temperature fault limit threshold R/W 2/0960h(150°C)
            {0x51, READ | WRITE, 2}, // OT_WARN_LIMIT Retrieves or stores over temperature warn limit threshold R/W 2/07D0h (125°C)
            {0x57, READ | WRITE, 2}, // VIN_OV_WARN_LIMIT Retrieves or stores input overvoltage warn limit threshold R/W 2 0FFFh
            {0x58, READ | WRITE, 2}, // VIN_UV_WARN_LIMIT Retrieves or stores input undervoltage warn limit threshold R/W 2 0000h
            {0x78, READ, 1},         // STATUS_BYTE Retrieves information about the parts operating status R 1 49h
            {0x79, READ, 2},         // STATUS_WORD Retrieves information about the parts operating status R 2 3849h
            {0x7A, READ, 1},         // STATUS_VOUT Retrieves information about output voltage status R 1 00h
            {0x7C, READ, 1},         // STATUS_INPUT Retrieves information about input status R 1 10h
            {0x7D, READ, 1},         // STATUS_TEMPERATURE Retrieves information about temperature status R 1 00h
            {0x7E, READ, 1},         // STATUS_CML Retrieves information about communications status R 1 00h
            {0x80, READ, 1},         // STATUS_MFR_SPECIFIC Retrieves information about circuit breaker and MOSFET shorted status R 1 10h
            {0x88, READ, 2},         // READ_VIN Retrieves input voltage measurement R 2 0000h
            {0x8B, READ, 2},         // READ_VOUT Retrieves output voltage measurement R 2 0000h
            {0x8D, READ, 2},         // READ_TEMPERATURE_1 Retrieves temperature measurement R 2 0190h
            {0x99, READ, 3},         // MFR_ID Retrieves manufacturer ID in ASCII characters (NSC) R 3/4Eh/53h/43h
            {0x9A, READ, 8},         // MFR_MODEL Retrieves part number in ASCII characters. (LM5066) R 8/4Ch/4Dh/35h/30h/36h/36h/0h/0h
            {0x9B, READ, 2},         // MFR_REVISION Retrieves part revision letter or number in ASCII (for example, AA) R 2/41h/41h/
            {0xD0, READ, 2},         // MFR_SPECIFIC_00/READ_VAUX Retrieves auxiliary voltage measurement R 2 0000h
            {0xD1, READ, 2},         // MFR_SPECIFIC_01/MFR_READ_IIN Retrieves input current measurement R 2 0000h
            {0xD2, READ, 2},         // MFR_SPECIFIC_02/MFR_READ_PIN Retrieves input power measurement R 2 0000h
            {0xD3, READ | WRITE, 2}, // MFR_SPECIFIC_03/MFR_IIN_OC_WARN_LIMIT Retrieves or stores input current limit warn threshold R/W 2 0FFFh
            {0xD4, READ | WRITE, 2}, // MFR_SPECIFIC_04/MFR_PIN_OP_WARN_LIMIT Retrieves or stores input power limit warn threshold R/W 2 0FFFh
            {0xD5, READ, 2},         // MFR_SPECIFIC_05/READ_PIN_PEAK Retrieves measured peak input power measurement R 2 0000h
            {0xD6, SEND, 0},         // MFR_SPECIFIC_06/CLEAR_PIN_PEAK Resets the contents of the peak input power register to 0 Send byte 0
            {0xD7, READ | WRITE, 1}, // MFR_SPECIFIC_07/GATE_MASK Allows the user to disable MOSFET gate shutdown for various fault conditions R/W 1 0000h
            {0xD8, READ | WRITE, 2}, // MFR_SPECIFIC_08/ALERT_MASK Retrieves or stores user SMBA fault mask R/W 2 0820h
            {0xD9, READ | WRITE, 1}, // MFR_SPECIFIC_09/DEVICE_SETUP Retrieves or stores information about number of retry attempts R/W 1 0000h
            {0xDA, READ, 12},        // MFR_SPECIFIC_10/BLOCK_READ Retrieves most recent diagnostic and telemetry information in a single transaction R 12/ 08E0h/0000h/0000h/0000h/0000h/0000h
            {0xDB, READ | WRITE, 1}, // MFR_SPECIFIC_11/SAMPLES_FOR_AVG Exponent value AVGN for number of samples to be averaged (N = 2 AVGN), range = 00h to 0Ch R/W 1 00h
            {0xDC, READ, 2},         // MFR_SPECIFIC_12/READ_AVG_VIN Retrieves averaged input voltage measurement R 2 0000h
            {0xDD, READ, 2},         // MFR_SPECIFIC_13/READ_AVG_VOUT Retrieves averaged output voltage measurement R 2 0000h
            {0xDE, READ, 2},         // MFR_SPECIFIC_14/READ_AVG_IIN Retrieves averaged input current measurement R 2 0000h
            {0xDF, READ, 2},         // MFR_SPECIFIC_15/READ_AVG_PIN Retrieves averaged input power measurement R 2 0000h
            {0xE0, READ, 12},        // MFR_SPECIFIC_16/BLACK_BOX_READ Captures diagnostic and telemetry information, which are latched when the first SMBA event occurs after faults are cleared R 12/08E0h/0000h/0000h/0000h/0000h/0000h
            {0xE1, READ, 2},         // MFR_SPECIFIC_17/DIAGNOSTIC_WORD_READ Manufacturer-specific parallel of the STATUS_WORD to convey all FAULT/WARN data in a single transaction R 2 08E0h
            {0xE2, READ, 12},        // MFR_SPECIFIC_18/AVG_BLOCK_READ Retrieves most recent average telemetry and diagnostic information in a single transaction R 12/08E0h/0000h/0000h/0000h/0000h/0000h
            {0x00, NONE, 0}          // END marker
        };

        static void getCommands(Direction direction, Buffer<Command> &buf)
        {
            for (const auto &cmd : COMMANDS)
            {
                if (cmd.direction & direction)
                {
                    buf.append(cmd);
                }
            }
        };
    };

}

#endif
