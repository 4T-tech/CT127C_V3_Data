/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB6   ------> I2C1_SCL
  PB7   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x10805D88;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}
/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetI2CClockSource(LL_RCC_I2C2_CLKSOURCE_PCLK1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**I2C2 GPIO Configuration
  PA11   ------> I2C2_SDA
  PA12   ------> I2C2_SCL
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11|LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C2);
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x10805D88;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C2, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/* USER CODE BEGIN 1 */
/* OLED写入 */
void OLED_Write(uint8_t ucType, uint8_t ucData)	
{
  LL_I2C_HandleTransfer(I2C1, 0x78, LL_I2C_ADDRSLAVE_7BIT, 2,
    LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
  while(!LL_I2C_IsActiveFlag_TXIS(I2C1));				/* 等待发送就绪 */
  LL_I2C_TransmitData8(I2C1, ucType);			  		/* 发送控制字节 */
  while(!LL_I2C_IsActiveFlag_TXIS(I2C1));				/* 等待发送就绪 */
  LL_I2C_TransmitData8(I2C1, ucData);			  		/* 发送命令数据 */
}
/* EEPROM写入 */
void EEPROM_Write(uint8_t *ucBuf, uint8_t ucAddr, uint8_t ucNum)
{
  LL_I2C_HandleTransfer(I2C1, 0xA0, LL_I2C_ADDRSLAVE_7BIT, ucNum+1,
    LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
  while(!LL_I2C_IsActiveFlag_TXIS(I2C1));				/* 等待发送就绪 */
  LL_I2C_TransmitData8(I2C1, ucAddr);			      /* 发送数据地址 */
  for (uint8_t i=0; i<ucNum; i++)
  {
    while (!LL_I2C_IsActiveFlag_TXE(I2C1)) ;		/* 等待发送寄存器空 */
    LL_I2C_TransmitData8(I2C1, ucBuf[i]);       /* 发送数据 */
  }
}
/* EEPROM读取 */
void EEPROM_Read(uint8_t *ucBuf, uint8_t ucAddr, uint8_t ucNum)
{
  LL_I2C_HandleTransfer(I2C1, 0xa0, LL_I2C_ADDRSLAVE_7BIT, 1,
    LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
  while(!LL_I2C_IsActiveFlag_TXIS(I2C1));				/* 等待发送就绪 */
//while(!LL_I2C_IsActiveFlag_TXE(I2C1));				/* 等待发送寄存器空 */
  LL_I2C_TransmitData8(I2C1, ucAddr);				  	/* 发送数据地址 */
  while(!LL_I2C_IsActiveFlag_TC(I2C1));	   			/* 等待发送完成 */
  LL_I2C_HandleTransfer(I2C1, 0xa0, LL_I2C_ADDRSLAVE_7BIT, ucNum,
    LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
  for (uint8_t i=0; i<ucNum; i++)
  {
    while (!LL_I2C_IsActiveFlag_RXNE(I2C1));		/* 等待接收寄存器不空 */
    ucBuf[i] = LL_I2C_ReceiveData8(I2C1);       /* 接收数据 */
  }
}
/* 温度读取 */
float TEMP_Read(void)
{
  uint8_t ucData[3];
	
  LL_I2C_HandleTransfer(I2C2, 0x94, LL_I2C_ADDRSLAVE_7BIT, 2,
    LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
  while(!LL_I2C_IsActiveFlag_TXIS(I2C2));		/* 等待发送就绪 */
  LL_I2C_TransmitData8(I2C2, 0x24);				/* 发送命令高字节 */
  while(!LL_I2C_IsActiveFlag_TXIS(I2C2));		/* 等待发送就绪 */
  LL_I2C_TransmitData8(I2C2, 0x0B);				/* 发送命令低字节 */
  HAL_Delay(5);

  LL_I2C_HandleTransfer(I2C2, 0x95, LL_I2C_ADDRSLAVE_7BIT, 3,
    LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
  for(uint8_t i=0; i<3; i++)
  {
    while(!LL_I2C_IsActiveFlag_RXNE(I2C2)){};	/* 等待接收就绪 */
    ucData[i] = LL_I2C_ReceiveData8(I2C2);		/* 接收数据 */
  }
  return (ucData[0] << 8 | ucData[1]) * 175.0 / 65535 - 45;
}
/* USER CODE END 1 */
