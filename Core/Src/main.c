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

#include "Adafruit_GFX.h"
#include "../Fonts/FreeMonoBold18pt7b.h"
#include "lcd.h"
#include "gui.h"
#include "test.h"
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

//
	//DrawTestPage("Manthan Jal");
//
//  //Show_Str(180,30,YELLOW,BLACK,"Manthan Jal",16,1);
//  //LCD_DrawFillRectangleColor(60,60,200,110,LIGHTGREEN);
//  //LCD_DrawFillRectangleColor(280,60,400,110,GREEN);
//  //LCD_DrawFillRectangleColor(60,170,200,220,0x36B9);
// // LCD_DrawFillRectangleColor(280,170,400,220,BLUE);
//  Set_Draw_color(RED);
//  //Draw_Circle(20,20,20);
//  //Fill_Rect(60,60,140,50,LIGHTGREEN);
//
//  Fill_Round_Rectangle(60,90,200,115,10,0X841A);
//  Fill_Round_Rectangle(60,60,200,110,10,LIGHTGREEN);
//  Set_Draw_color(LIGHTGREEN);
//  Draw_Round_Rectangle(60,60,200,110,10);
//
//  Fill_Round_Rectangle(280,90,400,115,10,0x07E2);
//  Fill_Round_Rectangle(280,60,400,110,10,GREEN);
//  Set_Draw_color(GREEN);
//  Draw_Round_Rectangle(280,60,400,110,10);
//
//  Fill_Round_Rectangle(60,200,200,225,10,0x36B4);
//  Fill_Round_Rectangle(60,170,200,220,10,0x36B9);
//  Set_Draw_color(0x36B9);
//  Draw_Round_Rectangle(60,170,200,220,10);
//
//  Fill_Round_Rectangle(280,200,400,225,10,0x001A);
//  Fill_Round_Rectangle(280,170,400,220,10,BLUE);
//  Set_Draw_color(BLUE);
//  Draw_Round_Rectangle(280,170,400,220,10);
//
//  Set_Text_colour(WHITE);
//  Set_Text_Mode(1);
//  Set_Text_Back_colour(BLACK);
//  Set_Text_Size(1);
//  lcd_Print_String("pH 7", 90, 75);
//
//  Set_Text_colour(WHITE);
//    Set_Text_Mode(1);
//    Set_Text_Back_colour(BLACK);
//    Set_Text_Size(1);
//    lcd_Print_String("pH 8", 310, 75);
//
//    Set_Text_colour(WHITE);
//      Set_Text_Mode(1);
//      Set_Text_Back_colour(BLACK);
//      Set_Text_Size(1);
//      lcd_Print_String("pH 9", 90, 185);
//
//      Set_Text_colour(WHITE);
//        Set_Text_Mode(1);
//        Set_Text_Back_colour(BLACK);
//        Set_Text_Size(1);
//        lcd_Print_String("pH 10", 300, 185);

//  Show_Str(110,75,BLACK,LIGHTGREEN,"pH 7",16,0);
//  Show_Str(330,75,BLACK,GREEN,"pH 8",16,0);
//  Show_Str(110,185,BLACK,0x36B9,"pH 9",16,0);
//  Show_Str(330,185,BLACK,BLUE,"pH 10",16,0);

  gfx_init(LCD_H,LCD_W);
  gfx_fillScreen(BLACK);
  gfx_setFont(&FreeMonoBold18pt7b);
  gfx_setTextColor(WHITE);
  gfx_setCursor(120, 40);
  gfx_print("Manthan Jal");
  gfx_btn_initButton(140,120,150,80,BLACK,LIGHTGREEN,WHITE,"pH 8",1);
  gfx_btn_drawButton(0);
  gfx_btn_initButton(350,120,150,80,BLACK,0x658C,WHITE,"pH 9",1);
  gfx_btn_drawButton(0);
  gfx_btn_initButton(140,230,150,80,BLACK,0x36B9,WHITE,"pH 10",1);
  gfx_btn_drawButton(0);
  gfx_btn_initButton(350,230,150,80,BLACK,BLUE,WHITE,"pH 11",1);
  gfx_btn_drawButton(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(100);
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
