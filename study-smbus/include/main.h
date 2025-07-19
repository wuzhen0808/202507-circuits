
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32_SMBUS_stack.h"
#include "stm32_PMBUS_stack.h"

#define SMBUS1 I2C1
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

static void SystemClock_Config(void);
static void Error_Check(SMBUS_StackHandleTypeDef *pStackContext);
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

EXTI_HandleTypeDef hExtiButtonHandle[BUTTONn];
SMBUS_HandleTypeDef handle1;
SMBUS_StackHandleTypeDef context1;

typedef enum
{
    BUTTON_USER = 0U
} Button_TypeDef;

typedef enum
{
    BUTTON_MODE_GPIO = 0U,
    BUTTON_MODE_EXTI = 1U
} ButtonMode_TypeDef;

typedef void (*BSP_EXTI_LineCallback)(void);

static GPIO_TypeDef *BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
static const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};

#endif /*MAIN_H*/