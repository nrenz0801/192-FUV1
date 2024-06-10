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

#ifndef LCD1602_H_
#define LCD1602_H_

#include "stm32f4xx.h"                  // Device header
#include <stdint.h>

// LCD1602 commands and flags
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

#define ADDR     0x27  // I2C address for the LCD
#define COLS     16    // Number of columns
#define ROWS     2     // Number of rows

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

#endif /* LCD1602_H_ */

