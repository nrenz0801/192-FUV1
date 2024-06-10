/**
 * @file	dht22.c
 * @brief	Library code: Digital Temperature and Humidity Sensor (DHT22)
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

/*
 * System configuration/build:
 * 	- Clock source == HSI (~16 MHz)
 * 		- No AHB & APB1/2 prescaling
 *	- DHT22 Pins (Bidirectional):
 * 		- data feed in & out @ PB8
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 */

#include "Mod/dht22.h"
#include "Mod/timing.h"

void dht22_PinA8_Init(void){
	// Enable GPIOA clock
	RCC->AHB1ENR |= (1 << 0);
}

void dht22_start(void){
	/*Set PA8 as output*/
	GPIOA->MODER |= (1 << 16);
	GPIOA->MODER &= ~(1 << 17);

	/*Set the pin low*/
	GPIOA->BSRR = (1 << 24);

	/*Wait for 18ms*/

	delaymS(18);

	/*Set the pin to input*/
	GPIOA->MODER &= ~(3 << 16);
}

int Check_Response(void){
	uint8_t response = 0;
	delayuS(40);
	if ((GPIOA->IDR & GPIO_IDR_ID8)!=GPIO_IDR_ID8)
	{
		delayuS(80);
		if ((GPIOA->IDR & GPIO_IDR_ID8)==GPIO_IDR_ID8)
		{
			response = 1;
		}
		else
		{
			response = 0;
		}
	}
	while ((GPIOA->IDR & GPIO_IDR_ID8)==GPIO_IDR_ID8);   // wait for the pin to go low
	return response;
}

uint8_t DHT22_Read (void){
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while ((GPIOA->IDR & GPIO_IDR_ID8)!=GPIO_IDR_ID8);   // wait for the pin to go high
		delayuS (40);   // wait for 40 us
		if ((GPIOA->IDR & GPIO_IDR_ID8)!=GPIO_IDR_ID8)   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((GPIOA->IDR & GPIO_IDR_ID8)==GPIO_IDR_ID8);  // wait for the pin to go low
	}
	return i;
}

void Get_DHT_Data(float * TEMP, float *RH){
    uint8_t Rh_byte1 = DHT22_Read ();
    uint8_t Rh_byte2 = DHT22_Read ();

    uint8_t Temp_byte1 = DHT22_Read ();
    uint8_t Temp_byte2 = DHT22_Read ();

    uint8_t SUM = DHT22_Read();

    if (SUM == ((Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2) & 0x00FF)){
        if (Temp_byte1>127){ // if Temp_byte1 = 10000000, negative temperature
            *TEMP = (float)Temp_byte2/10*(-1);
        }
        else {
            *TEMP = (float)((Temp_byte1<<8)|Temp_byte2)/10;
        }

        *RH = (float)((Rh_byte1<<8)|Rh_byte2)/10;
    }
}
