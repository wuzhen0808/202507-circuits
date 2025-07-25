
#include "notify.h"
#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_13

int32_t BSP_LED_Init()
{
    
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable the GPIO LED Clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure the GPIO_LED pin */

    gpio_init_structure.Pin = LED_PIN;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED_PORT, &gpio_init_structure);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

    return 0;
}

void BSP_LED_On(Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}
void BSP_LED_Off(Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
}
void Fail_Notify()
{
    BSP_LED_On(LED_GREEN);
    HAL_Delay(500);
    BSP_LED_Off(LED_GREEN);
    HAL_Delay(100);
    for (int i = 0; i < 5; i++)
    {
        BSP_LED_On(LED_GREEN);
        HAL_Delay(50);
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(50);
    }
}

void Ok_Notify()
{
    BSP_LED_On(LED_GREEN);
    HAL_Delay(500);
    BSP_LED_Off(LED_GREEN);
    HAL_Delay(100);
    for (int i = 0; i < 2; i++)
    {
        BSP_LED_On(LED_GREEN);
        HAL_Delay(125);
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(125);
    }
}