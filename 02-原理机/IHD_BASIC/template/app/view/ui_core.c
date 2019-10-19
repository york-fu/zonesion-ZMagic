/*********************************************************************************************
* 文件：ui_core.c
* 作者：fuyou 2019.1.26
* 说明：ui文件
* 修改：
* 注释：
*********************************************************************************************/
#include "ui_core.h"
#include "fml_lcd/fml_lcd.h"
#include "fml_lcd/st7789.h"
#include "fml_lcd/ili93xx.h"

void ui_setBar(UI_Bar_t* bar,uint16_t wide,uint16_t high,uint16_t color,char* text,uint16_t textColor,uint8_t style)
{
  bar->wide = wide;
  bar->high = high;
  bar->color = color;
  bar->text = text;
  bar->textColor = textColor;
  bar->style = style;
}

uint8_t ui_drawBar(uint16_t x,uint16_t y,uint8_t redraw,UI_Bar_t* Bar)
{
  uint8_t fontSize = 16;
  
  if(redraw)
  {
    LCD.FillRectangle(x,y,x+Bar->wide-1,y+Bar->high-1,Bar->color);
    if(Bar->style&0x01)
    {
      uint16_t color = LCD_COLOR_WHITE;
      LCD_DrawRectangle(x,y,x+Bar->wide-1,y+Bar->high-1,color);
      
      color = LCD_COLOR_GRAY_3;
      LCD_DrawLine(x+1,y+1,x+Bar->wide-2,y+1,color);
      LCD_DrawLine(x+1,y+1,x+1,y+Bar->high-1,color);
      color = LCD_COLOR_GRAY_4;
      LCD_DrawLine(x+1,y+Bar->high-1,x+Bar->wide-2,y+Bar->high-1,color);
      LCD_DrawLine(x+Bar->wide-2,y+1,x+Bar->wide-2,y+Bar->high-1,color);
    }
  }
  
  if(Bar->high<fontSize) return 1;
  if((strlen(Bar->text)*(fontSize/2)+4)>Bar->wide) return 1;
  
  if(Bar->style&0x02)//左对齐
  {
    LCDShowFont16(x+4,y+(Bar->high-fontSize)/2,Bar->text,
                  strlen(Bar->text)*fontSize/2,Bar->textColor,Bar->color);
  }
  else//居中
  {
    LCDShowFont16(x+(Bar->wide-(strlen(Bar->text)*(fontSize/2)))/2,y+(Bar->high-fontSize)/2,
                  Bar->text,strlen(Bar->text)*fontSize/2,Bar->textColor,Bar->color);
  }
  return 0;
}


void ui_setIcon(UI_Icon_t* Icon,uint16_t wide,uint16_t high,uint8_t* bitmap)
{
  Icon->wide = wide;
  Icon->high = high;
  Icon->bitmap = bitmap;
}

void ui_setIconStyle(UI_Icon_t* icon,uint8_t style)
{
  icon->style = style;
}

void ui_drawIcon(uint16_t x,uint16_t y,UI_Icon_t* icon)
{
  uint16_t m,n;
  uint16_t bitmap = 0;
  uint8_t space = 5;
  uint16_t wide = icon->wide-(space*2);
  uint16_t high = icon->high-(space*2);
  if(icon->style)
  {
    uint16_t color = LCD_COLOR_GRAY_4;
    LCD.FillRectangle(x,y,x+(icon->wide/4),y+(icon->high/4),color);
    LCD.FillRectangle(x+(icon->wide/4*3),y,x+icon->wide-1,y+(icon->high/4),color);
    LCD.FillRectangle(x,y+(icon->high/4*3),x+(icon->wide/4),y+icon->high-1,color);
    LCD.FillRectangle(x+(icon->wide/4*3),y+(icon->high/4*3),x+icon->wide-1,y+icon->high-1,color);
  }
  else
  {
    LCD.FillRectangle(x,y,x+icon->wide-1,y+icon->high-1,LCD_COLOR_WHITE);
  }
  
  if(icon->bitmap != NULL)
  {
    LCD.PrepareFill(x+space,y+space,x+icon->wide-space-1,y+icon->high-space-1);
    for(m=0; m<high; m+=1)
    {
      for(n=0; n<wide; n+=1)
      {
        bitmap = icon->bitmap[2*(m*wide+n)];
        bitmap |= (icon->bitmap[2*(m*wide+n)+1])<<8;
        if(lcd_dev.screen->id == 0x7789)
          ST7789_DAT = bitmap;//画点
        else
          ILI93XX_DAT = bitmap;//画点
      }
    }
    LCD.PrepareFill(0, lcd_dev.screen->wide, 0, lcd_dev.screen->high);              //设置窗口为整个屏幕
  }
  else
    LCD.FillRectangle(x+space,y+space,x+icon->wide-space-1,y+icon->high-space-1,LCD_COLOR_CYAN);
}


void ui_setOption(UI_Option_t* option,UI_Bar_t* bar1,UI_Bar_t* bar2,void (*func)(void))
{
  option->bar1 = *bar1;
  option->bar2 = *bar2;
  option->func = func;
}

void ui_setOptionText(UI_Option_t* option,char* text1,char* text2)
{
  if(text1 != NULL)
    option->bar1.text = text1;
  if(text1 != NULL)
    option->bar2.text = text2;
}

void ui_setOptionColor(UI_Option_t* option,uint16_t color1,uint16_t color2)
{
  option->bar1.color = color1;
  option->bar2.color = color2;
}

uint8_t ui_drawOption(uint16_t x,uint16_t y,uint8_t redraw,UI_Option_t* option)
{
  if((ui_drawBar(x,y,redraw,&option->bar1)) ||
     (ui_drawBar(x+option->bar1.wide+8,y,redraw,&option->bar2)))
    return 1;
  return 0;
}

void ui_setWindow(Window_t* window,uint16_t wide,uint16_t high,uint16_t barColor,uint16_t boxColor,char* barText,char* boxText,uint8_t style)
{
  window->wide = wide;
  window->high = high;
  window->barColor = barColor;
  window->boxColor = boxColor;
  window->barText = barText;
  window->boxText = boxText;
  window->style = style;
  
  window->barTextColor = window->boxTextColor = LCD_COLOR_BLACK;
}

uint8_t ui_drawWindow(uint16_t x,uint16_t y,uint8_t redraw,Window_t* window)
{
  uint16_t fontSize = 16;
  uint16_t barHigh = 20;
  if(redraw)
  {
    LCD.FillRectangle(x+2,y+2,x+window->wide-2,y+barHigh+2,window->barColor);
    LCD.FillRectangle(x+2,y+barHigh+2,x+window->wide-2,y+window->high-2,window->boxColor);
    
    LCD_DrawRectangle(x,y,x+window->wide,y+window->high,LCD_COLOR_BLACK);
    LCD_DrawLine(x+1,y+1,x+window->wide-1,y+1,LCD_COLOR_GRAY_1);
    LCD_DrawLine(x+1,y+1,x+1,y+window->high-1,LCD_COLOR_GRAY_1);
    LCD_DrawLine(x+1,y+window->high-1,x+window->wide-1,y+window->high-1,LCD_COLOR_GRAY_3);
    LCD_DrawLine(x+window->wide-1,y+1,x+window->wide-1,y+window->high-1,LCD_COLOR_GRAY_3);
  }
  if(window->high<fontSize) return 1;
  if((strlen(window->barText)*(fontSize/2))>window->wide) return 1;
  
  if(window->style&0x02)//左对齐
  {
    LCDShowFont16(x+4,y+(barHigh/2)-(fontSize/2),window->barText,
                  strlen(window->barText)*fontSize/2,window->barTextColor,window->barColor);
  }
  else//居中
  {
    LCDShowFont16((x+window->wide/2)-(strlen(window->barText)*fontSize/4),y+(barHigh/2)-(fontSize/2)+2,window->barText,
                  strlen(window->barText)*fontSize/2,window->barTextColor,window->barColor);
  }
  LCDShowFont16((x+window->wide/2)-(strlen(window->boxText)*fontSize/4),y+barHigh+((window->high-barHigh)/2)-(fontSize/2),window->boxText,
                strlen(window->boxText)*fontSize/2,window->boxTextColor,window->boxColor);
  return 0;
}


