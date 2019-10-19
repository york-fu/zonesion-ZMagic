#ifndef _exti_h_
#define _exti_h_

#include <stdio.h>
#include "stm32f4xx.h"

#include "drive_led.h"

void exti9_5_init(void);
uint8_t EXTI9_5_CALL_Set(uint8_t index,void (*func)(void));

#endif
