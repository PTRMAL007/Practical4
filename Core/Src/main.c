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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32f4xx.h"
#include "lcd_stm32f4.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// TODO: Add values for below variables
#define NS        // Number of samples in LUT
#define TIM2CLK   // STM Clock frequency: Hint You might want to check the ioc file
#define F_SIGNAL  // Frequency of output analog signal

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;

/* USER CODE BEGIN PV */
// TODO: Add code for global variables, including LUTs
uint32_t Sin_LUT[NS] = {2048,2148,2248,2348,2447,2545,2642,2737,2831,
2923,3013,3100,3185,3267,3346,3423,3495,3565,3630,3692,3750,3804,3853,3898,
3939,3975,4007,4034,4056,4073,4085,4093,4095,4093,4085,4073,4056,4034,4007,
3975,3939,3898,3853,3804,3750,3692,3630,3565,3495,3423,3346,3267,3185,3100,3013,2923,
2831,2737,2642,2545,2447,2348,2248,2148,2048,1947,1847,1747,1648,1550,1453,1358,
1264,1172,1082,995,910,828,749,672,600,530,465,403,345,291,242,197,156,120,88,
61,39,22,10,2,0,2,10,22,39,61,88,120,156,197,242,291,345,403,465,530,600,672,749,828,910,995,
1082,1172,1264,1358,1453,1550,1648,1747,1847,1947};
uint32_t Saw_LUT[NS] = {0,32,64,97,129,161,193,226,258,290,322,355,387,419,451,484,516,548,580,613,
645,677,709,742,774,806,838,871,903,935,967,1000,1032,1064,1096,1129,1161,1193,1225,1258,1290,1322,
1354,1386,1419,1451,1483,1515,1548,1580,1612,1644,1677,1709,1741,1773,1806,1838,1870,1902,1935,1967,1999,
2031,2064,2096,2128,2160,2193,2225,2257,2289,2322,2354,2386,2418,2451,2483,2515,2547,2580,2612,2644,
2676,2709,2741,2773,2805,2837,2870,2902,2934,2966,2999,3031,3063,3095,3128,3160,3192,3224,3257,3289,
3321,3353,3386,3418,3450,3482,3515,3547,3579,3611,3644,3676,3708,3740,3773,3805,3837,3869,3902,
3934,3966,3998,4031,4063,4095};
uint32_t Triangle_LUT[NS] = {0,65,130,195,260,325,390,455,520,585,650,715,780,845,910,975,1040,1105,
1170,1235,1300,1365,1430,1495,1560,1625,1690,1755,1820,1885,1950,2015,2080,2145,2210,2275,2340,2405,
2470,2535,2600,2665,2730,2795,2860,2925,2990,3055,3120,3185,3250,3315,3380,3445,3510,3575,3640,3705,
3770,3835,3900,3965,4030,4095,4095,4030,3965,3900,3835,3770,3705,3640,3575,3510,3445,3380,3315,3250,3185,3120,3055,2990,2925,
2860,2795,2730,2665,2600,2535,2470,2405,2340,2275,2210,2145,2080,2015,1950,1885,1820,1755,1690,1625,1560,1495,1430,1365,1300,
1235,1170,1105,1040,975,910,845,780,715,650,585,520,455,390,325,260,195,130,65,0};
uint32_t Piano_LUT = {3720, 2341, 1324, 2198, 2400, 1976, 1791, 1877, 2334, 1743, 1824, 2019, 2075, 1937, 1776, 2202, 
1779, 2076, 1706, 2307, 1776, 1981, 1896, 2018, 1966, 1832, 2064, 1909, 2012, 1804, 2105, 1873, 
1944, 1954, 1931, 2053, 1779, 2120, 1818, 2053, 1829, 2062, 1911, 1918, 1974, 1935, 2031, 1766, 
2115, 1867, 2086, 1801, 1965, 2008, 1908, 1979, 1904, 2032, 1886, 1972, 1941, 1972, 1983, 1848, 
2094, 1803, 2117, 1761, 2131, 1836, 2031, 1884, 2018, 1915, 1993, 1907, 2013, 1888, 1996, 1877, 
2107, 1720, 2393, 0, 1393, 3910, 1458, 1523, 4095, 268, 1722, 3285, 773, 2827, 2886, 139, 
2997, 1758, 1460, 3010, 1666, 946, 3140, 988, 2235, 2793, 947, 2192, 2406, 996, 2906, 1852, 
1241, 2641, 1835, 1297, 3194, 1000, 2144, 2221, 1567, 1935, 2714, 816, 2803, 1561, 2002, 1793};
uint32_t Guitar_LUT = {1649, 1860, 1879, 1828, 1865, 1853, 1865, 1823, 1891, 1830, 1865, 1834, 1896, 1808, 1883, 1840, 
1872, 1820, 1893, 1828, 1869, 1832, 1883, 1825, 1883, 1831, 1870, 1834, 1889, 1816, 1885, 1827, 
1890, 1799, 1915, 1803, 1862, 1931, 1762, 1741, 2226, 1565, 1670, 2381, 1558, 1620, 2338, 1678, 
1502, 2351, 1711, 1556, 2253, 1767, 1561, 2232, 1729, 1647, 2161, 1771, 1620, 2167, 1779, 1623, 
2131, 1840, 1601, 2094, 1875, 1631, 1983, 1977, 1615, 1977, 1937, 1685, 1930, 1886, 1851, 1741, 
1936, 1958, 1800, 2021, 1322, 1151, 4095, 154, 1726, 3612, 0, 2552, 2386, 194, 3680, 897, 
1609, 2934, 690, 2581, 1765, 1287, 2898, 639, 2518, 2077, 930, 2995, 1336, 1525, 2704, 1035, 
2240, 1883, 1546, 2273, 1534, 1982, 2027, 1480, 2227, 1623, 1825, 2060, 1657, 1904, 1914, 1770};
uint32_t Drum_LUT = {1864, 2687, 1564, 2541, 1400, 3174, 1909, 1875, 1090, 1645, 2189, 3775, 3333, 2635, 732, 378, 
1479, 2772, 3928, 3412, 1671, 1365, 1979, 1640, 2071, 2462, 2740, 2627, 2104, 1464, 1340, 1825, 
2658, 3275, 2687, 1725, 1303, 1977, 2505, 2393, 2043, 2169, 2177, 2238, 1899, 2049, 2205, 2594, 
2180, 2082, 1711, 2185, 2177, 2417, 1869, 2604, 1789, 2166, 1905, 2899, 1906, 1284, 1343, 1928, 
2998, 4095, 2779, 1761, 0, 1675, 2686, 3080, 3396, 2662, 765, 1901, 2243, 2169, 2229, 2476, 
2257, 2260, 1423, 1671, 2350, 2787, 2479, 2536, 1781, 1589, 1744, 2410, 2630, 2383, 1931, 2111, 
2125, 2123, 1935, 2163, 2188, 2621, 2312, 1959, 1561, 2127, 2322, 2610, 2241, 2051, 1719, 2283, 
2227, 2330, 1901, 2174, 2184, 2480, 1978, 2103, 1835, 2369, 2175, 2484, 1932, 2234, 1777, 2515};





// TODO: Equation to calculate TIM2_Ticks
uint32_t TIM2_Ticks = 0; // How often to write new LUT value
uint32_t DestAddress = (uint32_t) &(TIM3->CCR3); // Write LUT TO TIM3->CCR3 to modify PWM duty cycle


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void EXTI0_IRQHandler(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // TODO: Start TIM3 in PWM mode on channel 3

  // TODO: Start TIM2 in Output Compare (OC) mode on channel 1

  // TODO: Start DMA in IT mode on TIM2->CH1. Source is LUT and Dest is TIM3->CCR3; start with Sine LUT

  // TODO: Write current waveform to LCD(Sine is the first waveform)

  // TODO: Enable DMA (start transfer from LUT to CCR)

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* TIM2_CH1 DMA Init */
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_tim2_ch1.Instance = DMA1_Stream5;
  hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;         // TIM2_CH1 is on channel 3
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH; // Memory -> TIM3->CCR3
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;    // Peripheral address fixed
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;        // Memory address increments
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;            // Repeat LUT automatically
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
  {
      Error_Handler();
  }

  /* Link DMA handle to TIM2 handle */
  __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // -------------------------------
  // LCD pins configuration
  // -------------------------------
  // Configure PC14 (RS) and PC15 (E) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure PB8 (D4) and PB9 (D5) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure PA12 (D6) and PA15 (D7) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Set all LCD pins LOW initially
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);


  // -------------------------------
  // Button0 configuration (PA0)
  // -------------------------------
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Interrupt on rising edge
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // Use pull-up resistor
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Enable and set EXTI line 0 interrupt priority
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void EXTI0_IRQHandler(void){

	// TODO: Debounce using HAL_GetTick()


	// TODO: Disable DMA transfer and abort IT, then start DMA in IT mode with new LUT and re-enable transfer
	// HINT: Consider using C's "switch" function to handle LUT changes




	HAL_GPIO_EXTI_IRQHandler(Button0_Pin); // Clear interrupt flags
}
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
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
