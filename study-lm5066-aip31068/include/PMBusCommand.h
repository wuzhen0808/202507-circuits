#ifndef __PMBUS_COMMAND_H__
#define __PMBUS_COMMAND_H__
#include <stdint.h>
namespace a9
{
    class PMBusCommand
    {
    public:
        uint8_t code;
        uint8_t len;

        PMBusCommand(uint8_t code, uint8_t len)
            : code(code), len(len)
        {
        }
    };
}
#endif
