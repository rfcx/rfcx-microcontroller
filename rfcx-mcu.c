#include "rfcx-mcu.h"
int readFlag = 0;

int delay_ms(unsigned long int microseconds)
{
//	return; // bypass code and return for debug
	volatile unsigned cycles = microseconds/64;
	TCCR2A	= 0x00;
	TCCR2B =_BV(CS22) | _BV(CS21) | _BV(CS20); // set timer to use internal clock with 1:1024 pre-scale
	if(microseconds < 16321)
	{
		TCNT2 = 0;
		OCR2A = cycles;
		TIFR2 = _BV(OCF2A); // Set to clear bit 1
		while ((TIFR2 & _BV(OCF2A)) == 0); // NULL
		return(0);
	}
	else
	{
		TCNT2 = 0;
		OCR2A = 255;
		TIFR2 = _BV(OCF2A); // Set to clear bit 1
		while ((TIFR2 & _BV(OCF2A)) == 0); // NULL
		return(delay_ms(microseconds - 16320));
	}
}


int main(void)
{
  double currTemp, currInV, currInC, currOutV, currOutC = 0.0;
  char message[100];
  cli();
  ATMegaInit();
	USART_Init(MYUBRR);
  //ADCInit();
	USART_Send_string("Initializing Temp Sensor\n");
  //TempSensorInit();
	USART_Send_string("Done Initializing Temp Sensor\n");
  sei();

  ReadTemp();

  /*while(1)
  {
    USART_Send_string("DELAY\n");
    delay_ms(5000000);
    //if(readFlag)
    //{
      //currTemp = ((double)ReadTemp());
      //currInV = ReadInputVoltage();
      //currInC = ReadInputCurrent();
      //currOutV = ReadOutputVoltage();
      //currOutC = ReadOutputCurrent();
      //sprintf(message, "Temp: %f\nInput V: %f\nInput C: %f\nOutput V: %f\nOutput C: %f\n",
      //       currTemp, currInV, currInC, currOutV, currOutC);
      sprintf(message, "Temp: %f\n", currTemp);
			USART_Send_string(message);
      //readFlag = 0;
    //}

  }*/
  return 0;
}
