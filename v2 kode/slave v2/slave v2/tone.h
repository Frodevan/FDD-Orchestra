/*
 * lyd.h
 *
 * Created: 30.01.2018 00.56.30
 *  Author: medlem
 */ 


#ifndef LYD
#define LYD

void setup_lyd();
void playTone(uint8_t tone, uint8_t oktav);
void stopTone();
void soundTone(uint8_t bend_amount);
#endif /* LYD */