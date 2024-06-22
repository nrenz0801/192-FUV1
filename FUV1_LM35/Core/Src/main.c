/**
 * @file		main.c
 * @brief		EEE 192: IoT-Based Room Fire Detection and Alarm
 * 				LM35 Sensor System
 *
 * @details		This project implements an IoT-based fire detection
 *          	and alarm system using an LM35 temperature sensor.
 *
 * @section Configuration
 * System configuration/build:
 * 		- Clock source:
 * 				- HSI (~16 MHz)
 * 				- No AHB & APB1/2 prescaling
 *		- Inputs:
 * 				- ADC Analog @ PA1 (Channel 1)
 * 				- USART Input @ PA10 (USART1_RX)
 * 				- USART Input @ PA3 (USART2_RX)
 * 		- Outputs:
 * 				- Buzzer @ PB1
 *				- USART Output @ PA9 (USART1_TX)
 *				- USART Output @ PA2 (USART2_TX)
 *		- I2C Pins (Bidirectional):
 * 				- SCL @ PB8 (I2C1_SCL)
 * 				- SDA @ PB9 (I2C1_SDA)
 *
 * @note		This project uses the CMSIS standard for ARM-based
 * 	 			microcontrollers, which allows register names to be
 * 	 			used without regard to the exact addresses.
 *
 * @todo		TODO: Modify values in these files:
 * 				- ./Mod/usart1.c
 * 					- SSID
 *					- PASS
 *					- WRITE_API
 *					- READ_API
 *				- ./main.c
 *					- THRESHOLD
 *					- FIELD_NUM
 *
 * @date		9 June 2024
 *
 * @note		Copyright (C) 2024. This source code was created as
 *          	part of the author's official duties with the Electrical
 *          	and Electronics Engineering Institute, University of the
 *          	Philippines <https://eee.upd.edu.ph>.
 *
 * @author		Nathaniel Renz C. Domingo <ncdomingo1@up.edu.ph>
 */

#include "stm32f4xx.h" 			// Header for the specific device family
#include <stm32f411xe.h>

#include <Mod/timing.h>
#include <Mod/usart1.h>
#include <Mod/usart2.h>
#include <Mod/i2c1.h>
#include <Mod/lcd1602.h>
#include <Mod/adc1.h>

#include <stdio.h>				// For sprintf()

#define SEND_INTERVAL 	100000 	// 100 seconds interval for sending to cloud
#define THRESHOLD 		50		// (Celsius) System will trigger alarm if this value is reached
#define FIELD_NUM 		4		// ThingSpeak Field number for the specific sensor
#define WIFI_DELAY		2000	// (ms)
#define INITIAL_DELAY	75		// (s)

/************************** Function Prototypes *******************************/

void Buzzer_Init(void);
void TIM3_Init(void);
void TIM3_IRQHandler(void);
volatile uint32_t seconds_count = 0;

/************************* Main Function **************************************/

int main(void) {
	IWDG_Refresh();

	IWDG_Init();
	TIM2_Init();
	TIM3_Init();
	I2C_Init();
	LCD_Init();

	LCD_SendString("Initializing", 0, 2, true);
	LCD_SendString("System", 1, 5, true);
	delaymS(1000);

	Buzzer_Init();
	ADC_Init();
	SysTick_Init();
	usart1_Init();
	usart2_Init();

	delaymS(WIFI_DELAY);
	LCD_SendString("Connecting", 0, 3, true);
	LCD_SendString("WIFI", 1, 6, true);
	WiFi_Init();
	IWDG_Refresh();

	LCD_ClearRow(1);
	LCD_SendString("Success!", 0, 4, true);
	delaymS(1500);

	LCD_ClearRow(0);

	char tempbuff[50];

	int delayed = 0;
	int interval_start = 1;

//	char timebuff[100];
	seconds_count = 0;

	/* Loop forever */
	while (1) {
		IWDG_Refresh();
//		sprintf(timebuff, "%u\r\n", seconds_count);
//		serialPrint(timebuff);

		/*********************** Sense and Display to LCD ***********************/
		// Sense data
		float temperature = LM35_GetVal();

		// Monitor the temperature
		if (temperature >= THRESHOLD) {
			// Temperature sensed exceeds threshold -> trigger alarm
			GPIOB->ODR |= (1 << 1);				// Alarm ON
		} else {
			GPIOB->ODR &= ~(1 << 1);			// Alarm OFF
		}

		// Display the data to LCD
		LCD_SendString("E. Temp:", 0, 0, false);
		sprintf(tempbuff, "%.2f", temperature);
		LCD_SendString(tempbuff, 1, 0, false);
		IWDG_Refresh();
		/***********************************************************************/

		if ((seconds_count >= (INITIAL_DELAY - WIFI_DELAY/1000)) || delayed) {
			if (interval_start) {
				serialPrint("initial delay done\r\n");
				interval_start = 0;
				delayed = 1;
				seconds_count = 0;
			}

			if (seconds_count >= 100) {
				// transmit to Thingspeak
				LCD_ClearRow(1);
				LCD_SendString("Sending data", 0, 0, true);

				sendThingSpeak(temperature, FIELD_NUM);

				LCD_SendString("Success!", 1, 0, true);
				seconds_count = 0;
			}
		}

		// Loop every 1s
		delaymS(1000);
		LCD_Clear();
	}
}

/************************** Buzzer Initialization *****************************/

void Buzzer_Init(void) {
	// Enable GPIOB clock
	RCC->AHB1ENR |= (1 << 1);

	// Configure GPIOB pin PB1
	GPIOB->MODER &= ~(3 << 2);  		// Clear mode bits
	GPIOB->MODER |= (1 << 2);			// Output mode
	GPIOB->OTYPER &= ~(1 << 1);			// Push-pull output
}

void TIM3_Init(void) {
    // Enable TIM3 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Configure TIM3: count up, prescaler for 1 ms tick, update on overflow
    TIM3->PSC = 16000 - 1;  // Prescaler to divide the clock by 16000 (1 kHz timer clock)
    TIM3->ARR = 1000 - 1;   // Auto-reload value to generate 1-second tick
    TIM3->CNT = 0;          // Clear counter

    // Enable the update interrupt
    TIM3->DIER |= TIM_DIER_UIE;

    // Enable the TIM3 interrupt in the NVIC
    NVIC_EnableIRQ(TIM3_IRQn);

    // Enable the timer
    TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {  // Check for update interrupt flag
        TIM3->SR &= ~TIM_SR_UIF;  // Clear update interrupt flag
        seconds_count++;
    }
}
