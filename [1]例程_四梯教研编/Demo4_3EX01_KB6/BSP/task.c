#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;

void BSP_Init()                                                                         //新增 板级支持初始化
{
    OLED_Init();                                                                        //OLED初始化

#ifdef USE_EX01_KB6                                                                 //如果定义了 USE_EX01_KB6 则编译以下代码 直到遇见的第一个#endif
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                             //初始化结构体

    __HAL_RCC_GPIOB_CLK_ENABLE();                                                       //使能IO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);                //初始电平为低电平

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                                             //设置PB3 为上拉输入

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                                             //设置PA0 PA15 为上拉输入

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                                             //设置PA11 PA12 为推挽输出
#endif

}

uint16_t Time_Num;
uint8_t Key_Flag;                                                                       //定义一个变量来传递按键参数在KEY和OLED之间

void OLED_Proc(void)
{
    uint8_t oled_buf_Line1[20] = {0};
    uint8_t oled_buf_Line2[20] = {0};

    if(Task_Time.Oled_Time < OLED_TIME) return;
    Task_Time.Oled_Time = 0;

    switch(State)
    {
    case STATE_MAIN:
        if(Old_State != STATE_MAIN)
        {
            Old_State = STATE_MAIN;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "HELLO_WORLD");
        break;
    case STATE_TIME_NUM:
        if(Old_State != STATE_TIME_NUM)
        {
            Old_State = STATE_TIME_NUM;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "Time:%05d", Time_Num);
        break;
    case STATE_KEY:
        if(Old_State != STATE_KEY)
        {
            Old_State = STATE_KEY;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "KEY:%03d", Key_Flag);                      //将键值打印进显示缓存
        break;
    }
    OLED_ShowString(0, 0, oled_buf_Line1, 16);
    OLED_ShowString(0, 2, oled_buf_Line2, 16);
}

void KEY_Proc(void)
{
    uint8_t key_val;

    if(Task_Time.Key_Time < KEY_TIME) return;
    Task_Time.Key_Time = 0;

    if(HAL_GPIO_ReadPin(ASW1_GPIO_Port, ASW1_Pin) == GPIO_PIN_RESET) key_val = ASW1;
    else if(HAL_GPIO_ReadPin(ASW2_GPIO_Port, ASW2_Pin) == GPIO_PIN_RESET) key_val = ASW2;
    else key_val = ASW_None;                                                            //如果没有读取到按键 则认为ASW1和ASW2没有被按下
#ifdef USE_EX01_KB6
    if(key_val == ASW_None)                                                             //如果ASW1和ASW2没有被按下 则进行第一行扫描
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);                            //设置第二行为高电平
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);                          //设置第一行为低电平
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) key_val = EX01_B1;    //读取第一列 如果为低电平 则B1被按下（默认PB3为上拉输入 再没有被PA12接地时为高电平
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) key_val = EX01_B2; //如果B1没有被按下 则读取第二列
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) key_val = EX01_B3; //如果B1 B2都没被按下 则读取第三列
        else key_val = ASW_None;                                                        //如果第一行没有按键被按下 则认为没有按键被按下
    }
    if(key_val == ASW_None)                                                             //如果五个按键都没有被按下
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);                            //设置第一行为高电平
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);                          //设置第二行为低电平
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) key_val = EX01_B4;    //读取第一列 如果为低电平 则B4被按下
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) key_val = EX01_B5; //如果B5没有被按下 则读取第二列;
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) key_val = EX01_B6; //如果B4 B5都没被按下 则读取第三列
        else key_val = ASW_None;                                                        //如果第二行没有按键被按下 则认为没有按键被按下
    }
#endif

    KEY.KEY_down = key_val & (KEY.KEY_old ^ key_val);
    KEY.KEY_up = ~key_val & (KEY.KEY_old ^ key_val);
    KEY.KEY_old = key_val;

    if(KEY.KEY_down == ASW1)
    {
        if(State == STATE_MAIN) State = STATE_TIME_NUM;
        else if(State == STATE_TIME_NUM) State = STATE_KEY;
        else if(State == STATE_KEY) State = STATE_MAIN;
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_MAIN) State = STATE_KEY;
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;
        else if(State == STATE_KEY) State = STATE_TIME_NUM;
    }
    if(KEY.KEY_down == EX01_B1)                                                         //按键按下后 传递数值
    {
        Key_Flag = EX01_B1;
    }
    if(KEY.KEY_down == EX01_B2)
    {
        Key_Flag = EX01_B2;
    }
    if(KEY.KEY_down == EX01_B3)
    {
        Key_Flag = EX01_B3;
    }
    if(KEY.KEY_down == EX01_B4)
    {
        Key_Flag = EX01_B4;
    }
    if(KEY.KEY_down == EX01_B5)
    {
        Key_Flag = EX01_B5;
    }
    if(KEY.KEY_down == EX01_B6)
    {
        Key_Flag = EX01_B6;
    }
}