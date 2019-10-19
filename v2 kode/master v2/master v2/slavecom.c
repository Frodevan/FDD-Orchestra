#include "globals.h"
#include "slavecom.h"

#define DRIVE0	PIND2
#define DRIVE1	PINC5
#define DRIVE2	PINC4
#define DRIVE3	PINC3
#define DRIVE4	PINC2
#define DRIVE5	PIND3
#define DRIVE6	PIND4
#define DRIVE7	PIND5
#define DRIVE8	PINC0
#define DRIVE9	PINC1

#define COM_CLOCK	PINB5
#define COM_DATA	PINB3

#define GLOBAL_INT	PINB2

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////  Tonegenerator com
//////
//////		Init og Kontroll
//////
//////

void tonegensend(uint8_t kommando);
void aktiver(uint8_t id);
void deaktiver();

/////////////////////////////////////////////////////////
//
// Init
//
//		Resetter tonegeneratorene en og en,
//		og setter ID p� dem underveis.
//
//
void com_init(void)
{

	DDRB |= (1<<COM_DATA)|(1<<COM_CLOCK);
	PORTB |= (1<<COM_DATA);
	PORTB &= ~(1<<COM_CLOCK);
	
	DDRC |= (1<<DRIVE1)|(1<<DRIVE2)|(1<<DRIVE3)|(1<<DRIVE4)|(1<<DRIVE8)|(1<<DRIVE9);
	PORTC |= (1<<DRIVE1)|(1<<DRIVE2)|(1<<DRIVE3)|(1<<DRIVE4)|(1<<DRIVE8)|(1<<DRIVE9);
	
	DDRD |= (1<<DRIVE0)|(1<<DRIVE5)|(1<<DRIVE6)|(1<<DRIVE7);
	PORTD |= (1<<DRIVE0)|(1<<DRIVE5)|(1<<DRIVE6)|(1<<DRIVE7);
	
	DDRB |= (1<<GLOBAL_INT);
	PORTB |= (1<<GLOBAL_INT);
	_delay_ms(10);
	stopp_alle();
}

void spill(uint8_t tone, uint8_t id)
{
	aktiver(id);
	tonegensend(tone&0x7F);
	deaktiver();
}

void stopp(uint8_t id)
{
	aktiver(id);
	tonegensend(0x80);
	deaktiver();
}

void stem(uint8_t tune, uint8_t id)
{
	aktiver(id);
	tonegensend(0x80|tune);
	deaktiver();
}

void stopp_alle()
{
	deaktiver();
	PORTB &= ~(1<<GLOBAL_INT);
	_delay_ms(100);
	PORTB |= (1<<GLOBAL_INT);
}

void aktiver(uint8_t id)
{
	switch(id)
	{
		case 0:
		{
			PORTD &= ~(1<<DRIVE0);
			break;
		}
		case 1:
		{
			PORTC &= ~(1<<DRIVE1);
			break;
		}
		case 2:
		{
			PORTC &= ~(1<<DRIVE2);
			break;
		}
		case 3:
		{
			PORTC &= ~(1<<DRIVE3);
			break;
		}
		case 4:
		{
			PORTC &= ~(1<<DRIVE4);
			break;
		}
		case 5:
		{
			PORTD &= ~(1<<DRIVE5);
			break;
		}
		case 6:
		{
			PORTD &= ~(1<<DRIVE6);
			break;
		}
		case 7:
		{
			PORTD &= ~(1<<DRIVE7);
			break;
		}
		case 8:
		{
			PORTC &= ~(1<<DRIVE8);
			break;
		}
		case 9:
		{
			PORTC &= ~(1<<DRIVE9);
			break;
		}
	}
	_delay_us(5);
}

void deaktiver()
{
	_delay_us(5);
	PORTC |= (1<<DRIVE1)|(1<<DRIVE2)|(1<<DRIVE3)|(1<<DRIVE4)|(1<<DRIVE8)|(1<<DRIVE9);
	PORTD |= (1<<DRIVE0)|(1<<DRIVE5)|(1<<DRIVE6)|(1<<DRIVE7);
	_delay_us(5);
}

void tonegensend(uint8_t kommando)
{
	for (int i = 0; i < 8; i++)
	{
		if(kommando&(0x80>>i))
			PORTB |= 1<<COM_DATA;
		else
			PORTB &= ~(1<<COM_DATA);
		_delay_us(1);
		PORTB |= 1<<COM_CLOCK;
		_delay_us(1);
		PORTB &= ~(1<<COM_CLOCK);
	}
	PORTB |= 1<<COM_DATA;
}
