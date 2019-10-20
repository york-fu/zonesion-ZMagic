#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f4xx.h"

//0,不支持contiki
//1,支持contiki
#define SYSTEM_SUPPORT_CONTIKI		1				//定义系统文件夹是否支持CONTIKI

#if SYSTEM_SUPPORT_CONTIKI

#include "contiki.h"

extern void clock_delay_us(unsigned int us);
extern void clock_delay_ms(unsigned int ms);

#define delay_us(x) clock_delay_us(x)
#define delay_ms(x) clock_delay_ms(x)

#else

void delay_init(unsigned char SYSCLK);
void delay_ms(unsigned int nms);
void delay_us(unsigned int nus);
void delay_xms(unsigned int nms);

#endif

#endif



