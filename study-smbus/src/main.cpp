

#include "main.h"

static GPIO_TypeDef *BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
static const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};
static GPIO_TypeDef *LED_PORT[LEDn] = {LED2_GPIO_PORT};
static const uint16_t LED_PIN[LEDn] = {LED2_PIN};
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};
EXTI_HandleTypeDef hExtiButtonHandle[BUTTONn];
SMBUS_HandleTypeDef handle1;
SMBUS_StackHandleTypeDef context1;
int LED_INIT = 0;
#include "stm32g4xx.h"

int main3(void)
{
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Off(LED_GREEN);

    while (1)
    {
        BSP_LED_On(LED_GREEN);
        HAL_Delay(500); // 延时 500ms
        //for (int i = 0; i < 1000000; i++); // 简单延时
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(500); // 延时 500ms
        //for (int i = 0; i < 1000000; i++); // 简单延时
    }
}

int main1(void)
{
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init(LED_GREEN);
    BSP_LED_On(LED_GREEN);
    while (1)
    {
        HAL_Delay(200U);
    }
}
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init(LED_GREEN);
    BSP_LED_On(LED_GREEN);
    handle1.Instance = SMBUS1;
    handle1.Init.Timing = SMBUS_TIMING_100K;
    handle1.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
    handle1.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;

    handle1.Init.DualAddressMode = SMBUS_DUALADDRESS_ENABLE;
    handle1.Init.OwnAddress2 = 0x40U;
    handle1.Init.OwnAddress2Masks = SMBUS_OA2_MASK05;

    handle1.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
    handle1.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;

    handle1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_HOST;
    handle1.Init.OwnAddress1 = 0U;

    handle1.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
    handle1.Init.SMBusTimeout = SMBUS_TIMEOUT_DEFAULT;
    handle1.pBuffPtr = context1.Buffer;

    HAL_SMBUS_Init(&handle1);
    context1.CMD_table = (st_command_t *)&PMBUS_COMMANDS_TAB[0];
    context1.CMD_tableSize = PMBUS_COMMANDS_TAB_SIZE;
    context1.Device = &handle1;
    context1.SRByte = 0x55U;
    context1.CurrentCommand = NULL;
    context1.StateMachine = SMBUS_SMS_ARP_AR;
    context1.OwnAddress = SMBUS_ADDR_DEVICE;

    STACK_SMBUS_Init(&context1);

    while (1)
    {
        HAL_Delay(200U);
        Error_Check(&context1);
        if (context1.StateMachine & (SMBUS_SMS_QUICK_CMD_W | SMBUS_SMS_QUICK_CMD_R))
        {
            context1.StateMachine &= ~(SMBUS_SMS_QUICK_CMD_W | SMBUS_SMS_QUICK_CMD_R);
        }

        context1.StateMachine &= ~SMBUS_SMS_ERR_ARLO;

        BSP_LED_On(LED_GREEN);
        HAL_Delay(1000); 
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(1000); 
    }
    return 1;
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_CRSInitTypeDef pInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the SYSCFG APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();

  /** Configures CRS
  */
  pInit.Prescaler = RCC_CRS_SYNC_DIV1;
  pInit.Source = RCC_CRS_SYNC_SOURCE_GPIO;
  pInit.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  pInit.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1);
  pInit.ErrorLimitValue = 34;
  pInit.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&pInit);
}
void SystemClock_Config0(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}
void SystemClock_Config1(void)
{
    RCC_OscInitTypeDef RCC_OscInit = {0};
    RCC_ClkInitTypeDef RCC_ClkInit = {0};

    // 使用HSI16作为时钟源
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInit.HSIState = RCC_HSI_ON;
    RCC_OscInit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInit.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInit.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInit.PLL.PLLM = 4;  // HSI16/4 = 4MHz
    RCC_OscInit.PLL.PLLN = 85; // 4*85 = 340MHz
    RCC_OscInit.PLL.PLLP = 2;  // 340/2 = 170MHz (CPU)
    RCC_OscInit.PLL.PLLQ = 4;  // 340/4 = 85MHz (用于I2C)
    RCC_OscInit.PLL.PLLR = 2;  // 340/2 = 170MHz (其他外设)
    HAL_RCC_OscConfig(&RCC_OscInit);

    RCC_ClkInit.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV1; // 170MHz
    RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV1; // 170MHz
    HAL_RCC_ClockConfig(&RCC_ClkInit, FLASH_LATENCY_4);
}

static void Error_Check(SMBUS_StackHandleTypeDef *pStackContext)
{
    BSP_LED_On(LED_GREEN);
    if ((STACK_SMBUS_IsBlockingError(pStackContext)) || (STACK_SMBUS_IsCmdError(pStackContext)))
    {
        /* No action, error symptoms are ignored */
        pStackContext->StateMachine &= ~(SMBUS_ERROR_CRITICAL | SMBUS_COM_ERROR);
    }
    else if ((pStackContext->StateMachine & SMBUS_SMS_ERR_PECERR) ==
             SMBUS_SMS_ERR_PECERR) /* PEC error, we won't wait for any more action */
    {
        pStackContext->StateMachine |= SMBUS_SMS_READY;
        pStackContext->CurrentCommand = NULL;
        pStackContext->StateMachine &= ~(SMBUS_SMS_ACTIVE_MASK | SMBUS_SMS_ERR_PECERR);
    }
}

void BSP_PB_Callback(Button_TypeDef Button)
{
}

static void BUTTON_USER_EXTI_Callback(void)
{
    BSP_PB_Callback(BUTTON_USER);
}

int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
    GPIO_InitTypeDef gpio_init_structure;
    static BSP_EXTI_LineCallback ButtonCallback[BUTTONn] = {BUTTON_USER_EXTI_Callback};
    static uint32_t BSP_BUTTON_PRIO[BUTTONn] = {BSP_BUTTON_USER_IT_PRIORITY};
    static const uint32_t BUTTON_EXTI_LINE[BUTTONn] = {USER_BUTTON_EXTI_LINE};

    /* Enable the BUTTON Clock */
    USER_BUTTON_GPIO_CLK_ENABLE();

    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Pull = GPIO_PULLDOWN;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

    if (ButtonMode == BUTTON_MODE_GPIO)
    {
        /* Configure Button pin as input */
        gpio_init_structure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
    }
    else /* (ButtonMode == BUTTON_MODE_EXTI) */
    {
        /* Configure Button pin as input with External interrupt */
        gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

        HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);

        (void)HAL_EXTI_GetHandle(&hExtiButtonHandle[Button], BUTTON_EXTI_LINE[Button]);
        (void)HAL_EXTI_RegisterCallback(&hExtiButtonHandle[Button], HAL_EXTI_COMMON_CB_ID, ButtonCallback[Button]);

        /* Enable and set Button EXTI Interrupt to the lowest priority */
        HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BSP_BUTTON_PRIO[Button], 0x00);
        HAL_NVIC_EnableIRQ((BUTTON_IRQn[Button]));
    }

    return BSP_ERROR_NONE;
}
int32_t BSP_LED_Init(Led_TypeDef Led)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable the GPIO LED Clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin = LED_PIN[Led];
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED_PORT[Led], &gpio_init_structure);
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

    return BSP_ERROR_NONE;
}
int32_t BSP_LED_On(Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);

    return BSP_ERROR_NONE;
}
int32_t BSP_LED_Off(Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);

    return BSP_ERROR_NONE;
}
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();

    while (1)
    {
         BSP_LED_On(LED_GREEN);
         HAL_Delay(100);
         BSP_LED_Off(LED_GREEN);
         HAL_Delay(100);
         
    }
    /* USER CODE END Error_Handler_Debug */
}