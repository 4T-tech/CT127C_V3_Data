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

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_2|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB5 PB2 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_2|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */
uint8_t KEY_Read(void)            	/* 按键读取 */
{
  uint8_t ucVal = 0;
  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0)
  {                                 /* SW1按键按下(PA8=0) */
    HAL_Delay(10);                 	/* 延时10ms消抖 */
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0)
      ucVal = '1';                 	/* 赋值键值'1' */
  }
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 0)
  {                                 /* SW2按键按下(PB8=0) */
    HAL_Delay(10);                 	/* 延时10ms消抖 */
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 0)
      ucVal = '2';                 	/* 赋值键值'2' */
  }
/* 扫描第1列 */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  if ((GPIOA->IDR & 0x1800) != 0x1800)
  {                                 /* B1或B4按下 */
    HAL_Delay(10);                 	/* 延时10ms消抖 */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0)
      ucVal = 'B'+1;                /* 赋值键值'C' */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0)
      ucVal = 'B'+4;                /* 赋值键值'F' */
  }
/* 扫描第2列 */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  if ((GPIOA->IDR & 0x1800) != 0x1800)
  {                                	/* B2或B5按下 */
    HAL_Delay(10);                 	/* 延时10ms消抖 */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0)
      ucVal = 'B'+2;                /* 赋值键值'D' */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0)
      ucVal = 'B'+5;                /* 赋值键值'G' */
  }
/* 扫描第3列 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
  if ((GPIOA->IDR & 0x1800) != 0x1800)
  {                                	/* B3或B6按下 */
    HAL_Delay(10);                 	/* 延时10ms消抖 */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0)
      ucVal = 'B'+3;               	/* 赋值键值'E' */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0)
      ucVal = 'B'+6;               	/* 赋值键值'H' */
  }
  return ucVal;                    	/* 返回键值 */
}

void LED_Disp(uint8_t ucLed)      	/* LED显示 */
{
  if((ucLed&1) == 1)               	/* 点亮LD1 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
  else                              /* 熄灭LD1 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);

  if((ucLed&2) == 2)               	/* 点亮LD2 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
  else                             	/* 熄灭LD2 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

  if((ucLed&4) == 4)               	/* 点亮LD3 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  else                              /* 熄灭LD3 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}
/* USER CODE END 2 */
