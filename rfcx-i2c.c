#include "rfcx-i2c.h"

void ATMegaInit()
{
  i2c_init();
}

void ADCInit()
{
  unsigned char value, value2, value3;
  value = 0x01;
  value2 = 0x41;
  value3 = 0xEF;
  i2c_start_wait(ADCADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Put the ADC in single conversion mode
  i2c_write(value2);
  //Set the data rate to 3300, disable the comparator
  i2c_write(value3);
}

void TempSensorInit()
{
  unsigned char value, value2;
  value = 0x01;
  value2 = 0x04;
  i2c_start(TEMPADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Enable the temp sensor
  i2c_write(value2);
  i2c_stop();
}

float ConvertTempData(char MSB, char LSB)
{
  float result;
  //Check if the temperature is positive or negative
  if((MSB & 10000000) == 00000000)
  {
    //Convert the positive 12 bit number into the temperature
    result = (float)(ConvertFromBinary(MSB) + (ConvertFromBinary(LSB >> 4) * .125));
  }
  else
  {
    //Convert the negative 12 bit number into the temperature
    result = -1 * (float)(ConvertFromBinary(MSB) + (ConvertFromBinary(LSB >> 4) * .125));
  }

  return result;
}

//Convert a binary value into a decimal number
int ConvertFromBinary(char byte)
{
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

float ConvertADCData(char MSB, char LSB)
{
  float result;
  //Just change the 12 bit number into a float, may change after testing
  result = (float)(ConvertFromBinary(MSB) + (ConvertFromBinary(LSB >> 4)));

  return result;
}

//Read Temperature data from the LM75B
float ReadTemp()
{
  float result;
  unsigned char response1, response2, address;
  address = TEMPADDR;
  i2c_start(address);
  //Write the pointer register in the sensor to point to the temp register
  i2c_write(0x00);
  i2c_rep_start(address);
  //Read both bytes for the temperature
  response1 = i2c_readAck();
  response2 = i2c_readNak();
  i2c_stop();
  result = ConvertTempData(response1, response2);

  return result;
}

float ReadInputCurrent()
{
  unsigned char value, value2, value3;
  value = 0x01;
  value2 = 0xC1;
  value3 = 0xEF;
  i2c_start_wait(ADCADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Put the ADC in single conversion mode, read from AIN0
  i2c_write(value2);
  //Set the data rate to 3300, disable the comparator
  i2c_write(value3);

  value = 0x00;
  i2c_rep_start(ADCADDR);
  //Set the pointer to the conversion register
  i2c_write(value);


  unsigned char response, response2;
  float result;
  i2c_rep_start(ADCADDR);
  //Read from the conversion register
  response = i2c_readAck();
  response2 = i2c_readNak();
  result = ConvertADCData(response, response2);
  return result;
}

float ReadOutputCurrent()
{
  unsigned char value, value2, value3;
  value = 0x01;
  value2 = 0xD1;
  value3 = 0xEF;
  i2c_start_wait(ADCADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Put the ADC in single conversion mode, read from AIN1
  i2c_write(value2);
  //Set the data rate to 3300, disable the comparator
  i2c_write(value3);

  value = 0x00;
  i2c_rep_start(ADCADDR);
  //Set the pointer to the conversion register
  i2c_write(value);


  unsigned char response, response2;
  float result;
  i2c_rep_start(ADCADDR);
  //Read from the conversion register
  response = i2c_readAck();
  response2 = i2c_readNak();
  result = ConvertADCData(response, response2);
  return result;
}

float ReadInputVoltage()
{
  unsigned char value, value2, value3;
  value = 0x01;
  value2 = 0xE1;
  value3 = 0xEF;
  i2c_start_wait(ADCADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Put the ADC in single conversion mode, read from AIN2
  i2c_write(value2);
  //Set the data rate to 3300, disable the comparator
  i2c_write(value3);

  value = 0x00;
  i2c_rep_start(ADCADDR);
  //Set the pointer to the conversion register
  i2c_write(value);


  unsigned char response, response2;
  float result;
  i2c_rep_start(ADCADDR);
  //Read from the conversion register
  response = i2c_readAck();
  response2 = i2c_readNak();
  result = ConvertADCData(response, response2);
  return result;
}

float ReadOutputVoltage()
{
  unsigned char value, value2, value3;
  value = 0x01;
  value2 = 0xF1;
  value3 = 0xEF;
  i2c_start_wait(ADCADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Put the ADC in single conversion mode, read from AIN3
  i2c_write(value2);
  //Set the data rate to 3300, disable the comparator
  i2c_write(value3);

  value = 0x00;
  i2c_rep_start(ADCADDR);
  //Set the pointer to the conversion register
  i2c_write(value);


  unsigned char response, response2;
  float result;
  i2c_rep_start(ADCADDR);
  //Read from the conversion register
  response = i2c_readAck();
  response2 = i2c_readNak();
  result = ConvertADCData(response, response2);
  return result;
}

void ATMegaSleep()
{
  i2c_stop();
}

void ATMegaShutdown()
{
  i2c_stop();
}

//Set Temp Sensor control address to active high, comparator mode, shutdown mode
void TempSensorShutdown()
{
  unsigned char value, value2;
  value = 0x01;
  value2 = 0x05;
  i2c_start_wait(TEMPADDR);
  //Set the pointer to the configuration register
  i2c_write(value);
  //Put the temp sensor in shutdown mode
  i2c_write(value2);
  i2c_stop();
}

void ADCShutdown()
{//Stub, may be used later
  return;
}
