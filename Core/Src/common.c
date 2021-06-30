#include <stdint.h>
#include <string.h>
#include <stdio.h> // sprintf
#include "common.h"
#include "tim.h"
#include "HD44780.h"
#include "gpio.h"

extern TIM_HandleTypeDef htim1;

MotorCluster data_cluster;

void Motor_Init()
{
    data_cluster.speed = 11;
    data_cluster.m_state = motor_DIS;
    data_cluster.p_state = HAL_GPIO_ReadPin(EN_PORT, EN_Pin);
    data_cluster.gear_ratio = 55.87; // 50/9)*(33/25)*(32/33)*(220/28)
    HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, SET);
    HAL_GPIO_WritePin(SLEEP_PORT, SLEEP_PIN, SET);
    HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, SET);
    HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, SET);
    
}

void UpdateSpeed(void)
{
    // if (data_cluster.m_state == motor_EN) return; // if the motor is running, do not change
    static uint16_t LastTimerCounter = 0;
    if (data_cluster.m_state == motor_EN) // if the motor is running, do not change
        LastTimerCounter = htim1.Instance->CNT; 
    int TimerDif = htim1.Instance->CNT - LastTimerCounter;
    if(TimerDif >= 4 || TimerDif <= -4)
    {
        TimerDif /= 4;
        data_cluster.speed += (int8_t)TimerDif;
        if(data_cluster.speed > 38) data_cluster.speed = 38;
        if(data_cluster.speed < 11) data_cluster.speed = 11;
        LastTimerCounter = htim1.Instance->CNT;
        ChangeSpeed();
    }
}


void UpdateState(void)
{

	static uint8_t key_next_lock=0;

	if( !key_next_lock && !(HAL_GPIO_ReadPin(EN_PORT, EN_Pin) ) ) {

		key_next_lock=1;

		if (data_cluster.p_state == pin_DIS) data_cluster.p_state = pin_EN;
        else if (data_cluster.p_state == pin_EN) data_cluster.p_state = pin_DIS;
        else ; // should be imposible

        if (data_cluster.m_state == motor_EN) {
            data_cluster.m_state = motor_DIS;
            StopMotor();
        }
        else if (data_cluster.m_state == motor_DIS) {
            data_cluster.m_state = motor_EN;
            StartMotor();
        }

	} else if( key_next_lock && (HAL_GPIO_ReadPin(EN_PORT, EN_Pin))) key_next_lock++;


}

void StartMotor(void)
{
    //TODO
    float multiplier = ((data_cluster.speed) * data_cluster.gear_ratio) / 600;
    // uint8_t multiplier = data_cluster.speed;
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 72 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = (2500 - 1)/multiplier;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = (1250)/multiplier;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim2);

    HAL_TIM_PWM_Start(TIM, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, RESET);
    
}

void StopMotor(void)
{
    //TODO
    HAL_TIM_PWM_Stop(TIM, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(ENABLE_PORT, ENABLE_PIN, SET);
}

void ChangeSpeed(void)
{
    //TODO
    // TIM_MasterConfigTypeDef sMasterConfig = {0};
    // TIM_OC_InitTypeDef sConfigOC = {0};

    // /* USER CODE BEGIN TIM2_Init 1 */

    // /* USER CODE END TIM2_Init 1 */
    // htim2.Instance = TIM2;
    // htim2.Init.Prescaler = 72 - 1;
    // htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    // htim2.Init.Period = 1000 - 1;
    // htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    // if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    // sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    // if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // sConfigOC.OCMode = TIM_OCMODE_PWM1;
    // sConfigOC.Pulse = 500;
    // sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    // sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    // if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    // {
    //     Error_Handler();
    // }
    // /* USER CODE BEGIN TIM2_Init 2 */

    // /* USER CODE END TIM2_Init 2 */
    // HAL_TIM_MspPostInit(&htim2);
}

void ReloadDisplay(disp_lv lv)
{
    if (lv == disp_lv_0) {
        char buff_txt[16];
        if (data_cluster.speed < 1) {
            sprintf(buff_txt, "0.%d obr/min", data_cluster.speed);
        }
        else {
            sprintf(buff_txt, "%d.%d obr/min", data_cluster.speed/10, data_cluster.speed%10);
        }
        
        LCD_Send_Str_Pos(buff_txt, 3, 0);

    }
    else if (lv == disp_lv_1) {
        if (data_cluster.m_state == motor_DIS) {
            char * txt_buff = "DISABLED";
            LCD_Send_Str_Pos(txt_buff, 4, 1);
        }
        else if (data_cluster.m_state == motor_EN) {
            char * txt_buff = "ENABLED ";
            LCD_Send_Str_Pos(txt_buff, 4, 1);
        }
        else {
            ;
        }
    }
    else {
        ;
        // should be imposible
    }
}

int8_t get_speed(void) 
{
    return data_cluster.speed;
}

motor_state get_state(void)
{
    return data_cluster.m_state;
}


