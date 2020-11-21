/*
 * motor_control.c
 *
 *  Created on: Aug 27, 2020
 *      Author: Joao Vitor Bassani
 */

#include <motor_control.h>
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1;

direction_t direction = STOP;
uint8_t speed = 0;

void Motor_ChangeDirection( void )
{
	int pwm1 = 0;
	int pwm2 = 0;

	switch( direction )
	{
		case FORWARD:
		{
			pwm1 = 4000 + (speed * 1000);
			pwm2 = 4000 + (speed * 1000);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,pwm1);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,pwm2);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
			break;
		}

		case BACKWARD:
		{
			pwm1 = 4000 + (speed * 1000);
			pwm2 = 4000 + (speed * 1000);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,pwm1);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,pwm2);
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
			break;
		}

		case LEFT:
		{
			pwm1 = 4000 + (speed * 1000);
			pwm2 = 1000 + (speed * 500);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,pwm1);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,pwm2);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
			break;
		}

		case RIGHT:
		{
			pwm1 = 1000 + (speed * 500);
			pwm2 = 4000 + (speed * 1000);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,pwm1);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,pwm2);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
			break;
		}

		case STOP:
		{
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_2);
			break;
		}
	}
}

void Line_Follower_Update( void )
{
	if(HAL_GPIO_ReadPin(TCRT_LF_GPIO_Port, TCRT_LF_Pin))
	{
		direction = RIGHT;
		Motor_ChangeDirection();
	}

	if(HAL_GPIO_ReadPin(TCRT_RG_GPIO_Port, TCRT_RG_Pin))
	{
		direction = LEFT;
		Motor_ChangeDirection();
	}
}
