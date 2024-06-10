/**
 * @file	adc1.c
 * @brief	Library code: Analog-to-Digital Converter (ADC)
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
 *	- Inputs:
 * 		- ADC Analog @ PA1 (Channel 1)
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 */

#include "Mod/adc1.h"
#include <Mod/timing.h>
#include <math.h>				// For pow()

void ADC_Init(void) {
	// Enable clocks
	RCC->APB2ENR |= (1 << 8);			// Enable ADC1 clock
	RCC->AHB1ENR |= (1 << 0);			// Enable GPIOA clock

	// Configure GPIOA pin PA1 for ADC
	GPIOA->MODER &= ~(3 << 2);			// Clear mode bits
	GPIOA->MODER |= (3 << 2);			// Analog Mode

	// ADC Configuration
	ADC->CCR &= ~(3 << 16);				// Prescaler = 2
	ADC1->CR1 &= ~(3 << 24);			// 12-bit resolution conversion
	ADC1->CR1 |= (1 << 8);				// Enable scan mode
	ADC1->CR1 |= (1 << 5);				// Generate interrupt when the EOC bit is set

	// 15 cycles sampling time for Channel 1
	ADC1->SMPR2 &= ~(1 << 5);
	ADC1->SMPR2 &= ~(1 << 4);
	ADC1->SMPR2 |= (1 << 3);

	ADC1->CR2 &= ~(1 << 10);			// EOC bit is set at the end of each sequence of regular conversions
	ADC1->CR2 &= ~(1 << 11);			// Right allignment for data

	// 1 conversion in the regular channel conversion sequence
	ADC1->SQR1 &= ~(1 << 23);
	ADC1->SQR1 &= ~(1 << 22);
	ADC1->SQR1 &= ~(1 << 21);
	ADC1->SQR1 &= ~(1 << 20);

	ADC1->SQR3 |= (1 << 0);				// Assign channel 1 as the 1st in the sequence to be converted
	ADC1->CR2 |= (1 << 1);				// Continuous conversion mode
}


int MQ2_GetVal(void) {
	ADC1->CR2 |= (1 << 0); 					// Enable the ADC
	delaymS(1); 							// Required to ensure adc stable
	ADC1->CR2 |= (1 << 30); 				// Start ADC conversion
	while (!((ADC1->SR) & (1 << 1))); 		// Wait for the end of conversion
	int adc_val = ADC1->DR;			// Store ADC value

	return adc_val; 	// Read the value contained at the data register
}
