
#ifndef __STM32G4XX_IT_H
#define __STM32G4XX_IT_H

extern "C"
{
    void SysTick_Handler(void);
    void I2C1_EV_IRQHandler(void);
    void I2C1_ER_IRQHandler(void);
}
#endif