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

#define SLAVE_ADDRESS_LCD 	0x27

void LCD_SendCommand(uint8_t cmd) {
    uint8_t upper_nibble = (cmd & 0xF0);
    uint8_t lower_nibble = ((cmd << 4) & 0xF0);
    uint8_t data_u = upper_nibble | 0x0C;  			// En = 1, Rs = 0
    uint8_t data_l = lower_nibble | 0x0C;  			// En = 1, Rs = 0

    I2C_Write(SLAVE_ADDRESS_LCD, data_u);
    data_u &= ~0x04; 								// En = 0
    I2C_Write(SLAVE_ADDRESS_LCD, data_u);

    I2C_Write(SLAVE_ADDRESS_LCD, data_l);
    data_l &= ~0x04; 								// En = 0
    I2C_Write(SLAVE_ADDRESS_LCD, data_l);
}

void LCD_SendData(uint8_t data) {
    uint8_t upper_nibble = (data & 0xF0);
    uint8_t lower_nibble = ((data << 4) & 0xF0);
    uint8_t data_u = upper_nibble | 0x0D;  			// En = 1, Rs = 1
    uint8_t data_l = lower_nibble | 0x0D;  			// En = 1, Rs = 1

    I2C_Write(SLAVE_ADDRESS_LCD, data_u);
    delayuS(1);
    data_u &= ~0x04; 								// En = 0
    I2C_Write(SLAVE_ADDRESS_LCD, data_u);

    I2C_Write(SLAVE_ADDRESS_LCD, data_l);
    delayuS(1);
    data_l &= ~0x04; 								// En = 0
    I2C_Write(SLAVE_ADDRESS_LCD, data_l);
}


void LCD_Init(void) {
    delaymS(250);                             // Wait for more than 15 ms after VCC rises to 4.5V
    LCD_SendCommand(0x30);                    // Wake up
    delaymS(5);                               // Wait for more than 4.1 ms
    LCD_SendCommand(0x30);                    // Wake up #2
    delayuS(110);                             // Wait for more than 100 us
    LCD_SendCommand(0x30);                    // Wake up #3
    delayuS(110);                             // Wait for more than 100 us
    LCD_SendCommand(0x20);                    // Function Set: 4-bit mode
    delayuS(110);                             // Wait for more than 100 us

    // Continue with LCD initialization commands
    LCD_SendCommand(0x28);                    // Function Set: 4-bit, 2 lines, 5x7 style
    delayuS(110);                             // Wait for more than 100 us
    LCD_SendCommand(0x08);                    // Display Switch: Display off
    delayuS(110);                             // Wait for more than 100 us
    LCD_SendCommand(0x01);                    // Screen Clear
    delaymS(5);                               // Wait for more than 4.1 ms
    LCD_SendCommand(0x02);                    // Cursor Return
    delaymS(5);                               // Wait for more than 4.1 ms
    LCD_SendCommand(0x06);                    // Input set: Increment Mode, No cursor shift
    delayuS(110);                             // Wait for more than 100 us
    LCD_SendCommand(0x0C);                    // Display Switch: Display on, cursor off, blink off
    delayuS(110);                             // Wait for more than 100 us
}

void LCD_ClearRow(int row) {
    LCD_put_cursor(row, 0);
    for (int i = 0; i < 16; i++) {
        LCD_SendData(' ');                    // Clear 16 characters on the row
    }
    LCD_put_cursor(row, 0);                   // Return cursor to the beginning of the row
}


void LCD_put_cursor(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    LCD_SendCommand(col);
}

void LCD_SendStringData(char *str, int row, int col, bool clear)
{
    // Clear the specified row
    if (clear) {
    	LCD_ClearRow(row);
    }

    // Delay to ensure the row is cleared properly
    delaymS(10);

    // Put the cursor at the specified row and column
    LCD_put_cursor(row, col);

    // Send the string data to the LCD
    while (*str) {
        LCD_SendData(*str++);
    }
}
