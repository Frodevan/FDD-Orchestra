#include "globals.h"

/*
 * GccApplication1.c
 *
 * Created: 27.07.2017 19.30.56
 * Author : frodo
 */ 

#include "uart.h"
#include "tonectl.h"

uint8_t decaypattern1[1] = {5};
uint8_t decaypattern2[2] = {5, 7};
uint8_t decaypattern3[3] = {5, 7, 13};
uint8_t decaypattern4[4] = {5, 7, 13, 23};
uint8_t decaypattern5[5] = {5, 7, 13, 23, 37};
uint8_t decaypattern6[6] = {5, 7, 13, 23, 37, 55};
uint8_t decaypattern7[7] = {5, 7, 13, 23, 37, 55, 77};
uint8_t decaypattern8[8] = {5, 7, 13, 23, 37, 55, 77, 103};
uint8_t decaypattern9[9] = {5, 7, 13, 23, 37, 55, 77, 103, 127};
uint8_t decaypattern10[10] = {5, 7, 13, 23, 37, 44, 55, 77, 103, 127};
uint8_t decaypattern11[11] = {5, 7, 13, 18, 23, 37, 44, 55, 77, 103, 127};
uint8_t decaypattern12[12] = {5, 7, 13, 18, 23, 37, 44, 55, 77, 90, 103, 127};
uint8_t decaypattern13[13] = {5, 7, 13, 18, 23, 37, 44, 55, 77, 90, 103, 115, 127};
uint8_t decaypattern14[14] = {5, 6, 7, 13, 18, 23, 37, 44, 55, 77, 90, 103, 115, 127};
uint8_t decaypattern15[15] = {5, 6, 7, 13, 18, 23, 37, 44, 55, 66, 77, 90, 103, 115, 127};
uint8_t decaypattern16[16] = {5, 6, 7, 13, 18, 20, 23, 37, 44, 55, 66, 77, 90, 103, 115, 127};
uint8_t decaypattern17[17] = {5, 6, 7, 13, 18, 20, 23, 37, 44, 55, 60, 66, 77, 90, 103, 115, 127};
uint8_t decaypattern18[18] = {5, 6, 7, 13, 18, 20, 23, 30, 37, 44, 55, 60, 66, 77, 90, 103, 115, 127};
uint8_t decaypattern19[19] = {5, 6, 7, 13, 15, 18, 20, 23, 30, 37, 44, 55, 60, 66, 77, 90, 103, 115, 127};
uint8_t decaypattern20[20] = {3, 5, 6, 7, 13, 15, 18, 20, 23, 30, 37, 44, 55, 60, 66, 77, 90, 103, 115, 127};
uint8_t* decaypatterns[20] = {decaypattern1, decaypattern2, decaypattern3, decaypattern4, decaypattern5, decaypattern6, decaypattern7, decaypattern8, decaypattern9, decaypattern10, decaypattern11, decaypattern12, decaypattern13, decaypattern14, decaypattern15, decaypattern16, decaypattern17, decaypattern18, decaypattern19, decaypattern20};

uint8_t cc007_master_volume[16] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
uint8_t cc100_registered_parameter_lsb[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t cc101_registered_parameter_msb[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// 128ths of a semitone
uint8_t reg01_pitchbend_range_fine[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t reg02_pitchbend_range_semitones[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int lastbend[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int main(void)
{	
    uart_init();
	
	// Attack Encoder
	DDRD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7));
	DDRB &= ~((1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5));
	PORTD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7);
	PORTB |= (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5);
	MCUCR &= ~(1<<PUD);

	_delay_ms(500);
	stoppalletoner();
	_delay_ms(200);
	regroup();
	_delay_ms(300);
	
	for(int i=0; i<16; i++)
	{
		tune(i, 0, 128);
	}
	
	uint8_t kommando = 0;
	uint8_t kommando_data1 = 0xFF;
	
    while (1) 
    {
		uint8_t data = motta();
		switch(data&0xF0)
		{
			case 0x00:
			case 0x10:
			case 0x20:
			case 0x30:
			case 0x40:
			case 0x50:
			case 0x60:
			case 0x70:
			{
				if((kommando&0xF0) == 0x80)
				{
					stopptone(data, (kommando&0x0F));
					kommando = 0;
					kommando_data1 = 0xFF;
				}
				else if((kommando&0xF0) == 0x90)
				{
					if(kommando_data1 == 0xFF)
						kommando_data1 = data;
					else
					{
						if(data != 0)
						{
							int nummer_sustain = (cc007_master_volume[kommando&0x0F]*(data+25)+1930)/3860;
							int number_attack = 0;
							uint16_t settings = (((~PIND)>>2)&0x3F)|(((~PINB)&0x3F)<<6);

							if((kommando&0x0F) == 9)
								nummer_sustain = 0;

							switch(settings)
							{
								case 0x001:
								number_attack = 0;
								break;
								case 0x002:
								number_attack = (((data>>5)+1)>>1);
								break;
								case 0x004:
								number_attack = (((data>>4)+1)>>1);
								break;
								case 0x008:
								number_attack = (((data>>3)+1)>>1);
								break;
								case 0x010:
								number_attack = 1;
								break;
								case 0x020:
								number_attack = 2;
								break;
								case 0x040:
								number_attack = 3;
								break;
								case 0x080:
								number_attack = 5;
								break;
								case 0x100:
								number_attack = ((nummer_sustain+1)>>1);
								break;
								case 0x200:
								number_attack = nummer_sustain;
								break;
								case 0x400:
								number_attack = (nummer_sustain<<1);
								break;
								case 0x800:
								number_attack = (nummer_sustain<<2);
								break;
							}
							
							
								
							
							
							for(int i = 0; i<nummer_sustain; i++)
							{
								spilltone(kommando_data1, (kommando&0x0F), 0);
								if(kommando_data1>61)
								{
									spilltone(kommando_data1, (kommando&0x0F), 0);
								}
							}
							for(int i = 0; i<number_attack; i++)
							{
								spilltone(kommando_data1, (kommando&0x0F), decaypatterns[number_attack][i]);
								if(kommando_data1>61)
								{
									spilltone(kommando_data1, (kommando&0x0F), decaypatterns[number_attack][i]);
								}
							}
						}
						else
						{
							stopptone(kommando_data1, (kommando&0x0F));
						}
						kommando = 0;
						kommando_data1 = 0xFF;
					}
				}
				else if((kommando&0xF0) == 0xB0)
				{
					if(kommando_data1 == 0xFF)
						kommando_data1 = data;
					else
					{
						if(kommando_data1 == 6)
						{
							switch(cc100_registered_parameter_lsb[kommando&0x0F] | (cc101_registered_parameter_msb[kommando&0x0F]<<7))
							{
								case 0x00:
								{
									reg02_pitchbend_range_semitones[kommando&0x0F] = data;
									int bend = lastbend[kommando&0x0F];
									int range = ((int)reg01_pitchbend_range_fine[kommando&0x0F])+((int)reg02_pitchbend_range_semitones[kommando&0x0F]<<7); // range in 128ths of a semitone
									tune(kommando&0x0F, bend, range);
									break;
								}
								default:
									break;
							}
						}
						else if(kommando_data1 == 7)
						{
							cc007_master_volume[kommando&0x0F] = data;
						}
						else if(kommando_data1 == 38)
						{
							switch(cc100_registered_parameter_lsb[kommando&0x0F] | (cc101_registered_parameter_msb[kommando&0x0F]<<7))
							{
								case 0x00:
								{
									reg01_pitchbend_range_fine[kommando&0x0F] = (((((int)data)<<6)+25)/50);
									int bend = lastbend[kommando&0x0F];
									int range = ((int)reg01_pitchbend_range_fine[kommando&0x0F])+((int)reg02_pitchbend_range_semitones[kommando&0x0F]<<7); // range in 128ths of a semitone
									tune(kommando&0x0F, bend, range);
									break;
								}
								default:
									break;
							}
						}
						else if(kommando_data1 == 120 || kommando_data1 == 121 || kommando_data1 == 123)
						{
							if(kommando_data1 == 121)
							cc007_master_volume[kommando&0x0F] = 127;
							cc100_registered_parameter_lsb[kommando&0x0F] = 0;
							cc101_registered_parameter_msb[kommando&0x0F] = 0;
							reg01_pitchbend_range_fine[kommando&0x0F] = 0;
							reg02_pitchbend_range_semitones[kommando&0x0F] = 1;
							lastbend[kommando&0x0F];
							tune(kommando&0x0F, 0, 128);
						}
						kommando = 0;
						kommando_data1 = 0xFF;
					}
				}
				else if((kommando&0xF0) == 0xE0)
				{
					if(kommando_data1 == 0xFF)
						kommando_data1 = data;
					else
					{
						int bend = ((int)kommando_data1 | ((int)data<<7))-0x2000;
						lastbend[kommando&0x0F] = bend;
						int range = ((int)reg01_pitchbend_range_fine[kommando&0x0F])+((int)reg02_pitchbend_range_semitones[kommando&0x0F]<<7); // range in 128ths of a semitone
						tune(kommando&0x0F, bend, range);
					}
				}
				break;
			}
			case 0x80:
			case 0x90:
			case 0xE0:
			{
				kommando = data;
				kommando_data1 = 0xFF;
				break;
			}
			case 0xA0:
			{
				kommando = COMMAND_NONE;
				kommando_data1 = 0xFF;
				break;
			}
			case 0xB0:
			{
				kommando = data;
				kommando_data1 = 0xFF;
				break;
			}
			case 0xC0:
			case 0xD0:
			case 0xF0:
			{
				kommando = COMMAND_NONE;
				kommando_data1 = 0xFF;
				break;
			}
		}
    }
}