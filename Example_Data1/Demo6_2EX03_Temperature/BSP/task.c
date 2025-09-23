#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;

#define UART_RX_OK 0x01
#define UART_RX_BUSY 0x02
#define UART_RX_IDLE 0x00
uint8_t UART_RX_State = UART_RX_IDLE;
uint8_t RX_buf[4];


#ifdef USE_EX03_STS30
I2C_HandleTypeDef hi2c2;
#endif

void BSP_Init()
{
    OLED_Init();

#ifdef USE_EX01_KB6
#endif

#ifdef USE_EX03_STS30                                                                           //����i2c2��ʼ��
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                                         //GPIO��ʼ���ṹ��
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};                                             //����ʱ�ӳ�ʼ���ṹ��

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)                                  //����I2C2ʱ��
    {
        Error_Handler();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();                                                                   //GPIOAʱ��ʹ��
    /**I2C2 GPIO Configuration
    PA11     ------> I2C2_SDA
    PA12     ------> I2C2_SCL
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;                                                      //��ʼ��IOΪ���ÿ�©
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();                                                                    //I2C2ʱ��ʹ��

    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x0090194B;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)                                                             //��ʼ��I2C2
    {
        Error_Handler();
    }
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        Error_Handler();
    }

    uint8_t temp_data_iic[2] = {0x24, 0x0B};                                                        //д�����ݻ���
    HAL_I2C_Master_Transmit(&hi2c2, 0x94, temp_data_iic, 2, 10);                                    //����һ���¶�ת�� ֮��Ϳ��Զ�ȡ�����õ�������ת���ٵȴ�ת�����

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
    case STATE_Temperature:
        if(Old_State != STATE_Temperature)
        {
            Old_State = STATE_Temperature;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "T:%3.1f", STS30_Proc());
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
        else if(State == STATE_KEY) State = STATE_Temperature;
        else if(State == STATE_Temperature) State = STATE_MAIN;
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_MAIN) State = STATE_Temperature;
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;
        else if(State == STATE_KEY) State = STATE_TIME_NUM;
        else if(State == STATE_Temperature) State = STATE_KEY;
    }
}

float STS30_Proc(void)                                                                              //�¶ȼƳ���
{
#ifdef USE_EX03_STS30
    uint8_t data_T[2] = {0x24, 0x0B};                                                               //д�����ݻ�����
    uint8_t data_R[2] = {0x00, 0x00};                                                               //��ȡ���ݻ�����

    HAL_I2C_Master_Receive(&hi2c2, 0x94, data_R, 2, 10);                                            //��ʼ��ʱ������һ���¶�ת�� ��������ȡ ����֮ǰ���й����¶�ת�� ���ڶ�ȡ
    HAL_I2C_Master_Transmit(&hi2c2, 0x94, data_T, 2, 10);                                           //�����¶�ת�� Ϊ��һ������׼������

    return (float)((data_R[0] << 8 | data_R[1]) * 175.0 / 65535 - 45);                              //���㲢�����¶Ƚ��
#endif
    return 0;
}