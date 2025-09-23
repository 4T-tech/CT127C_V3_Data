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
/* 将温度传感器模块插在终端A的扩展插座上 */
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
uint8_t  ucState;                   	/* 状态 */
uint8_t  ucLed=1;                     /* LED值：b0-模式 */
uint8_t  ucSec;                     	/* 秒计时 */
uint16_t usTemp;                    	/* 温度值 */
uint8_t  ucPara=25;                  	/* 温度参数 */
uint8_t  ucAddr=5;                   	/* 终端地址 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* 按键处理 */
void LED_Proc(void);                	/* LED处理 */
void OLED_Proc(void);               	/* OLED处理 */
void LORA_Proc(void);               	/* LoRa处理 */
void UART_Proc(void);              	  /* UART处理 */
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
  MX_I2C2_Init();
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
    LORA_Proc();                    	/* LoRa处理 */
    UART_Proc();                     	/* UART处理 */
		/* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                     	/* 按键值 */
uint8_t  ucLong;                    	/* 长按键标志 */
uint32_t uwKdly;                    	/* 按键延时 */
void KEY_Proc(void)                 	/* 按键处理 */
{
  uint8_t ucKdn, ucKup;

  ucKdn = KEY_Read();               	/* 按键读取 */
  if(ucKdn != ucKey)                	/* 键值变化 */
  {
    ucKup = ucKey;                   	/* 保存键值 */
    ucKey = ucKdn;
  } else {
    ucKup = 0;                       	/* 清除键值 */
    ucKdn = 0;
  }

  if(ucKdn == '1')                  	/* SW1按下 */
  {
    uwKdly = uwTick;                	/* 按下计时 */
    ucLong = 0;
  }

  if((ucKup == '1') && (ucLong == 0)) /* 按键松开 */
    if(++ucState == 3)
      ucState = 0;                 	  /* 返回状态0 */

  if(uwTick - uwKdly > 1000)      	  /* 长按键 */
    if(ucKey && (ucLong == 0))
    {
      ucLong = 1;
      ucLed ^= 1;                   	/* 切换模式 */
      if((ucLed&1) == 0)           	  /* 远程模式 */
        ucLed &= ~6;               	  /* 熄灭LD2和LD3 */
    }
}

void LED_Proc(void)                 	/* LED处理 */
{
  if((ucLed&1) == 1)                	/* 自动控制模式 */
  {
    if(usTemp > (ucPara+1)*10)
    {
      ucLed |= 2;                   	/* 点亮LD2 */
      ucLed &= ~4;                   	/* 熄灭LD3 */
    }
    else if(usTemp < (ucPara-1)*10)
    {
      ucLed |= 4;                    	/* 熄灭LD2 */
      ucLed &= ~2;                   	/* 点亮LD3 */
    }
    else
      ucLed |= 6;                    	/* 点亮LD2和LD3 */
  }
  LED_Disp(ucLed);                  	/* LED显示 */
}

uint16_t usDly;                     	/* 延时 */
uint8_t  ucBuf[20];                  	/* OLED显示值 */
void OLED_Proc(void)                	/* OLED处理 */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* 延时到 */

  usTemp = TEMP_Read()*10;
  switch(ucState)
  {
    case 0:                          	/* 温度显示 */
      OLED_ShowString(0, 0, (uint8_t *)"  Temperature", 16);
      sprintf((char*)ucBuf, "      %4.1f", usTemp/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 1:                         	/* 参数显示 */
      OLED_ShowString(0, 0, (uint8_t *)"   Parameter ", 16);
      sprintf((char*)ucBuf, "       %02u ", ucPara);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 2:                          	/* 地址显示 */
      OLED_ShowString(0, 0, (uint8_t *)"    Address  ", 16);
      sprintf((char*)ucBuf, "      %03u ", ucAddr);
      OLED_ShowString(0, 2, ucBuf, 16);
  }
}

uint8_t  ucLrx[20];                  	/* LoRa接收值 */
void LORA_Proc(void)                	/* LoRa处理 */
{
  if((LORA_Rx(ucLrx)==0) && (ucLrx[0]=='*'))
    if(ucAddr == ((ucLrx[2]-0x30)*100+(ucLrx[3]-0x30)*10+ucLrx[4]-0x30))
    {
      if(ucLrx[1] == '2')
      {
        sprintf((char*)ucBuf, "%02u*", ucPara);
        LORA_Tx(ucBuf, 3);        	  /* 发送参数 */
      }
      if((ucLed&1) == 0)
      {
        if(ucLrx[1] == '4')
          ucLed ^= 2;               	/* 切换LD2状态 */
        if(ucLrx[1] == '5')
          ucLed ^= 4;               	/* 切换LD3状态 */
      }
    }
}

uint8_t  ucUsec;                    	/* UART发送延时 */
uint8_t  ucUrx[20];                 	/* UART接收值 */
uint8_t  ucUrn;                      	/* UART接收字节数（最高位为接收完成标志） */
void UART_Proc(void)                	/* UART处理 */
{
  uint8_t ucTemp;

  if(ucUsec != ucSec)                 /* 测试用 */
  { 
    ucUsec = ucSec;

    printf("%03u\r\n", ucSec);
  }

  if ((ucUrn&0x80) != 0)
  {
    if (((ucUrn&0x7f) == 3) && (ucUrx[0] == 'T'))
    {
      ucTemp = (ucUrx[1]-0x30)*10+ucUrx[2]-0x30;
      if((ucTemp>9) && (ucTemp<100))
        ucPara = ucTemp;
    }
    if (((ucUrn&0x7f) == 7) && (ucUrx[0] == 'A'))
    {
      ucTemp = (ucUrx[4]-0x30)*100+(ucUrx[5]-0x30)*10+ucUrx[6]-0x30;
      if(ucTemp != 0)
        ucAddr = ucTemp;
    }
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
