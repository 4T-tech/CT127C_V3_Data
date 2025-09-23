#include "task.h"                                                                               //任务头文件

Interface State = STATE_MAIN;                                                                   //当前显示界面枚举
Interface Old_State = STATE_MAIN;                                                               //之前显示界面枚举
Task_Time_Adj Task_Time;                                                                        //任务运行间隔结构体
KEY_State KEY;                                                                                  //按键按下状态结构体

uint16_t Time_Num;                                                                              //一个随启动时间递增的数

void OLED_Proc(void)                                                                            //OLED任务
{
    uint8_t oled_buf_Line1[20] = {0};                                                           //定义第一行的显示缓存
    uint8_t oled_buf_Line2[20] = {0};                                                           //定义第二行的显示缓存

    if(Task_Time.Oled_Time < OLED_TIME) return;                                                 //如果没有到达运行间隔要求则返回
    Task_Time.Oled_Time = 0;                                                                    //如果到达了运行间隔要求则清零运行间隔并开始运行

    switch(State)                                                                               //根据当前显示界面枚举来选择应该显示什么
    {
    case STATE_MAIN:                                                                        //如果当前显示主界面
        if(Old_State != STATE_MAIN)
        {
            Old_State = STATE_MAIN;    //如果之前不是在主界面 则设置之前显示界面为主界面 并清除屏幕上的全部内容
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "HELLO_WORLD");                                     //将我们要显示的内容填充进显示缓冲区
        break;                                                                              //结束
    case STATE_TIME_NUM:                                                                    //如果当前显示界面为TIME_NUM界面
        if(Old_State != STATE_TIME_NUM)
        {
            Old_State = STATE_TIME_NUM;    //如果之前不是在TIME_NUM界面 则设置之前显示界面为TIME_NUM界面 并清除屏幕上的全部内容
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "Time:%05d", Time_Num);                             //将我们要显示的内容填充进显示缓冲区
        break;                                                                              //结束
    }
    OLED_ShowString(0, 0, oled_buf_Line1, 16);                                                  //根据显示缓冲区的内容来将数据刷新到屏幕
    OLED_ShowString(0, 2, oled_buf_Line2, 16);
}

void KEY_Proc(void)                                                                             //按键任务
{
    uint8_t key_val;                                                                            //定义按键键值存储变量 这个变量会实时反应按键状态

    if(Task_Time.Key_Time < KEY_TIME) return;                                                   //如果没有到达运行间隔要求则返回
    Task_Time.Key_Time = 0;                                                                     //如果到达了运行间隔要求则清零运行间隔并开始运行

    if(HAL_GPIO_ReadPin(ASW1_GPIO_Port, ASW1_Pin) == GPIO_PIN_RESET) key_val = ASW1;            //如果按下的按键是ASW1按键 则将数值存入key_val
    else key_val = ASW_None;                                                                    //否则认为没有按键按下

    KEY.KEY_down = key_val & (KEY.KEY_old ^ key_val);                                           //经典三行消抖
    KEY.KEY_up = ~key_val & (KEY.KEY_old ^ key_val);
    KEY.KEY_old = key_val;

    if(KEY.KEY_down == ASW1)                                                                    //如果按键ASW1按下
    {
        if(State == STATE_MAIN) State = STATE_TIME_NUM;                                         //如果当前在主界面 则切换界面到TIME_NUM界面
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;                                    //如果当前在TIME_NUM界面 则切换界面到主界面
        KEY.KEY_down = ASW_None;                                                                //清除数值 防止多次触发
    }
}