#include "LM5066.h"
#include "Wire.h"
#include "a8/util.h"

namespace a9
{
    using a8::util::Array;

    LM5066::LM5066(uint8_t address, AIP31068 *aip31068, PMBus * pmbus)
    {
        this->address = address;
        this->aip31068 = aip31068;
        this->pmbus = pmbus;
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

    int LM5066::read(PMBus::Command & cmd, Buffer<char> &buffer)
    {
       return pmbus->read(this->address, cmd, buffer);
    }

}