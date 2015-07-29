/**********************************************************
*	RFCx Microcontroller Software - Android Serial Comm.
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#include "rfcx-android.h"

//Package all sensor structure pointers in a single android_serial_t structure
void rfcx_android_package(android_serial_t * android, temp_data_t * lm75, humid_data_t * hih6130, adc_data_t * ads1015, batteries_t * batteries) {
    android->lm75 = lm75;
    android->hih6130 = hih6130;
    android->ads1015 = ads1015;
    android->batteries = batteries;
}

//Serialize the data to the correct format expected by the Android application:
//
//The format is as follows (all comma separated)
//  ANDROID_BEGIN_FLAG (0x7B)
//  Input Voltage
//  Input Current
//  Output Voltage
//  Output Current
//  Temperature
//  Humidity
//  Humidity Sensor Status
//  Battery 1 Status
//  Battery 2 Status
//  ANDROID_END_FLAG (0x7E)
//
void rfcx_android_serialize(char * buffer, android_serial_t * android) {
    char iv_str[6];
    char ic_str[6];
    char ov_str[6];
    char oc_str[6];
    char temp_str[6];
    char humid_str[6];

    //Convert from float to strings
    dtostrf((double)android->ads1015->input_voltage, 5, 2, iv_str);
    dtostrf((double)android->ads1015->input_current, 5, 2, ic_str);
    dtostrf((double)android->ads1015->output_voltage, 5, 2, ov_str);
    dtostrf((double)android->ads1015->output_current, 5, 2, oc_str);
    dtostrf((double)android->lm75->temperature, 5, 2, temp_str);
    dtostrf((double)android->hih6130->humidity, 5, 2, humid_str);

    //Serialize in buffer
    sprintf(buffer, "%lu,%s,%s,%s,%s,%s,%s,%lu,%lu,%lu,%lu\r\n",
        (unsigned long)ANDROID_BEGIN_FLAG,
        iv_str,
        ic_str,
        ov_str,
        oc_str,
        temp_str,
        humid_str,
        (unsigned long)android->hih6130->status,
        (unsigned long)android->batteries->battery_1.status,
        (unsigned long)android->batteries->battery_2.status,
        (unsigned long)ANDROID_END_FLAG);
}
