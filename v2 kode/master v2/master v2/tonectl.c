#include "globals.h"

#define NOTONE 0xFF
#define NOTIMEOUT 0x00
#define ANTALLTONER 10

#include "slavecom.h"
#include "uart.h"
#include "tonectl.h"

void starttone(uint8_t tone, uint8_t i, uint8_t kanal, uint8_t timeout);


uint8_t channelfinetune[16];
uint8_t channelcoarsetune[16];
uint8_t spillendekanal[ANTALLTONER];
uint8_t spillendetone[ANTALLTONER];
uint8_t spillendetimeout[ANTALLTONER];
uint32_t spillenderekkefoelge[ANTALLTONER];
uint32_t toneteller;

/////////////////////////////////////////////////////////
//
//  Tonerouting
//
//		Pr�v � spill/stopp toner, og hold basic oversikt.
//
//

void tonectl_init()
{
	for(uint8_t i = 0; i<16; i++)
	{
		channelfinetune[i] = 0;
		channelcoarsetune[i] = 0;
	}
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	{
		spillendekanal[i] = 255;
		spillendetone[i] = NOTONE;
		spillendetimeout[i] = NOTIMEOUT;
		spillenderekkefoelge[i] = 0;
	}
	toneteller = 0;
}

void tonectl_tikk()
{
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	{
		if(spillendetimeout[i] != NOTIMEOUT)
		{
			spillendetimeout[i]--;
			if(spillendetimeout[i] == 0)
			{
				stopp(i);
				spillendetimeout[i] = NOTIMEOUT;
				spillendetone[i] = NOTONE;
				spillendekanal[i] = 0;
			}
		}
	}
}


void spilltone(uint8_t tone, uint8_t kanal, uint8_t timeout)
{
	for(uint8_t i = 0; i<ANTALLTONER; i++)
		if(spillendetone[i] == NOTONE)
		{
			starttone(tone, i, kanal, timeout);
			return;
		}
	
	if(siste)
	{
		uint8_t laveste = 0;
		uint8_t laveste_timeout = 0xFF;
		for(uint8_t i=1; i<ANTALLTONER; i++)
		{								// overkj�r eldste tone hvis n�dvendig, evt toner som har minst igjen
			if(spillenderekkefoelge[i] < spillenderekkefoelge[laveste] && timeout == 0)
				laveste = i;
			if(spillendetimeout[i] != NOTIMEOUT && (laveste_timeout == 0xFF || spillendetimeout[i] < spillendetimeout[laveste_timeout]) && (timeout == NOTIMEOUT || timeout >= spillendetimeout[i]))
				laveste_timeout = i;
		}
		
		if(laveste_timeout == 0xFF && timeout == 0)
			starttone(tone, laveste, kanal, timeout);
		else if(laveste_timeout != 0xFF)
			starttone(tone, laveste_timeout, kanal, timeout);
		return;
	}
	else
	{
		send((kanal&0x0F)|COMMAND_NOTE_ON);				// Send videre hvis vi ikke kan spille med en gang
		send(tone&0x7F);
		send(timeout&0x7F);
	}
}

void starttone(uint8_t tone, uint8_t i, uint8_t kanal, uint8_t timeout)
{
	spillendetone[i] = tone;
	spillendekanal[i] = kanal;
	spillenderekkefoelge[i] = toneteller++;
	spillendetimeout[i] = timeout;
	
		
	while(tone>75)
	tone = tone - 12;
	while(tone<36)
	tone = tone + 12;
	tone = tone - 24;

	tone = (tone + channelcoarsetune[kanal])&0x7F;

	spill(tone, i);
	
	if(channelfinetune[kanal] != 0)
		stem(channelfinetune[kanal], i);
}

void stoppalletoner()
{
	send(COMMAND_ALL_OFF);
	stopp_alle();
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	{
		spillendetimeout[i] = NOTIMEOUT;
		spillendetone[i] = NOTONE;
		spillendekanal[i] = 255;
	}
}

void offsettone(uint8_t semitones, uint8_t finetune, uint8_t channel)
{
	send((channel&0x0F)|COMMAND_TUNE_CHANNEL);
	send(semitones&0x7F);
	send(finetune&0x7F);
	uint8_t old_coarse = channelcoarsetune[channel];
	uint8_t old_fine = channelfinetune[channel];
	channelcoarsetune[channel] = semitones;
	channelfinetune[channel] = finetune;
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	{
		if(spillendekanal[i] == channel && spillendetone[i] != NOTONE)
		{
			if(old_coarse != semitones || (finetune == 0 && old_fine != 0))
			{
				starttone(spillendetone[i], i, spillendekanal[i], spillendetimeout[i]);
			}
			else if(finetune != 0)
				stem(finetune, i);
		}
	}
}

void stoppalletoner_channel(uint8_t kanal)
{
	send((kanal&0x0F)|COMMAND_ALL_OFF_CHANNEL);
	for(uint8_t i = 0; i<ANTALLTONER; i++)
		if(spillendekanal[i] == kanal)
		{
			stopp(i);
			spillendetimeout[i] = NOTIMEOUT;
			spillendetone[i] = NOTONE;
			spillendekanal[i] = 0;
		}
}

void stopptone(uint8_t tone, uint8_t kanal)
{
	send((kanal&0x0F)|COMMAND_NOTE_OFF);
	send(tone&0x7F);
	for(uint8_t i = 0; i<ANTALLTONER; i++)
	if(spillendetone[i] == tone && spillendekanal[i] == kanal)
	{
		stopp(i);
		spillendetimeout[i] = NOTIMEOUT;
		spillendetone[i] = NOTONE;
		spillendekanal[i] = 0;
	}
}