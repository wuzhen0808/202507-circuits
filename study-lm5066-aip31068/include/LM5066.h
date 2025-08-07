#ifndef __LM5066_H_
#define __LM5066_H_
#include "Device.h"
#include "AIP31068.h"
#include "PMBus.h"
namespace a9
{
    class LM5066 : public Device
    {
    private:
        uint8_t address;
        AIP31068 *aip31068;
        PMBus *pmbus;

    public:
        LM5066(uint8_t address, AIP31068 *aip31068, PMBus *pmbus);
        ~LM5066();
        int init();
        bool isReady();
        int read(PMBus::Command &cmd, Buffer<char> &buffer);
    };
}
#endif