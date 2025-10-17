#ifndef __TASK_H
#define __TASK_H

#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "oled.h"                               //ע��oled.h Ϊ���������� ��Ҫ��i2c.h�в���
#include "stdio.h"
#include "string.h"

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

//#define USE_EX01_KB6

// ����ö�����ͱ�ʾ��ͬ�Ľ���ģʽ
typedef enum
{
    STATE_MAIN = 0,
    STATE_TIME_NUM,
    STATE_KEY
} Interface;

extern Interface State;

//�ṹ��
typedef struct
{
    uint8_t Key_Time;
    uint8_t Oled_Time;
} Task_Time_Adj;

extern Task_Time_Adj Task_Time;


//�ṹ��
typedef struct                                  //�������м����ʱ���� ����ֵ��SysTick��ʱ���ж������� ע����Ӵ���
{
    uint8_t KEY_down;
    uint8_t KEY_up;
    uint8_t KEY_old;
} KEY_State;


extern uint16_t Time_Num;

void BSP_Init(void);
void OLED_Proc(void);
void KEY_Proc(void);
void UART_Proc(void);

#endif