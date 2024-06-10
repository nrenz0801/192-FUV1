/**
 * @file    lcd1602.h
 * @brief   Header file: Liquid Crystal Display (LCD)
 *
 * Author:  Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 * Date:    9 June 2024
 *
 * This header file contains the function declarations and macro definitions
 * for the LCD1602 library.
 */

#ifndef LCD1602_H
#define LCD1602_H

#include "stm32f4xx.h"                  // Device header
#include <stdint.h>
#include <stdbool.h>

extern uint8_t displayfunction;
extern uint8_t displaycontrol;
extern uint8_t displaymode;
extern uint8_t backlightval;
extern uint8_t numlines;

void LCD_Init(void);
void LCD_Begin(uint8_t cols, uint8_t lines, uint8_t dotsize);
void LCD_SendCommand(uint8_t command);
void LCD_Send(uint8_t value, uint8_t mode);
void LCD_Write4Bits(uint8_t value);
void LCD_ExpanderWrite(uint8_t data);
void LCD_PulseEnable(uint8_t data);
void LCD_Display(void);
void LCD_Home(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Clear(void);
void LCD_SendString(const char *str, uint8_t row, uint8_t col, bool clear);
void LCD_ClearRow(uint8_t row);

#endif // LCD1602_H
