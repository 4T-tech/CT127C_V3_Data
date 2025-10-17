#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;


//串口会存在的状态
#define UART_RX_OK 0x01
#define UART_RX_BUSY 0x02
#define UART_RX_IDLE 0x00
//串口状态变量
uint8_t UART_RX_State = UART_RX_IDLE;
//串口接收缓冲区
uint8_t RX_buf[4];

void BSP_Init()
{
    OLED_Init();

#ifdef USE_EX01_KB6

#endif

    HAL_UART_Receive_IT(&huart2, RX_buf, 4);                                                        //以中断方式开始接收数据 接收4位
    UART_RX_State = UART_RX_BUSY;                                                                   //设置串口接收状态位忙状态
}

uint16_t Time_Num;
uint8_t Key_Flag;

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
        sprintf((char *)oled_buf_Line1, "KEY:%03d", Key_Flag);
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
    else key_val = ASW_None;
#ifdef USE_EX01_KB6

#endif

    KEY.KEY_down = key_val & (KEY.KEY_old ^ key_val);
    KEY.KEY_up = ~key_val & (KEY.KEY_old ^ key_val);
    KEY.KEY_old = key_val;

    if(KEY.KEY_down == ASW1)
    {
        if(State == STATE_MAIN) State = STATE_TIME_NUM;
        else if(State == STATE_TIME_NUM) State = STATE_KEY;
        else if(State == STATE_KEY) State = STATE_MAIN;
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)"ASW1_Is_Down", sizeof("ASW1_Is_Down") - 1);       //以中断方式发送信息（sizeof会将字符串末尾的/0也算上  所以要减一
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_MAIN) State = STATE_KEY;
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;
        else if(State == STATE_KEY) State = STATE_TIME_NUM;
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)"ASW2_Is_Down", sizeof("ASW2_Is_Down") - 1);       //以中断方式发送信息（sizeof会将字符串末尾的/0也算上  所以要减一
    }
}


void UART_Proc(void)                                                                                //串口程序
{
    if(UART_RX_State == UART_RX_OK)                                                                 //轮询检查接收状态是否为接收完成
    {
        UART_RX_State = UART_RX_IDLE;                                                               //如果是接收完成状态 则设置状态为空闲
        if(RX_buf[0] == 'T' && RX_buf[1] == 'i' && RX_buf[2] == 'm' && RX_buf[3] == 'e')            //判断缓冲区内的数据是否为我们的想要的
        {
            uint8_t TX_buf[20];                                                                     //如果是我们想要的 定义一个发送缓冲区
            sprintf((char *)TX_buf, "%d\r\n", Time_Num);                                            //使用sprintf 将要发送的数据添加进缓冲区
            HAL_UART_Transmit(&huart2, TX_buf, strlen((char *)TX_buf), 200);                        //使用阻塞式发送函数 将数据发送出去
        }
        HAL_UART_Receive_IT(&huart2, RX_buf, 4);                                                    //重新开始新的接收
        UART_RX_State = UART_RX_BUSY;                                                               //设置串口状态为忙状态
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)                                             //串口接收回调函数
{
    if(huart->Instance == USART2)                                                                   //如果接收来自串口2
    {
        UART_RX_State = UART_RX_OK;                                                                 //设置串口接收状态为接收完成
    }
}