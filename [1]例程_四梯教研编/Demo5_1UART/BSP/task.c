#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;


//���ڻ���ڵ�״̬
#define UART_RX_OK 0x01
#define UART_RX_BUSY 0x02
#define UART_RX_IDLE 0x00
//����״̬����
uint8_t UART_RX_State = UART_RX_IDLE;
//���ڽ��ջ�����
uint8_t RX_buf[4];

void BSP_Init()
{
    OLED_Init();

#ifdef USE_EX01_KB6

#endif

    HAL_UART_Receive_IT(&huart2, RX_buf, 4);                                                        //���жϷ�ʽ��ʼ�������� ����4λ
    UART_RX_State = UART_RX_BUSY;                                                                   //���ô��ڽ���״̬λæ״̬
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
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)"ASW1_Is_Down", sizeof("ASW1_Is_Down") - 1);       //���жϷ�ʽ������Ϣ��sizeof�Ὣ�ַ���ĩβ��/0Ҳ����  ����Ҫ��һ
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_MAIN) State = STATE_KEY;
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;
        else if(State == STATE_KEY) State = STATE_TIME_NUM;
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)"ASW2_Is_Down", sizeof("ASW2_Is_Down") - 1);       //���жϷ�ʽ������Ϣ��sizeof�Ὣ�ַ���ĩβ��/0Ҳ����  ����Ҫ��һ
    }
}


void UART_Proc(void)                                                                                //���ڳ���
{
    if(UART_RX_State == UART_RX_OK)                                                                 //��ѯ������״̬�Ƿ�Ϊ�������
    {
        UART_RX_State = UART_RX_IDLE;                                                               //����ǽ������״̬ ������״̬Ϊ����
        if(RX_buf[0] == 'T' && RX_buf[1] == 'i' && RX_buf[2] == 'm' && RX_buf[3] == 'e')            //�жϻ������ڵ������Ƿ�Ϊ���ǵ���Ҫ��
        {
            uint8_t TX_buf[20];                                                                     //�����������Ҫ�� ����һ�����ͻ�����
            sprintf((char *)TX_buf, "%d\r\n", Time_Num);                                            //ʹ��sprintf ��Ҫ���͵�������ӽ�������
            HAL_UART_Transmit(&huart2, TX_buf, strlen((char *)TX_buf), 200);                        //ʹ������ʽ���ͺ��� �����ݷ��ͳ�ȥ
        }
        HAL_UART_Receive_IT(&huart2, RX_buf, 4);                                                    //���¿�ʼ�µĽ���
        UART_RX_State = UART_RX_BUSY;                                                               //���ô���״̬Ϊæ״̬
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)                                             //���ڽ��ջص�����
{
    if(huart->Instance == USART2)                                                                   //����������Դ���2
    {
        UART_RX_State = UART_RX_OK;                                                                 //���ô��ڽ���״̬Ϊ�������
    }
}