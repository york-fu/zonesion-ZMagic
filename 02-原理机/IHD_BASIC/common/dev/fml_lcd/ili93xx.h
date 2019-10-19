#ifndef _ILI93XX_H_
#define _ILI93XX_H_

#include "stdio.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "fsmc.h"
#include "delay.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
//lcd 使用PD7脚 NE1功能基地址为0x60000000,引脚A17 作为rs选择
#define ILI93XX_RS_PIN       17
#define ILI93XX_REG          (*((volatile uint16_t *)(0x60000000)))
#define ILI93XX_DAT          (*((volatile uint16_t *)(0x60000000 | (1<<(ILI93XX_RS_PIN+1)))))

#define ILI93XX_WCMD(x)       (ILI93XX_REG=(x))
#define ILI93XX_WDATA(x)      (ILI93XX_DAT=(x))
#define ILI93XX_RDATA()       ILI93XX_DAT

#define ILI93XX_WIDE     240
#define ILI93XX_HIGH     320

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

void Ili93xx_PrepareWrite(void);
void Ili93xx_SetCursorPos(short x,short y);
void Ili93xx_PrepareFill(short x1, short y1, short x2, short y2);
void Ili93xx_SetWindow(short x1, short y1, short x2, short y2);

uint8_t Ili93xx_init(void);
void Ili93xx_DrawPoint(short x,short y,uint32_t color);
uint32_t Ili93xx_ReadPoint(short x,short y);
void Ili93xx_FillColor(short x1,short y1,short x2,short y2,uint32_t color);
void Ili93xx_FillData(short x1,short y1,short x2,short y2,unsigned short* dat);

#endif