#ifndef _ft6x36_h_
#define _ft6x36_h_

#include "stm32f4xx.h"	
#include "fml_lcd/st7789.h"

/*********************************************************************************************
* ∫Í∂®“Â
*********************************************************************************************/
#define FT6X36_IIC_ADDR 0x38 

#define TOUCH_NUM 2

typedef struct
{
    uint8_t direction;
    uint16_t wide;
    uint16_t high;
    uint16_t id;
    uint8_t status;
    uint16_t x[TOUCH_NUM];
    uint16_t y[TOUCH_NUM];
}TOUCH_Dev_t;


extern TOUCH_Dev_t TouchDev;

void TouchIrqSet(void (*func)(void));
int ft6x36_init(void);
void ft6x36_TouchScan(void);

#endif