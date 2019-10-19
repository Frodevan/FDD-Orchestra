
#ifndef TONECTL_H_
#define TONECTL_H_

void spilltone(uint8_t tone, uint8_t kanal, uint8_t timeout);
void stoppalletoner();
void stoppalletoner_channel(uint8_t kanal);
void stopptone(uint8_t tone, uint8_t kanal);
void tune(uint8_t channel, int bend, int range);

#endif /* TONECTL_H_ */