#ifndef _FML_LCD_H_
#define _FML_LCD_H_

#include "stdio.h"
#include "stm32f4xx.h"

#define LCD_FONT_EN   1

#define TOUCH_POINT   2

#define RGB888toRGB565(c24)  ((unsigned short)(((c24&0x0000f8)>>3)+((c24&0x00fc00)>>5)+((c24&0xf80000)>>8)))

//lcd颜色参数
#define LCD_COLOR_BLACK             0x0000
#define LCD_COLOR_WHITE             0xFFFF

#define LCD_COLOR_GRAY_1            RGB888toRGB565(0xEFEFEF)  //1级灰度（浅）
#define LCD_COLOR_GRAY_2            RGB888toRGB565(0xDFDFDF)  //2级灰度
#define LCD_COLOR_GRAY_3            RGB888toRGB565(0xBFBFBF)  //3级灰度
#define LCD_COLOR_GRAY_4            RGB888toRGB565(0x7F7F7F)  //4级灰度（深）

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


typedef struct  
{										    
	short wide;			      //宽度
	short high;			      //高度
  uint32_t id;
}screen_dev_t; 	  

typedef struct
{
  short wide;
  short high;
  uint16_t id;
  uint16_t status;
  short x[TOUCH_POINT];
  short y[TOUCH_POINT];
}touch_dev_t;

typedef struct
{
  screen_dev_t* screen;
  touch_dev_t* touch;
}lcd_dev_t;


typedef struct
{
  void (*PrepareFill)(short x1, short y1, short x2, short y2);
  void (*DrawPoint)(short x,short y,uint32_t color);
  uint32_t (*ReadPoint)(short x,short y);
  void (*FillRectangle)(short x1,short y1,short x2,short y2,uint32_t color);
  void (*FillData)(short x1,short y1,short x2,short y2,unsigned short* dat);
}lcd_phy_t;


extern lcd_dev_t lcd_dev;
extern lcd_phy_t LCD;
extern screen_dev_t screen_dev;


uint8_t LCD_DriverInit(void);

//draw
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color);

//font
void LCD_ShowFontString(u16 x,u16 y,char* text,u16 textSize,u16 showLen,u16 textColor,u16 backColor);


void LCD_Clear(uint32_t color);
void LCDShowFont16(u16 x,u16 y,char* text,u16 width,u16 color,u16 bcolor);

#endif