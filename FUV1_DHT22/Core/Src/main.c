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

#define SEND_INTERVAL 	70000 	// 32 seconds interval for sending to cloud
#define THRESHOLD 		60		// (Celsius) System will trigger alarm if this value is reached
#define RH_FIELD_NUM 	2		// ThingSpeak Field number for the specific sensor
#define TEMP_FIELD_NUM 	3		// ThingSpeak Field number for the specific sensor


/************************** Function Prototypes *******************************/

void Buzzer_Init(void);
void TIM3_Init(void);
int TIM3_GetTick(void);

/************************* Main Function **************************************/

int main(void) {
	IWDG_Init();
	TIM2_Init();
	TIM3_Init();
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



	float temp, hum;
	bool send_temp = true;
	bool send_rh = false;

	LCD_ClearRow(0);
	IWDG_Refresh();

	int state = 0;
	int send_interval = 0;

	char sendbuff[100];
	char timebuff[60];
	int start = 0;
	int end = 0;

	char tempbuff[50];
	char humbuff[50];
	millis = 0;
	int last_send_time = 0;
	int time_send_interval = 0;
	/* Loop forever */
	while (1) {
		sprintf(sendbuff, "state: %d\r\nmillis: %d\r\nsend_interval: %d\r\n", state, millis - last_send_time, send_interval);
		serialPrint(sendbuff);
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

		if (state == 0) {
			send_interval = 100000;
		}
		else if (state == 1) {
			send_interval = 50000 - time_send_interval;
		}
		else if (state == 2) {
			send_interval = 50000;
		}

		// Send data to ThingSpeak server every 15 seconds
		if ((millis - last_send_time) >= send_interval) {
			LCD_Clear();
			if (send_temp) {
				LCD_SendString("Sending Temp.", 0, 0, true);

				start = TIM3_GetTick();
				sendThingSpeak(temp, TEMP_FIELD_NUM);
				end = TIM3_GetTick();

				last_send_time = millis;
				send_temp = false;
				send_rh = true;
				LCD_SendString("Success!", 1, 0, true);
				state = 1;
			} else if (send_rh) {
				LCD_SendString("Sending R.H.", 0, 0, true);

				start = TIM3_GetTick();
				sendThingSpeak(hum, RH_FIELD_NUM);
				end = TIM3_GetTick();

				last_send_time = millis;
				send_temp = true;
				send_rh = false;
				LCD_SendString("Success!", 1, 0, true);
				state = 2;
			}

			time_send_interval = end - start;
			sprintf(timebuff, "%d \r\n", time_send_interval);
			serialPrint(timebuff);
			TIM3->CNT = 0;

		}
		IWDG_Refresh();
		delaymS(1000);
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

    // Configure TIM3: count up, no prescaler, update on overflow
    TIM3->PSC = 16000 - 1;  // Prescaler to get 1 ms tick (assuming 16 MHz clock)
    TIM3->ARR = 0xFFFFFFFF; // Max auto-reload value
    TIM3->CNT = 0;          // Clear counter
    TIM3->CR1 |= TIM_CR1_CEN; // Enable the timer
}

int TIM3_GetTick(void) {
    return TIM3->CNT;
}


