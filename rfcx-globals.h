/**********************************************************
*	RFCx Microcontroller Software - Global Definitions
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#ifndef RFCX_GLOBALS_H
#define RFCX_GLOBALS_H

#ifdef ARDUINO
    #define F_CPU 16000000UL
#else
    #define F_CPU 8000000UL     //Clock Speed (Hz)
#endif

#endif//RFCX_GLOBALS_H
