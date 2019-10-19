/*
 * Floppytron.c
 *
 * Created: 07.06.2017 16.20.30
 * Author : frodevan
 */ 

#define F_CPU 8000000UL		// Klokkehastighet
#define i2chn 0x40			// I2C Channel
#define false 0
#define true ~false

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void setup_com();
ISR(USI_OVF_vect);
void setup_timer();
ISR(TIMER1_OVF_vect);
void setup_io();
void turnon();
void turnoff();
void pulse();
void center();
void delay();
void shortdelay();
uint16_t kaltest();

volatile uint_fast8_t timerlowerbyte = 0xFF;
volatile uint_fast8_t timerupperbyte = 0xFF;
volatile uint_fast8_t stepnow = false;
uint_fast8_t stepdist = 80;						 // Low power of 2 for loud bassy tone. High number below 80 for clean tone.

uint_fast16_t scale[] = {64793,
	61156, 57724, 54484, 51426, 48540, 45815, 43244, 40817, 38526, 36364, 34323, 32396,
	30578, 28862, 27242, 25713, 24270, 22908, 21622, 20408, 19263, 18182, 17161, 16198,
	15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204, 9631, 9091, 8581, 8099,
	7645, 7215, 6810, 6428, 6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,
	3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551, 2408, 2273, 2145, 2025,
	1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,
	956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506,
	478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253,
	239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127,
	119, 113, 106, 100};
	
uint_fast8_t midiscale[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, // 36
	13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // 48
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, // 60
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 72
	49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, // 84
	61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, // 96
	73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, // 108
	85, 86, 87, 88, 89, 90, 91, 92};				// 120

uint8_t miditone = 0;
uint8_t kill = true;
volatile uint_fast8_t cominn = 0;			// 0 = Ingenting nytt, 128 = Rekalibrer stasjon
uint8_t active = false;
uint8_t jam = false;
uint8_t outdir = true;
volatile uint8_t tonegenid = 0;

int main(void) {
	cli();
	CLKPR = 0x80;
	CLKPR = 0x00;
	sei();
	
	setup_io();
	setup_timer();
	setup_com();
	
	//
	// For � sette ID:
	//
	//   1. Reset brikke med /RESET pinna.
	//   2. Vent et par ms.
	//   3. Send* bytesekvens "0x55, 0x2B, 0xAA, 0xD4, (ID<<4)|ID" hvor ID er et tall mellom (inklusiv) 1 og 16.
	//   4. Gjenta Steg 3 et par ganger.
	//   5. Vent til det har g�tt godt og vel ett helt sekund, hels to, fra siste fors�k**, begynn � sette ID p� den nesten lydgeneratoren i mellomtiden.
	//   6a. Hvis alt har g�tt bra, kan du starte lydgeneratoren ved � sende* byte "ID" for � aktivere skriving til lydgeneratoren, etterfulgt av en MIDI-tone mellom 0x12 og 0x7F.
	//   6b. For � snakke med en annen lydgenerator, send 0x80|ID for � deaktivere skriving til denne lydgeneratoren.
	//
	//   *  Bytes m� ikke sendes for raskt! Det b�r v�re minst en 15-20mS mellomrom mellom hver byte.
	//   ** F�rst ca. ett sekund fra mottatt ID begynner lydgeneratoren � tolke inkommende data som kommandoer.
	//
	uint8_t sequencecount = 0;
	while(tonegenid == 0)
	{
		cli();
		uint8_t datainn = cominn;
		cominn = 0;
		sei();
		
		if(datainn != 0)
		{
			if(sequencecount == 0 && datainn == 0x55) sequencecount++;
			else if(sequencecount == 1 && datainn == 0x2B) sequencecount++;
			else if(sequencecount == 2 && datainn == 0xAA) sequencecount++;
			else if(sequencecount == 3 && datainn == 0xD4) sequencecount++;
			else if(sequencecount == 4 && (datainn&0x0F) == (datainn&0xF0)>>4) tonegenid = datainn&0x0F;
			else sequencecount = 0;
		}
	}
	
	turnon();
	center();
	turnoff();
	delay(1000);
	cominn = 0;
	
	//
	// Kommandoer:
	//
	//  0x00		Ingenting skjer
	//  0x01 - 0x10	Aktiver skriving til spesifik tonegenerator med ID nummer 1-16
	//	0x11		Aktiver skriving til alle tonegeneratorene
	//  0x12		Normal Mode 40 track
	//	0x13		Sub-Bass Loud mode /1
	//	0x14		Sub-Bass Loud mode /4 
	//	0x15		Jammin mode p� 
	//	0x16		Jammin retning utover 
	//	0x17		Kalirbrering til 20KHz start 
	//
	//  0x18 - 0x7F	Spill MIDI-tone nr.
	//
	//  0x80		Rekalibrer og stopp tonegenerator
	//  0x81 - 0x90	Deaktiver skriving til spesifik tonegenerator med ID nummer 1-16
	//  0x91		Deaktiver skriving til alle tonegeneratorene
	//	0x92		Normal Mode 80 track
	//	0x93		Sub-Bass Loud Mode/2
	//	0x94		Sub-Bass Loud Mode/8
	//	0x95		Jammin mode av 
	//	0x96		Jammin retning innover 
	//	0x97		Kalbrering slutt 
	//  0x98 - 0xFF	Stopp tonegenerator, men bare hvis MIDI-tone nr. (bit 0-6) spilles
	//
	while(true)
	{
		cli();
		uint8_t miditoneinn = cominn;
		cominn = 0;
		sei();
		
		if(miditoneinn != 0)																					/// Vent p� data
		{
			if((miditoneinn < 0x11 && tonegenid == miditoneinn) || miditoneinn == 0x11)							// Case 1: Aktiver skriving av toneinformasjon
			{
				active = true;
			}
			else if(miditoneinn == 0x12 && active)																// Case 2a: 40 track movement
			{
				stepdist = 40;
			}
			else if(miditoneinn == 0x13 && active)																// Case 2b: 1 track movement
			{
				stepdist = 1;
			}
			else if(miditoneinn == 0x14 && active)																// Case 2c: 4 track movement
			{
				stepdist = 4;
			}
			else if(miditoneinn == 0x15 && active)																// Case 2d: jammin movement
			{
				jam = true;
			}
			else if(miditoneinn == 0x16 && active)																// Case 2d: jammin movement
			{
				outdir = true;
			}
			else if(miditoneinn == 0x17)																		// calibrate
			{
				timerlowerbyte = 56;	// Tune to 10KHz subharmonic
				timerupperbyte = 0;

				delay(5);
				while(PINB&(1<<PB0));	// Vent p� at datapin g�r lav 
				int kold = kaltest();
				int kdir = -1;
				if(OSCCAL > 0x80)			// Set riktig retning f�r loopen
					OSCCAL--;
				shortdelay();
				int knew = kaltest();
				if(knew > kold || OSCCAL == 0x80)
					kdir = 1;

				while(cominn != 0x97)
				{
					kold = knew;
					int oscold = OSCCAL;
					
					if(kdir == 1 && OSCCAL<0xFF)
						OSCCAL++;
					if(kdir == -1 && OSCCAL>0x80)
						OSCCAL--;
						
					shortdelay();
					knew = kaltest();
					if(knew > kold || OSCCAL == 0x80 || OSCCAL == 0xFF)
					{
						OSCCAL = oscold;
						shortdelay();
						kaltest();
						turnon();
						while(cominn != 0x97);		// vent p� slutt f�r break
						break;
					}
				}
				turnoff();
				cominn = 0;
				kill = true;
				stepnow = false;
			}
			else if(miditoneinn < 0x80 && active)																// Jammin Direction outward
			{
				turnon();
				miditone = miditoneinn;
				int cnt = scale[midiscale[miditone]];
				kill = false;
				stepnow = false;
				timerlowerbyte = 256-(cnt&0xFF);
				timerupperbyte = ((cnt>>8)&0xFF);
			}
			else if((miditoneinn&0x7F) == tonegenid || miditoneinn == 0x91)										// Case 5: Deaktiver skriving av toneinformasjon
			{
				active = false;
			}
			else if(miditoneinn == 0x92 && active)																// Case 2e: 80 track movement (!!!DEFAULT!!!)
			{
				stepdist = 80;
			}
			else if(miditoneinn == 0x93 && active)																// Case 2f: 2 track movement
			{
				stepdist = 2;
			}
			else if(miditoneinn == 0x94 && active)																// Case 2g: 8 track movement
			{
				stepdist = 8;
			}
			else if(miditoneinn == 0x95 && active)																// Case 2h: no jammin movement
			{
				jam = false;
			}
			else if(miditoneinn == 0x96 && active)																// Case 2d: jammin movement
			{
				outdir = false;
			}
			else if((miditoneinn >= 0x98 && (miditoneinn&0x7F) == miditone && active) || miditoneinn == 0x80)	// Case 4: Stopp tone
			{
				if(miditoneinn==0x80)																			// Case 4b: Rekalibrer ogs�
				{
					turnon();
					center();
				}
				turnoff();
				kill = true;
				miditone = 0;
				stepnow = false;
			}
		}
		if(stepnow && !kill)						// Ellers, pulse tone hvis det trengs.
		{
			pulse();
			stepnow = false;
		}
	}
	
	return 0;					// Kommer aldri s� lang
}

/************************************************************************/
/* Komunikasjon. Enhveis SPI!                                           */
/************************************************************************/
//
// Er ikke pinner nok til treveis og I2C suger v�t eselballe.
//

void setup_com()
{
	cli();
	DDRB &= ~(1<<PINB0) & ~(1<<PINB2);
	USIDR = 0;																	// Reset data
	USISR &= ~(1<<USICNT0) & ~(1<<USICNT1) & ~(1<<USICNT2) & ~(1<<USICNT3);		// Reset input clock
	USISR |= (1<<USIOIF);														// & interrupt
	USICR = (1<<USICS1) | (1<<USIOIE);											// Set input only and enable interrupt
	sei();
}

ISR(USI_OVF_vect)
{
	cominn = USIDR;
	if(tonegenid == 0)
	{
		USIDR = 0;																	// Reset data
		USISR &= ~(1<<USICNT0) & ~(1<<USICNT1) & ~(1<<USICNT2) & ~(1<<USICNT3);		// Reset input clock
		USISR |= (1<<USIOIF);														// & interrupt
		sei();
		delay(10);
	}
	setup_com();
}

/************************************************************************/
/* TIMER                                                                */
/************************************************************************/
//
// Vi har ikke en 16-bit timer, s� vi m� bruke en 8-bit timer og gj�re
// resten i software.
//

uint_fast8_t divn = 0;				// Ripple-counter for de �vre 8 bittene.
									// Skal BARE brukes i interrupt.

void setup_timer()
{
	cli();
	PLLCSR &= ~(1<<PCKE);	// Synchronous mode using system clock (8MHz) / 4 = 2MHz
	TCCR1 = 0;
	TCNT1 = 0;
	TCCR1 |=  (1 << CS10) | (1 << CS11);
	TIMSK |= (1 << TOIE1);
	sei();
}

ISR(TIMER1_OVF_vect)
{
	TCNT1 = 0;						// Tell til 256*upperbyte
	if(divn++>=timerupperbyte)
	{
		TCNT1 = timerlowerbyte;		// Tell til lowerbyte f�r man begynner � telle upperbyte.
		divn = 0;
		stepnow = true;
	}
}

/************************************************************************/
/* IO                                                                   */
/************************************************************************/

int steppin = PINB4;
int dirpin = PINB3;
int drselpin = PINB1;

void setup_io()
{
	// Setup Outputs and pull high
	DDRB |= (1<<steppin) | (1<<dirpin) | (1<<drselpin);
	PORTB |= (1<<steppin) | (1<<dirpin) | (1<<drselpin);
}

//
// Diskettstasjon Drive-select kontroll
//
inline void turnon(){PORTB &= ~(1<<drselpin);}
inline void turnoff(){PORTB |= (1<<drselpin);}

int dir = 0;

//
// Diskettstasjon step
//
void pulse()
{
	PORTB &= ~(1<<steppin);
	shortdelay();
	PORTB |= (1<<steppin);
	if ((dir++ >= stepdist && ~jam) || (jam && outdir))
		PORTB |= (1<<dirpin);
	else
		PORTB &= ~(1<<dirpin);
	dir = dir % (stepdist * 2);
}

//
// Sentrer lese/skrive-hodet
//
void center()
{
	PORTB |= (1<<dirpin);
	for (int i = 0; i < 100; i++)
	{
		PORTB &= ~(1<<steppin);
		shortdelay();
		PORTB |= (1<<steppin);
		delay(5);
	}
	PORTB &= ~(1<<dirpin);
	for (int i = 0; i < 40; i++)
	{
		PORTB &= ~(1<<steppin);
		shortdelay();
		PORTB |= (1<<steppin);
		delay(5);
	}
	dir = stepdist / 2;
}

/************************************************************************/
/* Noncritical Timetjafs                                                */
/************************************************************************/
void delay(int mill)
{
	for(int i=0; i<mill; i++)
		_delay_ms(1);
}

void shortdelay()
{
	_delay_us(100);
}

void debug(int val)
{
	turnoff();
	delay(1000);
	for(int i=0; i<8; i++)
	{
		if(val&(0x80>>i))
		{
			turnon();
			delay(4000);
			turnoff();
			delay(1000);
		}
		else
		{
			turnon();
			delay(1000);
			turnoff();
			delay(4000);			
		}
	}
}

uint16_t kaltest()
{
	uint16_t count = 0;
	while(PINB&(1<<PB0));
	while(!(PINB&(1<<PB0)));
	TCNT1 = timerlowerbyte;
	stepnow = false;
	while(!stepnow);
	int oldval = PINB&(1<<PB0);
	stepnow = false;

	for(uint32_t i=0; i<10000; i++)		// 1 second at 10 KHz
	{
		while(!stepnow);
		int newval = PINB&(1<<PB0);
		stepnow = false;
		
		if(newval != oldval)
		{
			count++;
			if(count&0x10)
				turnon();
			else
				turnoff();
			oldval = newval;
		}
	}
	turnoff();
	return count;
}