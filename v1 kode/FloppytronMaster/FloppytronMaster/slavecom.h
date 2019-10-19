

#ifndef SLAVECOM_H_
#define SLAVECOM_H_


void com_init(void);
void sendid(uint8_t);
void spill(uint8_t, uint8_t);
void stopp(uint8_t, uint8_t);
void aktiver(uint8_t);
void deaktiver(uint8_t);
void bassmode(uint8_t);
void tonegensend(uint8_t);


#endif /* SLAVECOM_H_ */