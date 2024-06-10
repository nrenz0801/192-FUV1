/**
 * @file	timing.c
 * @brief	Library code: Independent Watchdog (IWDG),
 * 						  System tick timer (SysTick),
 * 						  Timer 2 (TIM2)
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
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 */

#include "Mod/timing.h"


volatile uint32_t millis = 0;

void IWDG_Init(void) {
    // Enable the LSI clock
    RCC->CSR |= (1 << 0);

    while ((RCC->CSR & (1 << 1)) == 0);			// Wait for LSI to be ready
    IWDG->KR = 0x5555;							// Enable write access to IWDG_PR and IWDG_RLR registers
    IWDG->PR = 0x5;								// Prescaler of 1/128; 16.384s timeout period
    IWDG->RLR = 0xFFF; 							// Set the reload value to 4095
    IWDG->KR = 0xAAAA;							// Reload the IWDG counter
    IWDG->KR = 0xCCCC;							// Start the IWDG
}

void IWDG_Refresh(void) {
	// Call this function to avoid timeout
    IWDG->KR = 0xAAAA;							// Reload the IWDG counter
}

void SysTick_Init(void) {
    SysTick_Config(16000000 / 1000);        	// Configure SysTick to trigger every 1 millisecond
}

void SysTick_Handler(void) {
    millis++;
}

void TIM2_Init(void) {
    // Enable TIM2 clock
    RCC->APB1ENR |= (1 << 0);

    // Configure TIM2 for 1 microsecond tick
    TIM2->PSC = (16000000 / 1000000) - 1;  				// Prescaler value
    TIM2->ARR = 0xFFFF;  								// Set auto-reload register to maximum value
    TIM2->EGR |= (1 << 0); 								// Reset timer counter registers
    TIM2->CNT = 0;										// Clear the counter
    TIM2->CR1 |= (1 << 0);  							// Enable TIM2
}

void delayuS(uint32_t us) {
	TIM2->EGR |= (1 << 0); 								// Reset timer counter registers
    TIM2->CNT = 0;										// Clear the counter
    TIM2->CR1 |= (1 << 0);								// Enable timer

    // Wait for the specified number of microseconds
    while (TIM2->CNT < us);
}

void delaymS(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        delayuS(1000);
    }
}
