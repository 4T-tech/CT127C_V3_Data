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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "subghz.h"
#include "usart.h"
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
uint16_t usDly;                     	/* 延时 */
uint8_t  ucLed;                     	/* LED值 */
uint8_t  ucSec;                     	/* 秒计时 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* 按键处理 */
void LED_Proc(void);                	/* LED处理 */
void OLED_Proc(void);               	/* OLED处理 */
void UART_Proc(void);               	/* UART处理 */
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
  MX_SUBGHZ_Init();
  MX_USART2_UART_Init();
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
    UART_Proc();                     	/* UART处理 */
		/* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                     	/* 按键值 */
uint8_t  ucLtx[2]={0x55, 0};         	/* LoRa发送值 */
void KEY_Proc(void)                 	/* 按键处理 */
{
  uint8_t ucKdn;

  ucKdn = KEY_Read();               	/* 按键读取 */
  if(ucKdn != ucKey)                	/* 键值变化 */
    ucKey = ucKdn;                   	/* 保存键值 */
  else
    ucKdn = 0;                       	/* 清除键值 */
	
  if(ucKdn == '1')                  	/* SW1按下 */
  {
    ucLtx[1] = ucLed;
    LORA_Tx(ucLtx, 2);              	/* 发送2个字符 */
  }
}

void LED_Proc(void)                 	/* LED处理 */
{
  LED_Disp(ucLed);                  	/* LED显示 */
}

uint8_t  ucBuf[20];                  	/* OLED显示值 */
uint8_t  ucUrx[20];                  	/* UART接收值 */
uint8_t  ucUnum;                    	/* UART接收字符计数 */
void OLED_Proc(void)                	/* OLED处理 */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* 延时到 */

  sprintf((char*)ucBuf, "      RX:%c", ucUrx[0]);
  OLED_ShowString(0, 0, ucBuf, 16);
  sprintf((char*)ucBuf, "     NUM:%02d", ucUnum);
  OLED_ShowString(0, 2, ucBuf, 16);
}

uint8_t  ucUsec;                    	/* UART秒计时 */
uint8_t  ucUrn;                      	/* UART接收字节数（最高位为接收完成标志） */
void UART_Proc(void)                	/* UART处理 */
{
  if(ucUsec != ucSec)             		/* 测试用 */
  { 
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);      	/* 发送秒值和回车换行 */
  }

  if ((ucUrn&0x80) != 0)             	/* 接收完成 */
  {
    if ((ucUrn&0x7f) == 1)          	/* 接收到1个字符 */
    {
      ucUnum++;
      switch (ucUrx[0])
      {
        case '@':
          ucLed ^= 1;                	/* LD1状态翻转 */
          break;
        case '#':
          ucLed ^= 2;                	/* LD2状态翻转 */
          break;
        case '$':
          ucLed ^= 4;                	/* LD3状态翻转 */
          break;
        default:
          printf("error\r\n");
          ucUnum--;
      }
    } else
      printf("error\r\n");
    ucUrn = 0;
  }
}

int fputc(int ch, FILE *f)       	    /* printf()实现 */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
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
