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
uint8_t  ucSec;                     	/* ���ʱ */
uint16_t usTemp;                     	/* �¶�ֵ*10 */
uint8_t  ucTmax=26, ucTmin=22;      	/* �¶������� */
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
void KEY_Proc(void)                 	/* �������� */
{
  uint8_t ucKdn;

  ucKdn = KEY_Read();               	/* ������ȡ */
  if(ucKdn != ucKey)                	/* ��ֵ�仯 */
    ucKey = ucKdn;                  	/* �����ֵ */
  else
    ucKdn = 0;                       	/* �����ֵ */

  if(ucKdn == '1')                  	/* SW1�������� */
    ucState ^= 1;                   	/* �л�״̬ */
}

uint8_t  ucLed;                     	/* LEDֵ */
void LED_Proc(void)                 	/* LED���� */
{
  usTemp = TEMP_Read()*10;
  if(usTemp > ucTmax*10)
    ucLed = 2;												/* LD2������LD3Ϩ�� */
  else if(usTemp < ucTmin*10)
    ucLed = 4;												/* LD2Ϩ��LD3���� */
  else
    ucLed = 0;												/* LD2��LD3Ϩ�� */
  LED_Disp(ucLed);
}

uint16_t usDly;                     	/* ��ʱ */
uint8_t  ucBuf[20];                  	/* OLED��ʾֵ */
void OLED_Proc(void)                	/* OLED���� */
{
  if(usDly < 500)
    return;
  usDly = 0;                         	/* ��ʱ�� */

  if(ucState == 0)
  {
    OLED_ShowString(0, 0, (uint8_t *)"  Temperature", 16);
    sprintf((char*)ucBuf, "      %4.1f    ", usTemp/10.0);
    OLED_ShowString(0, 2, ucBuf, 16);
  } else {
    sprintf((char*)ucBuf, "    Tmax:%02u    ", ucTmax);
    OLED_ShowString(0, 0, ucBuf, 16);
    sprintf((char*)ucBuf, "    Tmin:%02u    ", ucTmin);
    OLED_ShowString(0, 2, ucBuf, 16);
  }
}

uint8_t  ucLrx[20];                  	/* LoRa����ֵ */
void LORA_Proc(void)                	/* LoRa���� */
{
  if ((LORA_Rx(ucLrx) == 0) && (ucLrx[0] == '*'))  	/* ����2���ַ� */
  {
    switch(ucLrx[1])
    {
      case 'B'+1:
        sprintf((char*)ucBuf, "1%04u*", usTemp);
        break;
      case 'B'+2:
        sprintf((char*)ucBuf, "2%02u%02u*", ucTmax, ucTmin);
        break;
      case 'B'+4:
        sprintf((char*)ucBuf, "4%04u*", (ucLed>>1)&1);
        break;
      case 'B'+5:
        sprintf((char*)ucBuf, "5%04u*", ucLed>>2);
    }
    LORA_Tx(ucBuf, 6);
  }
}

uint8_t  ucUsec;                    	/* UART���ʱ */
uint8_t  ucUrx[20];                 	/* UART����ֵ */
uint8_t  ucUrn;                      	/* UART�����ֽ��������λΪ������ɱ�־�� */
void UART_Proc(void)              	  /* UART���� */
{
  uint8_t ucTemp;

  if(ucUsec != ucSec)                 /* ������ */
  { 
    ucUsec = ucSec;
    printf("%03u\r\n", ucSec);    	  /* ������ֵ�ͻس����� */
  }
	
  if ((ucUrn&0x80) != 0)
  {
    if ((ucUrn&0x7f) == 5)
    {
			if((ucUrx[0]=='M')&&(ucUrx[1]=='A')
        &&(ucUrx[2]=='X'))		        /* ���� */
      {
        ucTemp = (ucUrx[3]-0x30)*10+ucUrx[4]-0x30;
        if(ucTemp > ucTmin)
        {
          ucTmax = ucTemp;
          printf("OK\r\n");
        } else
          printf("ERROR\r\n");	      /* �߼����� */
      }
      else if((ucUrx[0]=='M')&&(ucUrx[1]=='I')
        &&(ucUrx[2]=='N'))		        /* ���� */
      {
        ucTemp = (ucUrx[3]-0x30)*10+ucUrx[4]-0x30;
        if(ucTemp < ucTmax)
        {
          ucTmin = ucTemp;
          printf("OK\r\n");
        } else
          printf("ERROR\r\n");		    /* �߼����� */
      } else
        printf("ERROR\r\n");		      /* ������� */
    } else
      printf("ERROR\r\n");		        /* ������� */
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
