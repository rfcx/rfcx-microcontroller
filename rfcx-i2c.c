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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rfcx-i2c.h"

//Initialize I2C peripheral
void rfcx_i2c_init(void) {
    i2c_init();
}

//Initialize temperature sensor
int rfcx_temp_init() {
    unsigned char value, value2;
    value = 0x01;
    value2 = 0x04;

    int ret = 0;

    char str[512];
    memset(str, 0, 512);

    unsigned char address;

    address = TEMP_ADDR;

    //sprintf(str, "Device address: 0x%02X\r\n", address);
    //usart_send_string(str);

    //Begin TWI communication
    ret = i2c_start(address + I2C_WRITE);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Unable to start communication-->\r\n");
        return ERROR;
    }

    //Set the pointer to the configuration register
    ret = i2c_write(value);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not set pointer to LM75 config register -->\r\n");
        return ERROR;
    }

    //Enable the temp sensor
    ret = i2c_write(value2);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not enable LM75 -->\r\n");
        return ERROR;
    }

    //Release bus
    i2c_stop();

    return OK;
}

//Initialize ADC
int rfcx_adc_init() {
    unsigned char value, value2, value3;
    value = 0x01;
    value2 = 0x41;
    value3 = 0xEF;

    i2c_start_wait(ADC_ADDR);

    //Set the pointer to the configuration register
    i2c_write(value);

    //Put the ADC in single conversion mode
    i2c_write(value2);

    //Set the data rate to 3300, disable the comparator
    i2c_write(value3);

    return OK;
}

//Initialize humidity sensor
int rfcx_humid_init(void) {
    return OK;
}

//Shutdown (stop) I2C peripheral
//@TODO Does this function even make sense?
void rfcx_i2c_shutdown(void) {
    i2c_stop();
}

//Shutdown temperature sensor:
// Set Temp Sensor control address to
// active high comparator mode, shutdown mode
void rfcx_temp_shutdown() {
    unsigned char value, value2;
    value = 0x01;
    value2 = 0x05;

    i2c_start_wait(TEMP_ADDR);

    //Set the pointer to the configuration register
    i2c_write(value);

    //Put the temp sensor in shutdown mode
    i2c_write(value2);
    i2c_stop();
}

void rfcx_adc_shutdown(void) {
    return;
}

void rfcx_humid_shutdown(void) {
    return;
}

//Read Temperature data from the LM75B
int rfcx_read_temp(temp_data_t * data) {
    int ret = 0;

    //Write the pointer register in the sensor to point to the temp register
    i2c_start_wait(TEMP_ADDR + I2C_WRITE);
    ret = i2c_write(0x00);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not set pointer register to temp (0x00)-->\r\n");
        return ERROR;
    }

    //Read both bytes for the temperature (msb first, then lsb)
    ret = i2c_rep_start(TEMP_ADDR + I2C_READ);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not repeat start temp sensor-->\r\n");
        return ERROR;
    }

    data->raw.msb = i2c_readAck();
    data->raw.lsb = i2c_readNak();
    i2c_stop();

    //Convert data
    convert_temp_data(data);

    return OK;
}

int rfcx_read_adc(adc_data_t * data) {
    float input_voltage = 0.0;
    float output_voltage = 0.0;
    float input_current = 0.0;
    float output_current = 0.0;

    //Perform all reads
    rfcx_read_adc_pin(data, ADC_INPUT_VOLTAGE_PIN);
    rfcx_read_adc_pin(data, ADC_OUTPUT_VOLTAGE_PIN);
    rfcx_read_adc_pin(data, ADC_INPUT_CURRENT_PIN);
    rfcx_read_adc_pin(data, ADC_OUTPUT_CURRENT_PIN);

    //Convert data
    convert_adc_data(data);

    return OK;
}

int rfcx_read_adc_pin(adc_data_t * data, int pin) {
    unsigned char value, value2, value3;
    value = 0x01;

    switch(pin) {
        case ADC_INPUT_VOLTAGE_PIN:     //Pin AIN0 - Input voltage
            value2 = 0xC1;
            break;
        case ADC_OUTPUT_VOLTAGE_PIN:    //Pin AIN1 - Output voltage
            value2 = 0xD1;
            break;
        case ADC_INPUT_CURRENT_PIN:     //Pin AIN2 - Input current
            value2 = 0xE1;
            break;
        case ADC_OUTPUT_CURRENT_PIN:    //Pin AIN3 - Output current
            value2 = 0xF1;
            break;
        default:                        //Pin AIN0 - Input voltage
            value2 = 0xC1;
            break;
    }

    value3 = 0xEF;
    i2c_start_wait(ADC_ADDR + I2C_WRITE);

    //Set the pointer to the configuration register
    i2c_write(value);

    //Put the ADC in single conversion mode, read from AIN0
    i2c_write(value2);

    //Set the data rate to 3300, disable the comparator
    i2c_write(value3);

    value = 0x00;
    i2c_rep_start(ADC_ADDR + I2C_WRITE);
    //Set the pointer to the conversion register
    i2c_write(value);
    i2c_stop();

    unsigned char msb, lsb;

    //Read from the conversion register
    i2c_rep_start(ADC_ADDR + I2C_READ);
    msb = i2c_readAck();
    lsb = i2c_readNak();

    //Store correctly based on pin
    switch(pin) {
        case ADC_INPUT_VOLTAGE_PIN:
            data->raw.input_voltage_msb = msb;
            data->raw.input_voltage_lsb = lsb;
            break;
        case ADC_OUTPUT_VOLTAGE_PIN:
            data->raw.output_voltage_msb = msb;
            data->raw.output_voltage_lsb = lsb;
            break;
        case ADC_INPUT_CURRENT_PIN:
            data->raw.input_current_msb = msb;
            data->raw.input_current_lsb = lsb;
            break;
        case ADC_OUTPUT_CURRENT_PIN:
            data->raw.output_current_msb = msb;
            data->raw.output_current_lsb = lsb;
            break;
        default:
            data->raw.input_voltage_msb = msb;
            data->raw.input_voltage_lsb = lsb;
            break;
    }

    return OK;
}

int rfcx_read_humid(humid_data_t * data) {
    int ret= 0;

    //Issue a 'Measurement Request' command
    i2c_start_wait(HUMID_ADDR + I2C_WRITE);
    i2c_stop();

    //Delay ~37ms (datasheet specifies 36.65ms)
    // NOTE:    This could be avoided if necessary, and we
    //          will just always get the data from the
    //          previous conversion.
    delay_us(HUMID_CONV_TIME);

    //Fetch humidity + temp data
    ret = i2c_rep_start(HUMID_ADDR + I2C_READ);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not repeat start humidity sensor-->\r\n");
        return ERROR;
    }

    data->raw.humid_msb = i2c_readAck();
    data->raw.humid_lsb = i2c_readAck();
    data->raw.temp_msb = i2c_readAck();
    data->raw.temp_lsb = i2c_readNak();
    i2c_stop();

    //Status bits are two msb's of humid_msb
    data->status = (data->raw.humid_msb & 0xC0) >> 6;

    //Perform conversion
    convert_humid_data(data);

    return OK;
}


void convert_temp_data(temp_data_t * data) {
    int tmp = 0;
    float result = 0.0;

    int msb = (int)data->raw.msb;
    int lsb = (int)data->raw.lsb;

    //Shift 'dem bits around
    tmp = ((msb << 8) | (lsb & ~0x1F)) >> 5;

    //Check sign of data
    if((msb & 0x80) == 0x80) {
        result = (float)(tmp) * 0.125;
    }
    else {
        result = -1.0 * (float)(~tmp + 1) * 0.125;
    }

    data->temperature = result;
}

void convert_adc_data(adc_data_t * data) {
    data->input_voltage = convert_adc_data_pin(data, ADC_INPUT_VOLTAGE_PIN);
    //...
}

float convert_adc_data_pin(adc_data_t * data, int pin) {
    //Perform conversion for individual pin
    //Should be able to use pin nubmer as offset into the raw struct to make this easier

    //2-byte conversion register
    int tmp = 0;
    int msb = 0; //Set to correct msb
    int lsb = 0; //Set to correct lsb

    tmp = ((msb << 8) | lsb) >> 4;
}

void convert_humid_data(humid_data_t * data) {
    uint16_t tmp_humid = 0;
    uint16_t tmp_temp = 0;

    uint16_t humid_msb = (uint16_t)data->raw.humid_msb;
    uint16_t humid_lsb = (uint16_t)data->raw.humid_lsb;
    uint16_t temp_msb = (uint16_t)data->raw.temp_msb;
    uint16_t temp_lsb = (uint16_t)data->raw.temp_lsb;

    //Bit shifting
    tmp_humid = ((humid_msb & ~0xC0) << 8) | humid_lsb;
    tmp_temp = ((temp_msb << 8) | (temp_lsb & ~0x03)) >> 2;

    data->humidity = ((float)tmp_humid / HUMID_COUNTS) * 100.0;
    data->temperature = (((float)tmp_temp / TEMP_COUNTS) * 165.0) - 40.0;

    return;
}

//Convert a binary value into a decimal number
//@TODO This function doesn't make sense... this is no different than just typecasting a char to an int: `... = (int)byte;` What is this function for?
// int convert_from_binary(char byte) {
//     int sum = 0;
//     sum += ((int)(byte & 00000001));
//     sum += ((int)(byte & 00000010)*2);
//     sum += ((int)(byte & 00000100)*4);
//     sum += ((int)(byte & 00001000)*8);
//     sum += ((int)(byte & 00010000)*16);
//     sum += ((int)(byte & 00100000)*32);
//     sum += ((int)(byte & 01000000)*64);
//     return sum;
// }
