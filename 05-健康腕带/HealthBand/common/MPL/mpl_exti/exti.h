#ifndef _exti_h_
#define _exti_h_

#include <stdio.h>
#include "stm32f4xx.h"

void Exti0_Init(uint8_t EXTI_PortSourceGPIOx);
uint8_t Exti0IrqCall_Set(void (*func)(void));

void Exti1_Init(uint8_t EXTI_PortSourceGPIOx);
uint8_t Exti1IrqCall_Set(void (*func)(void));

void Exti2_Init(uint8_t EXTI_PortSourceGPIOx);
uint8_t Exti2IrqCall_Set(void (*func)(void));

void Exti9_5_Init(uint8_t EXTI_PortSourceGPIOx,uint8_t EXTI_PinSourceX);
uint8_t Exti9_5_IrqCall_Set(uint8_t index,void (*func)(void));

void Exti15_10_Init(uint8_t EXTI_PortSourceGPIOx,uint8_t EXTI_PinSourceX);
uint8_t Exti15_10_IrqCall_Set(uint8_t index,void (*func)(void));

#endif
