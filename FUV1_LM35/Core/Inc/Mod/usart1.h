/**
 * @file	usart1.h
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

#ifndef USART1_H
#define USART1_H

#include "stm32f4xx.h"                  // Device header
#include <stdbool.h>

void usart1_Init(void);
int usart1_tx_send(int c);
void sendESP_NoResponse(const char* s);
bool sendESP(const char* s, const char* response);
void WiFi_Init(void);
void sendThingSpeak(int val, int field);

#endif // USART1_H

