#include "AIP31068.h"

AIP31068::AIP31068(uint8_t address)
{
    this->lcd = 0;
    this->address = address;
}

AIP31068::~AIP31068()
{
    if (this->lcd != 0)
    {
        delete this->lcd;
        this->lcd = 0;
    }
}

int AIP31068::init()
{
    if (IS_FAIL(Device::init()))
    {
        return -1;
    }

    this->lcd = new LiquidCrystal_AIP31068_I2C(this->address, 16, 2);

    Wire.setSCL(PB8);
    Wire.setSDA(PB9);

    this->lcd->init(); // initialize the lcd
    return OK;
}

void AIP31068::print(const char *text)
{
    this->lcd->clear();

    this->lcd->setCursor(0, 0);
    this->lcd->print(this->lastLine);

    this->lcd->setCursor(0, 1);
    this->lcd->print(text);

    this->lastLine[16] = '\0';
    for (int i = 0; i < 16; i++)
    {
        this->lastLine[i] = text[i];
        if (this->lastLine[i] == '\0')
        {
            break;
        }
    }
}
