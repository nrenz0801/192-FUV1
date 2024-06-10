/**
 * @file	i2c1.c
 * @brief	Library code: Inter-Integrated Circuit (I2C)
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
 *	- I2C Pins (Bidirectional):
 * 		- SCL @ PB8 (I2C1_SCL)
 * 		- SDA @ PB9 (I2C1_SDA)
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 */


#include "Mod/i2c1.h"


void I2C_Init(void) {
    // Enable clocks
    RCC->APB1ENR |= (1 << 21);                 // Enable I2C1 clock
    RCC->AHB1ENR |= (1 << 1);                  // Enable GPIOB clock

    // Configure GPIOB pins PB8 and PB9 for I2C1 SCL and SDA
    GPIOB->MODER &= ~((3 << 16) | (3 << 18));  // Clear mode bits for PB8 and PB9
    GPIOB->MODER |= (2 << 16) | (2 << 18);     // Alternate Function for PB8 and PB9
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);      // Open drain for PB8 and PB9
    GPIOB->OSPEEDR |= (3 << 16) | (3 << 18);   // High speed for PB8 and PB9
    GPIOB->PUPDR &= ~((3 << 16) | (3 << 18));  // Clear pull-up/pull-down bits for PB8 and PB9
    GPIOB->PUPDR |= (1 << 16) | (1 << 18);     // Pull-up for PB8 and PB9
    GPIOB->AFR[1] |= (4 << (4 * (8 - 8)));     // AF4 for PB8
    GPIOB->AFR[1] |= (4 << (4 * (9 - 8)));     // AF4 for PB9

	/*
     *  Program the peripheral input clock in I2C_CR2 Register in
     *  order to generate correct timings
     *
     *  From the reference manual,
     *      T_high = CCR * T_PCLK1
     *      t_high = t_r(SCL) + t_w(SCLH)
     *
     *  From the datasheet,
     *      t_w(SCLH) = 4 us           SCL clock high time
     *      t_r(SCL) = 1000 ns          SDA and SCL rise time
     *      T_PCLK1 = 1/16 MHz
     *
     *  Therefore,
     *      CCR = (t_r(SCL) + t_w(SCLH)) / T_PCLK1
     *      CCR = 80
     *
     *      TRISE = (t_r(SCL) / T_PCLK1) + 1
     *      TRISE = 17
     */

    I2C1->CR1 |= (1 << 15);                	   // Software reset I2C1
    I2C1->CR1 &= ~(1 << 15);               	   // Clear reset

    I2C1->CR1 &= ~(1 << 0);                    // Disable I2C1
    I2C1->CR2 = 16;                            // Set PCLK1 frequency (16 MHz)
    I2C1->CCR = 80;                            // Set CCR value (Standard mode, 100 kHz)
    I2C1->TRISE = 17;                          // Set TRISE value
    I2C1->CR1 |= (1 << 0);                     // Enable I2C1
}

void I2C_Write(uint8_t addr, uint8_t data) {
    while (I2C1->SR2 & (1 << 1)){;}  			// Wait until the I2C bus is not busy
    I2C1->CR1 |= (1 << 8);         				// Generate a START condition
    while (!(I2C1->SR1 & (1 << 0))){;} 			// Wait for START condition
    I2C1->DR = addr << 1;          				// Send the slave address with write bit
    while (!(I2C1->SR1 & (1 << 1))){;} 			// Wait for address to be sent
    (void)I2C1->SR2;               				// Clear ADDR flag
    while (!(I2C1->SR1 & (1 << 7))){;} 			// Wait for data register to be empty
    I2C1->DR = data;               				// Send the data byte
    while (!(I2C1->SR1 & (1 << 2))){;} 			// Wait for data transfer to finish
    I2C1->CR1 |= (1 << 9);         				// Generate a STOP condition
}
