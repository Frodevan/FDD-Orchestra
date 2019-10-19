/*
 * lysoglydkasse.c
 *
 * Created: 28.01.2018 09.57.27
 * Author : medlem
 */ 

#define F_CPU 16000000UL		// Klokkehastighet

//#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "tone.h"
#include "com.h"

// Potentiometer = A1 (ikke tilkoblet enda)
// SPI p� ISP header (ikke i bruk enda)

int main(void)
{
	//
	// Setup
	//

	setup_lyd();
	setup_com();

	//
	// Do stuff
	//
    while (1) 
    {
		uint8_t command = 0;
		do
		{
			command = get_com();
		}while(command == 0);
		
		
		if(command<0x80)
		{
			playTone(command%12, command/12);
		}
		else if(command==0x80)
		{
			stopTone();
		}
		else
		{
			soundTone(command&0x7F);
		}
    }
}


