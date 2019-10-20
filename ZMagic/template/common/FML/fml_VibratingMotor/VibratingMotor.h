#ifndef _VibratingMotor_h_
#define _VibratingMotor_h_

#include "stm32f4xx.h"

#define VIBRATING_MOTOR_RCC       RCC_AHB1Periph_GPIOB
#define VIBRATING_MOTOR_GPIO      GPIOB
#define VIBRATING_MOTOR_PIN       GPIO_Pin_0


void VibratingMotor_Init();
void VibratingMotor_Set(uint8_t s);

#endif