#include "globals.h"

/*
 * GccApplication1.c
 *
 * Created: 27.07.2017 19.30.56
 * Author : frodo
 */ 

#include "tonectl.h"
#include "slavecom.h"
#include "uart.h"
#include "tunelib.h"

int main(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD |= 0xFF&(~(1<<DDD0 | 1<<DDD1));		// Keep serial port alive, otherwise all output for now.
	
    uart_init();
    com_init();
	
	aktiver(0xFF);
	tune_init();
	deaktiver(0xFF);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////
	//////  Main Engine
	//////
	//////		1. Motta MIDI note on/note off
	//////		2. Sjekk om den kan spilles her
	//////		3. Send videre hvis ikke
	//////		4. Rinse and Repeat
	//////
	//////		Hvis det er høy velocity kan man vurdere å
	//////		starte flere diskettstasjoner med samme tone.
	//////		Also, holder rede på kanal og sånt pga note-off.
	//////
	//////
	
	uint8_t sekvensteller = 0;
	uint8_t innkommando = 0;
	uint8_t inntone = 0;
	
    while (1) 
    {
		uint8_t data = motta();
		//send(data);
		if(data&0x80)			// Start å samle data hvis kommando
		{
			innkommando = data;
			sekvensteller = 1;
		}
		else if(sekvensteller == 1)		// Tonenummer
		{
			inntone = data;
			sekvensteller = 2;
		}
		else if(sekvensteller == 2)		// Velocity; alt mottatt så spill/stopp tone direkte
		{
			if (((innkommando&0xF0) == 0x80) || ((innkommando&0xF0) == 0x90 && data == 0))
			{
				stopptone(innkommando&0x0F, inntone, data);
				send(innkommando);		// NoteOff går til alle
				send(inntone);
				send(data);
			}
			else if ((innkommando&0xF0) == 0x90)
				spilltone((innkommando&0x0F), inntone, data);
			else if ((innkommando&0xF0) == 0xB0 && (inntone == 120 || inntone == 121 || inntone == 120) && data == 0)
			{
				stoppalletoner(innkommando&0x0F);
				send(innkommando);		// NoteOff går til alle
				send(inntone);
				send(data);
			}
			sekvensteller = 0;
			innkommando = 0;
		}
    }
	
	return -1;
}

