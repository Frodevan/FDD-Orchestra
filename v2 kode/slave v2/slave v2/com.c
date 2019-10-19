/*
 * com.c
 *
 * Created: 30.07.2019 02:49:57
 *  Author: medlem
 */ 

#define F_CPU 16000000UL  // 16 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define COM_DATA	PINA6
#define COM_CLOCK	PINA4
#define COM_ENABLE	PINB1			// Active low
#define COM_EN_INT	PCINT9

void reset_com();

uint8_t cominn;

void setup_com()
{
	cli();
	DDRA &= ~((1<<COM_DATA)|(1<<COM_CLOCK));
	DDRB &= ~(1<<COM_ENABLE);
	PCMSK1 |= (1<<COM_EN_INT);
	GIMSK |= (1<<PCIE1);
	
	cominn = 0;
	
	reset_com();
	USICR &= ~((1<<USICS0) | (1<<USICLK));											// Set input only, rising edge, and enable interrupt
	USICR |= (1<<USICS1) | (1<<USIOIE);											// Set input only, rising edge, and enable interrupt
	sei();
}

void reset_com()
{
	USIDR = 0;																	// Reset data
	USISR &= ~((1<<USICNT0)|(1<<USICNT1)|(1<<USICNT2)|(1<<USICNT3));			// Reset input clock
	USISR |= (1<<USIOIF);														// & interrupt
}

uint8_t get_com()
{
	cli();
	uint8_t ret = cominn;
	cominn = 0;
	sei();
	return ret;
}

ISR(USI_OVF_vect)
{
	if(!(PINB&(1<<COM_ENABLE)))
	{
		cominn = USIDR;
	}
}

ISR(PCINT1_vect)
{
	reset_com();
}