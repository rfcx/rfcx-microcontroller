/**********************************************************
*	RFCx Microcontroller Software - Battery
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson      (gibsjose@mail.gvsu.edu)
*	Jesse Millwood  (millwooj@mail.gvsu.edu)
*
*	08 July 2015
*	02 November 2015
*
*   www.rfcx.org
**********************************************************/

#include "rfcx-battery.h"

//Declare pins as inputs/outputs
void rfcx_batteries_init(batteries_t * batteries) {
    // Set up battery structure
    batteries->battery_1.stat1_prt = BAT_1_STAT_1_PORT;
    batteries->battery_1.stat1_ddr = BAT_1_STAT_1_DDR;
    batteries->battery_1.stat1_in  = BAT_1_STAT_1_INPUT_PIN;
    batteries->battery_1.stat1_out = BAT_1_STAT_1_OUTPUT_PIN;
    batteries->battery_1.stat2_prt = BAT_1_STAT_2_PORT;
    batteries->battery_1.stat2_ddr = BAT_1_STAT_2_DDR;
    batteries->battery_1.stat2_in  = BAT_1_STAT_2_INPUT_PIN;
    batteries->battery_1.stat2_out = BAT_1_STAT_2_OUTPUT_PIN;

    batteries->battery_2.stat1_prt = BAT_2_STAT_1_PORT;
    batteries->battery_2.stat1_ddr = BAT_2_STAT_1_DDR;
    batteries->battery_2.stat1_in  = BAT_2_STAT_1_INPUT_PIN;
    batteries->battery_2.stat1_out = BAT_2_STAT_1_OUTPUT_PIN;
    batteries->battery_2.stat2_prt = BAT_2_STAT_2_PORT;
    batteries->battery_2.stat2_ddr = BAT_2_STAT_2_DDR;
    batteries->battery_2.stat2_in  = BAT_2_STAT_2_INPUT_PIN;
    batteries->battery_2.stat2_out = BAT_2_STAT_2_OUTPUT_PIN;


    // Set Data Direction to Inputs
    batteries->battery_1.stat1_ddr &= ~_BV(batteries->battery_1.stat1_in);
    batteries->battery_1.stat2_ddr &= ~_BV(batteries->battery_1.stat2_in);
    batteries->battery_2.stat1_ddr &= ~_BV(batteries->battery_2.stat1_in);
    batteries->battery_2.stat2_ddr &= ~_BV(batteries->battery_2.stat2_in);

    // Set Data Direction to Outputs
    batteries->battery_1.stat1_ddr |= _BV(batteries->battery_1.stat1_out);
    batteries->battery_1.stat2_ddr |= _BV(batteries->battery_1.stat2_out);
    batteries->battery_2.stat1_ddr |= _BV(batteries->battery_2.stat1_out);
    batteries->battery_2.stat2_ddr |= _BV(batteries->battery_2.stat2_out);
    //Initialize outputs low
    batteries->battery_1.stat1_ddr &= ~_BV(batteries->battery_1.stat1_out);
    batteries->battery_1.stat2_ddr &= ~_BV(batteries->battery_1.stat2_out);
    batteries->battery_2.stat1_ddr &= ~_BV(batteries->battery_2.stat1_out);
    batteries->battery_2.stat2_ddr &= ~_BV(batteries->battery_2.stat2_out);
}

void rfcx_batteries_data_init(batteries_t * batteries) {
    batteries->battery_1.status = BAT_STATUS_ERROR;
    batteries->battery_2.status = BAT_STATUS_ERROR;
}

void rfcx_batteries_status(batteries_t * batteries) {
    batteries->battery_1.status = rfcx_battery_status(BATTERY_1);
    batteries->battery_2.status = rfcx_battery_status(BATTERY_2);
}

unsigned char rfcx_battery_status(unsigned char id) {
    bool stat1_first = 0x00;
    bool stat1_second = 0x00;
    bool stat2_first = 0x00;
    bool stat2_second = 0x00;

    unsigned char stat1_in_pin = 0x00;
    unsigned char stat1_out_pin = 0x00;
    unsigned char stat2_in_pin = 0x00;
    unsigned char stat2_out_pin = 0x00;
    unsigned char stat1_prt = 0x00;
    unsigned char stat2_prt = 0x00;
    unsigned char stat1_pin = 0x00;
    unsigned char stat2_pin = 0x00;
    unsigned char stat1_mode = 0x00;
    unsigned char stat2_mode = 0x00;

    //Battery 1
    if(id == BATTERY_1) {
        stat1_in_pin = BAT_1_STAT_1_INPUT_PIN;
        stat2_in_pin = BAT_1_STAT_2_INPUT_PIN;
        stat1_out_pin = BAT_1_STAT_1_OUTPUT_PIN;
        stat2_out_pin = BAT_1_STAT_2_OUTPUT_PIN;
        stat1_prt = BAT_1_STAT_1_PORT;
        stat2_prt = BAT_1_STAT_2_PORT;
        stat1_pin = BAT_1_STAT_1_PIN;
        stat2_pin = BAT_1_STAT_2_PIN;
    }

    //Battery 2
    else if(id == BATTERY_2) {
        stat1_in_pin = BAT_2_STAT_1_INPUT_PIN;
        stat2_in_pin = BAT_2_STAT_2_INPUT_PIN;
        stat1_out_pin = BAT_2_STAT_1_OUTPUT_PIN;
        stat2_out_pin = BAT_2_STAT_2_OUTPUT_PIN;
        stat1_prt = BAT_2_STAT_1_PORT;
        stat2_prt = BAT_2_STAT_2_PORT;
        stat1_pin = BAT_2_STAT_1_PIN;
        stat2_pin = BAT_2_STAT_2_PIN;
    }

    //This should never happen...
    else {
        return BAT_STATUS_ERROR;
    }

    //Clear output pin LOW
    stat1_prt &= ~_BV(stat1_out_pin);

    //Read input pin initially
    stat1_first = (bool)(stat1_pin & _BV(stat1_in_pin));
    stat2_first = (bool)(stat2_pin & _BV(stat2_in_pin));

    //Set output pin HIGH
    stat1_prt |= _BV(stat1_out_pin);
    stat2_prt |= _BV(stat2_out_pin);

    //Read again
    stat1_second = (bool)(stat1_pin & _BV(stat1_in_pin));
    stat2_second = (bool)(stat2_pin & _BV(stat2_in_pin));

    //Clear output again
    stat1_prt &= ~_BV(stat1_out_pin);
    stat2_prt &= ~_BV(stat2_out_pin);

    // input pin goes from low to high: High Z mode
    if((!stat1_first) && stat1_second) {
        stat1_mode = HIGHZ;
    }
    // Remained high both times
    else if(stat1_first && stat1_second) {
        stat1_mode = HIGH;
    }
    // Remained low both times
    else if(!(stat1_first && stat1_second)) {
        stat1_mode = LOW;
    }
    // should never reach this
    else {
        return BAT_STATUS_ERROR;
    }
    // input pin goes from low to high: High Z mode
    if((!stat2_first) && stat2_second) {
        stat2_mode = HIGHZ;
    }
    // Remained high both times
    else if(stat2_first && stat2_second) {
        stat2_mode = HIGH;
    }
    // Remained low both times
    else if(!(stat2_first && stat2_second)) {
        stat2_mode = LOW;
    }
    // should never reach this
    else {
        return BAT_STATUS_ERROR;
    }

    // Determine charge state
    if((stat1_mode == HIGHZ) && (stat2_mode == HIGHZ)) {
        return SHUTDOWN_MODE;
    }
    else if((stat1_mode == LOW) && (stat2_mode == HIGHZ )) {
        return CHARGING;
    }
    else if((stat1_mode == HIGHZ) && (stat2_mode == LOW)) {
        return CHARGE_COMPLETE;
    }
    else if((stat1_mode == LOW) && (stat2_mode == LOW)) {
        return FAULT;
    }

    //Shouldn't ever get here
    return BAT_STATUS_ERROR;
}

void rfcx_battery_status_string(char * str, unsigned char status) {
    switch(status) {
        case CHARGING:
            sprintf(str, "Charging");
            break;
        case CHARGE_COMPLETE:
            sprintf(str, "Charge Complete");
            break;
        case FAULT:
            sprintf(str, "Sleep Mode");
            break;
        //@TODO How to identify between Sleep Mode and Temp Fault?
        // case TEMPERATURE_FAULT:
        // 	sprintf(str, "Temperature Fault");
        // 	break;
        case BAT_STATUS_ERROR:
            sprintf(str, "ERROR");
            break;
        default:
            sprintf(str, "UNKNOWN");
            break;
    }
}
