/**********************************************************
*	RFCx Microcontroller Software - Battery
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#ifndef RFCX_BATTERY_H
#define RFCX_BATTERY_H

#include <avr/io.h> //Pin definitions

//Battery Identifiers
#define BATTERY_1           0x01
#define BATTERY_2           0x02

//Battery Status Pins
#define BAT_1_INPUT_PIN     PC0
#define BAT_1_OUTPUT_PIN    PC1
#define BAT_2_INPUT_PIN     PC2
#define BAT_2_OUTPUT_PIN    PC3

//Battery Charging Status
#define CHARGING            0x01    //HIGH
#define CHARGE_COMPLETE     0x00    //LOW
#define SLEEP_MODE          0x02    //HIGH_Z (Input = Output)
#define TEMPERATURE_FAULT   SLEEP_MODE    //Temperature fault and sleep mode share the same value for some reason...
#define BAT_STATUS_ERROR    0xE0    //Error indicator

typedef struct battery_t {
    unsigned char status;
}battery_t;

typedef struct batteries_t {
    battery_t battery_1;
    battery_t battery_2;
}batteries_t;

void rfcx_batteries_init(void);
void rfcx_batteries_status(batteries_t *);
unsigned char rfcx_battery_status(unsigned char);

#endif//RFCX_BATTERY_H
