#ifndef __bsp_TIM_H
#define	__bsp_TIM_H

#include "stm32f4xx.h"
#include "stdio.h"

#define GENERAL_TIM           		TIM2
#define GENERAL_TIM_CLK       		RCC_APB1Periph_TIM2

#define GENERAL_TIM_IRQn					TIM2_IRQn
#define GENERAL_TIM_IRQHandler    TIM2_IRQHandler


void TIMx_Configuration(uint16_t Prescaler,uint16_t Period);

short TimeIrq_Set(void (*func)(void));

#endif /* __GENERAL_TIM_H */

