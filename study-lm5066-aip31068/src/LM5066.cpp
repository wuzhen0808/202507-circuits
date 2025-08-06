#include "LM5066.h"
#include "Wire.h"
#define PMBC_STATUS_TEMPERATURE ((uint8_t)0x7D)
#define READ ((uint8_t)0x20) /*!< Read operation / fixed size read only command */
namespace a9
{

    LM5066::LM5066(uint8_t address, AIP31068 *aip31068)
    {
        this->address = address;
        this->aip31068 = aip31068;
    }
    LM5066::~LM5066()
    {
    }

    int LM5066::init()
    {
        if (Device::init() != OK)
        {
            return -1;
        }

        // pHsmbus1->Instance = I2C1;
        // pHsmbus1->Init.ClockSpeed = 100000;
        // pHsmbus1->Init.OwnAddress1 = 0;
        // pHsmbus1->Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
        // pHsmbus1->Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
        // pHsmbus1->Init.OwnAddress2 = 0;
        // pHsmbus1->Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
        // pHsmbus1->Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
        // pHsmbus1->Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
        // pHsmbus1->Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
        // if (HAL_SMBUS_Init(pHsmbus1) == HAL_OK)
        // {
        //     return OK;
        // }
        // else
        // {
        //     return -1;
        // }
        return 0;
    }

    bool LM5066::isReady()
    {

        // return status == HAL_OK;
        uint32_t start = millis();
        int trials = 3;
        uint32_t timeout = 1000; // milliseconds

        for (uint8_t i = 0; i < trials; i++)
        {
            if (millis() - start > timeout)
                return 3; // TIMEOUT
            Wire.beginTransmission(this->address);
            if (Wire.endTransmission(true) == 0)
            {
                return true; // OK
            }
            delay(100);
        }
        return false; // ERROR
    }

    int LM5066::read(uint8_t code, uint8_t dataLen, char *buffer)
    {
        Wire.beginTransmission(this->address);
        Wire.write(code);                             // 发送命令
        uint8_t result = Wire.endTransmission(false); // false = 不发送 STOP，使用 Repeated Start

        if (result != 0)
        {
            aip31068->print("<SdErr:").print(result).print(">");
            return -1; // 发送失败
        }

        Wire.requestFrom(this->address, dataLen);
        int availableBytes = Wire.available();
        if (availableBytes < dataLen)
        {
            aip31068->print("<AvErr:").print(availableBytes).print(">");
            return -1; // 没有可用数据
        }
        Wire.readBytes(buffer, dataLen); // 读取数据
        return dataLen;                  // 返回读取的字节数
    }

    int LM5066::readInt8(uint8_t code, int8_t &value)
    {
        char buf[1];
        int ret = read(code, 1, buf);
        if (ret == 1)
        {
            value = (int8_t)buf[0]; // 将读取的字节转换为 int8_t
            return ret;
        }
        else
        {
            return -1;
        }
    }
    int LM5066::readByte(uint8_t code, char &data)
    {
        char buf[1];
        int ret = read(code, 1, buf);
        if (ret == 1)
        {
            data = buf[0]; // 将读取的字节存储在 data 中
            return ret;
        }
        else
        {
            return -1;
        }
    }
    int LM5066::readWord(uint8_t code, uint16_t &data)
    {
        char buf[2];
        int ret = read(code, 2, buf);
        if (ret == 2)
        {
            data = (uint16_t)((buf[0] << 8) | (buf[1] & 0xFF)); // 将读取的字节转换为 uint16_t
            return ret;
        }
        else
        {
            return -1;
        }
    }
}