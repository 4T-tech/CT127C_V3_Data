#include "task.h"                                                                               //����ͷ�ļ�

Interface State = STATE_MAIN;                                                                   //��ǰ��ʾ����ö��
Interface Old_State = STATE_MAIN;                                                               //֮ǰ��ʾ����ö��
Task_Time_Adj Task_Time;                                                                        //�������м���ṹ��
KEY_State KEY;                                                                                  //��������״̬�ṹ��

uint16_t Time_Num;                                                                              //һ��������ʱ���������

void OLED_Proc(void)                                                                            //OLED����
{
    uint8_t oled_buf_Line1[20] = {0};                                                           //�����һ�е���ʾ����
    uint8_t oled_buf_Line2[20] = {0};                                                           //����ڶ��е���ʾ����

    if(Task_Time.Oled_Time < OLED_TIME) return;                                                 //���û�е������м��Ҫ���򷵻�
    Task_Time.Oled_Time = 0;                                                                    //������������м��Ҫ�����������м������ʼ����

    switch(State)                                                                               //���ݵ�ǰ��ʾ����ö����ѡ��Ӧ����ʾʲô
    {
    case STATE_MAIN:                                                                        //�����ǰ��ʾ������
        if(Old_State != STATE_MAIN)
        {
            Old_State = STATE_MAIN;    //���֮ǰ������������ ������֮ǰ��ʾ����Ϊ������ �������Ļ�ϵ�ȫ������
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "HELLO_WORLD");                                     //������Ҫ��ʾ������������ʾ������
        break;                                                                              //����
    case STATE_TIME_NUM:                                                                    //�����ǰ��ʾ����ΪTIME_NUM����
        if(Old_State != STATE_TIME_NUM)
        {
            Old_State = STATE_TIME_NUM;    //���֮ǰ������TIME_NUM���� ������֮ǰ��ʾ����ΪTIME_NUM���� �������Ļ�ϵ�ȫ������
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "Time:%05d", Time_Num);                             //������Ҫ��ʾ������������ʾ������
        break;                                                                              //����
    }
    OLED_ShowString(0, 0, oled_buf_Line1, 16);                                                  //������ʾ��������������������ˢ�µ���Ļ
    OLED_ShowString(0, 2, oled_buf_Line2, 16);
}

void KEY_Proc(void)                                                                             //��������
{
    uint8_t key_val;                                                                            //���尴����ֵ�洢���� ���������ʵʱ��Ӧ����״̬

    if(Task_Time.Key_Time < KEY_TIME) return;                                                   //���û�е������м��Ҫ���򷵻�
    Task_Time.Key_Time = 0;                                                                     //������������м��Ҫ�����������м������ʼ����

    if(HAL_GPIO_ReadPin(ASW1_GPIO_Port, ASW1_Pin) == GPIO_PIN_RESET) key_val = ASW1;            //������µİ�����ASW1���� ����ֵ����key_val
    else key_val = ASW_None;                                                                    //������Ϊû�а�������

    KEY.KEY_down = key_val & (KEY.KEY_old ^ key_val);                                           //������������
    KEY.KEY_up = ~key_val & (KEY.KEY_old ^ key_val);
    KEY.KEY_old = key_val;

    if(KEY.KEY_down == ASW1)                                                                    //�������ASW1����
    {
        if(State == STATE_MAIN) State = STATE_TIME_NUM;                                         //�����ǰ�������� ���л����浽TIME_NUM����
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;                                    //�����ǰ��TIME_NUM���� ���л����浽������
        KEY.KEY_down = ASW_None;                                                                //�����ֵ ��ֹ��δ���
    }
}