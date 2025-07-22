#ifndef __STM32G4XX_HAL_MSP_H
#define __STM32G4XX_HAL_MSP_H
#include <stm32g4xx_hal.h>

extern "C"{
    void HAL_SMBUS_MspInit(SMBUS_HandleTypeDef *hsmbus);
}

#endif