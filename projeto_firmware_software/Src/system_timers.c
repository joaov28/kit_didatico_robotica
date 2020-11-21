/*
 * system_timers.c
 *
 *  Created on: Aug 30, 2020
 *      Author: Joao Vitor Bassani
 */

#include "stm32f1xx_hal.h"
#include "system_timers.h"

void delay_us( uint16_t interval )
{
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	while((uint16_t)__HAL_TIM_GET_COUNTER(&htim3) < interval);
}

void Timer_Init(timers_t *timer, modes_t mode)
{
	timer->start_tick = 0;
	timer->timeout = 0;
	timer->timer_mode = mode;
}

void Timer_Start(timers_t *timer, time curr_tick, time interval)
{
	timer->start_tick = (curr_tick? curr_tick : 1);
	timer->timeout = interval;
}

void Timer_Stop(timers_t *timer)
{
	timer->start_tick = 0;
}

time timeDiff(time curr_tick, time prev_tick)
{
	time diff;

	if(curr_tick >= prev_tick) { diff = curr_tick - prev_tick;}
	else { diff = (0xFFFFFFFF - prev_tick) + curr_tick;}

	return diff;
}

bool Timer_Timeout(timers_t *timer, time curr_tick)
{
	const bool t_out = (timer->start_tick && timeDiff(curr_tick, timer->start_tick) > timer->timeout);
	if(t_out)
	{
		Timer_Stop(timer);
		if(timer->timer_mode == CONTINUOUS_MODE)
			Timer_Start(timer, curr_tick, timer->timeout);
	}

	return t_out;
}
