#ifndef __SMBUS_H_
#define __SMBUS_H_
#include "stm32f4xx_hal.h"
#include "Lang.h"

class Device
{

public:
    Device();
    ~Device();
    int init();
};
#endif // __SMBUS_H_