#ifndef _audioAmplifier_h_
#define _audioAmplifier_h_


#include "stm32f4xx.h"
#include "contiki.h"


void LoudspeakerInit(void);
void LoudspeakerSet(uint8_t status);

uint8_t EarphoneGet(void);
void EarphoneInit(void);

#endif