
#ifndef AIP31068_H
#define AIP31068_H
#include "Device.h"
#include <LiquidCrystal_AIP31068_I2C.h>

class AIP31068 : public Device
{
private:
    uint8_t address;
    LiquidCrystal_AIP31068_I2C *lcd;
    char lastLine[16 + 1];

public:
    AIP31068(uint8_t address);
    ~AIP31068();
    int init();
    void print(const char *text);
};

#endif // AIP31068_H
