#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;

uint8_t LoRa_TX_Buf[5];
uint8_t LoRa_RX_Buf[5];


void BSP_Init()
{
    OLED_Init();
    LORA_Init();                                                    //LoRa初始化
    BSP_RADIO_Init();                                               //loRa控制引脚初始化

#ifdef USE_EX01_KB6
#endif

#ifdef USE_EX03_STS30
#endif
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
        sprintf((char *)oled_buf_Line1, "LoRa_Test");
        break;
    case STATE_LORA_TX:
        if(Old_State != STATE_LORA_TX)
        {
            Old_State = STATE_LORA_TX;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "LoRa_TX:%3d", LoRa_TX_Buf[1]);
        break;
    case STATE_LORA_RX:
        if(Old_State != STATE_LORA_TX)
        {
            Old_State = STATE_LORA_TX;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "LoRa_RX:%3d", LoRa_RX_Buf[0]);
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
        if(State == STATE_MAIN) State = STATE_LORA_TX;
        else if(State == STATE_LORA_TX) State = STATE_LORA_RX;
        else if(State == STATE_LORA_RX) State = STATE_MAIN;
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_LORA_TX)
        {
			HAL_GPIO_WritePin(GPIOB, AL1_Pin, GPIO_PIN_RESET);
            LoRa_TX_Buf[0] = 0xAA;                      //组装帧头
            LoRa_TX_Buf[1] ++;                          //组装数据
            LoRa_TX_Buf[2] = 0x55;                      //组装帧尾
            LORA_Tx(LoRa_TX_Buf, 3);                    //发送帧
			HAL_GPIO_WritePin(GPIOB, AL1_Pin, GPIO_PIN_SET);
        }
    }
}

void LoRa_Proc(void)                                        //LoRa程序
{
    //0xAA 0x55
    uint8_t lora_rx[8];                                     //定义变量来存储收到的信息
    uint8_t lora_con;                                       //收到信息计数用

    if(LORA_Rx(lora_rx, &lora_con) != 0)                    //LoRa_Rx返回值非零表示接收到信息
    {
		HAL_GPIO_WritePin(GPIOB, AL2_Pin, GPIO_PIN_RESET);
		HAL_Delay(20);
        if(lora_rx[0] == 0xAA && lora_rx[2] == 0x55)        //如果接收到的信息符合我们的帧格式
        {
            LoRa_RX_Buf[0] = lora_rx[1];                    //将数据传递给显示
        }
		HAL_GPIO_WritePin(GPIOB, AL2_Pin, GPIO_PIN_SET);
    }
}