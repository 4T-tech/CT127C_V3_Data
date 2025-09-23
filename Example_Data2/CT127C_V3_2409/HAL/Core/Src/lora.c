/*
 * STM32WL LoRa库文件
 * gsjzbj 2024/08/12
 */

#include "lora.h"

extern SUBGHZ_HandleTypeDef hsubghz;
uint8_t ucLP[6];
uint8_t ucRt;
/*
  设置睡眠待机
  ucSleepCfg-睡眠配置： b2-启动选择（0-冷启动, 1-热启动）, b0-RTC唤醒使能
  ucStandbyCfg-待机配置: b0-待机时钟（0-RC13，1-HSE32）
*/
void LORA_SetSleepStandby(uint8_t ucSleepCfg, uint8_t ucStandbyCfg)
{
  ucLP[0] = ucSleepCfg;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_SLEEP, ucLP, 1);
  ucLP[0] = ucStandbyCfg;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_STANDBY, ucLP, 1);
}
/*
  设置缓存基地址
  ucTBA-发送基地址（0~255）
  ucRBA-发送基地址（0~255）
*/
void LORA_SetBufferBaseAddress(uint8_t ucTBA, uint8_t ucRBA)
{
  ucLP[0] = ucTBA;
  ucLP[1] = ucRBA;
  HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, ucLP, 2);
}
/*
  设置包类型
  ucPT-包类型: 0-FSK, 1-LORA, 2-BPFK, 3-MSK
*/
void LORA_SetPacketType(uint8_t ucPT)
{
  ucLP[0] = ucPT;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, ucLP, 1);
}
/*
  设置包参数
  usPL-前导码长度： 1~0xffff-1~65535
  ucHT-报头类型： 0-显式，1-隐式
  ucDL-数据长度： 0~0xff-0~255字节
  ucCM-CRC模式： 0-禁止，1-允许
  ucIQ-IQ设置： 0-标准IQ，1-翻转IQ
*/
void LORA_SetPacketParams(uint16_t usPL, uint8_t ucHT, uint8_t ucDL,
	uint8_t ucCM, uint8_t ucIQ)
{
  ucLP[0] = usPL >> 8;
  ucLP[1] = usPL;
  ucLP[2] = ucHT;
  ucLP[3] = ucDL;
  ucLP[4] = ucCM;
  ucLP[5] = ucIQ;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, ucLP, 6);
}
/*
  设置射频频率
  ulRF: 射频频率： 433~510（MHz）
*/
void LORA_SetRFFrequency(uint32_t ulRF)
{
  ulRF = ulRF << 20;
  ucLP[0] = ulRF >> 24;
  ucLP[1] = ulRF >> 16;
  ucLP[2] = ulRF >> 8;
  ucLP[3] = ulRF;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RFFREQUENCY, ucLP, 4);
}
/*
  设置功放
  ucPdc-功放占空比：LP：1~7，HP：2~4
  ucHpm-功放输出功率：LP：0，HP：2~7
  ucPas-功放选择：0-HP（22dBm），1-LP（15dBm）
*/
void LORA_SetPaConfig(uint8_t ucPdc, uint8_t ucHpm, uint8_t ucPas)
{
  ucLP[0] = ucPdc;
  ucLP[1] = ucHpm;
  ucLP[2] = ucPas;
  ucLP[3] = 1;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACONFIG, ucLP, 4);
}
/*
  设置发送参数
  ucPW-功率：LP：0xEF~0x0E（-17~14dBm），HP：0xF7~0x16（-9~22dBm）
  ucRT-上升时间：0-10us，1-20us，2-40us，3-80us，...，7-3400us
*/
void LORA_SetTxParams(uint8_t ucPW, uint8_t ucRT)
{
  ucLP[0] = ucPW;
  ucLP[1] = ucRT;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TXPARAMS, ucLP, 2);
}
/*
  设置调制参数
  ucSF-扩频因子: 0x05~0x0c-SF5~SF12
  ucBW-信号带宽: 0x00-7.81kHz, 0x08-10.42kHz, 0x01-15.63kHz, 0x09-20.83kHz,
                  0x02-31.25kHz, 0x0a-41.67kHz, 0x03-62.50kHz, 0x04-125kHz,
                  0x05-250kHz, 0x06-500kHz
  ucCR-纠错编码率: 0x00-4/4, 0x01-4/5, 0x02-4/6, 0x03-4/7, 0x04-4/8
  ucLO-低速率优化: 0-禁止, 1-允许
*/
void LORA_SetModulationParams(uint8_t ucSF, uint8_t ucBW, uint8_t ucCR,
  uint8_t ucLO)
{
  ucLP[0] = ucSF;
  ucLP[1] = ucBW;
  ucLP[2] = ucCR;
  ucLP[3] = ucLO;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, ucLP, 4);
}
/* 设置接收模式 */
void LORA_SetRxMode(void)
{
  ucLP[0] = ucLP[1] = ucLP[2] = 0xFF;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RX, ucLP, 3);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
/* LoRa初始化: usFq-频率（MHz），ucPw-功率（dBm） */
void LORA_Init(uint16_t usFq, uint8_t ucPw)
{
  LORA_SetSleepStandby(0, 0);
  LORA_SetBufferBaseAddress(0, 0x80);	  /* 发送基地址：0，接收基地址：0x80 */
  LORA_SetPacketType(1);               	/* 1-LORA */
  LORA_SetPacketParams(8, 0, 20, 1, 0);	/* 显式报头，数据长度20字节，允许CRC，标准IQ */
  LORA_SetRFFrequency(usFq);
  LORA_SetPaConfig(4, 0, 1);           	/* 1-低功率（15dBm） */
  LORA_SetTxParams(ucPw, 2);
  LORA_SetModulationParams(7, 4, 1, 0);	/* SF7，125kHz，4/5，禁止低速率优化 */
  LORA_SetRxMode();
}
/* LoRa发送: ucBuf-发送数据，ucSize-数据个数 */
void LORA_Tx(uint8_t *ucBuf, uint8_t ucSize)
{
  uint16_t i=0;

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  ucRt = HAL_SUBGHZ_WriteBuffer(&hsubghz, 0, ucBuf, ucSize);
  ucLP[0] = ucLP[1] = ucLP[2] = 0;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, ucLP, 3);
  do {
    ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, ucLP, 1);
    i--;
  } while((ucLP[0] != 0xAC) && (i != 0));
  /* 等待发送完成：高4位0xA-RC13待机，低4位0xC-发送完成 */
  LORA_SetRxMode();
}
/* Lora接收: ucBuf-接收数据，返回值：状态（0-成功，1-失败） */
uint8_t LORA_Rx(uint8_t *ucBuff)
{
  ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, ucLP, 1);
  if (ucLP[0] == 0xD4)
  { /* 接收完成: 高4位0xD-接收模式，低4位0x4-接收完成 */
    ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_RXBUFFERSTATUS, ucLP, 2);
    /* ucLP[0]-数据长度，ucLP[1]-起始指针 */
    ucRt = HAL_SUBGHZ_ReadBuffer(&hsubghz, ucLP[1], ucBuff, ucLP[0]);
    LORA_SetRxMode();
    return 0;
  } else
    return 1;
}
/*
  Lora获取状态和错误: ucStaErr[0]-状态, ucStaErr[1]错误
  状态： 高4位-工作模式： 0xA-RC13待机，0xB-HSE32待机，0xC-频率合成，0xD-接收模式，0xE-发送模式
         低4位-命令状态： 0x4-接收完成，0x6-命令超时，0x8-命令错误，0xA-命令失败，0xC-发送完成
  错误： 0x40-PLL锁定失败，0x20-HSE32启动失败，0x08-RF-ADC校准失败，0x04-RF-PLL校准失败
*/
void LORA_GetStaErr(uint8_t *ucStaErr)
{
  ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, ucLP, 1);
  ucStaErr[0] = ucLP[0];
  ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, ucLP, 2);
  ucStaErr[1] = ucLP[1];
}
