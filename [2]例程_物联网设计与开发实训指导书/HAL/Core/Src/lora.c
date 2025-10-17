/*
 * STM32WL LoRa���ļ�
 * gsjzbj 2024/08/12
 */

#include "lora.h"

extern SUBGHZ_HandleTypeDef hsubghz;
uint8_t ucLP[6];
uint8_t ucRt;
/*
  ����˯�ߴ���
  ucSleepCfg-˯�����ã� b2-����ѡ��0-������, 1-��������, b0-RTC����ʹ��
  ucStandbyCfg-��������: b0-����ʱ�ӣ�0-RC13��1-HSE32��
*/
void LORA_SetSleepStandby(uint8_t ucSleepCfg, uint8_t ucStandbyCfg)
{
  ucLP[0] = ucSleepCfg;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_SLEEP, ucLP, 1);
  ucLP[0] = ucStandbyCfg;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_STANDBY, ucLP, 1);
}
/*
  ���û������ַ
  ucTBA-���ͻ���ַ��0~255��
  ucRBA-���ͻ���ַ��0~255��
*/
void LORA_SetBufferBaseAddress(uint8_t ucTBA, uint8_t ucRBA)
{
  ucLP[0] = ucTBA;
  ucLP[1] = ucRBA;
  HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, ucLP, 2);
}
/*
  ���ð�����
  ucPT-������: 0-FSK, 1-LORA, 2-BPFK, 3-MSK
*/
void LORA_SetPacketType(uint8_t ucPT)
{
  ucLP[0] = ucPT;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, ucLP, 1);
}
/*
  ���ð�����
  usPL-ǰ���볤�ȣ� 1~0xffff-1~65535
  ucHT-��ͷ���ͣ� 0-��ʽ��1-��ʽ
  ucDL-���ݳ��ȣ� 0~0xff-0~255�ֽ�
  ucCM-CRCģʽ�� 0-��ֹ��1-����
  ucIQ-IQ���ã� 0-��׼IQ��1-��תIQ
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
  ������ƵƵ��
  ulRF: ��ƵƵ�ʣ� 433~510��MHz��
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
  ���ù���
  ucPdc-����ռ�ձȣ�LP��1~7��HP��2~4
  ucHpm-����������ʣ�LP��0��HP��2~7
  ucPas-����ѡ��0-HP��22dBm����1-LP��15dBm��
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
  ���÷��Ͳ���
  ucPW-���ʣ�LP��0xEF~0x0E��-17~14dBm����HP��0xF7~0x16��-9~22dBm��
  ucRT-����ʱ�䣺0-10us��1-20us��2-40us��3-80us��...��7-3400us
*/
void LORA_SetTxParams(uint8_t ucPW, uint8_t ucRT)
{
  ucLP[0] = ucPW;
  ucLP[1] = ucRT;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TXPARAMS, ucLP, 2);
}
/*
  ���õ��Ʋ���
  ucSF-��Ƶ����: 0x05~0x0c-SF5~SF12
  ucBW-�źŴ���: 0x00-7.81kHz, 0x08-10.42kHz, 0x01-15.63kHz, 0x09-20.83kHz,
                  0x02-31.25kHz, 0x0a-41.67kHz, 0x03-62.50kHz, 0x04-125kHz,
                  0x05-250kHz, 0x06-500kHz
  ucCR-���������: 0x00-4/4, 0x01-4/5, 0x02-4/6, 0x03-4/7, 0x04-4/8
  ucLO-�������Ż�: 0-��ֹ, 1-����
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
/* ���ý���ģʽ */
void LORA_SetRxMode(void)
{
  ucLP[0] = ucLP[1] = ucLP[2] = 0xFF;
  ucRt = HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RX, ucLP, 3);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
/* LoRa��ʼ��: usFq-Ƶ�ʣ�MHz����ucPw-���ʣ�dBm�� */
void LORA_Init(uint16_t usFq, uint8_t ucPw)
{
  LORA_SetSleepStandby(0, 0);
  LORA_SetBufferBaseAddress(0, 0x80);	  /* ���ͻ���ַ��0�����ջ���ַ��0x80 */
  LORA_SetPacketType(1);               	/* 1-LORA */
  LORA_SetPacketParams(8, 0, 20, 1, 0);	/* ��ʽ��ͷ�����ݳ���20�ֽڣ�����CRC����׼IQ */
  LORA_SetRFFrequency(usFq);
  LORA_SetPaConfig(4, 0, 1);           	/* 1-�͹��ʣ�15dBm�� */
  LORA_SetTxParams(ucPw, 2);
  LORA_SetModulationParams(7, 4, 1, 0);	/* SF7��125kHz��4/5����ֹ�������Ż� */
  LORA_SetRxMode();
}
/* LoRa����: ucBuf-�������ݣ�ucSize-���ݸ��� */
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
  /* �ȴ�������ɣ���4λ0xA-RC13��������4λ0xC-������� */
  LORA_SetRxMode();
}
/* Lora����: ucBuf-�������ݣ�����ֵ��״̬��0-�ɹ���1-ʧ�ܣ� */
uint8_t LORA_Rx(uint8_t *ucBuff)
{
  ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, ucLP, 1);
  if (ucLP[0] == 0xD4)
  { /* �������: ��4λ0xD-����ģʽ����4λ0x4-������� */
    ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_RXBUFFERSTATUS, ucLP, 2);
    /* ucLP[0]-���ݳ��ȣ�ucLP[1]-��ʼָ�� */
    ucRt = HAL_SUBGHZ_ReadBuffer(&hsubghz, ucLP[1], ucBuff, ucLP[0]);
    LORA_SetRxMode();
    return 0;
  } else
    return 1;
}
/*
  Lora��ȡ״̬�ʹ���: ucStaErr[0]-״̬, ucStaErr[1]����
  ״̬�� ��4λ-����ģʽ�� 0xA-RC13������0xB-HSE32������0xC-Ƶ�ʺϳɣ�0xD-����ģʽ��0xE-����ģʽ
         ��4λ-����״̬�� 0x4-������ɣ�0x6-���ʱ��0x8-�������0xA-����ʧ�ܣ�0xC-�������
  ���� 0x40-PLL����ʧ�ܣ�0x20-HSE32����ʧ�ܣ�0x08-RF-ADCУ׼ʧ�ܣ�0x04-RF-PLLУ׼ʧ��
*/
void LORA_GetStaErr(uint8_t *ucStaErr)
{
  ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, ucLP, 1);
  ucStaErr[0] = ucLP[0];
  ucRt = HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, ucLP, 2);
  ucStaErr[1] = ucLP[1];
}
