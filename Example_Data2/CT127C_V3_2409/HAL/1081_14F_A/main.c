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
/* 将温度传感器模块插在终端A的扩展插座上 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
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
uint8_t  ucState;                   	/* 状态 */
uint8_t  ucLed;                     	/* LED值 */
uint8_t  ucSec=55;                   	/* 秒 */
uint8_t  ucMin=59;                  	/* 分 */
uint8_t  ucHour=23;                 	/* 时 */
uint16_t usTemp;                    	/* 温度值*10 */
uint8_t  ucTmax=30;                 	/* 温度上限 */
uint8_t  ucTmin=20;                 	/* 温度下限 */
uint8_t  ucAnum;                    	/* 报警次数 */
uint16_t usAtem;                    	/* 报警温度*10 */
uint8_t  ucPw[6]={"111111"};        	/* 密码 */
uint8_t  ucPp;                       	/* 密码验证标志 */
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
uint16_t usKdly;                     	/* 按键 2s计时 */
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
  if (ucKdn == '1')                  	/* SW1按下 */
    usKdly = 0;
  if (ucKup == '1')                  	/* SW1松开 */
  {
    if (usKdly < 2000)               	/* SW1短按 */
		{
      if (++ucState == 3)           	/* 切换状态 */
        ucState = 0;
    } else if (ucState == 2)          /* SW1长按 */
      ucAnum = 0;                   	/* 清零报警次数 */
  }
}

uint8_t  ucLdly;                    	/* LED闪烁延时 */
uint8_t  ucLsec;                	    /* LED 2s计时 */
void LED_Proc(void)                 	/* LED处理 */
{
  if(ucLdly < 100)
    return;
  ucLdly = 0;                       	/* 延时到 */

  if ((ucSec - ucLsec) < 2)
  {
    if (ucPp == 1)                  	/* 密码正确 */
      ucLed |= 1;
    if (ucPp == 2)                  	/* 密码错误 */
      ucLed ^= 1;
  } else {
    ucPp = 0;
    ucLed &= ~1;
  }

  usTemp = TEMP_Read()*10;           	/* 温度读取 */
  if (usTemp > ucTmax*10)
	{
		if ((ucLed & 2) == 0)
    {
      ucAnum++;
      usAtem = usTemp;
      ucLed |= 2;
		}
  } else
    ucLed &= ~2;

  if (usTemp < ucTmin*10)
  {
	  if ((ucLed & 4) == 0)
    {
      ucAnum++;
      usAtem = usTemp;
      ucLed |= 4;
		}
	} else
    ucLed &= ~4;

  LED_Disp(ucLed);	
}

uint8_t  ucBuf[20];                  	/* OLED显示值 */
void OLED_Proc(void)                	/* OLED处理 */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* 延时到 */

  switch (ucState)
  {
    case 0:
      sprintf((char*)ucBuf, "    %02u:%02u:%02u", ucHour, ucMin, ucSec);
      OLED_ShowString(0, 0, ucBuf, 16);
	  	sprintf((char*)ucBuf, "     T:%3.1f  ", usTemp/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 1:
      sprintf((char*)ucBuf, "    Tmax:%02u ", ucTmax);
      OLED_ShowString(0, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "    Tmin:%02u ", ucTmin);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 2:
      sprintf((char*)ucBuf, "     N:%u   ", ucAnum);
      OLED_ShowString(0, 0, ucBuf, 16);
      if (ucAnum == 0)
        sprintf((char*)ucBuf, "    AT:N/A  ");
      else
        sprintf((char*)ucBuf, "    AT:%3.1f  ", usAtem/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
  }
}

uint8_t  ucLrx[20];                  	/* LoRa接收值 */
void LORA_Proc(void)                	/* LoRa处理 */
{
  if (LORA_Rx(ucLrx) == 0)        	  /* 接收完成 */
  {
		if ((ucPw[0] == ucLrx[0]) && (ucPw[1] == ucLrx[1])
      &&(ucPw[2] == ucLrx[2]) && (ucPw[3] == ucLrx[3])
      &&(ucPw[4] == ucLrx[4]) && (ucPw[5] == ucLrx[5]))
    {                                 /* 密码正确 */
      ucBuf[0] = usTemp;
      ucBuf[1] = usTemp>>8;
      ucBuf[2] = ucAnum;
      ucPp = 1;
    } else {                          /* 密码错误 */      ucBuf[0] = 0;
      ucPp = 2;
		}
    LORA_Tx(ucBuf, 3);            	  /* 发送3个字节 */
    ucLsec = ucSec;
	}
}

uint8_t  ucUsec;                     	/* UART发送延时 */
uint8_t  ucUrx[20];                 	/* UART接收值 */
uint8_t  ucUrn;                      	/* UART接收字节数（最高位为接收完成标志） */
void UART_Proc(void)              	  /* UART处理 */
{
  uint8_t ucTmax1, ucTmin1;

  if(ucUsec != ucSec)
  {
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);
  }

  if (ucUrn > 0x80)                	  /* 接收完成 */
  {
    switch (ucUrn&0x7f)
    {
      case 5:
        ucTmax1 = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30;
        ucTmin1 = (ucUrx[3]-0x30)*10+ucUrx[4]-0x30;
        if ((ucUrx[2] == ',') && (ucTmax1 > ucTmin1))
        {
          ucTmax = ucTmax1;
          ucTmin = ucTmin1;
          printf("OK\r\n");
        } else
          printf("ERROR\r\n");
        break;
      case 6:
        if (((ucUrx[0] == '0') || (ucUrx[0] == '1'))
          &&((ucUrx[1] == '0') || (ucUrx[1] == '1'))
          &&((ucUrx[2] == '0') || (ucUrx[2] == '1'))
          &&((ucUrx[3] == '0') || (ucUrx[3] == '1'))
          &&((ucUrx[4] == '0') || (ucUrx[4] == '1'))
          &&((ucUrx[5] == '0') || (ucUrx[5] == '1')))
        {
          ucPw[0] = ucUrx[0];
          ucPw[1] = ucUrx[1];
          ucPw[2] = ucUrx[2];
          ucPw[3] = ucUrx[3];
          ucPw[4] = ucUrx[4];
          ucPw[5] = ucUrx[5];
          printf("OK\r\n");
        } else
          printf("ERROR\r\n");
        break;
      default:
        printf("ERROR\r\n");
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
