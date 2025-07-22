
#include "stm32g4xx_hal_msp.h"
#include "main.h"

extern "C"
{

    void HAL_SMBUS_MspInit(SMBUS_HandleTypeDef *hsmbus)
    {
        __GPIOA_CLK_ENABLE();
        __HAL_RCC_I2C1_CLK_ENABLE();

        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = SMBUS_SCL_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP; /*GPIO_PULLUP if no external pullup*/

        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2CX;    // 复用为 I2C
        HAL_GPIO_Init(SMBUS_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = SMBUS_SDA_PIN;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2CX;    // 复用为 I2C
        HAL_GPIO_Init(SMBUS_GPIO_PORT, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0U, 1U);
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0U, 2U);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    }

    void HAL_SMBUS_MspDeInit(SMBUS_HandleTypeDef *hsmbus)
    {
        __HAL_RCC_I2C1_FORCE_RESET();
        __HAL_RCC_I2C1_RELEASE_RESET();
        HAL_GPIO_DeInit(SMBUS_GPIO_PORT, SMBUS_SCL_PIN);
        HAL_GPIO_DeInit(SMBUS_GPIO_PORT, SMBUS_SDA_PIN);
        HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    }
}