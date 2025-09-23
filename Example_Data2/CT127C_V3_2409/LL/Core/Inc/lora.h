/*
 * STM32WL LoRaÍ·ÎÄ¼þ
 * gsjzbj 2024/08/12
 */

#ifndef __LORA_H__
#define __LORA_H__
#include "main.h"

void LORA_Init(uint16_t ucFq, uint8_t ucPw);
void LORA_Tx(uint8_t *ucBuf, uint8_t ucSize);
uint8_t LORA_Rx(uint8_t *ucBuf);
void LORA_GetStaErr(uint8_t *ucStaErr);
#endif /* __LORA_H__ */
