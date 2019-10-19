#include "globals.h"

#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "uart.h"


volatile uint8_t serialbuffer[BUFFERLENGDE];
volatile uint16_t hode = 0;
volatile uint16_t hale = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////  Serieportstuff
//////
//////    Blablabla... https://appelsiini.net/2011/simple-usart-with-avr-libc/
//////
//////
void uart_init(void) {
	
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
	UCSR0A |= 1<<U2X0;
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif

	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); /* 8-bit data */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);   /* Enable RX and TX */


	UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
	sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

ISR(USART_RX_vect)
{
	cli();
	serialbuffer[hode++] = UDR0;
	hode = hode%BUFFERLENGDE;
	sei();
}

void send(uint8_t c) {
	UDR0 = c;
	while(~(UCSR0A | ~(1<<UDRE0))){};		/* Wait until transmission ready. */
}

uint8_t tilgjengelig()
{
	return !(hode == hale);
}

void regroup()
{
	send(COMMAND_REGROUP);				// Relay
}

uint8_t motta(void) {
	while(hode == hale){}
	uint8_t returverdi = serialbuffer[hale++];
	hale = hale%BUFFERLENGDE;
	return returverdi;
}
