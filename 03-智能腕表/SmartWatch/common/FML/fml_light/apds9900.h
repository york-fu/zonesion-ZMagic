#ifndef __APDS9900_H_
#define __APDS9900_H_

#include "stm32F4xx.h"

#define APDS9900                0x01
#define OTHER                   0x02
#define APDS9900_ADDR           0x72

void apds9900_init(void);
int get_apds9900Lux(void);
int get_apds9900PromixitStatus(void);

#endif // end __APDS9900_H_