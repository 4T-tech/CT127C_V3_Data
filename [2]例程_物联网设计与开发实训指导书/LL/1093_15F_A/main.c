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
/* 将ADC & PULSE模块（EX04）插在终端A的扩展插座上 */
#include "main.h"
#include "i2c.h"
#include "subghz.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "oled.h"
#include "stdio.h"
#include "lora.h"

uint16_t usDly;                     	/* 延时 */
uint8_t  ucLed;                     	/* LED值 */
uint8_t  ucSec;                     	/* 秒计时 */
uint8_t  ucBuf[20];                  	/* OLED显示值 */
uint8_t  ucLrx[20];                  	/* LoRa接收值 */
uint8_t  ucLrn;                      	/* LoRa接收字节数 */
uint8_t  ucUrx[20];                 	/* UART接收值 */
uint8_t  ucUsec;                    	/* UART秒计时 */
uint8_t  ucUrn;                      	/* UART接收字节数（最高位为接收完成标志） */
uint16_t usTim[2];                    /* TIM捕捉值 */

void SystemClock_Config(void);

void LED_Proc(void);                	/* LED处理 */
void OLED_Proc(void);               	/* OLED处理 */
void LORA_Proc(void);               	/* LoRa处理 */
void UART_Proc(void);               	/* UART处理 */

int main(void)
{
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SUBGHZ_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();

  OLED_Init();
  LORA_Init(434, 14);                 /* 434MHz, 14dBm */
  while (1)
  {
    LED_Proc();                     	/* LED处理 */
    OLED_Proc();                    	/* OLED处理 */
    LORA_Proc();                    	/* LoRa处理 */
    UART_Proc();                    	/* UART处理 */
  }
}

void LED_Proc(void)                 	/* LED处理 */
{
  if ((ucLrx[0] != 0) && (ucSec >= 3))
    ucLed &= ~1;         	            /* LD1熄灭 */
  LED_Disp(ucLed);                  	/* LED显示 */
}

void OLED_Proc(void)               	  /* OLED处理 */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* 延时到 */

  if (ucUrx[0]==0)
    sprintf((char*)ucBuf, "NF");
  else
    sprintf((char*)ucBuf, "%s        ", ucUrx);
  OLED_ShowString(0, 0, ucBuf, 16);

  TIM2_GetCapture(usTim);             /* TIM2获取捕获值 */
  sprintf((char*)ucBuf, "F:%05uHz    ", 1000000/usTim[1]);
  OLED_ShowString(0, 2, ucBuf, 16);
}

void LORA_Proc(void)                	/* LoRa处理 */
{
  if (LORA_Rx(ucLrx) == 0)          	/* 接收完成 */
  {
    printf("%s", ucLrx);
    ucLed |= 1;          	            /* LD1点亮 */
    ucSec = 0;        	              /* 开始计时 */
  }
}

void UART_Proc(void)              	  /* UART处理 */
{
  if(ucUsec != ucSec)
  {                                   /* 1s到 */
    ucUsec = ucSec;

    printf("%03u\r\n", ucSec);        /* 发送秒值和回车换行 */
  }

  if ((ucUrn&0x80) != 0)            	/* 接收完成 */
  {
    if ((ucUrn&0x7f) == 1)		    	  /* 接收到1个字符 */
    {
      switch (ucUrx[0])
      {
        case '$':
        case '?':
          ucUrx[1] = 0;
          LORA_Tx(ucUrx, 1);	      	/* 发送配置/查询命令 */
          break;
        case '@':
          ucUrx[1] = 0;
          ucUrx[2] = usTim[1];
          ucUrx[3] = usTim[1]>>8;
          LORA_Tx(ucUrx, 4);   	      /* 发送同步命令 */
          break;
        default:
          printf("ERROR\r\n");
      }
    }
    else if ((ucUrn&0x7f) == 7)		    /* 接收到7个字符 */
    {
      ucUrx[7] = 0;
      LORA_Tx(ucUrx, 7);            	/* 发送参数命令 */
    } else
      printf("ERROR\r\n");
    ucUrn = 0;                      	/* 清除接收计数 */
  }
}

int fputc(int ch, FILE *f)			      /* printf()实现 */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
}

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
