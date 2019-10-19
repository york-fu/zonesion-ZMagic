/*********************************************************************************************
* 文件：ui_core.h
* 作者：fuyou 2019.1.26
* 说明：ui文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _ui_core_h_
#define _ui_core_h_

#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t wide;
    uint16_t high;
    uint16_t color;
    char* text;
    uint16_t textColor;
    uint8_t style;
}UI_Bar_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t wide;
    uint16_t high;
    uint8_t style;
    uint8_t* bitmap;
}UI_Icon_t;

typedef struct
{
    UI_Bar_t bar1;
    UI_Bar_t bar2;
    uint8_t status;
    void (*func)(void);
}UI_Option_t;

typedef struct
{
    uint16_t wide;
    uint16_t high;
    uint16_t barColor;
    uint16_t boxColor;
    uint16_t barTextColor;
    uint16_t boxTextColor;
    char* barText;
    char* boxText;
    uint8_t style;
}Window_t;


void ui_setBar(UI_Bar_t* bar,uint16_t wide,uint16_t high,uint16_t color,char* text,uint16_t textColor,uint8_t style);
uint8_t ui_drawBar(uint16_t x,uint16_t y,uint8_t redraw,UI_Bar_t* Bar);

void ui_setIcon(UI_Icon_t* Icon,uint16_t wide,uint16_t high,uint8_t* bitmap);
void ui_setIconStyle(UI_Icon_t* icon,uint8_t style);
void ui_drawIcon(uint16_t x,uint16_t y,UI_Icon_t* icon);

void ui_setOption(UI_Option_t* option,UI_Bar_t* bar1,UI_Bar_t* bar2,void (*func)(void));
void ui_setOptionText(UI_Option_t* option,char* text1,char* text2);
void ui_setOptionColor(UI_Option_t* option,uint16_t color1,uint16_t color2);
uint8_t ui_drawOption(uint16_t x,uint16_t y,uint8_t redraw,UI_Option_t* option);

void ui_setWindow(Window_t* window,uint16_t wide,uint16_t high,uint16_t barColor,uint16_t boxColor,char* barText,char* boxText,uint8_t style);
uint8_t ui_drawWindow(uint16_t x,uint16_t y,uint8_t redraw,Window_t* window);


#endif