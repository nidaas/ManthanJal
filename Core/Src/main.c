/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "gfx.h"
#include "../Fonts/FreeMonoBold18pt7b.h"
#include <math.h>


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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int crossX1=228,crossY1=151,crossX2=261,crossY2=186;
float angleF=0,angleValueX=0,angleValueY=0;
int aniAngle[8]={15,30,45,60,75,90};
int pHColor[4]={LIGHTGREEN,0x658C,0x36B9,BLUE};
uint16_t aniAnglePos=0;

void ClearprocessAni(int angle){

	gfx_drawLine(crossX1, crossY1, crossX2, crossY2, RED);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, RED);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, RED);

	angleF=((float)angle/180.0)*3.14;

	angleValueX=25*cos(angleF);
	angleValueY=25*sin(angleF);

	crossX1=245+angleValueX;
	crossY1=170+angleValueY;
	crossX2=245-angleValueX;
	crossY2=170-angleValueY;

	gfx_drawLine(crossX1, crossY1, crossX2, crossY2, RED);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, RED);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, RED);

}

void processAni(int angle,int pos){
	angleF=((float)angle/180.0)*3.14;

	angleValueX=25*cos(angleF);
	angleValueY=25*sin(angleF);

	crossX1=245+angleValueX;
	crossY1=170+angleValueY;
	crossX2=245-angleValueX;
	crossY2=170-angleValueY;

	//gfx_fillCircle(245, 170, 25, RED);

	gfx_drawLine(crossX1, crossY1, crossX2, crossY2, WHITE);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, WHITE);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, WHITE);

	angleF=(((float)angle+90.0)/180.0)*3.14;

	angleValueX=25*cos(angleF);
	angleValueY=25*sin(angleF);

	crossX1=245+angleValueX;
	crossY1=170+angleValueY;
	crossX2=245-angleValueX;
	crossY2=170-angleValueY;

	gfx_drawLine(crossX1, crossY1, crossX2, crossY2, WHITE);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, WHITE);
	gfx_drawLine(crossX1, crossY1+1, crossX2, crossY2+1, WHITE);

	static int test=0;
	if(test==0){
		test=1;
		gfx_drawLine(238, 240, 238, 245, BLACK);
		gfx_drawLine(238, 250, 238, 255, BLACK);
		gfx_drawLine(238, 260, 238, 265, BLACK);

		gfx_drawLine(246, 235, 246, 240, BLACK);
		gfx_drawLine(246, 245, 246, 250, BLACK);
		gfx_drawLine(246, 255, 246, 260, BLACK);

		gfx_drawLine(254, 240, 254, 245, BLACK);
		gfx_drawLine(254, 250, 254, 255, BLACK);
		gfx_drawLine(254, 260, 254, 265, BLACK);

		gfx_drawLine(238, 235, 238, 240, pHColor[pos]);
		gfx_drawLine(238, 245, 238, 250, pHColor[pos]);
		gfx_drawLine(238, 255, 238, 260, pHColor[pos]);

		gfx_drawLine(246, 240, 246, 245, pHColor[pos]);
		gfx_drawLine(246, 250, 246, 255, pHColor[pos]);
		gfx_drawLine(246, 260, 246, 266, pHColor[pos]);

		gfx_drawLine(254, 235, 254, 240, pHColor[pos]);
		gfx_drawLine(254, 245, 254, 250, pHColor[pos]);
		gfx_drawLine(254, 255, 254, 260, pHColor[pos]);
	}
	else{
		test=0;
		gfx_drawLine(238, 235, 238, 240, BLACK);
		gfx_drawLine(238, 245, 238, 250, BLACK);
		gfx_drawLine(238, 255, 238, 260, BLACK);

		gfx_drawLine(246, 240, 246, 245, BLACK);
		gfx_drawLine(246, 250, 246, 255, BLACK);
		gfx_drawLine(246, 260, 246, 266, BLACK);

		gfx_drawLine(254, 235, 254, 240, BLACK);
		gfx_drawLine(254, 245, 254, 250, BLACK);
		gfx_drawLine(254, 255, 254, 260, BLACK);

		gfx_drawLine(238, 240, 238, 245, pHColor[pos]);
		gfx_drawLine(238, 250, 238, 255, pHColor[pos]);
		gfx_drawLine(238, 260, 238, 265, pHColor[pos]);

		gfx_drawLine(246, 235, 246, 240, pHColor[pos]);
		gfx_drawLine(246, 245, 246, 250, pHColor[pos]);
		gfx_drawLine(246, 255, 246, 260, pHColor[pos]);

		gfx_drawLine(254, 240, 254, 245, pHColor[pos]);
		gfx_drawLine(254, 250, 254, 255, pHColor[pos]);
		gfx_drawLine(254, 260, 254, 265, pHColor[pos]);
	}


}

void destroyTap(int pos){

	switch(pos){
			case 0: gfx_FillRectWithAngle(220,150,40,10,210, BLACK);
					gfx_btn_initButton(120,120,150,80,BLACK,LIGHTGREEN,WHITE,"pH 8",1);
					gfx_btn_drawButton(0);
					break;
			case 1: gfx_FillRectWithAngle(265,150,40,10,330, BLACK);
					gfx_btn_initButton(370,120,150,80,BLACK,0x658C,WHITE,"pH 9",1);
					gfx_btn_drawButton(0);
					break;
			case 2: gfx_FillRectWithAngle(210,180,40,10,150, BLACK);
		  	  		gfx_btn_initButton(120,230,150,80,BLACK,0x36B9,WHITE,"pH 10",1);
			  	  	gfx_btn_drawButton(0);
			  	  	break;
			case 3: gfx_FillRectWithAngle(270,190,40,10,30, BLACK);
					gfx_btn_initButton(370,230,150,80,BLACK,BLUE,WHITE,"pH 11",1);
					gfx_btn_drawButton(0);
					break;
			default: break;
		}
}

void createTap(uint8_t pos){
	gfx_fillCircle(245, 170, 35, pHColor[pos]);
	gfx_drawCircle(245, 170, 25, WHITE);
	gfx_fillCircle(245, 170, 25, RED);
	gfx_drawCircle(245, 170, 26, WHITE);
	gfx_fillRoundRect(235, 200, 22, 30, 2,pHColor[pos] );

	gfx_drawLine(225,310,265,310, WHITE);
	gfx_drawLine(225,310,215,270, WHITE);
	gfx_drawLine(265,310,275,270, WHITE);

	int xr=0;
	for(int i=0;i<5;i++){
		for(int j=0;j<5;j++){
			gfx_drawLine(225-i,309-xr,265+i,309-xr, pHColor[pos]);
			xr++;
		}
	}

	gfx_drawEllipse(245, 270, 30, 3, WHITE);

	switch(pos){
			case 0: gfx_FillRectWithAngle(220,150,40,10,210, pHColor[pos]); break;
			case 1: gfx_FillRectWithAngle(265,150,40,10,330, pHColor[pos]); break;
			case 2: gfx_FillRectWithAngle(210,180,40,10,150, pHColor[pos]); break;
			case 3: gfx_FillRectWithAngle(270,190,40,10,30, pHColor[pos]); break;
			default: break;
		}


}

void tapLoop(uint8_t pos){
	ClearprocessAni(aniAngle[aniAnglePos]);
	aniAnglePos++;
	if(aniAnglePos>=6) aniAnglePos=0;
	processAni(aniAngle[aniAnglePos],pos);


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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();

  gfx_init(LCD_H,LCD_W);
  gfx_fillScreen(BLACK);
  gfx_setFont(&FreeMonoBold18pt7b);
  gfx_setTextColor(WHITE);
  gfx_setCursor(160, 40);
  gfx_print("Manthan Jal");
 // gfx_drawRGBBitmap1(100,10,gImage_testImg,40,40);
  gfx_btn_initButton(120,120,150,80,BLACK,LIGHTGREEN,WHITE,"pH 8",1);
  gfx_btn_drawButton(0);
  gfx_btn_initButton(370,120,150,80,BLACK,0x658C,WHITE,"pH 9",1);
  gfx_btn_drawButton(0);
  gfx_btn_initButton(120,230,150,80,BLACK,0x36B9,WHITE,"pH 10",1);
  gfx_btn_drawButton(0);
  gfx_btn_initButton(370,230,150,80,BLACK,BLUE,WHITE,"pH 11",1);
  gfx_btn_drawButton(0);

  long ts=0;
  int post=0;
  createTap(post);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);


	if(HAL_GetTick()-ts>6000){
		ts=HAL_GetTick();
		destroyTap(post);
		post++;
		if(post>=4){
			post=0;
		}
		createTap(post);

	}
	tapLoop(post);
	HAL_Delay(200);
	//main_test();
	//Test_Color();


	//Test_Triangle();
	//English_Font_test();
	//Chinese_Font_test();
	//Pic_test();

	//Rotate_Test();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RS_Pin|LCD_WR_Pin|LCD_RST_Pin|LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D10_Pin
                          |LCD_D11_Pin|LCD_D12_Pin|LCD_D13_Pin|LCD_D14_Pin
                          |LCD_D15_Pin|LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin
                          |LCD_D6_Pin|LCD_D7_Pin|LCD_D8_Pin|LCD_D9_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RS_Pin LCD_WR_Pin LCD_RST_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_WR_Pin|LCD_RST_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_D0_Pin LCD_D1_Pin LCD_D2_Pin LCD_D10_Pin
                           LCD_D11_Pin LCD_D12_Pin LCD_D13_Pin LCD_D14_Pin
                           LCD_D15_Pin LCD_D3_Pin LCD_D4_Pin LCD_D5_Pin
                           LCD_D6_Pin LCD_D7_Pin LCD_D8_Pin LCD_D9_Pin */
  GPIO_InitStruct.Pin = LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D10_Pin
                          |LCD_D11_Pin|LCD_D12_Pin|LCD_D13_Pin|LCD_D14_Pin
                          |LCD_D15_Pin|LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin
                          |LCD_D6_Pin|LCD_D7_Pin|LCD_D8_Pin|LCD_D9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
