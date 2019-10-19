#include "globals.h"

#define PRTB 1
#define PRTC 2
#define PRTD 3



#include <util/delay.h>
#include "slavecom.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////  Tonegenerator com
//////
//////		Init og Kontroll
//////
//////

/////////////////////////////////////////////////////////
//
// Init
//
//		Resetter tonegeneratorene en og en,
//		og setter ID på dem underveis.
//
//
void com_init(void)
{
	uint8_t resetpinner[] = {PORTB3, PORTB2, PORTB1, PORTB0, PORTD7, PORTD6, PORTD5, PORTD4, PORTD3, PORTD2, PORTC0, PORTC0, PORTC0, PORTC0, PORTC0, PORTC0};	// 11-8: reset tonegen 0-3, A5-A0: reset tonegen 4-10
	uint8_t resetporter[] = {PRTB,   PRTB,   PRTB,   PRTB,   PRTD,   PRTD,   PRTD,   PRTD,   PRTD,   PRTD,   PRTC,   PRTC,   PRTC,   PRTC,   PRTC,   PRTC};
	uint8_t resetdirbit[] = {DDB3,   DDB2,   DDB1,   DDB0,   DDD7,   DDD6,   DDD5,   DDD4,   DDD3,   DDD2,   DDC0,   DDC0,   DDC0,   DDC0,   DDC0,   DDC0};

	DDRB |= (1<<KLOKKEDIRBIT) | (1<<DATADIRBIT);
	PORTB |= (1<<KLOKKEPINNE) | (1<<DATAPINNE);
	
	for(int i=0; i<ANTALLTONER && i<16; i++)
	{
		if(resetporter[i] == PRTB)						// Reset en slave om gangen
		{
			DDRB |= 1<<resetdirbit[i];
			PORTB &= ~(1<<resetpinner[i]);
			_delay_ms(10);
			PORTB |= 1<<resetpinner[i];
		}
		if(resetporter[i] == PRTC)
		{
			DDRC |= 1<<resetdirbit[i];
			PORTC &= ~(1<<resetpinner[i]);
			_delay_ms(10);
			PORTC |= 1<<resetpinner[i];
		}
		if(resetporter[i] == PRTD)
		{
			DDRD |= 1<<resetdirbit[i];
			PORTD &= ~(1<<resetpinner[i]);
			_delay_ms(10);
			PORTD |= 1<<resetpinner[i];
		}

		_delay_ms(100);								// Send ID til slaven etter hver reset
		sendid(i);
		sendid(i);
		sendid(i);
	}
	_delay_ms(1500);
	aktiver(0xFF);
	tonegensend(0x16);
	deaktiver(0xFF);
}

void sendid(uint8_t id)
{
	id = (id&0x0F) + 1;
	tonegensend(0x55);
	_delay_ms(20);
	tonegensend(0x2B);
	_delay_ms(20);
	tonegensend(0xAA);
	_delay_ms(20);
	tonegensend(0xD4);
	_delay_ms(20);
	tonegensend((id<<4)|id);
	_delay_ms(20);
}

/////////////////////////////////////////////////////////
//
//  Kontroll
//
//		Hver tonegenerator må aktiveres før
//		tonen som spilles på den kan endres.
//
//
void aktiver(uint8_t id)
{
	if(id == 0xFF)
	{
		tonegensend(0x11);
	}
	else if(id < 16 && id < ANTALLTONER)
	{
		tonegensend(id+1);
	}
}

void deaktiver(uint8_t id)
{
	if(id == 0xFF)
	{
		tonegensend(0x91);
	}
	else if(id < 16 && id < ANTALLTONER)
	{
		tonegensend((id+1)|0x80);
	}
}

void spill(uint8_t tone, uint8_t id)
{
	if((tone&0x80) == 0 && id < ANTALLTONER)
	{
		deaktiver(0xFF);
		aktiver(id);
		tonegensend(tone);
	}
}

void stopp(uint8_t tone, uint8_t id)
{
	if((tone&0x80) == 0 && id < ANTALLTONER)
	{
		deaktiver(0xFF);
		aktiver(id);
		tonegensend(tone|0x80);
		aktiver(id);
		tonegensend(tone|0x80);
		aktiver(id);
		tonegensend(tone|0x80);
	}
}

void bassmode(uint8_t suboctave)
{
	if(suboctave == 0) tonegensend(0x92);
	if(suboctave == 1) tonegensend(0x13);
	if(suboctave == 2) tonegensend(0x93);
	if(suboctave == 3) tonegensend(0x14);
	if(suboctave == 4) tonegensend(0x94);
}

void tonegensend(uint8_t kommando)
{
	for (int i = 0; i < 8; i++)
	{
		if(kommando&(0x80>>i))
		PORTB |= 1<<DATAPINNE;
		else
		PORTB &= ~(1<<DATAPINNE);
		_delay_us(20);
		PORTB |= 1<<KLOKKEPINNE;
		_delay_us(20);
		PORTB &= ~(1<<KLOKKEPINNE);
		_delay_us(20);
	}
	PORTB |= 1<<DATAPINNE;
}
