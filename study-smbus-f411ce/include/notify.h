#include <stm32f4xx_hal.h>

#ifndef __NOTIFY_H
#define __NOTIFY_H

typedef enum
{
    LED_GREEN
} Led_TypeDef;

int32_t BSP_LED_Init(void);
void BSP_LED_On(Led_TypeDef Led);
void BSP_LED_On(Led_TypeDef Led);

void Ok_Notify(void);
void Fail_Notify(void);

#endif