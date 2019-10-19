
#ifndef TONECTL_H_
#define TONECTL_H_

void tonectl_init();
void tonectl_tikk();
void spilltone(uint8_t tone, uint8_t kanal, uint8_t timeout);
void stoppalletoner();
void stoppalletoner_channel(uint8_t kanal);
void stopptone(uint8_t tone, uint8_t kanal);
void offsettone(uint8_t semitones, uint8_t finetune, uint8_t channel);

#endif /* TONECTL_H_ */