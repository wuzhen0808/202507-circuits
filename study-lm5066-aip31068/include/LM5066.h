#ifndef __LM5066_H_
#define __LM5066_H_
#include "Device.h"
#include "AIP31068.h"
#include "PMBusCommand.h"
namespace a9
{
    class LM5066 : public Device
    {
    private:
        uint8_t address;
        AIP31068 *aip31068;

    public:
        LM5066(uint8_t address, AIP31068 *aip31068);
        ~LM5066();
        int init();
        bool isReady();
        int read(PMBusCommand cmd, char *buffer);
        int read(uint8_t code, uint8_t dataLen, char *buffer);
        int readByte(uint8_t code, char &data);     // 读取温度并存储在 temperature 中
        int readWord(uint8_t code, uint16_t &data); // 读取温度并存储在 temperature 中
        int readInt8(uint8_t code, int8_t &value);
    };
}
#endif