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
#include "adc.h"
#include "i2c.h"
#include "spi.h"
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
uint8_t  ucState;                   	/* ״̬ */
uint8_t  ucSec;                     	/* ���ʱ */
uint8_t  ucCnt;                      	/* �������� */
uint16_t usTim[2];                    /* TIM��׽ֵ */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* �������� */
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
//MX_ADC_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_SUBGHZ_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();                       	/* OLED��ʼ�� */

  EEPROM_Read((uint8_t *)&ucCnt, 0, 1); 	/* �洢���� */
  ucCnt++;
  EEPROM_Write((uint8_t *)&ucCnt, 0, 1);	/* �洢��д */

  LORA_Init(434, 14);                 /* 434MHz, 14dBm */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                     	/* �������� */
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
uint8_t  ucKey;                     	/* ����ֵ */
void KEY_Proc(void)                 	/* �������� */
{
  uint8_t ucKdn, ucKup;

  ucKdn = KEY_Read();               	/* ������ȡ */
  if(ucKdn != ucKey)                	/* ��ֵ�仯 */
  {
    ucKup = ucKey;                   	/* �����ֵ */
    ucKey = ucKdn;
  } else {
    ucKup = 0;                       	/* �����ֵ */
    ucKdn = 0;
  }
  if(ucKdn == '1')                  	/* SW1�������� */
    ucState ^= 1;                   	/* �л�״̬ */
  if(ucKdn == '2')                  	/* SW2�������� */
    ucState ^= 2;                   	/* �л�״̬ */
}

uint8_t  ucLed;                     	/* LEDֵ */
void LED_Proc(void)                 	/* LED���� */
{
  LED_Disp(ucLed);                  	/* LED��ʾ */
}

uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
void OLED_Proc(void)                	/* OLED���� */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* ��ʱ�� */

  sprintf((char*)ucBuf, "SEC:%03u CNT:%03u ", ucSec, ucCnt);
  OLED_ShowString(0, 0, ucBuf, 16);

  TIM2_GetCapture(usTim);             /* TIM2��ȡ����ֵ */
  sprintf((char*)ucBuf, "WID:%03u PER:%03u ", usTim[1], usTim[0]);
  OLED_ShowString(0, 2, ucBuf, 16);
}

uint8_t  ucLsec;                    	/* LoRa���ʱ */
uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
void LORA_Proc(void)                	/* LoRa���� */
{
  if (ucLsec != ucSec)
  {
    ucLsec = ucSec;
    ucLed ^= 1;                      	/* �л�����״̬ */
    sprintf((char*)ucBuf, "%03u", ucSec);
    LORA_Tx(ucBuf, 3);              	/* ����3���ַ� */
  }
  if (LORA_Rx(ucLrx) != 0)  	/* ����3���ַ� */
  {
    ucLed ^= 2;                      	/* �л�����״̬ */
    ucLrx[3] = 0;
  }
}

uint8_t  ucUsec;                    	/* UART���ʱ */
uint8_t  ucUrx[20];                 	/* UART����ֵ */
uint8_t  ucUrn;                      	/* UART�����ֽ��������λΪ������ɱ�־�� */
void UART_Proc(void)              	/* UART���� */
{
  if(ucUsec != ucSec)
  {                                   	/* 1s�� */
    ucUsec = ucSec;

    printf("%03u\r\n", ucSec);    	/* ������ֵ�ͻس����� */
  }
  if ((ucUrn&0x80) != 0)           	/* ������� */
  {
    if ((ucUrn&0x7f) == 2)         	/* ���յ�2���ַ� */
      ucSec = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30;
    ucUrn = 0;
  }
}

int fputc(int ch, FILE *f)       	/* printf()ʵ�� */
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
