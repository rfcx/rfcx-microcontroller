/**********************************************************
*	RFCx Microcontroller Software - Battery
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson	(gibsjose@mail.gvsu.edu)
*	Jesse Millwood  (millwooj@mail.gvsu.edu)
*
*	08 July 2015
*	revised 02 November 2015
*
*	This header file is meant to work with the
*	microchip MCP73871 battery charge management
*	controller 
*
*   www.rfcx.org
**********************************************************/

#ifndef RFCX_BATTERY_H
#define RFCX_BATTERY_H

#include <avr/io.h> //Pin definitions
#include <stdbool.h>
#include <stdio.h>

//Battery Identifiers
#define BATTERY_1           0x01
#define BATTERY_2           0x02

//Battery Status Pins
#define BAT_1_STAT_1_INPUT_PIN     PC1
#define BAT_1_STAT_1_OUTPUT_PIN    PC2
#define BAT_1_STAT_2_INPUT_PIN     PD2
#define BAT_1_STAT_2_OUTPUT_PIN    PD3
#define BAT_2_STAT_1_INPUT_PIN     PC0
#define BAT_2_STAT_1_OUTPUT_PIN    PC1
#define BAT_2_STAT_2_INPUT_PIN     PB0
#define BAT_2_STAT_2_OUTPUT_PIN    PB1
#define BAT_1_STAT_1_PORT          PORTC
#define BAT_1_STAT_2_PORT          PORTD
#define BAT_2_STAT_1_PORT          PORTC
#define BAT_2_STAT_2_PORT          PORTB
#define BAT_1_STAT_1_PIN           PINC
#define BAT_1_STAT_2_PIN           PIND
#define BAT_2_STAT_1_PIN           PINC
#define BAT_2_STAT_2_PIN           PINB
#define BAT_1_STAT_1_DDR           DDRC
#define BAT_1_STAT_2_DDR           DDRD
#define BAT_2_STAT_1_DDR           DDRC
#define BAT_2_STAT_2_DDR           DDRB
//Battery Charging Status
#define LOW		    0x00
#define HIGH		    0x01
#define HIGHZ               0x03
#define CHARGING            0x01    //HIGH
#define CHARGE_COMPLETE     0x00    //LOW
#define FAULT               0x02    //HIGH_Z (Input = Output)
#define SHUTDOWN_MODE       0x03
#define BAT_STATUS_ERROR    0xE0    //Error indicator

typedef struct battery_t {
    unsigned char status;
    unsigned char stat1_in;
    unsigned char stat1_out;
    unsigned char stat1_prt;
    unsigned char stat1_pin;
    unsigned char stat1_ddr;
    unsigned char stat2_in;
    unsigned char stat2_out;
    unsigned char stat2_prt;
    unsigned char stat2_pin;
    unsigned char stat2_ddr;
}battery_t;

typedef struct batteries_t {
    battery_t battery_1;
    battery_t battery_2;
unsigned char numberofbats;
}batteries_t;

void rfcx_batteries_init(batteries_t *);
void rfcx_batteries_data_init(batteries_t *);
void rfcx_batteries_status(batteries_t *);
unsigned char rfcx_battery_status(unsigned char);
void rfcx_battery_status_string(char *, unsigned char);

#endif//RFCX_BATTERY_H
