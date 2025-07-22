
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32_SMBUS_stack.h"
#include "stm32_PMBUS_stack.h"

#define SMBUS_GPIO_PORT GPIOB
#define SMBUS_SCL_PIN GPIO_PIN_8
#define SMBUS_SDA_PIN GPIO_PIN_9

#define SMBUS1 I2C1
#define GPIO_AF4_I2CX GPIO_AF4_I2C1

#define SMBUS_TIMING_10K 0x3042C3C7U  /* 10kHz speed */
#define SMBUS_TIMING_100K 0x30420F13U /* 100kHz speed */
#define SMBUS_TIMEOUT_DEFAULT 0x80618061U
#define BUTTONn 1U
#define USER_BUTTON_GPIO_PORT GPIOC
#define USER_BUTTON_PIN GPIO_PIN_13

#define BSP_BUTTON_USER_IT_PRIORITY 15U
#define USER_BUTTON_EXTI_LINE EXTI_LINE_13
#define USER_BUTTON_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define USER_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define BSP_ERROR_NONE 0
#define LEDn 1U
#define LED2_PIN GPIO_PIN_13
#define LED2_GPIO_PORT GPIOC

#define PMBC_STATUS_INPUT ((uint8_t)0x7C)
#define SLAVE_ADDRESS 0x15U

typedef enum
{
    BUTTON_USER = 0U
} Button_TypeDef;

typedef enum
{
    BUTTON_MODE_GPIO = 0U,
    BUTTON_MODE_EXTI = 1U
} ButtonMode_TypeDef;

typedef enum
{
    LED2 = 0U,
    LED_GREEN = LED2
} Led_TypeDef;

typedef void (*BSP_EXTI_LineCallback)(void);

void SystemClock_Config(void);

void Error_Check(SMBUS_StackHandleTypeDef *pStackContext);

int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);

void Error_Handler(void);

#endif /*MAIN_H*/