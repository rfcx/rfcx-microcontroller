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
#define ADC_ADDR    0x92    //1001XX1X, ADDR connected to VCC
#define HUMID_ADDR  0x4E    //01001110, Pre-defined address (0x27 << 1)

//ADC Pins
#define ADC_INPUT_VOLTAGE_PIN   0x00
#define ADC_OUTPUT_VOLTAGE_PIN  0x01
#define ADC_INPUT_CURRENT_PIN   0x02
#define ADC_OUTPUT_CURRENT_PIN  0x03

//Humidity Sensor Status
#define HUMID_STATUS_NORMAL     0x00    //Normal operation
#define HUMID_STATUS_STALE      0x01    //Stale data
#define HUMID_STATUS_COMMAND    0x02    //Command mode
#define HUMID_STATUS_DIAG       0x03    //Diagnostic condition

//Humidity Sensor Conversion
#define HUMID_COUNTS            0x3FFF  //2^14 - 1 = 16383
#define TEMP_COUNTS             0x3FFF

//Data structure for LM75BD temp sensor
typedef struct temp_raw_t {
    unsigned char msb;
    unsigned char lsb;
}temp_raw_t;

typedef struct temp_data_t {
    temp_raw_t raw;         //Raw bytes
    float temperature;      //Temperature
}temp_data_t;

//Data structure for ADS1015 external ADC
typedef struct adc_data_t {
    float input_voltage;    //Input voltage
    float output_voltage;   //Output voltage
    float input_current;    //Input current
    float output_current;   //Output current
}adc_data_t;

//Data structures for HIH6130 humidity + temp sensor
typedef struct humid_raw_t {
    unsigned char humid_msb;
    unsigned char humid_lsb;
    unsigned char temp_msb;
    unsigned char temp_lsb;
}humid_raw_t;

typedef struct humid_data_t {
    humid_raw_t raw;        //Raw bytes
    float humidity;         //Relative humidity
    float temperature;      //Temperature
    unsigned char status;   //Status
}humid_data_t;

//Initialization
void rfcx_i2c_init(void);
int rfcx_temp_init(void);
int rfcx_adc_init(void);
int rfcx_humid_init(void);

//Shutdown
void rfcx_i2c_shutdown(void);
void rfcx_temp_shutdown(void);
void rfcx_adc_shutdown(void);
void rfcx_humid_shutdown(void);

//Read
int rfcx_read_temp(temp_data_t *);
float rfcx_read_adc_pin(int pin);
int rfcx_read_adc(adc_data_t *);
int rfcx_read_humid(humid_data_t *);

//Static Conversion Helpers
void convert_temp_data(temp_data_t *);
int convert_from_binary(char byte);
float convert_adc_data(char MSB, char LSB);
void convert_humid_data(humid_data_t *);

#endif//RFCX_I2C_H
