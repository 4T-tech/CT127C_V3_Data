#include "task.h"

Interface State = STATE_MAIN;
Interface Old_State = STATE_MAIN;
Task_Time_Adj Task_Time;
KEY_State KEY;


void BSP_Init()
{
    OLED_Init();

#ifdef USE_EX01_KB6
#endif

#ifdef USE_EX02_ADC
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                     //ADC初始化 从CubeMX生成的代码中获得
    /* USER CODE BEGIN ADC_MspInit 0 */

    /* USER CODE END ADC_MspInit 0 */
    /* ADC clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC GPIO Configuration
    PB3     ------> ADC_IN2
    PB4     ------> ADC_IN3
    PA15     ------> ADC_IN11
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_11, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
    case STATE_ADC_IN3:
        if(Old_State != STATE_ADC_IN3)
        {
            Old_State = STATE_ADC_IN3;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "IN3:%4d", ADC_Read(ADC_IN3));                          //ADC显示不同的通道 不同的入口参数
        break;
    case STATE_ADC_AIN1:
        if(Old_State != STATE_ADC_AIN1)
        {
            Old_State = STATE_ADC_AIN1;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "AIN1:%4d", ADC_Read(ADC_AIN1));
        break;
    case STATE_ADC_AIN2:
        if(Old_State != STATE_ADC_AIN2)
        {
            Old_State = STATE_ADC_AIN2;
            OLED_Clear();
        }
        sprintf((char *)oled_buf_Line1, "AIN2:%4d", ADC_Read(ADC_AIN2));
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
        else if(State == STATE_KEY) State = STATE_ADC_IN3;
        else if(State == STATE_ADC_IN3) State = STATE_ADC_AIN1;
        else if(State == STATE_ADC_AIN1) State = STATE_ADC_AIN2;
        else if(State == STATE_ADC_AIN2) State = STATE_MAIN;
    }
    if(KEY.KEY_down == ASW2)
    {
        if(State == STATE_MAIN) State = STATE_ADC_AIN2;
        else if(State == STATE_TIME_NUM) State = STATE_MAIN;
        else if(State == STATE_KEY) State = STATE_TIME_NUM;
        else if(State == STATE_ADC_IN3) State = STATE_KEY;
        else if(State == STATE_ADC_AIN1) State = STATE_ADC_IN3;
        else if(State == STATE_ADC_AIN2) State = STATE_ADC_AIN1;
    }
}

uint16_t ADC_Read(uint8_t ADC_CHANNEL)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    if(ADC_CHANNEL == ADC_IN3)                                          //板载电位器
    {
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
        if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        HAL_ADC_Start(&hadc);
        while(HAL_ADC_PollForConversion(&hadc, 200) != HAL_OK);
        return HAL_ADC_GetValue(&hadc);
    }
    else if(ADC_CHANNEL == ADC_AIN1)                                    //扩展电位器一
    {
        sConfig.Channel = ADC_CHANNEL_2;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
        if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        HAL_ADC_Start(&hadc);
        while(HAL_ADC_PollForConversion(&hadc, 200) != HAL_OK);
        return HAL_ADC_GetValue(&hadc);
    }
    else if(ADC_CHANNEL == ADC_AIN2)                                    //扩展电位器二
    {
        sConfig.Channel = ADC_CHANNEL_11;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
        if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        HAL_ADC_Start(&hadc);
        while(HAL_ADC_PollForConversion(&hadc, 200) != HAL_OK);
        return HAL_ADC_GetValue(&hadc);
    }
    else return 0;
}