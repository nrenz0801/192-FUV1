/**
 * @file	lcd1602.h
 * @brief	Prototypes: Liquid Crystal Display (LCD) library
 *
 * @author	Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * @date	9 June 2024
 * @copyright
 * Copyright (C) 2024. This source code was created as part of the author's
 * official duties with the Electrical and Electronics Engineering Institute,
 * University of the Philippines <https://eee.upd.edu.ph>
 */

#ifndef LCD1602_H
#define LCD1602_H

#include "stm32f4xx.h"                  // Device header
#include <stdbool.h>
#include <stdint.h>

void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_SendStringData(char *str, int row, int col, bool clear);
void LCD_ClearRow(int row);
void LCD_put_cursor(int row, int col);


#endif /* LCD1602_H */
