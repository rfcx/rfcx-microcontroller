/**********************************************************
*	RFCx Microcontroller Software - Android Serial Comm.
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#ifndef RFCX_ANDROID_H
#define RFCX_ANDROID_H

#include <stdio.h>
#include <string.h>

//Include for struct definitions
#include "rfcx-i2c.h"
#include "rfcx-battery.h"

//Packet Framing Bytes
#define ANDROID_BEGIN_FLAG  0x7B  //Marks the beginning of a packet  //0111 1011
#define ANDROID_END_FLAG    0x7E  //Marks the end of a packet        //0111 1110

typedef struct android_serial_t {
    temp_data_t * lm75;
    humid_data_t * hih6130;
    adc_data_t * ads1015;
    batteries_t * batteries;
}android_serial_t;

void rfcx_android_package(android_serial_t *, temp_data_t *, humid_data_t *, adc_data_t *, batteries_t *);
void rfcx_android_serialize(char *, android_serial_t *);

#endif//RFCX_ANDROID_H
