/**
 * @file	usart2.h
 * @brief	Prototypes: Universal Synchronous/Asynchronous
 * 						Receiver/Transmitter library
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

#ifndef USART2_H
#define USART2_H

#include "stm32f4xx.h"                  // Device header

void usart2_Init(void);
int usart2_tx_send(int c);
void serialPrint(const char* s);

#endif // USART2_H

