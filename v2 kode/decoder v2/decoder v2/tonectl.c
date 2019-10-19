#include "globals.h"

#include "uart.h"
#include "tonectl.h"

/////////////////////////////////////////////////////////
//
//  Tonerouting
//
//		Pr�v � spill/stopp toner, og hold basic oversikt.
//
//

void spilltone(uint8_t tone, uint8_t kanal, uint8_t timeout)
{
		send((kanal&0x0F)|COMMAND_NOTE_ON);				// Send videre hvis vi ikke kan spille med en gang
		send(tone&0x7F);
		send(timeout&0x7F);
}

void stoppalletoner()
{
	send(COMMAND_ALL_OFF);
}

void stoppalletoner_channel(uint8_t kanal)
{
	send((kanal&0x0F)|COMMAND_ALL_OFF_CHANNEL);
}

void stopptone(uint8_t tone, uint8_t kanal)
{
	send((kanal&0x0F)|COMMAND_NOTE_OFF);
	send(tone&0x7F);
}

void tune(uint8_t channel, int bend, int range)
{
	// Bend is where in the 2^14ths of the range
	// Range is number of 128ths of a semitone each way
	send((channel&0x0F)|COMMAND_TUNE_CHANNEL);
	int32_t final_bend = ((((int32_t)bend*(int32_t)range)>>13)+1)>>1;
	send((final_bend>>6)&0x7F);
	if(final_bend >= 0)
		send(final_bend&0x3F);
	else
		send((0-((0-final_bend)&0x3F))&0x7F);
}