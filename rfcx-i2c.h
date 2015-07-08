#ifndef RFCX_I2C_H
#define RFCX_I2C_H

#include <avr/io.h>
#include "i2cmaster/i2cmaster.h"

#define TEMPADDR 72
#define ADCADDR 73

void ATMegaInit(void);

void ADCInit(void);

void TempSensorInit(void);

float ConvertTempData(char MSB, char LSB);

int ConvertFromBinary(char byte);

float ConvertADCData(char MSB, char LSB);

float ReadTemp(void);

float ReadInputCurrent(void);

float ReadOutputCurrent(void);

float ReadInputVoltage(void);

float ReadOutputVoltage(void);

void ATMegaSleep(void);

void ATMegaShutdown(void);

void TempSensorShutdown(void);

void ADCShutdown(void);

#endif//RFCX_I2C_H
