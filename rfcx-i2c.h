/**********************************************************
*	RFCx Microcontroller Software - I2C
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#ifndef RFCX_I2C_H
#define RFCX_I2C_H

#include <avr/io.h>
#include "i2cmaster/i2cmaster.h"

//Simple Error Representation
#define OK      0
#define ERROR   1

//I2C Addresses
#define TEMP_ADDR   0x90    //1001CBA0, A = B = C = 0 (pulled up/down in hardware)
#define ADC_ADDR    0x00    //SET ADDR

//Initialization
void rfcx_i2c_init(void);
int rfcx_temp_init(void);
int rfcx_adc_init(void);

//Shutdown
void rfcx_i2c_shutdown(void);
void rfcx_temp_shutdown(void);
void rfcx_adc_shutdown(void);

//Read
float rfcx_read_temp(void);
float rfcx_read_input_current(void);
float rfcx_read_output_current(void);
float rfcx_read_input_voltage(void);
float rfcx_read_output_voltage(void);

//Static Conversion Helpers
float convert_temp_data(int MSB, int LSB);
int convert_from_binary(char byte);
float convert_adc_data(char MSB, char LSB);

#endif//RFCX_I2C_H
