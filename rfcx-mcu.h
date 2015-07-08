/**********************************************************
*	RFCx Microcontroller Software - Main
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#ifndef RFCX_MCU_H
#define RFCX_MCU_H

#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "rfcx-i2c.h"
#include "utilities/usart.h"

//USART Settings
#define FOSC 16000000 // Clock Speed (Hz)
#define BAUD 9600	  // Baud Rate
#define UBRR (((((FOSC * 10) / (16L * BAUD)) + 5) / 10) - 1)

int init(void);
int peripheral_init(void);
int device_init(void);

#endif//RFCX_MCU_H
