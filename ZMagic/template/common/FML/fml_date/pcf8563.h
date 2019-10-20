#ifndef __PCF8563_H_
#define __PCF8563_H_
#include "stm32f4xx.h"

#define PCF8563_ADDR            0xA2

void pcf8563_init(void);
unsigned char get_pcf8563Time(unsigned char* date);
int set_pcf8563Time(unsigned char *buf);


#endif  // end __PCF8563_H_
