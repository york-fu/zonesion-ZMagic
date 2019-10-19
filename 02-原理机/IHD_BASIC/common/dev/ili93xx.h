#ifndef _ILI93XX_H_
#define _ILI93XX_H_

#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "fsmc.h"


#define SCREEN_ORIENTATION_LANDSCAPE    1


#define LCD_WIDTH_SIZE    240
#define LCD_HEIGHT_SIZE   320

#if (SCREEN_ORIENTATION_LANDSCAPE==1)
#define LCD_WIDTH    LCD_HEIGHT_SIZE
#define LCD_HEIGHT   LCD_WIDTH_SIZE
#else
#define LCD_WIDTH    LCD_WIDTH_SIZE
#define LCD_HEIGHT   LCD_HEIGHT_SIZE
#endif

#define SWAP_RGB (1<<3)

/* lcd 使用PD7脚 NE1功能基地址为0x60000000, PD13引脚A18 作为rs选择*/
#define ILI93xx_REG (*((volatile uint16_t *)(0x60000000)))
#define ILI93xx_DAT (*((volatile uint16_t *)(0x60000000 | (1<<(17+1)))))


#define RGB888toRGB565(c24) ((unsigned short)(((c24&0x0000f8)>>3)+((c24&0x00fc00)>>5)+((c24&0xf80000)>>8)))

//lcd颜色参数
#define LCD_COLOR_BLACK             0x0000
#define LCD_COLOR_WHITE             0xFFFF

#define LCD_COLOR_GRAY_1              RGB888toRGB565(0xEFEFEF)  //1级灰度（浅）
#define LCD_COLOR_GRAY_2              RGB888toRGB565(0xDFDFDF)  //2级灰度
#define LCD_COLOR_GRAY_3              RGB888toRGB565(0xBFBFBF)  //3级灰度
#define LCD_COLOR_GRAY_4              RGB888toRGB565(0x7F7F7F)  //4级灰度（深）

//#define LCD_COLOR_GRAY              RGB888toRGB565(0x7F7F7F)    //灰色
//#define LCD_COLOR_LIGHT_GRAY        RGB888toRGB565(0xEFEFEF)    //浅灰（更浅）
//#define LCD_COLOR_LIGHT_GRAY        RGB888toRGB565(0xBFBFBF)    //浅灰
//#define LCD_COLOR_DARK_GRAY         RGB888toRGB565(0x3F3F3F)    //深灰

#define LCD_COLOR_RED               RGB888toRGB565(0xFF0000)    //红色
#define LCD_COLOR_LIGHT_RED         RGB888toRGB565(0xFF8080)    //浅红色
#define LCD_COLOR_DARK_RED          RGB888toRGB565(0x800000)    //深红色

#define LCD_COLOR_GREEN             RGB888toRGB565(0x00FF00)
#define LCD_COLOR_LIGHT_GREEN       RGB888toRGB565(0x80FF80)    //浅绿色
#define LCD_COLOR_DARK_GREEN        RGB888toRGB565(0x008000)    //深绿色

#define LCD_COLOR_BLUE              RGB888toRGB565(0x0000FF)
#define LCD_COLOR_LIGHT_BLUE        RGB888toRGB565(0x8080FF)    //浅蓝色
#define LCD_COLOR_DARK_BLUE         RGB888toRGB565(0x000080)    //深蓝色

#define LCD_COLOR_YELLOW            RGB888toRGB565(0xFFFF00)    //黄色
#define LCD_COLOR_LIGHT_YELLOW      RGB888toRGB565(0xFFFF80)
#define LCD_COLOR_DARK_YELLOW       RGB888toRGB565(0x808000)

#define LCD_COLOR_PURPLE            RGB888toRGB565(0xFF00FF)    //紫色
#define LCD_COLOR_LIGHT_PURPLE      RGB888toRGB565(0xFF80FF)
#define LCD_COLOR_DARK_PURPLE       RGB888toRGB565(0x800080)

#define LCD_COLOR_CYAN              RGB888toRGB565(0x00FFFF)    //青色
#define LCD_COLOR_LIGHT_CYAN        RGB888toRGB565(0x80FFFF)
#define LCD_COLOR_DARK_CYAN         RGB888toRGB565(0x008080)

#define LCD_COLOR_BROWN             RGB888toRGB565(0x804000)    //棕色
#define LCD_COLOR_LIGHT_BROWN       RGB888toRGB565(0xFF8000)    //浅棕色




//LCD管理器
typedef struct
{
    uint16_t id;            //lcd ID
    uint16_t RAM_W_CMD;     //ram 写指令
}LCDdev_t;


void LCD_WR_REG(vu16 regval);
void LCD_WR_DATA(vu16 data);
u16 LCD_RD_DATA(void);
void ILI93xx_WriteReg(uint16_t r, uint16_t d);
uint16_t ILI93xx_ReadReg(uint16_t r);

void ILI9325_ScanDir(uint8_t dir);

void BLOnOff(int st);
void BLInit(void);
void ILI93xxInit(void);

void LCDSetWindow(int x, int xe, int y, int ye);
void LCDSetCursor(int x, int y);
void LCD_WriteRAM_Prepare(void);
void LCDWriteLenData(uint16_t *dat, int len);
void LCDClear(uint16_t color);
void TFTLCD_DrawPixel(int x, int y, uint16_t color);
uint16_t TFTLCD_ReadPixel(int x, int y);

void LCD_FillColor(int x1,int y1,int x2,int y2,uint16_t color);
void LCD_FillData(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCDShowFont16(u16 x,u16 y,char* p,u16 width,u16 color,u16 Bcolor);

#endif
