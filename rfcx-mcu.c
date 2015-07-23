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

#include "rfcx-mcu.h"

//Don't forget `volatile`!
volatile bool sensors = false;

//Timer 1 Compare Interrupt Vector (1s CTC Timer)
ISR(TIMER1_COMPA_vect) {
	//Blink LED
	PORTB ^= _BV(LED_PIN);

	//Initiate a sensor reading
	sensors = true;
}

int main(void) {
	temp_data_t 	lm75;
	// adc_data_t 		ads1015;
	humid_data_t 	hih6130;

	char message[128];
	char status[128];
	char tmp_str[6];
	int ret = 0;

	memset(tmp_str, 0, 6);

	//Initialize USART at 9600 baud (UBRR defined in rfcx-mcu.h)
	usart_init(UBRR);

	//Initialization
	usart_send_string("Initializing...\r\n");
	ret = init();
	if(ret) {
		usart_send_string("<-- ERROR: Initialization failed -->\r\n");
	} else {
		usart_send_string("Initialization successful\r\n");
	}

	//Main Loop
	while(true) {
		//Sensor Loop
		if(sensors) {
			usart_send_string("\r\n-----------------------------\r\n");
			//Temperature Sensor
			rfcx_read_temp(&lm75);

			usart_send_string("LM75BD:\r\n");
			dtostrf((double)lm75.temperature, 5, 2, tmp_str);
			sprintf(message, "\tTemperature: %sC\r\n\r\n", tmp_str);
			usart_send_string(message);

			//Humidity Sensor
			rfcx_read_humid(&hih6130);

			usart_send_string("HIH6130:\r\n");
			dtostrf((double)hih6130.humidity, 5, 2, tmp_str);
			sprintf(message, "\tHumidity: %s%%\r\n", tmp_str);
			usart_send_string(message);

			dtostrf((double)hih6130.temperature, 5, 2, tmp_str);
			sprintf(message, "\tTemperature: %sC\r\n", tmp_str);
			usart_send_string(message);

			switch(hih6130.status) {
				case HUMID_STATUS_NORMAL:
					sprintf(status, "Normal");
					break;
				case HUMID_STATUS_STALE:
					sprintf(status, "STALE DATA");
					break;
				case HUMID_STATUS_COMMAND:
					sprintf(status, "Command Mode");
					break;
				case HUMID_STATUS_DIAG:
					sprintf(status, "Diagnostic Condition");
					break;
				default:
					sprintf(status, "UNKNOWN");
					break;
			}

			sprintf(message, "\tStatus: %s\r\n", status);
			usart_send_string(message);

			//Voltage/Current ADC
			// rfcx_read_adc(&ads1015);
			//
			// dtostrf((double)ads1015.input_voltage, 5, 2, tmp_str);
			// sprintf(message, "Input Voltage: %sV\r\n", tmp_str);
			// usart_send_string(message);
			//
			// dtostrf((double)ads1015.output_voltage, 5, 2, tmp_str);
			// sprintf(message, "Output Voltage: %sV\r\n", tmp_str);
			// usart_send_string(message);
			//
			// dtostrf((double)ads1015.input_current, 5, 2, tmp_str);
			// sprintf(message, "Input Current: %smA\r\n", tmp_str);
			// usart_send_string(message);
			//
			// dtostrf((double)ads1015.output_current, 5, 2, tmp_str);
			// sprintf(message, "Output Current: %smA\r\n", tmp_str);
			// usart_send_string(message);

			usart_send_string("-----------------------------\r\n");

			sensors = false;
		}
	}

	return 0;
}

int init(void) {
	int ret = 0;

	//Clear interrupts
	cli();

	//Initialize ports
	port_init();

	//Initialize peripherals
	peripheral_init();

	//Initialize devices
	ret = device_init();

	//Enable interrupts
	sei();

	return ret;
}

int port_init(void) {
	//Initialize LED as output
	DDRB |= _BV(LED_DD);

	return 0;
}

int timer1_init(void) {
	//Initialize Timer 1
	TCCR1A = 0;
	TCCR1B = 0;

	//Set CTC compare value (1 second)
	OCR1A = TIMER1_COUNT;

	//Enable CTC mode
	TCCR1B |= (1 << WGM12);

	//Enable 1024 prescaler
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);

	//Enable Timer 1 output compare interrupt
	TIMSK1 |= (1 << OCIE1A);

	return 0;
}

int peripheral_init(void) {
	//Initialize Timer 1
	timer1_init();

	//Initialize I2C (TWI) peripheral as a whole
	rfcx_i2c_init();

	return 0;
}

int device_init(void) {
	int ret = 0;

	//Initialize external I2C temp sensor (LM75BD)
	ret = rfcx_temp_init();
	if(ret) {
		usart_send_string("<-- ERROR: Error initializing temp sensor -->\r\n");
		return ret;
	} else {
		usart_send_string("Successfully initialized temp sensor\r\n");
	}

	//Initialize external I2C ADC (ADS1015)
	// ret = rfcx_adc_init();
	// if(ret) {
	// 	 usart_send_string("<-- ERROR: Error initializing ADC -->\r\n");
	// } else {
	// 	 usart_send_string("Successfully initialized ADC\r\n");
	// }

	//Initialize external I2C humidity sensor (HIH6130)
	ret = rfcx_humid_init();
	if(ret) {
		usart_send_string("<-- ERROR: Error initializing humidity sensor -->\r\n");
		return ret;
	} else {
		usart_send_string("Successfully initialized humidity sensor\r\n");
	}

	return ret;
}
