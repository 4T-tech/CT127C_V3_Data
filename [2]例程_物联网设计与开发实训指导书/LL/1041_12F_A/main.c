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
/* ���¶ȴ�����ģ������ն�A����չ������ */
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
uint8_t  ucState;                   	/* ״̬ */
uint8_t  ucLed=1;                     /* LEDֵ��b0-ģʽ */
uint8_t  ucSec;                     	/* ���ʱ */
uint16_t usTemp;                    	/* �¶�ֵ */
uint8_t  ucPara=25;                  	/* �¶Ȳ��� */
uint8_t  ucAddr=5;                   	/* �ն˵�ַ */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);                	/* �������� */
void LED_Proc(void);                	/* LED���� */
void OLED_Proc(void);               	/* OLED���� */
void LORA_Proc(void);               	/* LoRa���� */
void UART_Proc(void);              	  /* UART���� */
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
    UART_Proc();                     	/* UART���� */
		/* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
uint8_t  ucKey;                     	/* ����ֵ */
uint8_t  ucLong;                    	/* ��������־ */
uint32_t uwKdly;                    	/* ������ʱ */
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

  if(ucKdn == '1')                  	/* SW1���� */
  {
    uwKdly = uwTick;                	/* ���¼�ʱ */
    ucLong = 0;
  }

  if((ucKup == '1') && (ucLong == 0)) /* �����ɿ� */
    if(++ucState == 3)
      ucState = 0;                 	  /* ����״̬0 */

  if(uwTick - uwKdly > 1000)      	  /* ������ */
    if(ucKey && (ucLong == 0))
    {
      ucLong = 1;
      ucLed ^= 1;                   	/* �л�ģʽ */
      if((ucLed&1) == 0)           	  /* Զ��ģʽ */
        ucLed &= ~6;               	  /* Ϩ��LD2��LD3 */
    }
}

void LED_Proc(void)                 	/* LED���� */
{
  if((ucLed&1) == 1)                	/* �Զ�����ģʽ */
  {
    if(usTemp > (ucPara+1)*10)
    {
      ucLed |= 2;                   	/* ����LD2 */
      ucLed &= ~4;                   	/* Ϩ��LD3 */
    }
    else if(usTemp < (ucPara-1)*10)
    {
      ucLed |= 4;                    	/* Ϩ��LD2 */
      ucLed &= ~2;                   	/* ����LD3 */
    }
    else
      ucLed |= 6;                    	/* ����LD2��LD3 */
  }
  LED_Disp(ucLed);                  	/* LED��ʾ */
}

uint16_t usDly;                     	/* ��ʱ */
uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
void OLED_Proc(void)                	/* OLED���� */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* ��ʱ�� */

  usTemp = TEMP_Read()*10;
  switch(ucState)
  {
    case 0:                          	/* �¶���ʾ */
      OLED_ShowString(0, 0, (uint8_t *)"  Temperature", 16);
      sprintf((char*)ucBuf, "      %4.1f", usTemp/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 1:                         	/* ������ʾ */
      OLED_ShowString(0, 0, (uint8_t *)"   Parameter ", 16);
      sprintf((char*)ucBuf, "       %02u ", ucPara);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 2:                          	/* ��ַ��ʾ */
      OLED_ShowString(0, 0, (uint8_t *)"    Address  ", 16);
      sprintf((char*)ucBuf, "      %03u ", ucAddr);
      OLED_ShowString(0, 2, ucBuf, 16);
  }
}

uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
void LORA_Proc(void)                	/* LoRa���� */
{
  if((LORA_Rx(ucLrx)==0) && (ucLrx[0]=='*'))
    if(ucAddr == ((ucLrx[2]-0x30)*100+(ucLrx[3]-0x30)*10+ucLrx[4]-0x30))
    {
      if(ucLrx[1] == '2')
      {
        sprintf((char*)ucBuf, "%02u*", ucPara);
        LORA_Tx(ucBuf, 3);        	  /* ���Ͳ��� */
      }
      if((ucLed&1) == 0)
      {
        if(ucLrx[1] == '4')
          ucLed ^= 2;               	/* �л�LD2״̬ */
        if(ucLrx[1] == '5')
          ucLed ^= 4;               	/* �л�LD3״̬ */
      }
    }
}

uint8_t  ucUsec;                    	/* UART������ʱ */
uint8_t  ucUrx[20];                 	/* UART����ֵ */
uint8_t  ucUrn;                      	/* UART�����ֽ��������λΪ������ɱ�־�� */
void UART_Proc(void)                	/* UART���� */
{
  uint8_t ucTemp;

  if(ucUsec != ucSec)                 /* ������ */
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

int fputc(int ch, FILE *f)       	    /* printf()ʵ�� */
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
