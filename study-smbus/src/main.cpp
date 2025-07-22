

#include "main.h"
#include "stm32g4xx.h"

static GPIO_TypeDef *BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
static const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};
static GPIO_TypeDef *LED_PORT[LEDn] = {LED2_GPIO_PORT};
static const uint16_t LED_PIN[LEDn] = {LED2_PIN};
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

st_command_t const STATUS_INPUT = {PMBC_STATUS_INPUT, READ, 1, 1}; /* code 7C */
st_command_t const STATUS_TEMPERATURE = {PMBC_STATUS_TEMPERATURE, READ, 1, 1};

EXTI_HandleTypeDef hExtiButtonHandle[BUTTONn];
SMBUS_HandleTypeDef handle1;
SMBUS_StackHandleTypeDef context1;
int LED_INIT = 0;

void fillHandle(SMBUS_HandleTypeDef &handle1);
void fillContext(SMBUS_StackHandleTypeDef &context1);
void Ok_Notify();
void Fail_Notify();

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init(LED_GREEN);
    BSP_LED_On(LED_GREEN);
    fillHandle(handle1);
    if (HAL_SMBUS_Init(&handle1) != HAL_OK)
    {
        Error_Handler();
        return -1;
    }

    fillContext(context1);
    if (STACK_SMBUS_Init(&context1, 0x1U) != STACK_OK)
    {
        Error_Handler();
        return -1;
    }

    uint8_t *piobuf = NULL;

    piobuf = STACK_SMBUS_GetBuffer(&context1);
    if (piobuf != NULL)
    {
        for (uint8_t i = 0; i < STACK_NBYTE_SIZE; i++)
        {
            piobuf[i] = i;
        }
    }
    while (1)
    {
        if (HAL_SMBUS_IsDeviceReady(&handle1, SLAVE_ADDRESS, 3, 10 * 1000) == HAL_OK)
        {
            break;
        }
        Fail_Notify();
    }

    while (1)
    {

        BSP_LED_On(LED_GREEN);
        HAL_Delay(500);
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(500);

        Error_Check(&context1);

        HAL_StatusTypeDef result = STACK_SMBUS_HostCommand(&context1, (st_command_t *)&STATUS_TEMPERATURE, SLAVE_ADDRESS, READ);
        if (result == HAL_OK)
        {
            Ok_Notify();
        }
        else
        {
            Fail_Notify();
            continue;
        }

        BSP_LED_On(LED_GREEN);
        HAL_Delay(500);
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(500);
        while (1)
        {
        }
        piobuf = NULL;
        piobuf = STACK_SMBUS_GetBuffer(&context1);
        if (piobuf == NULL)
        {
            Fail_Notify();
        }
        else
        {

            switch ((uint8_t)piobuf[0])
            {
            case 0U:

                break;
            case 1U:

                break;
            case 2U:

                break;
            case 3U:

                break;
            case 4U:

                break;
            case 5U:

                break;
            case 6U:

                break;
            case 7U:

                break;

            default:
                Fail_Notify();
            }
        }
    }
    return 1;
}
void fillHandle(SMBUS_HandleTypeDef &handle1)
{
    handle1.Instance = SMBUS1;
    handle1.Init.Timing = SMBUS_TIMING_100K; //
    handle1.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
    handle1.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;

    handle1.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
    handle1.Init.OwnAddress2 = 0U;
    handle1.Init.OwnAddress2Masks = SMBUS_OA2_NOMASK;

    handle1.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
    handle1.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;

    handle1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_HOST;
    handle1.Init.OwnAddress1 = 0U;

    handle1.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
    handle1.Init.SMBusTimeout = SMBUS_TIMEOUT_DEFAULT;
    handle1.pBuffPtr = context1.Buffer;
}
void fillContext(SMBUS_StackHandleTypeDef &context1)
{
    context1.CMD_table = (st_command_t *)&PMBUS_COMMANDS_TAB[0];
    context1.CMD_tableSize = PMBUS_COMMANDS_TAB_SIZE;
    context1.Device = &handle1;
    context1.SRByte =  0x00U;
    context1.CurrentCommand = NULL;
    context1.StateMachine = SMBUS_SMS_READY;
    context1.OwnAddress = 0U;
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
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
    pInit.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1);
    pInit.ErrorLimitValue = 34;
    pInit.HSI48CalibrationValue = 32;

    HAL_RCCEx_CRSConfig(&pInit);
}

void Error_Check(SMBUS_StackHandleTypeDef *pStackContext)
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
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    while (1)
    {
        BSP_LED_On(LED_GREEN);
        HAL_Delay(100);
        BSP_LED_Off(LED_GREEN);
        HAL_Delay(100);
    }
    /* USER CODE END Error_Handler_Debug */
}
