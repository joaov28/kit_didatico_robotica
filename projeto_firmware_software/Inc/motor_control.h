/*
 * motor_control.h
 *
 *  Created on: Aug 27, 2020
 *      Author: Joao Vitor Bassani
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

typedef enum
{
	FORWARD = 0x01,
	BACKWARD = 0x02,
	LEFT = 0x03,
	RIGHT = 0x04,
	STOP = 0x05,
} direction_t;

void Motor_ChangeDirection( void );
void Line_Follower_Update( void );

#endif /* MOTOR_CONTROL_H_ */
