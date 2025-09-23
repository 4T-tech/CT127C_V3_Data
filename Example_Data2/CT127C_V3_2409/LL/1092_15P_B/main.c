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
/* ����λ��ģ������ն�B����չ������ */
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
uint16_t usDly;                     	/* ��ʱ */
uint8_t  ucLed;                     	/* LEDֵ */
uint16_t usAdc[3];				           	/* ADCת��ֵ */
uint8_t  ucMode;                     	/* b2-LD3״̬��b1-LD2״̬��b0-ģʽ */
uint8_t  ucPV=10;                   	/* ����*10 */
uint8_t  ucCV;                       	/* ��ֵ*10 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* �������� */
void LED_Proc(void);                	/* LED���� */
void OLED_Proc(void);               	/* OLED���� */
void LORA_Proc(void);               	/* LoRa���� */
void DATA_Proc(void);               	/* ���ݴ��� */
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
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_SUBGHZ_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();                       	/* OLED��ʼ�� */
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
    DATA_Proc();                    	/* ���ݴ��� */
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                     	/* ����ֵ */
void KEY_Proc(void)                 	/* �������� */
{
  uint8_t ucKdn;

  ucKdn = KEY_Read();               	/* ������ȡ */
  if(ucKdn != ucKey)                	/* ��ֵ�仯 */
    ucKey = ucKdn;
  else
    ucKdn = 0;

  if(ucKdn == '1')                  	/* SW1���� */
    ucMode ^= 1;                    	/* �л����� */
}

void LED_Proc(void)                 	/* LED���� */
{
  if ((ucLed & 1) == 0)             	/* �Զ����� */
  {
    if (ucCV > ucPV)
    {
      ucLed |= 2;                   	/* LD2���� */
      ucLed &= ~4;                  	/* LD3Ϩ�� */
    } else {
      ucLed &= ~2;                   	/* LD2Ϩ�� */
      ucLed |= 4;                    	/* LD3���� */
    }
  }
  LED_Disp(ucLed);                  	/* LED��ʾ */
}

uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
void OLED_Proc(void)                	/* OLED���� */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* ��ʱ�� */

  if (ucMode == 0)
  {
    sprintf((char*)ucBuf, "    RP1:%2.1fV", usAdc[0]/10.0);
    OLED_ShowString(0, 0, ucBuf, 16);
    sprintf((char*)ucBuf, "    RP2:%2.1fV", usAdc[2]/10.0);
    OLED_ShowString(0, 2, ucBuf, 16);
  } else {
    sprintf((char*)ucBuf, "    CV:%2.1fV ", ucCV/10.0);
    OLED_ShowString(0, 0, ucBuf, 16);
    sprintf((char*)ucBuf, "    PV:%2.1fV ", ucPV/10.0);
    OLED_ShowString(0, 2, ucBuf, 16);
  }
}

uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
void LORA_Proc(void)                	/* LoRa���� */
{
  if(LORA_Rx(ucLrx) == 0)           	/* ������� */
  {
    if (ucLrx[0] == 'C')            	/* ��ѯ */
    {
      ucLrx[1] = ucCV;
      ucLrx[2] = ucPV;
      ucLrx[3] = ucLed;
      LORA_Tx(ucLrx, 4);
    } else if (ucLrx[0] == 'P')     	/* ���� */
    { 
      ucPV = ucLrx[2];
      ucLed = ucLrx[3];
    }
  }
}

uint8_t  ucDdly=50;                 	/* DATA��ʱ */
void DATA_Proc(void)                	/* ���ݴ��� */
{
  if(ucDdly < 100)
    return;
  ucDdly = 0;                      	  /* ��ʱ�� */

  ADC_Read(usAdc);
  usAdc[0] = usAdc[0]*33/4095;
  usAdc[2] = usAdc[2]*33/4095;

  if (usAdc[0] > usAdc[2])
    ucCV = usAdc[0] - usAdc[2];
  else
    ucCV = usAdc[2] - usAdc[0];
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
