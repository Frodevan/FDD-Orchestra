
#ifndef GLOBALS_H_
#define GLOBALS_H_

#define F_CPU 16000000UL  // 16 MHz
#define ANTALLTONER 10

#define false 0
#define true ~false

#include <avr/io.h>

/////////////////////////////////////////////////////////////////////////
//
// Innstillinger
//

#define LOWESTTONE 0x18			// MIDI tone
#define VELOCITYSENSE 2			// 1 til 4, maks antall grunn-disker per tone
#define SISTE true				// siste i rekken
#define FOERSTE false			// Første i rekken
#define BUFFERLENGDE 500		// Størelse på buffer med mottatte tegn
#define BAUD 56000				// BAUD rate

#define KLOKKEPINNE PORTB4	// 12: clock
#define KLOKKEDIRBIT DDB4

#define DATAPINNE PORTB5	// 13: data
#define DATADIRBIT DDB5

#endif /* GLOBALS_H_ */