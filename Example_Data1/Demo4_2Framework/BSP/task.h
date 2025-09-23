#ifndef __TASK_H                                //�����ظ�����
#define __TASK_H

#include "main.h"                               //����ͷ�ļ�
#include "i2c.h"
#include "oled.h"                               //ע��oled.h Ϊ���������� ��Ҫ��i2c.h�в���
#include "stdio.h"

#define OLED_TIME 200                           //�������м������
#define KEY_TIME 10


#define ASW_None 0x00                           //������ֵ���� ��ֵ���������� ���ظ�����
#define ASW1 0x01
#define ASW2 0x02

// ����ö�����ͱ�ʾ��ͬ�Ľ���ģʽ
typedef enum
{
    STATE_MAIN = 0,
    STATE_TIME_NUM
} Interface;

extern Interface State;

//�ṹ��
typedef struct                                  //�������м����ʱ���� ����ֵ��SysTick��ʱ���ж������� ע����Ӵ���
{
    uint8_t Key_Time;
    uint8_t Oled_Time;
} Task_Time_Adj;

extern Task_Time_Adj Task_Time;


//�ṹ��
typedef struct                                  //�������� ����״̬
{
    uint8_t KEY_down;
    uint8_t KEY_up;
    uint8_t KEY_old;
} KEY_State;


extern uint16_t Time_Num;                       //����ʱ���ʱ��

void OLED_Proc(void);                           //��������
void KEY_Proc(void);

#endif