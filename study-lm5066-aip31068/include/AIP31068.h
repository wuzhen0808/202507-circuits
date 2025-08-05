
#ifndef AIP31068_H
#define AIP31068_H
#include "Device.h"
#include <LiquidCrystal_AIP31068_I2C.h>
#include <a8/util/Buffer.h>
namespace a9
{
    using a8::util::String;
    using StringBuffer = a8::util::Buffer<String*>;

    class AIP31068 : public Device
    {
    private:
        uint8_t address;
        LiquidCrystal_AIP31068_I2C *lcd;

        StringBuffer lines;

    public:
        AIP31068(uint8_t address);
        ~AIP31068();
        int init();
        void print(const char * text);
        
        void print(const String text){
            print(text.text());
        }

        void printLn()
        {
            print("\n");
        }
        void lnPrint(const String text)
        {
            printLn();
            print(text);
        }

    };

}
#endif // AIP31068_H
