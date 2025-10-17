#ifndef __TASK_H
#define __TASK_H

#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "oled.h"
#include "stdio.h"

#define OLED_TIME 200
#define KEY_TIME 10


#define ASW_None 0x00
#define ASW1 0x01
#define ASW2 0x02
#define EX01_B1 0x11
#define EX01_B2 0x12
#define EX01_B3 0x13
#define EX01_B4 0x14
#define EX01_B5 0x15
#define EX01_B6 0x16

#define USE_EX03_STS30                          //定义使用温度计

#ifdef USE_EX03_STS30
extern I2C_HandleTypeDef hi2c2;                 //温度计使用I2C2接口
#endif

// 定义枚举类型表示不同的界面模式
typedef enum
{
    STATE_MAIN = 0,
    STATE_TIME_NUM,
    STATE_KEY,
    STATE_Temperature                           //新增温度计界面
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
float STS30_Proc(void);                         //声明温度计函数

#endif