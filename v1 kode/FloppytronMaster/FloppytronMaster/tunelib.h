/*
 * tunelib.h
 *
 * Created: 08.08.2018 00:24:59
 *  Author: medlem
 */ 


#ifndef TUNELIB_H_
#define TUNELIB_H_

void tune_init();
void start_tuning();
void stop_tuning();
ISR(TIMER0_COMPA_vect);
void tune_send(uint8_t);



#endif /* TUNELIB_H_ */