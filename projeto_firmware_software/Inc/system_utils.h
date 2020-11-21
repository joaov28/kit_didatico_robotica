/*
 * system_utils.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Joao Vitor Bassani
 */

#ifndef SYSTEM_UTILS_H_
#define SYSTEM_UTILS_H_

#define LED_ROUTINE_SIZE 12

typedef struct
{
	uint8_t led_1[LED_ROUTINE_SIZE];
	uint8_t led_2[LED_ROUTINE_SIZE];
	uint8_t led_3[LED_ROUTINE_SIZE];
} LEDs_sequences;

void System_Init( void );

void Buzzer_Routine( void );

void LED_Routine( void );
void LED_ChangeRoutine(uint8_t routine);

void Sensors_Init( void );
void Sensors_Update( void );

void ADC_ReadData( void );
void Battery_UpdateStatus( void );

void UART_ReadData( void );



void print_debug();

#endif /* SYSTEM_UTILS_H_ */
