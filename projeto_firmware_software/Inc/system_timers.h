/*
 * system_timers.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Joao Vitor Bassani
 */

#ifndef SYSTEM_TIMERS_H_
#define SYSTEM_TIMERS_H_

#include <stdbool.h>

extern TIM_HandleTypeDef htim3;

typedef uint32_t time;

typedef enum {ONE_TIME_MODE, CONTINUOUS_MODE} modes_t;

typedef struct
{
	time start_tick;
	time timeout;
	modes_t timer_mode;
} timers_t;

void delay_us( uint16_t interval );

void Timer_Init(timers_t *timer, modes_t mode);
void Timer_Start(timers_t *timer, time curr_tick, time interval);
void Timer_Stop(timers_t *timer);
bool Timer_Timeout(timers_t *timer, time curr_tick);

#endif /* SYSTEM_TIMERS_H_ */
