/**
 * @file	usart1.c
 * @brief	Library code: Universal Synchronous/Asynchronous
 * 						  Receiver/Transmitter
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
 *	- Inputs:
 * 		- USART Input @ PA10 (USART1_RX)
 * 	- Outputs:
 *		- USART Output @ PA9 (USART1_TX)
 *
 * NOTE: 	This project uses the CMSIS standard for ARM-based microcontrollers;
 * 	 		This allows register names to be used without regard to the exact
 * 	 		addresses.
 *
 * TODO: 	Modify values in this file:
 * 			./Mod/usart1.c
 * 				|	SSID
 *				|	PASS
 *				|	WRITE_API
 *				|	READ_API
 */

#include "Mod/usart1.h"
#include "Mod/usart2.h"
#include "Mod/timing.h"
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

/* TODO: Adjust these values as necessary */
#define SSID            "DOMINGO WIFI"
#define PASS            "Nathanie!0801"
#define WRITE_API       "ADNJT35T06EVYT9T"  // Write API Key for ThingSpeak server
#define READ_API        "YMI86E59HUTZAMK4"  // Read API Key for ThingSpeak server


void usart1_Init(void) {
    // Enable the clock for GPIOA and USART2
    RCC->AHB1ENR |= (0x1UL << (0U)); // enable RCC for port A
    RCC->APB2ENR |= (0x1UL << (4U)); // enable RCC for USART1

    // Set pins PA9 (TX) and PA10 (RX) for serial communication
    GPIOA->MODER |= (0x2UL << 18U); // PA9 is Alt fn mode (serial TX in this case)
    GPIOA->MODER |= (0x2UL << 20U); // PA10 is Alt fn mode (serial RX in this case)
    GPIOA->AFR[1] &= ~(0x00000FF0);
    GPIOA->AFR[1] |= (0x00000770); // USART1_TX = AF07 @ PA9, USART1_RX = AF07 @ PA10

    // Configure the baud rate
    USART1->BRR &= ~(0x0000FFFF);
    USART1->BRR |= (0x0000008B);

    // Now enable the USART peripheral
    USART1->CR1 |= (0x1UL << (2U)) // enable receive
        | (0x1UL << (3U)) // enable transmit
        | (0x1UL << (13U)); // enable usart
}


int usart1_tx_send(int c) {
    while (!(USART1->SR & (0x1UL << (7U)))) {}; // wait until we are able to transmit
    USART1->DR = c; // transmit the character
    return c;
}


void sendESP_NoResponse(const char* s) {
    while (*s){
    	usart1_tx_send(*s++);
    }
}


bool sendESP(const char* s, const char* response) {
    while (*s){
    	usart1_tx_send(*s++);
    }

    int max_size = BUFFER_SIZE;
	char buffer[max_size];
	memset(buffer, 0, max_size); // Clear the buffer before receiving new data
	int i = 0;
	char c;

	while (i < (max_size - 1)) {
		while (!(USART1->SR & (1 << 5)));

		c = USART1->DR;
		buffer[i++] = c;
		buffer[i] = '\0';

		// Check for the keyword "OK" in the received data
		if (strstr(buffer, response) != NULL) {
//			serialPrint("RESPONSE RECEIVED!!\r\n"); // echo the command to serial monitor
			break;
		}
	}

	buffer[i] = '\0';
	// transmit the response to the serial monitor
//	serialPrint(buffer);
//	serialPrint("\r\n");
	return true;
}


void WiFi_Init(void) {

	char data[200];

	while (true) {
//		serialPrint("Attempting WiFi Initialization...\r\n");

		if (!sendESP("AT\r\n", "OK")) {
			delaymS(5000);
//			serialPrint("AT command failed. Retrying...\r\n");
			continue;
		}

		IWDG_Refresh();
		delaymS(1000);

		if (!sendESP("AT+CWMODE=1\r\n", "OK")) {
			delaymS(5000);
//			serialPrint("CWMODE command failed. Retrying...\r\n");
			continue;
		}

		IWDG_Refresh();
		delaymS(1000);

		sprintf(data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
		if (!sendESP(data, "OK")) {
			delaymS(5000);
//			serialPrint("CWJAP command failed. Retrying...\r\n");
			continue;
		}

		// If all commands succeeded
		IWDG_Refresh();
//		serialPrint("WiFi Initialization Success!\r\n");
		break;
	}
}


void sendThingSpeak(int val, int field) {
    char data[200];
    char cipsend_cmd[100];

    memset(data, 0, sizeof(data));
    memset(cipsend_cmd, 0, sizeof(cipsend_cmd));

    sprintf(data, "GET /update?api_key=%s&field%d=%d\r\n\r\n", WRITE_API, field, val);
    sprintf(cipsend_cmd, "AT+CIPSEND=0,%d\r\n", strlen(data) + 1);

    IWDG_Refresh();

    while (true) {
//        serialPrint("Attempting Data Transmission to ThingSpeak...\r\n");

        if (!sendESP("AT+CIPMUX=1\r\n", "OK")) {
            delaymS(5000);
//            serialPrint("CIPMUX command failed. Retrying...\r\n");
            continue;
        }

        IWDG_Refresh();
        delaymS(1000);

        if (!sendESP("AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",80\r\n", "OK")) {
            delaymS(5000);
//            serialPrint("CIPSTART command failed. Retrying...\r\n");
            continue;
        }

        IWDG_Refresh();
        delaymS(1000);

        if (!sendESP(cipsend_cmd, ">")) {
        	delaymS(5000);
//            serialPrint("CIPSEND command failed. Retrying...\r\n");
            continue;
        }

        IWDG_Refresh();
        delaymS(1000);

        sendESP_NoResponse(data);

        IWDG_Refresh();
        delaymS(3000);

        if (!sendESP("AT+CIPCLOSE=0\r\n", "OK")) {
            delaymS(5000);
//            serialPrint("CIPCLOSE command failed. Retrying...\r\n");
            continue;
        }

        IWDG_Refresh();
//        serialPrint("Data Transmission Success!\r\n");
        break;
    }
}
