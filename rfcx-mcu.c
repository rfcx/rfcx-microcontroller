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
	//Sensor/battery structures
	batteries_t batteries;
	temp_data_t lm75;
	// adc_data_t ads1015;
	humid_data_t hih6130;

	char message[128];
	char humid_status[128];
	char battery_1_status[128];
	char battery_2_status[128];
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
			sprintf(message, "\tTemperature: %sC\r\n", tmp_str);
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

			// sprintf(message, "\tRaw:\t msb: 0x%02X\r\n"
			// 						"\t\t lsb: 0x%02X\r\n",
			// 						hih6130.raw.temp_msb,
			// 						hih6130.raw.temp_lsb);
			// usart_send_string(message);

			switch(hih6130.status) {
				case HUMID_STATUS_NORMAL:
					sprintf(humid_status, "Normal");
					break;
				case HUMID_STATUS_STALE:
					sprintf(humid_status, "STALE DATA");
					break;
				case HUMID_STATUS_COMMAND:
					sprintf(humid_status, "Command Mode");
					break;
				case HUMID_STATUS_DIAG:
					sprintf(humid_status, "Diagnostic Condition");
					break;
				default:
					sprintf(humid_status, "UNKNOWN");
					break;
			}

			sprintf(message, "\tStatus: %s\r\n", humid_status);
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

			//Battery Status
			usart_send_string("Batteries:\r\n");

			battery_status_string(battery_1_status, batteries.battery_1.status);
			battery_status_string(battery_2_status, batteries.battery_2.status);

			sprintf(message, 	"\tBattery 1 Status: %s\r\n"
								"\tBattery 2 Status: %s\r\n",
								battery_1_status,
								battery_2_status);
			usart_send_string(message);

			usart_send_string("-----------------------------\r\n");

			sensors = false;
		}
	}

	return 0;
}

void battery_status_string(char * str, unsigned char status) {
	switch(status) {
		case CHARGING:
			sprintf(str, "Charging");
			break;
		case CHARGE_COMPLETE:
			sprintf(str, "Charge Complete");
			break;
		case SLEEP_MODE:
			sprintf(str, "Sleep Mode");
			break;
		//@TODO How to identify between Sleep Mode and Temp Fault?
		// case TEMPERATURE_FAULT:
		// 	sprintf(str, "Temperature Fault");
		// 	break;
		case BAT_STATUS_ERROR:
			sprintf(str, "ERROR");
		default:
			sprintf(str, "UNKNOWN");
			break;
	}
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

	//Initialize battery inputs/outputs
	rfcx_batteries_init();

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
