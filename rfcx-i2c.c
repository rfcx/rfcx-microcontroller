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
#include "utilities/usart.h"

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
    unsigned char msb, lsb, address;
    address = TEMP_ADDR;
    int ret = 0;
    //char str[512];

    //Write the pointer register in the sensor to point to the temp register
    i2c_start_wait(address + I2C_WRITE);

    ret = i2c_write(0x00);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not set pointer register to temp (0x00)-->\r\n");
        return 0;
    }

    //Read both bytes for the temperature (msb first, then lsb)
    ret = i2c_rep_start(address + I2C_READ);
    if(ret) {
        i2c_stop();
        usart_send_string("<-- ERROR: Could not repeat start temp sensor-->\r\n");
    }

    msb = i2c_readAck();
    lsb = i2c_readNak();
    i2c_stop();

    //sprintf(str, "msb = 0x%02X, lsb = 0x%02X\r\n", msb, lsb);
    //usart_send_string(str);

    result = convert_temp_data(msb, lsb);

    return result;
}

//@TODO These can all be confined to a single function that takes
//      a value from 1-4 and returns the float associated with that
//      pin, where INPUT_CURRENT is defined as 1, OUTPUT_CURRENT as 2, and so on...

float rfcx_read_adc_pin(int pinNum)
{
  unsigned char value, value2, value3;
  value = 0x01;

  switch(pinNum){
    case 0: //Pin AIN0
      value2 = 0xC1;
      break;
    case 1: //Pin AIN1
      value2 = 0xD1;
      break;
    case 2: //Pin AIN2
      value2 = 0xE1;
      break;
    case 3: //Pin AIN3
      value2 = 0xF1;
      break;
    default: //Pin AIN0
      value2 = 0xC1;
      break;
  }

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

/*
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
}*/

// float convert_temp_data(char MSB, char LSB) {
//     float result;
//     //Check if the temperature is positive or negative
//     if((MSB & 10000000) == 00000000)
//     {
//         //Convert the positive 12 bit number into the temperature
//         result = (float)(convert_from_binary(MSB) + (convert_from_binary(LSB >> 4) * .125));
//     }
//     else
//     {
//         //Convert the negative 12 bit number into the temperature
//         result = -1 * (float)(convert_from_binary(MSB) + (convert_from_binary(LSB >> 4) * .125));
//     }
//
//     return result;
// }

float convert_temp_data(int msb, int lsb) {
    char str[512];
    float result = 0.0;

    int tmp = (msb << 8) | lsb;
    result = (float)tmp / 256.0;

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
