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
	#include "ssd1306.h"
	#include "fonts.h"
	#include "stdio.h"
	#include <math.h>
	#include "mpu6050.h"
	#include "dht11.h"
	/* Private includes ----------------------------------------------------------*/
	/* USER CODE BEGIN Includes */

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
	I2C_HandleTypeDef hi2c1;
	I2C_HandleTypeDef hi2c2;

	TIM_HandleTypeDef htim1;

	/* USER CODE BEGIN PV */
	uint8_t mode = 1;               // 1: Normal, 2: Sensitive, 3: Silent
	uint8_t lastButtonState = 1;    // phat hien chuyen doi

	float AX, AY, AZ;
	uint8_t RHI, RHD, TCI, TCD, SUM;
	float tCelsius = 0, tFahrenheit = 0, RH = 0;
	uint8_t TFI = 0, TFD = 0;
	char strCopy[20];
	/* USER CODE END PV */

	/* Private function prototypes -----------------------------------------------*/
	void SystemClock_Config(void);
	static void MX_GPIO_Init(void);
	static void MX_I2C1_Init(void);
	static void MX_I2C2_Init(void);
	static void MX_TIM1_Init(void);
	/* USER CODE BEGIN PFP */

	/* USER CODE END PFP */

	/* Private user code ---------------------------------------------------------*/
	/* USER CODE BEGIN 0 */
	void Buzzer_On(void)
	{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	}

	void Buzzer_Off(void)
	{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	}
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
		MX_I2C1_Init();
		MX_I2C2_Init();
		MX_TIM1_Init();
		/* USER CODE BEGIN 2 */
		HAL_TIM_Base_Start(&htim1);
		SSD1306_Init();
		 MPU6050_Init();
		/* USER CODE END 2 */

		/* Infinite loop */
		/* USER CODE BEGIN WHILE */
		
		
		
		

	while (1)
		{
			
			
	// ===== ĐỌC NÚT CHUYỂN CHẾ ĐỘ =====
	uint8_t currentButtonState = HAL_GPIO_ReadPin(Button_Mode_GPIO_Port, Button_Mode_Pin);

			SSD1306_GotoXY(100, 0);
	SSD1306_Puts(currentButtonState ? "1" : "0", &Font_7x10, 1);
	SSD1306_UpdateScreen();

	// Phát hiện nhấn (từ 1 -> 0)
	if (lastButtonState == 1 && currentButtonState == 0)
	{
			HAL_Delay(50); // chống dội nút

			// kiểm tra lại sau khi delay để chắc chắn vẫn nhấn
			if (HAL_GPIO_ReadPin(Button_Mode_GPIO_Port, Button_Mode_Pin) == 0)
			{
					mode++;
					if (mode > 3) mode = 1; // quay lại mode 1 sau mode 3
			}
	}

	lastButtonState = currentButtonState;

			/* USER CODE END WHILE */
		MPU6050_Read_Accel(&AX, &AY, &AZ);
			float vibration = fastSqrt(AX*AX + AY*AY + AZ*AZ) - 1.0f;
			if (vibration < 0) vibration = -vibration;
	 if(DHT11_Start())
			{
				RHI = DHT11_Read(); // do am nguyen
				RHD = DHT11_Read(); // do am thap phan
				TCI = DHT11_Read(); // nhiet do nguyen
				TCD = DHT11_Read(); // nhiet do thap phan 
				SUM = DHT11_Read(); // Check sum
				if (RHI + RHD + TCI + TCD == SUM)
				{
					// Can use RHI and TCI for any purposes if whole number only needed
					tCelsius = (float)TCI + (float)(TCD/10.0);
					tFahrenheit = tCelsius * 9/5 + 32;
					RH = (float)RHI + (float)(RHD/10.0);
					// Can use tCelsius, tFahrenheit and RH for any purposes
					TFI = tFahrenheit;  // Fahrenheit integral
					TFD = tFahrenheit*10-TFI*10; // Fahrenheit decimal
					sprintf(strCopy,"%d.%d C   ", TCI, TCD);
					SSD1306_GotoXY (0, 0);
					SSD1306_Puts (strCopy, &Font_7x10, 1);
					sprintf(strCopy,"Hum: %.1f %%", RH);
					SSD1306_GotoXY (0, 16);
					SSD1306_Puts (strCopy, &Font_7x10, 1);
						sprintf(strCopy, "Vib: %.2f g", vibration);
							SSD1306_GotoXY(0, 30);
							SSD1306_Puts(strCopy, &Font_7x10, 1);

	if (mode == 1)
			sprintf(strCopy, "Mode: 1-Normal");
	else if (mode == 2)
			sprintf(strCopy, "Mode: 2-Sensitive");
	else
			sprintf(strCopy, "Mode: 3-Silent");

	SSD1306_GotoXY(0, 45);
	SSD1306_Puts(strCopy, &Font_7x10, 1);

							SSD1306_UpdateScreen();
											// ===== KI?M TRA C?NH BÁO =====
				// ===== KIỂM TRA CẢNH BÁO THEO CHẾ ĐỘ =====
	if (mode == 1)
	{
			// Chế độ bình thường
			if (tCelsius > 50.0f || vibration > 0.7f)
					Buzzer_On();
			else
					Buzzer_Off();
	}
	else if (mode == 2)
	{
			// Chế độ nhạy cao
			if (tCelsius > 35.0f || vibration > 1.6f)
					Buzzer_On();
			else
					Buzzer_Off();
	}
	else if (mode == 3)
	{
			// Chế độ im lặng
			Buzzer_Off();
	}

				}
			}
			HAL_Delay(300);
			/* USER CODE BEGIN 3 */
		}
		/* USER CODEODE END 3 */
	}

	/**
		* @brief System Clock Configuration
		* @retval None
		*/
	void SystemClock_Config(void)
	{
		RCC_OscInitTypeDef RCC_OscInitStruct = {0};
		RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

		/** Initializes the RCC Oscillators according to the specified parameters
		* in the RCC_OscInitTypeDef structure.
		*/
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
		RCC_OscInitStruct.HSEState = RCC_HSE_ON;
		RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
		RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
		RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
		RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
		RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
		{
			Error_Handler();
		}
	}

	/**
		* @brief I2C1 Initialization Function
		* @param None
		* @retval None
		*/
	static void MX_I2C1_Init(void)
	{

		/* USER CODE BEGIN I2C1_Init 0 */

		/* USER CODE END I2C1_Init 0 */

		/* USER CODE BEGIN I2C1_Init 1 */

		/* USER CODE END I2C1_Init 1 */
		hi2c1.Instance = I2C1;
		hi2c1.Init.ClockSpeed = 400000;
		hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
		hi2c1.Init.OwnAddress1 = 0;
		hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		hi2c1.Init.OwnAddress2 = 0;
		hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(&hi2c1) != HAL_OK)
		{
			Error_Handler();
		}
		/* USER CODE BEGIN I2C1_Init 2 */

		/* USER CODE END I2C1_Init 2 */

	}

	/**
		* @brief I2C2 Initialization Function
		* @param None
		* @retval None
		*/
	static void MX_I2C2_Init(void)
	{

		/* USER CODE BEGIN I2C2_Init 0 */

		/* USER CODE END I2C2_Init 0 */

		/* USER CODE BEGIN I2C2_Init 1 */

		/* USER CODE END I2C2_Init 1 */
		hi2c2.Instance = I2C2;
		hi2c2.Init.ClockSpeed = 100000;
		hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
		hi2c2.Init.OwnAddress1 = 0;
		hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		hi2c2.Init.OwnAddress2 = 0;
		hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(&hi2c2) != HAL_OK)
		{
			Error_Handler();
		}
		/* USER CODE BEGIN I2C2_Init 2 */

		/* USER CODE END I2C2_Init 2 */

	}

	/**
		* @brief TIM1 Initialization Function
		* @param None
		* @retval None
		*/
	static void MX_TIM1_Init(void)
	{

		/* USER CODE BEGIN TIM1_Init 0 */

		/* USER CODE END TIM1_Init 0 */

		TIM_ClockConfigTypeDef sClockSourceConfig = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};

		/* USER CODE BEGIN TIM1_Init 1 */

		/* USER CODE END TIM1_Init 1 */
		htim1.Instance = TIM1;
		htim1.Init.Prescaler = 71;
		htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim1.Init.Period = 65535;
		htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim1.Init.RepetitionCounter = 0;
		htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
		{
			Error_Handler();
		}
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
		{
			Error_Handler();
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}
		/* USER CODE BEGIN TIM1_Init 2 */

		/* USER CODE END TIM1_Init 2 */

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
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_9, GPIO_PIN_RESET);

		/*Configure GPIO pin : Button_Mode_Pin */
		GPIO_InitStruct.Pin = Button_Mode_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(Button_Mode_GPIO_Port, &GPIO_InitStruct);

		/*Configure GPIO pins : PB5 PB9 */
		GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
