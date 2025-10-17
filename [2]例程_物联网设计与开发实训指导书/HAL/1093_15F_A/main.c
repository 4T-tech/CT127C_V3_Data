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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "subghz.h"
#include "tim.h"
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
uint16_t usDly;                     	/* ��ʱ */
uint8_t  ucLed;                     	/* LEDֵ */
uint8_t  ucSec;                     	/* ���ʱ */
uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
uint8_t  ucUrx[20];                 	/* UART����ֵ */
uint16_t usTim[2];                    /* TIM��׽ֵ */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_Proc(void);                	/* LED���� */
void OLED_Proc(void);               	/* OLED���� */
void LORA_Proc(void);               	/* LoRa���� */
void UART_Proc(void);               	/* UART���� */
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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();                       	/* OLED��ʼ�� */
  LORA_Init(434, 14);                 /* 434MHz, 14dBm */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    LED_Proc();                     	/* LED���� */
    OLED_Proc();                    	/* OLED���� */
    LORA_Proc();                    	/* LoRa���� */
    UART_Proc();                    	/* UART���� */
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void LED_Proc(void)                 	/* LED���� */
{
  if ((ucLrx[0] != 0) && (ucSec >= 3))
    ucLed &= ~1;         	            /* LD1Ϩ�� */
  LED_Disp(ucLed);                  	/* LED��ʾ */
}

uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
void OLED_Proc(void)                	/* OLED���� */
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

uint8_t  ucUsec;                    	/* UART���ʱ */
uint8_t  ucUrn;                      	/* UART�����ֽ��������λΪ������ɱ�־�� */
void UART_Proc(void)              	  /* UART���� */
{
  if(ucUsec != ucSec)
  {                                  	/* 1s�� */
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);    	  /* ������ֵ�ͻس����� */
  }

  if ((ucUrn&0x80) != 0)            	/* ������� */
  {
    if ((ucUrn&0x7f) == 1)		      	/* ���յ�1���ַ� */
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

int fputc(int ch, FILE *f)          	/* printf()ʵ�� */
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
