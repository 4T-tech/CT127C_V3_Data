#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;


#define USER_TASK_DEBUG                                                                                                                 //ʹ��USER_TASK_DEBUG ʹ�������Ҫ�����ض��� ��Ҫʹ��΢��
#ifdef USER_TASK_DEBUG                                                                                                                  //���ʹ��USER_TASK_DEBUG��������´���
#define user_task_printf(format, ...) printf( format "\r\n", ##__VA_ARGS__)                                                             //��user_task_printf�滻Ϊprintf���Զ����Ӹ�ʽ
#define user_task_info(format, ...) printf("[task]info:" format "\r\n", ##__VA_ARGS__)                                                  //...
#define user_task_debug(format, ...) printf("[task]debug:" format "\r\n", ##__VA_ARGS__)                                                //...
#define user_task_error(format, ...) printf("[task]error:" format "\r\n",##__VA_ARGS__)                                                 //...
#else                                                                                                                                   //�����ʹ����������´���
#define user_task_printf(format, ...)                                                                                                   //�����ʹ�����滻Ϊ �ո� ��ɾ������
#define user_task_info(format, ...)                                                                                                     //...
#define user_task_debug(format, ...)                                                                                                    //...
#define user_task_error(format, ...)                                                                                                    //...
#endif

int fputc(int ch, FILE *f)                                                                                                              //���ڴ�ӡ�ض���
{
    uint8_t temp[1] = {ch};
    HAL_UART_Transmit(&huart2, temp, 1, 2);
    return ch;
}


#define USE_DEBUG                                                                                                                       //ʹ��USE_DEBUG
#ifdef USE_DEBUG                                                                                                                        //���ʹ��USE_DEBUG ��������´���
uint8_t printfBuf[100];                                                                                                                 //���巢�ͻ����� ����־���� ����Ҫʹ���ض��� ����Ҫ����΢�� �ҿ�ʹ���жϷ�ʽ��ӡ
#define LOG_DEBUG(format, ...) HAL_UART_Transmit((UART_HandleTypeDef *)&huart2, (uint8_t *)printfBuf, sprintf((char*)printfBuf, "[DEBUG] %s:%d %s(): " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__), 200)
#define LOG_INFO(format, ...) HAL_UART_Transmit((UART_HandleTypeDef *)&huart2, (uint8_t *)printfBuf, sprintf((char*)printfBuf, "[INFO] %d %s(): " format "\n", __LINE__, __func__, ##__VA_ARGS__), 200)
#else
#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#endif



#define UART_RX_OK 0x01
#define UART_RX_BUSY 0x02
#define UART_RX_IDLE 0x00
uint8_t UART_RX_State = UART_RX_IDLE;
uint8_t RX_buf[4];

void BSP_Init()
{
    OLED_Init();

#ifdef USE_EX01_KB6
#endif

    HAL_UART_Receive_IT(&huart2, RX_buf, 4);
    UART_RX_State = UART_RX_BUSY;

    user_task_printf("Init_OK");                                                            //LOG��ӡЧ������
    user_task_info("Init_OK");                                                              //LOG��ӡЧ������
    user_task_debug("Init_OK");                                                             //LOG��ӡЧ������
    user_task_error("Init_OK");                                                             //LOG��ӡЧ������
    LOG_DEBUG("Init_OK");                                                                   //LOG��ӡЧ������
    LOG_INFO("Init_OK");                                                                    //LOG��ӡЧ������
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
        sprintf((char *)oled_buf_Line2, "KEY:%03d", Key_Flag);
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
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)"ASW1_Is_Down", sizeof("ASW1_Is_Down"));
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_MAIN) State = STATE_KEY;
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;
        else if(State == STATE_KEY) State = STATE_TIME_NUM;
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)"ASW2_Is_Down", sizeof("ASW2_Is_Down"));
    }
}


void UART_Proc(void)
{
    if(UART_RX_State == UART_RX_OK)
    {
        UART_RX_State = UART_RX_IDLE;
        if(RX_buf[0] == 'T' && RX_buf[1] == 'i' && RX_buf[2] == 'm' && RX_buf[3] == 'e')
        {
            uint8_t TX_buf[20];
            sprintf((char *)TX_buf, "%d\r\n", Time_Num);
            HAL_UART_Transmit(&huart2, TX_buf, strlen((char *)TX_buf), 200);
        }
        HAL_UART_Receive_IT(&huart2, RX_buf, 4);
        UART_RX_State = UART_RX_BUSY;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
        UART_RX_State = UART_RX_OK;
    }
}