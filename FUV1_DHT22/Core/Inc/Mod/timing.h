/**
 * @file	timing.h
 * @brief	Prototypes: Independent Watchdog (IWDG),
 * 						System tick timer (SysTick),
 * 						Timer 2 (TIM2) libary
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

#ifndef TIMING_H
#define TIMING_H

#include "stm32f4xx.h"                  // Device header
#include <stdint.h>


extern volatile int millis;

void IWDG_Init(void);
void IWDG_Refresh(void);
void SysTick_Init(void);
void TIM2_Init(void);
void delayuS(uint32_t us);
void delaymS(uint32_t ms);

#endif // TIMING_H

