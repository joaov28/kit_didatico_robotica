/*
 * system_utils.c
 *
 *  Created on: Aug 30, 2020
 *      Author: Joao Vitor Bassani
 */

#include <string.h>
#include "stm32f1xx_hal.h"
#include "hcsr04.h"
#include "bmp180.h"
#include "mpu6050.h"
#include "motor_control.h"

#include "system_utils.h"

/************* External structures ******************************/
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;

extern MPU6050_t MPU6050;
extern bmp_t bmp;

extern direction_t direction;

/************* Global variables **********************************/
uint8_t line_follower_mode_on = 0;

uint32_t distance = 0;
uint32_t batt_1 = 0;
uint32_t batt_2 = 0;

char rxBuffer[32] = {0};
char received_byte = 0;

LEDs_sequences sequence = {0};

enum {

	// App commands
	DIRECTION = 0x01,
	LEDS = 0x02,
	BUZZER = 0x03,
	SENSORS_UPDATE = 0x04,

	// System events
	TEMPERATURE_UPDATE = 0x10,
	PRESSURE_UPDATE = 0x11,
	ALTITUDE_UPDATE = 0x12,
	DISTANCE_UPDATE = 0x13,
	BATTERY_STATUS = 0x14,
};

/************* System configurations ****************************/
void System_Init( void )
{
	LED_ChangeRoutine(0x00);
	Sensors_Init();
	Motor_ChangeDirection();
}

/************* Buzzer routine ***********************************/
void Buzzer_Routine( void )
{
	static uint8_t duty_cycle = 0;

	if(duty_cycle < 254)
	{
		htim2.Instance->CCR2 = duty_cycle;
		duty_cycle += 30;
	}
	else
		duty_cycle = 0;
}

/************* LEDS control routines ****************************/

void LED_Routine( void )
{
	static uint8_t i = 0;

	HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, 0x01);
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, 0x01);
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, 0x01);

	if(i >= LED_ROUTINE_SIZE){ i = 0; }

	HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, sequence.led_1[i]);
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, sequence.led_2[i]);
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, sequence.led_3[i]);

	i++;

}

void LED_ChangeRoutine(uint8_t routine)
{
	switch(routine)
	{
		case 1:
		{
			const uint8_t led_1[] = {1,1,0,1,1,0,1,1,0,1,1,0};
			memcpy(sequence.led_1, led_1, LED_ROUTINE_SIZE);
			const uint8_t led_2[] = {1,0,1,1,0,1,1,0,1,1,0,1};
			memcpy(sequence.led_2, led_2, LED_ROUTINE_SIZE);
			const uint8_t led_3[] = {0,1,1,0,1,1,0,1,1,0,1,1};
			memcpy(sequence.led_3, led_3, LED_ROUTINE_SIZE);
			break;
		}

		case 2:
		{
			const uint8_t led_1[] = {1,0,1,0,1,0,1,0,1,0,1,0};
			memcpy(sequence.led_1, led_1, LED_ROUTINE_SIZE);
			const uint8_t led_2[] = {1,0,1,0,1,0,1,0,1,0,1,0};
			memcpy(sequence.led_2, led_2, LED_ROUTINE_SIZE);
			const uint8_t led_3[] = {1,0,1,0,1,0,1,0,1,0,1,0};
			memcpy(sequence.led_3, led_3, LED_ROUTINE_SIZE);
			break;
		}

		default:
		{
			const uint8_t led_1[] = {1,0,0,1,0,0,1,0,0,1,0,0};
			memcpy(sequence.led_1, led_1, LED_ROUTINE_SIZE);
			const uint8_t led_2[] = {0,1,0,0,1,0,0,1,0,0,1,0};
			memcpy(sequence.led_2, led_2, LED_ROUTINE_SIZE);
			const uint8_t led_3[] = {0,0,1,0,0,1,0,0,1,0,0,1};
			memcpy(sequence.led_3, led_3, LED_ROUTINE_SIZE);
			break;
		}
	}
}

/************* Sensors configuration and update routines *********/

void Sensors_Init( void )
{
	BMP180_Init(&bmp);
	MPU6050_Init(&hi2c1);
}

void Sensors_Update( void )
{
	static uint32_t distance = 0;
	char message[16] = {0};

	BMP180_ReadAll(&bmp);

	snprintf(message, 16, "@%.2x%d!", TEMPERATURE_UPDATE, (int)bmp.data.temp);
	HAL_UART_Transmit(&huart2, (uint8_t*)message, sizeof(message), 50);
	memset(message, 0x00, sizeof(message));

	snprintf(message, 16, "@%.2x%d!", PRESSURE_UPDATE, (int)bmp.data.press);
	HAL_UART_Transmit(&huart2, (uint8_t*)message, sizeof(message), 50);
	memset(message, 0x00, sizeof(message));

	snprintf(message, 16, "@%.2x%d!", ALTITUDE_UPDATE, (int)bmp.data.altitude);
	HAL_UART_Transmit(&huart2, (uint8_t*)message, sizeof(message), 50);
	memset(message, 0x00, sizeof(message));

	distance = HCSR04_Read();

	snprintf(message, 16, "@%.2x%d!", DISTANCE_UPDATE, (int)distance);
	HAL_UART_Transmit(&huart2, (uint8_t*)message, sizeof(message), 50);
	memset(message, 0x00, sizeof(message));

	MPU6050_ReadAll(&hi2c1, &MPU6050);

}

/************* ADC data acquisition routine *********************/

void ADC_ReadData( void )
{
	uint32_t adc1_value = 0;
	uint32_t adc2_value = 0;

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 50);
	adc1_value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 50);
	adc2_value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	adc1_value = (adc1_value*3300)/4095;
	adc2_value = (adc2_value*3300)/4095;

	batt_2 = (adc2_value*15)/10;
	batt_1 = (adc1_value*3) - batt_2;
}

void Battery_UpdateStatus( void )
{
	char message[16] = {0};
	uint8_t battery_status = 0;

	ADC_ReadData();

	if(batt_1 > 3600 && batt_2 > 3600)
	{
		battery_status = 0x03;
		HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET);
	}
	else if(batt_1 > 3400 && batt_2 > 3400)
	{
		battery_status = 0x02;
		HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET);
	}
	else if(batt_1 > 3200 && batt_2 > 3200)
	{
		battery_status = 0x01;
		HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_SET);
	}
	else
	{
		battery_status = 0x00;
		HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_SET);
	}

	snprintf(message, 16, "@%.2x%d!", BATTERY_STATUS, battery_status);
	HAL_UART_Transmit(&huart2, (uint8_t*)message, sizeof(message), 50);
}

/************* UART data manipulation ***************************/

static uint8_t get_number( uint8_t ascii )
{
	switch(ascii)
	{
	case '0' ... '9':
		return ascii - '0';
		break;
	case 'A' ... 'F':
		return ascii - 'A' + 0x0A;
		break;
	}

	return 0;
}

void UART_CommandsHandle( void )
{
	switch(rxBuffer[0])
	{
	case DIRECTION:
		line_follower_mode_on = rxBuffer[1];
		if(!line_follower_mode_on)
			direction = rxBuffer[2];
		else
			Motor_ChangeDirection();
		break;
	case LEDS:
		LED_ChangeRoutine(rxBuffer[1]);
		break;
	case SENSORS_UPDATE:
		Sensors_Update();
		break;
	}
}

void UART_RXHandle( uint8_t received_byte )
{
	static uint8_t i = 0, j = 0, start_rec = 0x00;

	if(received_byte == '@')
	{
		start_rec = 0x01;
		i = 0; j = 0;
		memset(rxBuffer, 0x00, sizeof(rxBuffer));
		return;
	}

	if(received_byte == '!')
	{
		start_rec = 0x00;
		UART_CommandsHandle();
	}

	if(start_rec)
	{
		uint8_t is_even = (j % 2)? 0 : 1;
		if(is_even)
			rxBuffer[i] |= ((get_number(received_byte) & 0x0F) << 4);
		else
		{
			rxBuffer[i] |= (get_number(received_byte) & 0x0F);
			i++;
		}
		j++;
	}
}

void UART_ReadData( void )
{
	if(HAL_UART_Receive(&huart2, (uint8_t *)&received_byte, 1, 1) == HAL_OK)
	{
		UART_RXHandle( received_byte );
	}

}

void print_debug()
{
	char msg[64] = {0};
	snprintf(msg, 64, "\nString: %.02X%.02X\n", rxBuffer[0], rxBuffer[1]);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg), 100);
}

