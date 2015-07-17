/**********************************************************
*	RFCx Microcontroller Software - Main
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#include "utilities/delay.h"
#include "rfcx-mcu.h"

int readFlag = 0;

int main(void) {
	double currTemp, currInV, currInC, currOutV, currOutC = 0.0;
	char message[100];
	char tmp_str[6], tmp_str2[6];
	int ret = 0;

	memset(tmp_str, 0, 6);
	memset(tmp_str2, 0, 6);

	//Initialize USART at 9600 baud (UBRR defined in rfcx-mcu.h)
	usart_init(UBRR);

	//Initialization
	usart_send_string("Initializing...\r\n");
	ret = init();
	if(ret) {
		usart_send_string("<-- ERROR: Initialization failed -->\r\n");
	} else {
		usart_send_string("Initialization successful\r\n");
		readFlag = 1;
	}

	while(readFlag) {
		//Delay 1 second
		delay_us(1000000);

		//Make a temperature reading for testing
		currTemp = rfcx_read_temp();
		dtostrf((double)currTemp, 5, 2, tmp_str);

		currInV = rfcx_read_adc_pin(0);
		dtostrf((double)currTemp, 5, 2, tmp_str2);

		sprintf(message, "Temperature: %sC\r\nInput Voltage: %sC\r\n", tmp_str, tmp_str2);
		usart_send_string(message);
	}

	// while(1) {
	// 	usart_Send_string("DELAY\r\n");
	// 	delay_ms(5000000);
	// 	//if(readFlag)
	// 	//{
	// 	//currTemp = ((double)ReadTemp());
	// 	//currInV = ReadInputVoltage();
	// 	//currInC = ReadInputCurrent();
	// 	//currOutV = ReadOutputVoltage();
	// 	//currOutC = ReadOutputCurrent();
	// 	//sprintf(message, "Temp: %f\r\nInput V: %f\r\nInput C: %f\r\nOutput V: %f\r\nOutput C: %f\r\n",
	// 	//       currTemp, currInV, currInC, currOutV, currOutC);
	// 	sprintf(message, "Temp: %f\r\n", currTemp);
	// 	usart_Send_string(message);
	// 	//readFlag = 0;
	// }

	return 0;
}

int init(void) {
	int ret = 0;

	//Clear interrupts
	cli();

	//Initialize peripherals
	peripheral_init();

	//Initialize devices
	ret = device_init();

	//Enable interrupts
	sei();

	return ret;
}

int peripheral_init(void) {
	//Initialize I2C (TWI) peripheral as a whole
	rfcx_i2c_init();

	return 0;
}

int device_init(void) {
	int ret = 0;

	//Initialize external I2C ADC (ADS1015)
	 ret = rfcx_adc_init();
	 if(ret) {
	 	 usart_send_string("<-- ERROR: Error initializing ADC -->\r\n");
	 } else {
	 	 usart_send_string("Successfully initialized ADC\r\n");
	 }

	//Initialize external I2C temp sensor (LM75BD)
	ret = rfcx_temp_init();
	if(ret) {
		usart_send_string("<-- ERROR: Error initializing temp sensor -->\r\n");
	} else {
		usart_send_string("Successfully initialized temp sensor\r\n");
	}

	return ret;
}
