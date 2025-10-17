#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;

void BSP_Init()                                                                         //���� �弶֧�ֳ�ʼ��
{
    OLED_Init();                                                                        //OLED��ʼ��

#ifdef USE_EX01_KB6                                                                 //��������� USE_EX01_KB6 ��������´��� ֱ�������ĵ�һ��#endif
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                             //��ʼ���ṹ��

    __HAL_RCC_GPIOB_CLK_ENABLE();                                                       //ʹ��IOʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);                //��ʼ��ƽΪ�͵�ƽ

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                                             //����PB3 Ϊ��������

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                                             //����PA0 PA15 Ϊ��������

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                                             //����PA11 PA12 Ϊ�������
#endif

}

uint16_t Time_Num;
uint8_t Key_Flag;                                                                       //����һ�����������ݰ���������KEY��OLED֮��

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
        sprintf((char *)oled_buf_Line1, "KEY:%03d", Key_Flag);                      //����ֵ��ӡ����ʾ����
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
    else key_val = ASW_None;                                                            //���û�ж�ȡ������ ����ΪASW1��ASW2û�б�����
#ifdef USE_EX01_KB6
    if(key_val == ASW_None)                                                             //���ASW1��ASW2û�б����� ����е�һ��ɨ��
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);                            //���õڶ���Ϊ�ߵ�ƽ
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);                          //���õ�һ��Ϊ�͵�ƽ
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) key_val = EX01_B1;    //��ȡ��һ�� ���Ϊ�͵�ƽ ��B1�����£�Ĭ��PB3Ϊ�������� ��û�б�PA12�ӵ�ʱΪ�ߵ�ƽ
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) key_val = EX01_B2; //���B1û�б����� ���ȡ�ڶ���
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) key_val = EX01_B3; //���B1 B2��û������ ���ȡ������
        else key_val = ASW_None;                                                        //�����һ��û�а��������� ����Ϊû�а���������
    }
    if(key_val == ASW_None)                                                             //������������û�б�����
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);                            //���õ�һ��Ϊ�ߵ�ƽ
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);                          //���õڶ���Ϊ�͵�ƽ
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) key_val = EX01_B4;    //��ȡ��һ�� ���Ϊ�͵�ƽ ��B4������
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) key_val = EX01_B5; //���B5û�б����� ���ȡ�ڶ���;
        else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) key_val = EX01_B6; //���B4 B5��û������ ���ȡ������
        else key_val = ASW_None;                                                        //����ڶ���û�а��������� ����Ϊû�а���������
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
    if(KEY.KEY_down == EX01_B1)                                                         //�������º� ������ֵ
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