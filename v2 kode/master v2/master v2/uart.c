#include "globals.h"

#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "uart.h"

#define  UART_RX	PIND0
#define  UART_TX	PIND1

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
	
	DDRD |= (1<<UART_RX);
	DDRD &= ~(1<<UART_TX);
	PORTD |= (1<<UART_TX);
	PORTD &= ~(1<<UART_RX);
	MCUCR &= ~(1<<PUD);
	
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
	UCSR0A |= 1<<U2X0;
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif

	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); /* 8-bit data */
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);   /* Enable RX and TX */


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
	return (hode != hale);
}

void regroup()
{
	cli();
	send(COMMAND_REGROUP);				// Relay
	_delay_ms(10);
	UCSR0B &= ~((1<<RXEN0)|(1<<TXEN0));	// Respond to caller and signal existence
	_delay_ms(190);
	siste = PIND&(1<<UART_TX);
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);   /* Enable RX and TX */
	sei();
}

uint8_t motta(void) {
	while(hode == hale){}
	uint8_t returverdi = serialbuffer[hale++];
	hale = hale%BUFFERLENGDE;
	return returverdi;
}
