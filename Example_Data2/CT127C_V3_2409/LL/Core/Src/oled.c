/*************************************************
Copyright (c)Guoxinchangtian Technologies Co., Ltd. 2008-2019. All rights reserved.
File name: oled.c
Author:      Version:   Date:2019.10.10
Description: OLED显示屏的读写操作。
Others:
History:

1. Date:
Author:
Modification:
*************************************************/
#include "i2c.h"
#include "oled.h"
#include "font.h"
/* 设置位置（参考表4.6） */
void OLED_SetPos(uint8_t x, uint8_t y)
{
  OLED_Write(TYPE_COMMAND, x & 0x0f);
  OLED_Write(TYPE_COMMAND, 0x10 + ((x & 0xf0) >> 4));
  OLED_Write(TYPE_COMMAND, 0xB0 + y);
}
/* 清除屏幕 */
void OLED_Clear(void)
{
  for (uint8_t i = 0; i < 4; i++) {
    OLED_SetPos(0, i);
    for (uint8_t j = 0; j < 128; j++) {
      OLED_Write(TYPE_DATA, 0);
		}
  }
}
/**
  * @brief OLED屏幕初始化。
  * @param None
  * @retval None
  */
void OLED_Init(void)
{
//OLED_Write(TYPE_COMMAND, 0xA0);       /* 段重映射, b0:0,0->0(复位值); */
  OLED_Write(TYPE_COMMAND, 0xA1);       /* 段重映射, b0:1,0->127 */

  OLED_Write(TYPE_COMMAND, 0xA8);       /* 复用比 */
  OLED_Write(TYPE_COMMAND, 0x1F);       /* 0F~3F,16MUX~64MUX(复位值) */

//OLED_Write(TYPE_COMMAND, 0xC0);       /* COM扫描方向, b3:0,正常模式(复位值) */
  OLED_Write(TYPE_COMMAND, 0xC8);       /* COM扫描方向, b3:1,反转模式 */

  OLED_Write(TYPE_COMMAND, 0xDA);       /* COM引脚配置 */
  OLED_Write(TYPE_COMMAND, 0x00);       /* D4:0,顺序;1,交替(复位值) */

  OLED_Write(TYPE_COMMAND, 0x8D);       /* 充电泵 */
  OLED_Write(TYPE_COMMAND, 0x14);       /* D2:0,禁止(复位值);1,允许 */

  OLED_Clear();                         /* 清除屏幕 */
  OLED_Write(TYPE_COMMAND, 0xAF);       /* 开启显示 */
}
/**
  * @brief OLED屏幕显示一个字符。
  * @param x 横向选择位置；
					 y 纵向选择位置；
					 chr 需要显示的字符；
					 size 需要显示的字符大小；
					 @arg 16,选择8*16点阵大小字符。其他均选择6*8点阵字符
  * @retval None
  */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size)
{
  uint8_t  c, i;

  c = chr - ' ';
  if (x > Max_Column - 1) {
    x = 0;
    y = y + 2;
  }
  if (size == 16) {
    OLED_SetPos(x, y);
    for (i = 0; i < 8; i++) {
      OLED_Write(TYPE_DATA, g_F8X16[c * 16 + i]);
    }
    OLED_SetPos(x, y + 1);
    for (i = 0; i < 8; i++) {
      OLED_Write(TYPE_DATA, g_F8X16[c * 16 + i + 8]);
    }
  } else {
    OLED_SetPos(x, y);
    for (i = 0; i < 6; i++) {
      OLED_Write(TYPE_DATA, g_F6x8[c][i]);
    }
  }
}
/**
  * @brief OLED屏幕显示一个字符串。
  * @param x 横向选择位置；
					 y 纵向选择位置；
					 chr 需要显示的字符串；
					 size 需要显示的字符大小；

  * @retval None
  */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size)
{
  uint8_t j = 0;

  while (chr[j] != '\0') {
    OLED_ShowChar(x, y, chr[j], size);
    x += 8;
    if (x > 120) {
      x = 0;
      y += 2;
    }
    ++j;
  }
}
