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
/* 将电位器模块（EX02）插在终端B的扩展插座上 */
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "subghz.h"
#include "tim.h"
#include "gpio.h"

#include "oled.h"
#include "stdio.h"
#include "lora.h"

uint16_t usDly;                     	/* 延时 */
uint8_t  ucState;                   	/* 状态 */
uint8_t  ucKey;                     	/* 按键值 */
uint8_t  ucLed;                     	/* LED值 */
uint8_t  ucLdly;                     	/* LED闪烁延时 */
uint8_t  ucSec;                     	/* 秒计时 */
uint8_t  ucBuf[20];                  	/* OLED显示值 */
uint8_t  ucLrx[20];                  	/* LoRa接收值 */
uint8_t  ucUrx[20];                 	/* UART接收值 */
uint8_t  ucUsec;                    	/* UART秒计时 */
uint8_t  ucUrn;                      	/* UART接收字节数（最高位为接收完成标志） */
uint16_t usAdc[3];                    /* ADC转换值 */
uint8_t  ucDdly;                      /* 数据延时 */
uint8_t  ucRsta;                     	/* 运行状态 */
uint8_t  ucF1;                        /* 拉力1 */
uint8_t  ucF2;                        /* 拉力2 */
int8_t   scDF;                        /* 拉力差 */
uint8_t  ucFS1=60;                    /* 拉力1满量程 */
uint8_t  ucFS2=60;                    /* 拉力2满量程 */
uint8_t  ucWF;                        /* 获胜标志 */

void SystemClock_Config(void);

void KEY_Proc(void);                	/* 按键处理 */
void LED_Proc(void);                	/* LED处理 */
void OLED_Proc(void);                	/* OLED处理 */
void LORA_Proc(void);                	/* LoRa处理 */
void DATA_Proc(void);               	/* 数据处理 */

int main(void)
{
  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_SUBGHZ_Init();
  MX_TIM1_Init();

  OLED_Init();
  LORA_Init(434, 14);                 /* 434MHz, 14dBm */

  while(1)
  {
    KEY_Proc();                     	/* 按键处理 */
    LED_Proc();                     	/* LED处理 */
    OLED_Proc();                    	/* OLED处理 */
    LORA_Proc();                    	/* LoRa处理 */
    DATA_Proc();                    	/* 数据处理 */
  }
}

void KEY_Proc(void)                 	/* 按键处理 */
{
  uint8_t ucKdn;

  ucKdn = KEY_Read();               	/* 按键读取 */
  if(ucKdn != ucKey)                	/* 键值变化 */
    ucKey = ucKdn;
  else
    ucKdn = 0;

  if(ucKdn == '1')                  	/* SW1按下 */
    if (++ucState == 3)              	/* 切换状态 */
      ucState = 0;
}

void LED_Proc(void)               	  /* LED处理 */
{
  if(ucLdly < 100)                  	/* 闪烁间隔100ms */
    return;
  ucLdly = 0;

  if (((ucWF==1)||(ucWF==2))&&(ucSec<5))
    ucLed ^= 1;                     	/* LD1闪烁 */
  else
    ucLed &= ~1;

  LED_Disp(ucLed);                  	/* LED显示 */
}

void OLED_Proc(void)               	  /* OLED处理 */
{
  uint8_t i;

  if(usDly < 500)
    return;
  usDly = 0;                        	/* 延时到 */

  switch (ucState)
  {
    case 0:
      switch (ucRsta)
      {
        case 0:
          sprintf((char*)ucBuf, "      Idle      ");
          break;
        case 1:
          sprintf((char*)ucBuf, "      Ready     ");
          break;
        case 2:
          sprintf((char*)ucBuf, "      Run       ");
      }
      OLED_ShowString(0, 0, ucBuf, 16);

      if (scDF<=-8)
        sprintf((char*)ucBuf, "##############AA");
      else if (scDF<8)
      {
        for (i=0; i<7-scDF; i++)
          ucBuf[i] = '#';
        ucBuf[i] = 'A';
        ucBuf[i+1] = 'A';
        for (i=9-scDF; i<16; i++)
          ucBuf[i] = '#';
        ucBuf[i] = 0;
      } else
        sprintf((char*)ucBuf, "AA##############");
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 1:
      sprintf((char*)ucBuf, "    F1:%2.1fkN    ", ucF1/10.0);
      OLED_ShowString(0, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "    F2:%2.1fkN    ", ucF2/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
      break;
    case 2:
      sprintf((char*)ucBuf, "    FS1:%2.1fkN   ", ucFS1/10.0);
      OLED_ShowString(0, 0, ucBuf, 16);
      sprintf((char*)ucBuf, "    FS2:%2.1fkN   ", ucFS2/10.0);
      OLED_ShowString(0, 2, ucBuf, 16);
  }
}

void LORA_Proc(void)                	/* LoRa处理 */
{
  if (LORA_Rx(ucLrx) == 0)  	        /* 接收完成 */
  {
    if (ucLrx[0] == '$')
    {
      if (++ucRsta == 3)            	/* 切换采集器状态 */
        ucRsta = 0;
      switch (ucRsta)
      {
        case 0:
          ucLed &= ~6;
          break;
        case 1:
          ucLed |= 2;
          ucLed &= ~4;
          break;
        case 2:
          ucLed |= 6;
      }
      sprintf((char*)ucLrx, "OK\r\n");
      LORA_Tx(ucLrx, 5);
    } else if (ucLrx[0] == '?')
    {
      if (ucWF == 1)
      {	
        sprintf((char*)ucLrx, "RP1:%2.1f\r\n", ucF1/10.0);
        LORA_Tx(ucLrx, 10);
      } else if (ucWF == 2)
      {	
        sprintf((char*)ucLrx, "RP2:%2.1f\r\n", ucF2/10.0);
        LORA_Tx(ucLrx, 10);
      } else{
        sprintf((char*)ucLrx, "NF\r\n");
        LORA_Tx(ucLrx, 5);
      }        
    } else if (ucLrx[0] == '@')
    {
      if (ucRsta == 0)
      {
        TIM1_SetReload((ucLrx[3]<<8)+ucLrx[2]);
        TIM1_SetCompare3((ucLrx[3]<<7)+(ucLrx[2]>>1));
        sprintf((char*)ucLrx, "OK\r\n");
        LORA_Tx(ucLrx, 5);
      } else {
        sprintf((char*)ucLrx, "ERROR\r\n");
        LORA_Tx(ucLrx, 8);
      }
    } else if (ucLrx[0] == 'F')
    {
      if (ucLrx[2] == '1')
        ucFS1 = (ucLrx[4]-'0')*10+ucLrx[6]-'0';
      else
        ucFS2 = (ucLrx[4]-'0')*10+ucLrx[6]-'0';
      sprintf((char*)ucLrx, "OK\r\n");
      LORA_Tx(ucLrx, 5);
    }
  }
}

void DATA_Proc(void)                	/* 数据处理 */
{
  if(ucDdly < 200)                  	/* 采样间隔200ms */
    return;
  ucDdly = 0;

  ADC_Read(usAdc);
  usAdc[0] = usAdc[0]*33/4095;
  usAdc[2] = usAdc[2]*33/4095;

  if (usAdc[0] < 3)
    ucF1 = 0;
  else
    ucF1 = usAdc[0]*ucFS1/30-ucFS1/10;

  if (usAdc[2] < 3)
    ucF2 = 0;
  else
    ucF2 = usAdc[2]*ucFS2/30-ucFS2/10;

  scDF = (ucF1-ucF2)>>1;
  if (ucRsta == 2)
  {
  	if ((scDF<=-7)||(scDF>=7))
  	{
  	  if (ucWF==0)
      {
        ucWF = 3;
        ucSec = 0;
      }
    } else
      ucWF = 0;
    if ((ucWF==3)&&(ucSec>=5))
    {
      if (scDF>=7)
        ucWF = 1;
      else
        ucWF = 2;
      ucRsta = 0;
      ucLed &= ~6;
      ucSec = 0;
    }
  }		
}

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
