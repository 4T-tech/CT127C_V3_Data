/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* 将电位器模块插在终端B的扩展插座上 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "subghz.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "stdio.h"
#include "lora.h"
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

/* USER CODE BEGIN PV */
uint8_t  ucState;                   	/* 状态 */
uint8_t  ucLed;                     	/* LED值 */
uint8_t  ucSec;                     	/* 秒计时 */
uint16_t usAdc[3];				           	/* ADC转换值 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* 按键处理 */
void LED_Proc(void);                	/* LED处理 */
void OLED_Proc(void);               	/* OLED处理 */
void LORA_Proc(void);               	/* LoRa处理 */
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

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
//MX_I2C2_Init();
//MX_SPI1_Init();
  MX_SUBGHZ_Init();
  MX_ADC_Init();
//MX_TIM1_Init();
//MX_TIM2_Init();
//MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();                       	/* OLED初始化 */
  LORA_Init(434, 14);                 /* 434MHz, 14dBm */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                     	/* 按键处理 */
    LED_Proc();                     	/* LED处理 */
    OLED_Proc();                    	/* OLED处理 */
    LORA_Proc();                    	/* LoRa处理 */
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                     	/* 按键值 */
void KEY_Proc(void)                 	/* 按键处理 */
{
  uint8_t ucKdn;

  ucKdn = KEY_Read();               	/* 按键读取 */
  if(ucKdn != ucKey)                	/* 键值变化 */
    ucKey = ucKdn;                   	/* 保存键值 */
  else
    ucKdn = 0;                       	/* 清除键值 */

  if(ucKdn == '1')                  	/* SW1按键按下 */
  {
    ucState ^= 1;                   	/* 切换状态 */
    if(ucState == 1)
	    OLED_Clear();      			    		/* OLED清屏 */
  }
}

void LED_Proc(void)                 	/* LED处理 */
{
  LED_Disp(ucLed);
}
	
uint16_t usDly;                     	/* 延时 */
uint8_t  ucBuf[20];                  	/* OLED显示值 */
void OLED_Proc(void)                	/* OLED处理 */
{
  if(usDly < 200)
    return;
  usDly = 0;                         	/* 延时到 */

  ADC_Read(usAdc);                  	/* ADC读取 */
  if(ucState == 0)
  {
		sprintf((char*)ucBuf, "VAIN1:%4.2fV", usAdc[0]*3.3/4095);
    OLED_ShowString(0, 0, ucBuf, 16);
    sprintf((char*)ucBuf, "VAIN2:%4.2fV", usAdc[2]*3.3/4095);
    OLED_ShowString(0, 2, ucBuf, 16);
  } 
}

uint8_t  ucLrx[20];                  	/* LoRa接收值 */
void LORA_Proc(void)                	/* LoRa处理 */
{
  if ((LORA_Rx(ucLrx) == 0) && (ucLrx[0]=='*'))
	{																	/* 接收2个字符 */
    switch(ucLrx[1])
		{
			case 'B'+1:
    		sprintf((char*)ucBuf, "VAIN1:%4.2fV*", usAdc[0]*3.3/4095);
        LORA_Tx(ucBuf, 12);					/* LoRa发送VAIN1 */
			  break;
		  case 'B'+2:
		    sprintf((char*)ucBuf, "VAIN2:%4.2fV*", usAdc[2]*3.3/4095);
        LORA_Tx(ucBuf, 12);					/* LoRa发送VAIN2 */
			break;
		  case 'B'+4:
        ucLed ^= 2;             		/* LD2切换 */
			  break;
		  case 'B'+5:
        ucLed ^= 4;             		/* LD3切换 */
		}
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
