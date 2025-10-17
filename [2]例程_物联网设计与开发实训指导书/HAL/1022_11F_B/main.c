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
/* �����󰴼�ģ������ն�B����չ������ */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
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
uint8_t  ucSec;                     	/* ���ʱ */
uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* �������� */
void LED_Proc(void);                	/* LED���� */
void LORA_Proc(void);               	/* LoRa���� */
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
  /* USER CODE BEGIN 2 */
  OLED_Init();                       	/* OLED��ʼ�� */
  LORA_Init(434, 14);                 /* 434MHz, 14dBm */

  OLED_ShowString(0, 0, (uint8_t *)"  Temperature   ", 16);
  OLED_ShowString(0, 2, (uint8_t *)"      20.0      ", 16);
	/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                     	/* �������� */
    LED_Proc();                     	/* LED���� */
    LORA_Proc();                    	/* LoRa���� */
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
    ucKey = ucKdn;                  	/* �����ֵ */
  else
    ucKdn = 0;                       	/* �����ֵ */

  if(ucKdn > 'B')
  {
    ucBuf[0] = '*';		  							/* ��ʼ�ַ� */
    ucBuf[1] = ucKdn;
    LORA_Tx(ucBuf, 2);								/* ����2���ַ� */
  }
}

void LED_Proc(void)
{
  LED_Disp(ucLed);
}

uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
void LORA_Proc(void)                	/* LoRa���� */
{
  uint16_t usTemp;
  uint8_t  ucTmax, ucTmin;

  if((LORA_Rx(ucLrx)==0) && (ucLrx[5]=='*'))
  {
    switch(ucLrx[0])
    {
      case '1':
        usTemp = (ucLrx[2]-'0')*100
          +(ucLrx[3]-'0')*10+ucLrx[4]-'0';
        OLED_ShowString(0, 0, (uint8_t *)"  Temperature", 16);
        sprintf((char*)ucBuf, "      %4.1f    ", usTemp/10.0);
        OLED_ShowString(0, 2, ucBuf, 16);
        break;
      case '2':
        ucTmax = (ucLrx[1]-'0')*10+ucLrx[2]-'0';
        sprintf((char*)ucBuf, "    Tmax:%02u    ", ucTmax);
        OLED_ShowString(0, 0, ucBuf, 16);
        ucTmin = (ucLrx[3]-'0')*10+ucLrx[4]-'0';
        sprintf((char*)ucBuf, "    Tmin:%02u    ", ucTmin);
        OLED_ShowString(0, 2, ucBuf, 16);
        break;
      case '4':
        OLED_ShowString(0, 0, (uint8_t *)"   LD2 Status   ", 16);
        sprintf((char*)ucBuf, "       %c    ", ucLrx[4]);
        OLED_ShowString(0, 2, ucBuf, 16);
        break;
      case '5':
        OLED_ShowString(0, 0, (uint8_t *)"   LD3 Status   ", 16);
        sprintf((char*)ucBuf, "       %c    ", ucLrx[4]);
        OLED_ShowString(0, 2, ucBuf, 16);
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
