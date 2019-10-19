#include "globals.h"

/*
 * GccApplication1.c
 *
 * Created: 27.07.2017 19.30.56
 * Author : frodo
 */ 

uint8_t siste = true;

#include "slavecom.h"
#include "uart.h"
#include "tonectl.h"

//
// Channel dependent commands
//		COMMAND_NOTE_ON, tone, timeout		Note on (tone: MIDI tone, timeout: number of 5ms ticks to hold tone)
//		COMMAND_NOTE_OFF, tone				Note off (tone: MIDI tone)
//		COMMAND_TUNE_CHANNEL, coarse, fine	Tune channel (coarse: semitones offset [-64 -> 63], fine: 128ths of a semitone [-64 -> 63])
//		COMMAND_ALL_OFF_CHANNEL				Silence channel
//
// Channel independent commands
//		COMMAND_REGROUP						Autodetect if last tower in chain
//		COMMAND_ALL_OFF						Silence everything
//

int main(void)
{	
    uart_init();
	tonectl_init();
	com_init();
	
	uint8_t kommando = COMMAND_NONE;
	uint8_t kommando_data = 0xFF;
	
    while (1)
    {	
		if(tilgjengelig())
		{
			uint8_t data = motta();
			
			if(!(data&0x80) && kommando != COMMAND_NONE)
			{
				//
				// Receiving data for an incoming command
				//
				if((kommando&0xF0) == COMMAND_NOTE_ON)
				{
					if(kommando_data == 0xFF)
						kommando_data = data;
					else
					{
						spilltone(kommando_data, kommando&0x0F, data);
						kommando = COMMAND_NONE;
						kommando_data = 0xFF;
					}
				}
				else if((kommando&0xF0) == COMMAND_TUNE_CHANNEL)
				{
					if(kommando_data == 0xFF)
						kommando_data = data;
					else
					{
						offsettone(kommando_data, data, kommando&0x0F);
						kommando = COMMAND_NONE;
					}
				}
				else if((kommando&0xF0) == COMMAND_NOTE_OFF)
				{
					stopptone(data, kommando&0x0F);
					kommando = COMMAND_NONE;
				}
			}
			else if((data&0xF0) == COMMAND_NOTE_ON || (data&0xF0) == COMMAND_NOTE_OFF || (data&0xF0) == COMMAND_TUNE_CHANNEL)
			{
				// Receiving command
				kommando = data;
				kommando_data = 0xFF;
			}
			else if((data&0xF0) == COMMAND_ALL_OFF_CHANNEL)
			{
				// Receiving stop all tones on channel command
				stoppalletoner_channel((data&0x0F));
				kommando = COMMAND_NONE;
			}
			else if((data&0xF0) == COMMAND_SPECIAL)
			{
				// Receiving special command
				if(data == COMMAND_ALL_OFF)
				{
					stoppalletoner();
				}
				else if(data == COMMAND_REGROUP)
				{
					regroup();
				}
				kommando = COMMAND_NONE;
			}
		}
		else
		{
			tonectl_tikk();
			_delay_ms(5);
		}
    }
}