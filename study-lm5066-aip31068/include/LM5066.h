#ifndef __LM5066_H_
#define __LM5066_H_
#include "Device.h"

class LM5066 : public Device
{
private:
    SMBUS_HandleTypeDef hsmbus1;
    uint8_t address;

public:
    LM5066(uint8_t address);
    ~LM5066();
    int init();
    bool isReady();
};

#endif