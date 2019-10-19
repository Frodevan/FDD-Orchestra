#include "globals.h"

#define NOTONE 0xFF

#include "slavecom.h"
#include "uart.h"
#include "tonectl.h"

uint8_t spillendekanal[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
uint8_t spillendetone[] = {NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE, NOTONE};
uint32_t spillenderekkefoelge[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t toneteller = 0;

/////////////////////////////////////////////////////////
//
//  Tonerouting
//
//		Prøv å spill/stopp toner, og hold basic oversikt.
//
//

void spilltone(uint8_t kanal, uint8_t tone, uint8_t velocity)
{
	uint8_t antall = 1;
	
	#if FOERSTE == true
	#if VELOCITYSENSE == 4
		if(velocity<60)
			antall = 1;
		else if(velocity<80)
			antall = 2;
		else if(velocity<105)
			antall = 3;
		else
			antall = 4;
	#elif VELOCITYSENSE == 3
		if(velocity<80)
			antall = 1;
		else if(velocity<100)
			antall = 2;
		else
			antall = 3;
	#elif VELOCITYSENSE == 2
		if(velocity<90)
			antall = 1;
		else
			antall = 2;
	#endif
	
	if(tone > 73)
	antall = antall*4;
	else if(tone > 64)
	antall = antall*3;
	else if(tone > 48)
	antall = antall*2;
	
	#endif
	
	for(uint8_t i = 0; i<ANTALLTONER && antall > 0; i++)
	if(spillendetone[i] == NOTONE)
	{
		starttone(i, tone, kanal);
		antall--;
	}
	
	while(antall>0)
	{
		#if SISTE == true
		uint8_t laveste = 0;
		for(uint8_t i=1; i<ANTALLTONER; i++)		// overkjør eldste tone hvis nødvendig
		if(spillenderekkefoelge[i] < spillenderekkefoelge[laveste])
		laveste = i;

		starttone(laveste, tone, kanal);
		antall--;
		#else
		send(kanal|0x90);				// Send videre hvis vi ikke kan spille med en gang
		send(tone);
		send(velocity);
		antall--;
		#endif
	}
}

void starttone(uint8_t i, uint8_t tone, uint8_t kanal)
{
	tone = tone + 12;
	while(tone>89)
	tone = tone - 12;
	while(tone<LOWESTTONE)
	tone = tone + 12;
	
	spill(tone, i);
	spillendetone[i] = tone;
	spillendekanal[i] = kanal;
	spillenderekkefoelge[i] = toneteller++;
}

void stopptone(uint8_t kanal, uint8_t tone, uint8_t velocity)
{
	tone = tone + 12;
	while(tone>89)
	tone = tone - 12;
	while(tone<LOWESTTONE)
	tone = tone + 12;
	
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	if(spillendetone[i] == tone && spillendekanal[i] == kanal)
	{
		stopp(tone, i);
		spillendetone[i] = NOTONE;
		spillendekanal[i] = 0;
	}
}

void stoppalletoner(uint8_t kanal)
{
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	if(spillendekanal[i] == kanal)
	{
		stopp(spillendetone[i], i);
		spillendetone[i] = NOTONE;
		spillendekanal[i] = 255;
	}
}

