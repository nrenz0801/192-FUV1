/**
 * @file	lcd1602.c
 * @brief	Library code: Liquid Crystal Display (LCD)
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
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 */

#include "Mod/lcd1602.h"
#include "Mod/i2c1.h"
#include "Mod/timing.h"


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


uint8_t displayfunction;
uint8_t displaycontrol;
uint8_t displaymode;
uint8_t backlightval = LCD_BACKLIGHT;
uint8_t numlines;

void LCD_Init(void) {
    displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    LCD_Begin(COLS, ROWS, 0);
}

void LCD_Begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    if (lines > 1) {
        displayfunction |= LCD_2LINE;
    }
    numlines = lines;

    if ((dotsize != 0) && (lines == 1)) {
        displayfunction |= LCD_5x10DOTS;
    }

    delaymS(50);
    LCD_ExpanderWrite(backlightval);
    delaymS(1000);

    LCD_Write4Bits(0x03 << 4);
    delayuS(4500);

    LCD_Write4Bits(0x03 << 4);
    delayuS(4500);

    LCD_Write4Bits(0x03 << 4);
    delayuS(150);

    LCD_Write4Bits(0x02 << 4);

    LCD_SendCommand(LCD_FUNCTIONSET | displayfunction);

    displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LCD_Display();

    LCD_Clear();

    displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    LCD_SendCommand(LCD_ENTRYMODESET | displaymode);

    LCD_Home();
}

void LCD_SendCommand(uint8_t command) {
    LCD_Send(command, 0);
}

void LCD_Send(uint8_t value, uint8_t mode) {
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    LCD_Write4Bits((highnib) | mode);
    LCD_Write4Bits((lownib) | mode);
}

void LCD_Write4Bits(uint8_t value) {
    LCD_ExpanderWrite(value);
    LCD_PulseEnable(value);
}

void LCD_ExpanderWrite(uint8_t data) {
    I2C_Write(ADDR, (int)(data) | backlightval);
}

void LCD_PulseEnable(uint8_t data) {
    LCD_ExpanderWrite(data | En);
    delayuS(1);
    LCD_ExpanderWrite(data & ~En);
    delayuS(50);
}

void LCD_Display(void) {
    displaycontrol |= LCD_DISPLAYON;
    LCD_SendCommand(LCD_DISPLAYCONTROL | displaycontrol);
}

void LCD_Home(void) {
    LCD_SendCommand(LCD_RETURNHOME);
    delayuS(2000);
}

void LCD_SetCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > numlines) {
        row = numlines - 1;
    }
    LCD_SendCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_Clear(void) {
    LCD_SendCommand(LCD_CLEARDISPLAY);
    delayuS(2000);
}
