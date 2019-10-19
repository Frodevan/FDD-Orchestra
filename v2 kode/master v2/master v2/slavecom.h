#ifndef SLAVECOM_H_
#define SLAVECOM_H_

void com_init();
void spill(uint8_t tone, uint8_t id);
void stopp(uint8_t id);
void stem(uint8_t tune, uint8_t id);
void stopp_alle();

#endif