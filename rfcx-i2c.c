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

    int ret = i2c_start(TEMP_ADDR);

    if(ret) {
        i2c_stop();
        return ERROR;
    }

    //Set the pointer to the configuration register
    i2c_write(value);

    //Enable the temp sensor
    i2c_write(value2);
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

void rfcx_adc_shutdown() {
    return;
}

//Read Temperature data from the LM75B
float rfcx_read_temp() {
    float result;
    unsigned char response1, response2, address;
    address = TEMP_ADDR;
    i2c_start(address);
    //Write the pointer register in the sensor to point to the temp register
    i2c_write(0x00);
    i2c_rep_start(address);
    //Read both bytes for the temperature
    response1 = i2c_readAck();
    response2 = i2c_readNak();
    i2c_stop();
    result = convert_temp_data(response1, response2);

    return result;
}

//@TODO These can all be confined to a single function that takes
//      a value from 1-4 and returns the float associated with that
//      pin, where INPUT_CURRENT is defined as 1, OUTPUT_CURRENT as 2, and so on...
float rfcx_read_input_current() {
    unsigned char value, value2, value3;
    value = 0x01;
    value2 = 0xC1;
    value3 = 0xEF;
    i2c_start_wait(ADC_ADDR);
    //Set the pointer to the configuration register
    i2c_write(value);
    //Put the ADC in single conversion mode, read from AIN0
    i2c_write(value2);
    //Set the data rate to 3300, disable the comparator
    i2c_write(value3);

    value = 0x00;
    i2c_rep_start(ADC_ADDR);
    //Set the pointer to the conversion register
    i2c_write(value);


    unsigned char response, response2;
    float result;
    i2c_rep_start(ADC_ADDR);
    //Read from the conversion register
    response = i2c_readAck();
    response2 = i2c_readNak();
    result = convert_adc_data(response, response2);
    return result;
}

float rfcx_read_output_current() {
    unsigned char value, value2, value3;
    value = 0x01;
    value2 = 0xD1;
    value3 = 0xEF;
    i2c_start_wait(ADC_ADDR);
    //Set the pointer to the configuration register
    i2c_write(value);
    //Put the ADC in single conversion mode, read from AIN1
    i2c_write(value2);
    //Set the data rate to 3300, disable the comparator
    i2c_write(value3);

    value = 0x00;
    i2c_rep_start(ADC_ADDR);
    //Set the pointer to the conversion register
    i2c_write(value);


    unsigned char response, response2;
    float result;
    i2c_rep_start(ADC_ADDR);
    //Read from the conversion register
    response = i2c_readAck();
    response2 = i2c_readNak();
    result = convert_adc_data(response, response2);
    return result;
}

float rfcx_read_input_voltage() {
    unsigned char value, value2, value3;
    value = 0x01;
    value2 = 0xE1;
    value3 = 0xEF;
    i2c_start_wait(ADC_ADDR);
    //Set the pointer to the configuration register
    i2c_write(value);
    //Put the ADC in single conversion mode, read from AIN2
    i2c_write(value2);
    //Set the data rate to 3300, disable the comparator
    i2c_write(value3);

    value = 0x00;
    i2c_rep_start(ADC_ADDR);
    //Set the pointer to the conversion register
    i2c_write(value);


    unsigned char response, response2;
    float result;
    i2c_rep_start(ADC_ADDR);
    //Read from the conversion register
    response = i2c_readAck();
    response2 = i2c_readNak();
    result = convert_adc_data(response, response2);
    return result;
}

float rfcx_read_output_voltage() {
    unsigned char value, value2, value3;
    value = 0x01;
    value2 = 0xF1;
    value3 = 0xEF;
    i2c_start_wait(ADC_ADDR);
    //Set the pointer to the configuration register
    i2c_write(value);
    //Put the ADC in single conversion mode, read from AIN3
    i2c_write(value2);
    //Set the data rate to 3300, disable the comparator
    i2c_write(value3);

    value = 0x00;
    i2c_rep_start(ADC_ADDR);
    //Set the pointer to the conversion register
    i2c_write(value);


    unsigned char response, response2;
    float result;
    i2c_rep_start(ADC_ADDR);
    //Read from the conversion register
    response = i2c_readAck();
    response2 = i2c_readNak();
    result = convert_adc_data(response, response2);
    return result;
}

float convert_temp_data(char MSB, char LSB) {
    float result;
    //Check if the temperature is positive or negative
    if((MSB & 10000000) == 00000000)
    {
        //Convert the positive 12 bit number into the temperature
        result = (float)(convert_from_binary(MSB) + (convert_from_binary(LSB >> 4) * .125));
    }
    else
    {
        //Convert the negative 12 bit number into the temperature
        result = -1 * (float)(convert_from_binary(MSB) + (convert_from_binary(LSB >> 4) * .125));
    }

    return result;
}

//Convert a binary value into a decimal number
int convert_from_binary(char byte) {
    int sum = 0;
    sum += ((int)(byte & 00000001));
    sum += ((int)(byte & 00000010)*2);
    sum += ((int)(byte & 00000100)*4);
    sum += ((int)(byte & 00001000)*8);
    sum += ((int)(byte & 00010000)*16);
    sum += ((int)(byte & 00100000)*32);
    sum += ((int)(byte & 01000000)*64);
    return sum;
}

float convert_adc_data(char MSB, char LSB) {
    float result;
    //Just change the 12 bit number into a float, may change after testing
    result = (float)(convert_from_binary(MSB) + (convert_from_binary(LSB >> 4)));

    return result;
}
