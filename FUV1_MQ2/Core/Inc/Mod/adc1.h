/**
 * @file	adc1.h
 * @brief	Prototypes: Analog-to-Digital Converter (ADC) library
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

#ifndef ADC1_H
#define ADC1_H

#include "stm32f4xx.h"                  // Device header

int MQ2_GetVal(void);
void ADC_Init(void);

#endif // ADC1_H
