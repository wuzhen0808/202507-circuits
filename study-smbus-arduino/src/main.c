/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "stm32f4xx_hal_smbus.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "notify.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SMBUS_HandleTypeDef hsmbus1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_SMBUS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t slave1Address = 64; // 0x40,ina226
uint8_t slave2Address = 21; // 0x15,lm5066
uint8_t slave1Ready = 0;
uint8_t slave2Ready = 0;
void doTest(SMBUS_HandleTypeDef *pHsmbus1);
void doInWhile();
/* USER CODE END 0 */
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  // Wire.setSCL(PB8);
  // Wire.setSDA(PB9);
  // Wire.begin();
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_SMBUS_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init();
  BSP_LED_On(LED_GREEN);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  doTest(&hsmbus1);
  while (1)
  {
    /* USER CODE END WHILE */
    doInWhile();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void doTest(SMBUS_HandleTypeDef *pHsmbus1)
{

  // 测试设备是否就绪
  
  HAL_StatusTypeDef ret1 = HAL_SMBUS_IsDeviceReady(pHsmbus1, (uint8_t)(slave1Address << 1), 3, 5);
  // TODO: Always return TIMEOUT at arduino framework stm32.
  if (ret1 == HAL_OK)
  { /* No ACK Received At That Address */
    // Device not ready at this address
    slave1Ready = 1; // Device found at this address
  }
  HAL_StatusTypeDef ret2 = HAL_SMBUS_IsDeviceReady(pHsmbus1, (uint8_t)(slave2Address << 1), 3, 5);
  if (ret2 == HAL_OK)
  { /* No ACK Received At That Address */
    // Device not ready at this address
    slave2Ready = 1; // Device found at this address
  }
}

void doInWhile()
{

  HAL_Delay(2000);
  if (slave1Ready)
  {
    Ok_Notify();
  }
  else
  {
    Fail_Notify();
    // 设备未就绪处理
  }
  
  HAL_Delay(1000);
  
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

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_SMBUS_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hsmbus1.Instance = I2C1;
  hsmbus1.Init.ClockSpeed = 100000;
  hsmbus1.Init.OwnAddress1 = 0;
  hsmbus1.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
  hsmbus1.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
  hsmbus1.Init.OwnAddress2 = 0;
  hsmbus1.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
  hsmbus1.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
  hsmbus1.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
  hsmbus1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
  if (HAL_SMBUS_Init(&hsmbus1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

