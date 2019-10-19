#ifndef _apl_heartrate_h_
#define _apl_heartrate_h_

#include "stm32f4xx.h"
#include "contiki.h"

#define HEART_RATE_IRQ_RCC      RCC_AHB1Periph_GPIOD
#define HEART_RATE_IRQ_GPIO     GPIOD
#define HEART_RATE_IRQ_PIN      GPIO_Pin_12

#define HEART_RATE_IRQ    (HEART_RATE_IRQ_GPIO->IDR & HEART_RATE_IRQ_PIN)

void HeartRate_Get(int32_t* heart_rate);
void Spo2_Get(int32_t* spo2);
PROCESS_NAME(HeartRateProcess);

#endif