

#include "main.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
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
    }
    return 1;
}

void SystemClock_Config(void)
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

void BSP_PB_Callback(Button_TypeDef Button){

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