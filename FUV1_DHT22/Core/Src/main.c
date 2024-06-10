/**
 * @file		main.c
 * @brief		EEE 192: IoT-Based Room Fire Detection and Alarm
 * 				DHT22 Sensor System
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
 * 				- USART Input @ PA10 (USART1_RX)
 * 				- USART Input @ PA3 (USART2_RX)
 * 		- Outputs:
 * 				- Buzzer @ PB1
 *				- USART Output @ PA9 (USART1_TX)
 *				- USART Output @ PA2 (USART2_TX)
 *		- I2C Pins (Bidirectional):
 * 				- SCL @ PB8 (I2C1_SCL)
 * 				- SDA @ PB9 (I2C1_SDA)
 * 		- DHT22 Pin (Bidirectional):
 * 				- Data feed in & out @ PA8
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
#include <Mod/dht22.h>

#include <stdio.h>				// For sprintf()

#define SEND_INTERVAL 	15000 	// 15 seconds interval for sending to cloud
#define THRESHOLD 		60		// (Celsius) System will trigger alarm if this value is reached
#define RH_FIELD_NUM 	2		// ThingSpeak Field number for the specific sensor
#define TEMP_FIELD_NUM 	3		// ThingSpeak Field number for the specific sensor

/************************** Function Prototypes *******************************/

void Buzzer_Init(void);

/************************* Main Function **************************************/

int main(void) {
	IWDG_Init();
	TIM2_Init();
	I2C_Init();
	LCD_Init();


	LCD_SendString("Initializing", 0, 2, true);
	LCD_SendString("System", 1, 5, true);

	IWDG_Refresh();

	Buzzer_Init();
	ADC_Init();
	SysTick_Init();
	usart1_Init();
	usart2_Init();

	delaymS(1500);
	IWDG_Refresh();


	LCD_SendString("Connecting", 0, 3, true);
	LCD_SendString("WIFI", 1, 6, true);

	WiFi_Init();

	LCD_ClearRow(1);
	LCD_SendString("Success!", 0, 4, true);
	delaymS(1500);

	char tempbuff[50];
	char humbuff[50];
	millis = 0;
	uint32_t last_send_time = 0;

	float temp, hum;
	bool send_temp = true;
	bool send_rh = false;

	LCD_ClearRow(0);
	IWDG_Refresh();

	/* Loop forever */
	while (1) {
		dht22_start();
		if(Check_Response() == 1) {
			Get_DHT_Data(&temp, &hum);

			// Display the data to LCD
			LCD_ClearRow(0);
			LCD_SendString("R. Temp:", 0, 0, false);
			sprintf(tempbuff, "%.2f", temp);
			LCD_SendString(tempbuff, 0, 9, false);

			LCD_ClearRow(1);
			LCD_SendString("Hum:", 1, 4, false);
			sprintf(humbuff, "%.2f", hum);
			LCD_SendString(humbuff, 1, 9, false);

			if (temp >= 40)
				GPIOB->ODR |= (1<<1); // Buzzer turns ON
			else if (hum <= 30)
				GPIOB->ODR |= (1<<1); // Buzzer turns ON
			else
				GPIOB->ODR &= ~(1<<1); // Buzzer is OFF
		}
		IWDG_Refresh();

		// Loop every 1s
		delaymS(1000);

		// Send data to ThingSpeak server every 15 seconds
		if ((millis - last_send_time) >= SEND_INTERVAL) {
			LCD_Clear();
			if (send_temp) {
				LCD_SendString("Sending Temp.", 0, 0, true);

				sendThingSpeak(temp, TEMP_FIELD_NUM);
				last_send_time = millis;
				send_temp = false;
				send_rh = true;
				LCD_SendString("Success!", 1, 0, true);
				delaymS(1000);
				LCD_Clear();
			} else if (send_rh) {
				LCD_SendString("Sending R.H.", 0, 0, true);

				sendThingSpeak(hum, RH_FIELD_NUM);
				last_send_time = millis;
				send_temp = true;
				send_rh = false;
				LCD_SendString("Success!", 1, 0, true);
				delaymS(1000);
				LCD_Clear();
			}
		}
		IWDG_Refresh();
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
