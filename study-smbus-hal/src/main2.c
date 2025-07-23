#include "notify.h"
static void Error_Handler(void);
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(I2C_HandleTypeDef *);

int main(void)
{

    I2C_HandleTypeDef hi2c1;
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init();

    MX_GPIO_Init();
    MX_I2C1_Init(&hi2c1);

    uint8_t slave1Address = 21; // 0x15
    uint8_t slave2Address = 64; // 0x40
    uint8_t slave1Ready = 0;
    uint8_t slave2Ready = 0;

    // 测试设备是否就绪

    HAL_StatusTypeDef ret1 = HAL_I2C_IsDeviceReady(&hi2c1, (uint8_t)(slave1Address << 1), 3, 5);
    if (ret1 == HAL_OK)
    { /* No ACK Received At That Address */
        // Device not ready at this address
        slave1Ready = 1; // Device found at this address
    }
    HAL_StatusTypeDef ret2 = HAL_I2C_IsDeviceReady(&hi2c1, (uint8_t)(slave2Address << 1), 3, 5);
    if (ret2 == HAL_OK)
    { /* No ACK Received At That Address */
        // Device not ready at this address
        slave2Ready = 1; // Device found at this address
    }

    while (1)
    {
        HAL_Delay(500);
        if (slave1Ready)
        {
            Ok_Notify();
        }
        else
        {
            Fail_Notify();
            // 设备未就绪处理
        }

        if (slave2Ready)
        {
            Ok_Notify();
        }
        else
        {
            Fail_Notify();
            // 设备未就绪处理
        }
    }
}

/**
 * @brief I2C1初始化
 */
static void MX_I2C1_Init(I2C_HandleTypeDef *pHi2c1)
{

    (pHi2c1)->Instance = I2C1;
    (pHi2c1)->Init.Timing = 0x00707CBB; // 100kHz @ 64MHz I2CCLK
    (pHi2c1)->Init.OwnAddress1 = 0;
    (pHi2c1)->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    (pHi2c1)->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    (pHi2c1)->Init.OwnAddress2 = 0;
    (pHi2c1)->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    (pHi2c1)->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    (pHi2c1)->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(pHi2c1) != HAL_OK)
    {
        Error_Handler();
    }

    // STM32G4不需要单独启用SMBUS模式
    // 但可以配置SMBus特定的超时设置
    // hi2c1.Instance->TIMEOUTR = (10 << 16) | (1 << 15); // 10ms超时    // 配置SMBus超时（可选）
}

/**
 * @brief GPIO初始化
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // 配置主PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 85;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    // 初始化CPU、AHB和APB总线时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

static void Error_Handler(void)
{
    while (1)
    {
        // 错误处理代码
        Fail_Notify();
    }
}