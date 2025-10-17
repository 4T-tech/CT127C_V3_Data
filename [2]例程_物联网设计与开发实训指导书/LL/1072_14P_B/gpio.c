/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5|LL_GPIO_PIN_2|LL_GPIO_PIN_12);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

  /**/
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_2|LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11|LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */
uint8_t KEY_Read(void)            	/* ������ȡ */
{
  uint8_t ucVal = 0;

  if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_8) == 0)
  {                                	/* SW1��������(PA8=0) */
    LL_mDelay(10);                 	/* ��ʱ10ms���� */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_8) == 0)
      ucVal = '1';                 	/* ��ֵ��ֵ'1' */
  }
  if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_8) == 0)
  {                                	/* SW2��������(PB8=0) */
    LL_mDelay(10);                 	/* ��ʱ10ms���� */
    if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_8) == 0)
      ucVal = '2';                 	/* ��ֵ��ֵ'2' */
  }
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
  if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11|LL_GPIO_PIN_12) != 1)
  {                               	/* B1��B4���� */
    LL_mDelay(10);			        		/* ��ʱ10ms���� */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12) == 0)
      ucVal = 'B'+1;       	     		/* ��ֵ��ֵ'C' */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11) == 0)
      ucVal = 'B'+4;  		       		/* ��ֵ��ֵ'F' */
  }
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
  if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11|LL_GPIO_PIN_12) != 1)
  {                               	/* B2��B5���� */
    LL_mDelay(10);			        		/* ��ʱ10ms���� */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12) == 0)
      ucVal = 'B'+2;       	     		/* ��ֵ��ֵ'D' */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11) == 0)
      ucVal = 'B'+5;  		       		/* ��ֵ��ֵ'G' */
  }
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);
  if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11|LL_GPIO_PIN_12) != 1)
  {                               	/* B3��B6���� */
    LL_mDelay(10);			        		/* ��ʱ10ms���� */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12) == 0)
      ucVal = 'B'+3;       	     		/* ��ֵ��ֵ'E' */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_11) == 0)
      ucVal = 'B'+6;  		       		/* ��ֵ��ֵ'H' */
  }
  return ucVal;                    	/* ���ؼ�ֵ */
}

void LED_Disp(uint8_t ucLed)      	/* LED��ʾ */
{
  if((ucLed&1) == 1)              	/* ����LD1 */
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);
  else                             	/* Ϩ��LD1 */
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

  if((ucLed&2) == 2)               	/* ����LD2 */
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
  else                             	/* Ϩ��LD2 */
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);

  if((ucLed&4) == 4)              	/* ����LD3 */
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
  else                             	/* Ϩ��LD3 */
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
}
/* USER CODE END 2 */
