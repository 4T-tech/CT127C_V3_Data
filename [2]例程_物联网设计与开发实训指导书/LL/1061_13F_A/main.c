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
/* 将矩阵按键模块插在终端A的扩展插座上 */
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
uint8_t  ucState;                   	/* 状态 */
uint8_t  ucLed;                     	/* LED值 */
uint8_t  ucSec;                     	/* 秒计时 */
uint16_t usWT=502;                   	/* 体重 */
uint16_t usBF=151;                   	/* 体脂 */
uint8_t  ucMIN=10;                  	/* 体脂下限 */
uint8_t  ucMAX=20;                  	/* 体脂上限 */
uint16_t usRec[20][2];              	/* 记录数组 */
uint8_t  ucNO=1;                     	/* 索引值 */
uint8_t  ucNUM;                     	/* 记录数 */
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
void KEY_Proc(void)                 	/* 按键处理 */
{
  uint8_t ucKdn;

  ucKdn = KEY_Read();               	/* 按键读取 */
  if(ucKdn != ucKey)                	/* 键值变化 */
    ucKey = ucKdn;                   	/* 保存键值 */
  else
    ucKdn = 0;                       	/* 清除键值 */
	
  switch (ucKdn)                    	/* 按键按下 */
  {
    case 'B'+1:
      if (++ucState == 3)
        ucState = 0;               	  /* 切换状态 */
      break;
    case 'B'+2:
      if ((ucState == 2) && (ucNUM > 1))
        if (++ucNO > ucNUM)
          ucNO = 1;
  }
}

uint8_t  ucLdly;                    	/* LED闪烁延时 */
void LED_Proc(void)                 	/* LED处理 */
{
  if (ucLdly < 100)                 	/* 闪烁延时 */
    return;
  ucLdly = 0;
  
  if ((usBF < ucMIN*10) || (usBF > ucMAX*10))
    ucLed ^= 1;
  else
    ucLed = 0;
	LED_Disp(ucLed);                  	/* LED显示 */
}

uint8_t  ucBuf[20];                 	/* OLED显示值 */
void OLED_Proc(void)                	/* OLED处理 */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* 延时到 */

  switch (ucState)
  {
    case 0:
      sprintf((char*)ucBuf, "WT:%3.1fKG   ", usWT/10.0);
      OLED_ShowString(0, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "BF:%3.1f%%    ", usBF/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 1:
      sprintf((char*)ucBuf, "MIN:%02u%%  ", ucMIN);
      OLED_ShowString(0, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "MAX:%02u%%  ", ucMAX);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 2:
      sprintf((char*)ucBuf, "WT:%3.1fKG ", usRec[ucNO-1][0]/10.0);
      OLED_ShowString(0, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "%02u", ucNUM);
      OLED_ShowString(80, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "BF:%3.1f%% ", usRec[ucNO-1][1]/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
      sprintf((char*)ucBuf, "%02u", ucNO);
      OLED_ShowString(80, 2, ucBuf, 16);
  }
}

uint8_t  ucLrx[20];                 	/* LORA接收值 */
void LORA_Proc(void)                	/* LoRa处理 */
{
  if (LORA_Rx(ucLrx) == 0)
  {
    usWT = ((ucLrx[0]-0x30)*10+ucLrx[1]-0x30)*10+ucLrx[2]-0x30;
    usBF = ((ucLrx[3]-0x30)*10+ucLrx[4]-0x30)*10+ucLrx[5]-0x30;
    usRec[ucNUM][0] = usWT;
    usRec[ucNUM][1] = usBF;
    ucNUM++;
  }
}

uint8_t  ucUsec;                    	/* UART发送延时 */
uint8_t  ucUrx[20];                 	/* UART接收值 */
uint8_t  ucUrn;                     	/* UART接收计数 */
void UART_Proc(void)              	  /* UART处理 */
{
  if(ucUsec != ucSec)
  { 
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);      	/* 发送秒值和回车换行 */
  }

  if((ucUrn&0x80) != 0)
  {
    if((ucUrn&0x7f) == 2)
    {
			ucUrx[0] = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30-1;
			if (ucUrx[0] < ucNUM)
				printf("WT:%4.1fKG,BF:%3.1f%%\r\n",
          usRec[ucUrx[0]][0]/10.0, usRec[ucUrx[0]][1]/10.0);
      else
		  printf("ERROR\r\n");
		} else if(((ucUrn&0x7f) == 5) && (ucUrx[2]==','))
    {
      ucUrx[0] = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30;
      ucUrx[3] = (ucUrx[3]-0x30)*10+ucUrx[4]-0x30;
      if ((ucUrx[0]>=5)&&(ucUrx[3]<=45)&&(ucUrx[0]<ucUrx[3]))
      {
				ucMIN = ucUrx[0];
				ucMAX = ucUrx[3];
			} else
		  printf("ERROR\r\n");
    } else
		  printf("ERROR\r\n");
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
