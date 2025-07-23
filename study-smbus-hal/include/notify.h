#include <stm32g4xx_hal.h>

#ifndef __NOTIFY_H
#define __NOTIFY_H

int32_t BSP_LED_Init(void);
void Ok_Notify(void);
void Fail_Notify(void);

#endif