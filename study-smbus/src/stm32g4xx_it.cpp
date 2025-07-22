#include "stm32g4xx_it.h"
#include "stm32g4xx_hal.h"
extern SMBUS_HandleTypeDef handle1;
extern "C"
{

  void SysTick_Handler(void)
  {
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    /* USER CODE BEGIN SysTick_IRQn 1 */

    /* USER CODE END SysTick_IRQn 1 */
  }

  void I2C1_EV_IRQHandler(void)
  {
    HAL_SMBUS_EV_IRQHandler(&handle1);
  }
  void I2C1_ER_IRQHandler(void)
  {
    HAL_SMBUS_ER_IRQHandler(&handle1);
  }
}
