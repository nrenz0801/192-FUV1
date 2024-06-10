/**
 * @file	usart2.c
 * @brief	Library code: Universal Synchronous/Asynchronous
 * 						  Receiver/Transmitter
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
 * 		- USART Input @ PA3 (USART2_RX)
 * 	- Outputs:
 *		- USART Output @ PA2 (USART2_TX)
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 */


#include "Mod/usart2.h"


void usart2_Init(void) {
    // Enable the clock for GPIOA and USART2
    RCC->AHB1ENR |= (0x1UL << (0U)); // enable RCC for port A
    RCC->APB1ENR |= (0x1UL << (17U)); // enable RCC for USART2

    // Set pins PA2 (TX) and PA3 (RX) for serial communication
    GPIOA->MODER |= (0x2UL << 4U); // PA2 is Alt fn mode (serial TX in this case)
    GPIOA->AFR[0] |= (7 << 8U); // That alt fn is alt 7 for PA2
    GPIOA->MODER |= (0x2UL << (6U)); // PA3 is Alt fn mode (serial RX in this case)
    GPIOA->AFR[0] |= (7 << 12U); // Alt fn for PA3 is same as for PA2

    // Configure the baud rate
    USART2->BRR &= ~(0x0000FFFF);
    USART2->BRR |= (0x0000008B);

    // Now enable the USART peripheral
    USART2->CR1 |= (0x1UL << (2U)) // enable receive
        | (0x1UL << (3U)) // enable transmit
        | (0x1UL << (13U)); // enable usart
}


int usart2_tx_send(int c) {
    while (!(USART2->SR & (0x1UL << (7U)))) {}; // wait until we are able to transmit
    USART2->DR = c; // transmit the character
    return c;
}


void serialPrint(const char* s) {
    while (*s) {
    	usart2_tx_send(*s++);
    }
}
