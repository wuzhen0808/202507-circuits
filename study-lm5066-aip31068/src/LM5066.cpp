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

}