
#ifndef GLOBALS_H_
#define GLOBALS_H_

#define F_CPU 16000000UL  // 16 MHz

#define false 0
#define true ~false

#include <util/delay.h>
#include <avr/io.h>

/////////////////////////////////////////////////////////////////////////
//
// Innstillinger
//

#define BUFFERLENGDE 500		// St�relse p� buffer med mottatte tegn
#define BAUD 56000				// BAUD rate

#define COMMAND_NONE				0x00
#define COMMAND_NOTE_OFF			0x80
#define COMMAND_NOTE_ON				0x90
#define COMMAND_TUNE_CHANNEL		0xA0
#define COMMAND_ALL_OFF_CHANNEL		0xB0

#define COMMAND_SPECIAL				0xF0

#define COMMAND_REGROUP				0xF0
#define COMMAND_ALL_OFF				0xF1

#endif /* GLOBALS_H_ */