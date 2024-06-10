/**
 * @file	i2c1.h
 * @brief	Prototypes: Inter-Integrated Circuit (I2C) library
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

#ifndef I2C1_H
#define I2C1_H

#include "stm32f4xx.h"                  // Device header
#include <stdint.h>

void I2C_Init(void);
void I2C_Write(uint8_t addr, uint8_t data);

#endif // I2C1_H

