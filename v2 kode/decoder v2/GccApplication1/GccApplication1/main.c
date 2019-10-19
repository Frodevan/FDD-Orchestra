/*
 * GccApplication1.c
 *
 * Created: 08.08.2019 19:19:23
 * Author : medlem
 */ 

#define F_CPU 20000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/xmega.h>

int main(void)
{
    /* Replace with your application code */
	PORTE.DIR |= (1 << 0);
		
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);
	
    while (1) 
    {
		PORTE.OUTTGL = (1 << 0);
		_delay_ms(500);
    }
}

