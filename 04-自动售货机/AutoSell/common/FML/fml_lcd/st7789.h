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
#define RS_PIN          17
#define ST7789_REG      (*((volatile uint16_t *)(0x60000000)))
#define ST7789_DAT      (*((volatile uint16_t *)(0x60000000 | (1<<(RS_PIN+1)))))

#define write_command(x)   (ST7789_REG=(x))
#define write_data(x)      (ST7789_DAT=(x))
#define read_data()        ST7789_DAT

#define LCD_WIDE        240
#define LCD_HIGH        320


typedef struct  
{										    
	short wide;			  //宽度
	short high;			  //高度
	uint16_t id;        //ID
	uint8_t dir;			  //横屏还是竖屏控制：0，竖屏；1，横屏。	
	uint8_t wramcmd;		//开始写gram指令	
	uint8_t rramcmd;		//开始读gram指令
	uint8_t setxcmd;		//设置x坐标指令
	uint8_t setycmd;		//设置y坐标指令	 
}screen_dev_t; 	  

extern screen_dev_t screen_dev;

void st7789_PrepareWrite(void);
void st7789_SetCursorPos(short x,short y);
void st7789_PrepareFill(short x1, short y1, short x2, short y2);

uint8_t st7789_init(void);
void st7789_DrawPoint(short x,short y,uint32_t color);
uint32_t LCD_ReadPoint(short x,short y);
void st7789_FillColor(short x1,short y1,short x2,short y2,uint16_t color);
void st7789_FillData(short x1,short y1,short x2,short y2,unsigned short* dat);

#endif