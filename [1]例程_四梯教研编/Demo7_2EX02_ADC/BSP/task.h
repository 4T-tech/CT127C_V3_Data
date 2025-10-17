#ifndef __TASK_H
#define __TASK_H

#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "oled.h"
#include "stdio.h"
#include "adc.h"

#define OLED_TIME 200
#define KEY_TIME 10


#define ASW_None 0x00
#define ASW1 0x01
#define ASW2 0x02

#define ADC_IN3 3                       //电位器旋钮 定义的数值随意 不重复即可
#define ADC_AIN1 2
#define ADC_AIN2 11

#define USE_EX02_ADC

// 定义枚举类型表示不同的界面模式
typedef enum
{
    STATE_MAIN = 0,
    STATE_TIME_NUM,
    STATE_KEY,
    STATE_ADC_IN3,
    STATE_ADC_AIN1,
    STATE_ADC_AIN2
} Interface;

extern Interface State;


//结构体
typedef struct
{
    uint8_t Key_Time;
    uint8_t Oled_Time;
} Task_Time_Adj;

extern Task_Time_Adj Task_Time;


//结构体
typedef struct
{
    uint8_t KEY_down;
    uint8_t KEY_up;
    uint8_t KEY_old;
} KEY_State;


extern uint16_t Time_Num;

void BSP_Init(void);
void OLED_Proc(void);
void KEY_Proc(void);
uint16_t ADC_Read(uint8_t ADC_CHANNEL); //声明ADC读取函数

#endif