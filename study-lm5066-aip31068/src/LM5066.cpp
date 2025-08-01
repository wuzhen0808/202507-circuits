#include "LM5066.h"
LM5066::LM5066(uint8_t address)
{
    this->address = address;
}
LM5066::~LM5066()
{
    // Destructor implementation
}

int LM5066::init()
{
    if (Device::init() != OK)
    {
        return -1;
    }

    hsmbus1.Instance = I2C1;
    hsmbus1.Init.ClockSpeed = 100000;
    hsmbus1.Init.OwnAddress1 = 0;
    hsmbus1.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
    hsmbus1.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
    hsmbus1.Init.OwnAddress2 = 0;
    hsmbus1.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
    hsmbus1.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
    hsmbus1.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
    hsmbus1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
    if (HAL_SMBUS_Init(&hsmbus1) == HAL_OK)
    {
        return OK;
    }
    else
    {
        return -1;
    }
}

bool LM5066::isReady(){

    HAL_StatusTypeDef status = HAL_SMBUS_IsDeviceReady(&hsmbus1, (uint16_t)(this->address << 1), 3, 5);
    return status == HAL_OK;
}
