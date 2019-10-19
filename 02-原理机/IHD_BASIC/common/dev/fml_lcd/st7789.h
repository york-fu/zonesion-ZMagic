#ifndef _ST7789_H_
#define _ST7789_H_

#include "stdio.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "fsmc.h"
#include "delay.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
//lcd 使用PD7脚 NE1功能基地址为0x60000000,引脚A17 作为rs选择
#define ST7789_RS_PIN       17
#define ST7789_REG          (*((volatile uint16_t *)(0x60000000)))
#define ST7789_DAT          (*((volatile uint16_t *)(0x60000000 | (1<<(ST7789_RS_PIN+1)))))

#define St7789_WCMD(x)    (ST7789_REG=(x))
#define St7789_WDATA(x)       (ST7789_DAT=(x))
#define St7789_RDATA()         ST7789_DAT

#define ST7789_WRAM     0X2C 		  //开始写gram指令	
#define ST7789_RRAM     0X2E 		  //开始读gram指令
#define ST7789_SETX     0X2A 		  //设置x坐标指令
#define ST7789_SETY     0X2B 		  //设置y坐标指令	 

#define ST7789_WIDE     320
#define ST7789_HIGH     240

void St7789_PrepareWrite(void);
void St7789_SetCursorPos(short x,short y);
void St7789_PrepareFill(short x1, short y1, short x2, short y2);

uint8_t St7789_init(void);
void St7789_DrawPoint(short x,short y,uint32_t color);
uint32_t St7789_ReadPoint(short x,short y);
void St7789_FillColor(short x1,short y1,short x2,short y2,uint32_t color);
void St7789_FillData(short x1,short y1,short x2,short y2,unsigned short* dat);

#endif