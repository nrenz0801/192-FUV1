/**
 * @file	dht22.h
 * @brief	Prototypes: Digital Temperature and Humidity Sensor (DHT22) library
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

#ifndef DHT22_H
#define DHT22_H

#include "stm32f4xx.h"                  // Device header
#include <stdint.h>

void dht22_PinA8_Init(void);
void dht22_start(void);
int Check_Response(void);
uint8_t DHT22_Read (void);
void Get_DHT_Data(float * TEMP, float *RH);

#endif /* DHT22_H */
