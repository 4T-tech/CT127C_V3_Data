#ifndef __TASK_H                                //避免重复定义
#define __TASK_H

#include "main.h"                               //引入头文件
#include "i2c.h"
#include "oled.h"                               //注意oled.h 为非完整代码 需要在i2c.h中补充
#include "stdio.h"

#define OLED_TIME 200                           //任务运行间隔控制
#define KEY_TIME 10


#define ASW_None 0x00                           //按键键值设置 数值可随意设置 不重复即可
#define ASW1 0x01
#define ASW2 0x02

// 定义枚举类型表示不同的界面模式
typedef enum
{
    STATE_MAIN = 0,
    STATE_TIME_NUM
} Interface;

extern Interface State;

//结构体
typedef struct                                  //任务运行间隔计时变量 该数值在SysTick定时器中断中增加 注意添加代码
{
    uint8_t Key_Time;
    uint8_t Oled_Time;
} Task_Time_Adj;

extern Task_Time_Adj Task_Time;


//结构体
typedef struct                                  //三行消抖 按键状态
{
    uint8_t KEY_down;
    uint8_t KEY_up;
    uint8_t KEY_old;
} KEY_State;


extern uint16_t Time_Num;                       //启动时间计时器

void OLED_Proc(void);                           //函数声明
void KEY_Proc(void);

#endif