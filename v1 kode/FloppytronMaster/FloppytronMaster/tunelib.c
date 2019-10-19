/*
 * tunelib.c
 *
 * Created: 07.08.2018 23:13:48
 *  Author: medlem
 */ 

#include "globals.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "tunelib.h"

void tune_init()
{
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01);
	OCR0A = 99;		// Toggle every 20KHz for 10KHz tone.

	start_tuning();

	_delay_ms(20000);
	stop_tuning();
}

void start_tuning()
{
	TIMSK0 = (1<<OCIE0A);
	_delay_ms(500);
	tune_send(0x17);
}

void stop_tuning()
{
	tune_send(0x97);
	tune_send(0x97);
	tune_send(0x97);
	_delay_ms(500);
	TIMSK0 = 0;
}

ISR(TIMER0_COMPA_vect)
{
	PORTB ^= (1<<DATAPINNE);
}

void tune_send(uint8_t kommando)
{
	for(int i=0; i<8; i++)
	{
		if(kommando&(0x80>>i))
		{
			while (PORTB&(1<<DATAPINNE));
			while (!(PORTB&(1<<DATAPINNE)));	// Wait for end of 0
			_delay_us(8);
			PORTB |= 1<<KLOKKEPINNE;
			_delay_us(9);
			PORTB &= ~(1<<KLOKKEPINNE);
		}
		else
		{
			while (!(PORTB&(1<<DATAPINNE)));
			while (PORTB&(1<<DATAPINNE));	// Wait for end of 1
			_delay_us(8);
			PORTB |= 1<<KLOKKEPINNE;
			_delay_us(9);
			PORTB &= ~(1<<KLOKKEPINNE);
		}
		
	}
}