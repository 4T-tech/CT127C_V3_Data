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
/* ��ADC & PULSEģ�飨EX04�������ն�A����չ������ */
#include "main.h"
#include "i2c.h"
#include "subghz.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "oled.h"
#include "stdio.h"
#include "lora.h"

uint16_t usDly;                     	/* ��ʱ */
uint8_t  ucLed;                     	/* LEDֵ */
uint8_t  ucSec;                     	/* ���ʱ */
uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
uint8_t  ucLrn;                      	/* LoRa�����ֽ��� */
uint8_t  ucUrx[20];                 	/* UART����ֵ */
uint8_t  ucUsec;                    	/* UART���ʱ */
uint8_t  ucUrn;                      	/* UART�����ֽ��������λΪ������ɱ�־�� */
uint16_t usTim[2];                    /* TIM��׽ֵ */

void SystemClock_Config(void);

void LED_Proc(void);                	/* LED���� */
void OLED_Proc(void);               	/* OLED���� */
void LORA_Proc(void);               	/* LoRa���� */
void UART_Proc(void);               	/* UART���� */

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
    LED_Proc();                     	/* LED���� */
    OLED_Proc();                    	/* OLED���� */
    LORA_Proc();                    	/* LoRa���� */
    UART_Proc();                    	/* UART���� */
  }
}

void LED_Proc(void)                 	/* LED���� */
{
  if ((ucLrx[0] != 0) && (ucSec >= 3))
    ucLed &= ~1;         	            /* LD1Ϩ�� */
  LED_Disp(ucLed);                  	/* LED��ʾ */
}

void OLED_Proc(void)               	  /* OLED���� */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* ��ʱ�� */

  if (ucUrx[0]==0)
    sprintf((char*)ucBuf, "NF");
  else
    sprintf((char*)ucBuf, "%s        ", ucUrx);
  OLED_ShowString(0, 0, ucBuf, 16);

  TIM2_GetCapture(usTim);             /* TIM2��ȡ����ֵ */
  sprintf((char*)ucBuf, "F:%05uHz    ", 1000000/usTim[1]);
  OLED_ShowString(0, 2, ucBuf, 16);
}

void LORA_Proc(void)                	/* LoRa���� */
{
  if (LORA_Rx(ucLrx) == 0)          	/* ������� */
  {
    printf("%s", ucLrx);
    ucLed |= 1;          	            /* LD1���� */
    ucSec = 0;        	              /* ��ʼ��ʱ */
  }
}

void UART_Proc(void)              	  /* UART���� */
{
  if(ucUsec != ucSec)
  {                                   /* 1s�� */
    ucUsec = ucSec;

    printf("%03u\r\n", ucSec);        /* ������ֵ�ͻس����� */
  }

  if ((ucUrn&0x80) != 0)            	/* ������� */
  {
    if ((ucUrn&0x7f) == 1)		    	  /* ���յ�1���ַ� */
    {
      switch (ucUrx[0])
      {
        case '$':
        case '?':
          ucUrx[1] = 0;
          LORA_Tx(ucUrx, 1);	      	/* ��������/��ѯ���� */
          break;
        case '@':
          ucUrx[1] = 0;
          ucUrx[2] = usTim[1];
          ucUrx[3] = usTim[1]>>8;
          LORA_Tx(ucUrx, 4);   	      /* ����ͬ������ */
          break;
        default:
          printf("ERROR\r\n");
      }
    }
    else if ((ucUrn&0x7f) == 7)		    /* ���յ�7���ַ� */
    {
      ucUrx[7] = 0;
      LORA_Tx(ucUrx, 7);            	/* ���Ͳ������� */
    } else
      printf("ERROR\r\n");
    ucUrn = 0;                      	/* ������ռ��� */
  }
}

int fputc(int ch, FILE *f)			      /* printf()ʵ�� */
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
