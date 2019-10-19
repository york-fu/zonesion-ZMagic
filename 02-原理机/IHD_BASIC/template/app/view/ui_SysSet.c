#include "ui_SysSet.h"


Menu_t Menu = {0,0,0,0};
UI_Option_t OptionArray[UI_OPTION_MAX] = {0};


void Option_1_Handle(void)
{
    if(OptionArray[0].status)
    {
        OptionArray[0].status = 0;
        LED3 = 1;//OFF
    }
    else
    {
        OptionArray[0].status = 1;
        LED3 = 0;//ON
    }
}

void Option_2_Handle(void)
{
    if(OptionArray[1].status)
    {
        OptionArray[1].status = 0;
        LED4 = 1;//OFF
    }
    else
    {
        OptionArray[1].status = 1;
        LED4 = 0;//ON
    }
}

void Option_3_Handle(void)
{
    if(OptionArray[2].status)
    {
        OptionArray[2].status = 0;
        relay_off(1);
    }
    else
    {
        OptionArray[2].status = 1;
        relay_on(1);
    }
}

void Option_4_Handle(void)
{
    if(OptionArray[3].status)
    {
        OptionArray[3].status = 0;
        relay_off(2);
    }
    else
    {
        OptionArray[3].status = 1;
        relay_on(2);
    }
}

void Option_5_Handle(void)
{
    if(OptionArray[4].status)
    {
        OptionArray[4].status = 0;
    }
    else
    {
        OptionArray[4].status = 1;
    }
}

uint8_t OptionRegister(UI_Option_t* option)
{
    if(Menu.Num <= UI_OPTION_MAX)
    {
        OptionArray[Menu.Num] = *option;
        Menu.Num++;
        return 0;
    }
    return 1;
}

void OptionInit()
{
    uint16_t backColor = LCD_COLOR_DARK_RED;
    uint16_t textColor = LCD_COLOR_BLACK;
    uint16_t OptionWide = 240-16;
    UI_Bar_t bar1,bar2;
    UI_Option_t option;
    
    Menu.Num = 0;
    
    ui_setBar(&bar1,OptionWide,24,backColor,"LED3设置",textColor,0x01+0x02);
    ui_setBar(&bar2,80-8,24,backColor,"状态",textColor,0x01);
    ui_setOption(&option,&bar1,&bar2,Option_1_Handle);
    OptionRegister(&option);
    
    ui_setBar(&bar1,OptionWide,24,backColor,"LED4设置",textColor,0x01+0x02);
    ui_setBar(&bar2,80-8,24,backColor,"状态",textColor,0x01);
    ui_setOption(&option,&bar1,&bar2,Option_2_Handle);
    OptionRegister(&option);
    
    ui_setBar(&bar1,OptionWide,24,backColor,"继电器一设置",textColor,0x01+0x02);
    ui_setBar(&bar2,80-8,24,backColor,"状态",textColor,0x01);
    ui_setOption(&option,&bar1,&bar2,Option_3_Handle);
    OptionRegister(&option);
    
    ui_setBar(&bar1,OptionWide,24,backColor,"继电器一设置",textColor,0x01+0x02);
    ui_setBar(&bar2,80-8,24,backColor,"状态",textColor,0x01);
    ui_setOption(&option,&bar1,&bar2,Option_4_Handle);
    OptionRegister(&option);
    
    ui_setBar(&bar1,OptionWide,24,backColor,"选项",textColor,0x01+0x02);
    ui_setBar(&bar2,80-8,24,backColor,"状态",textColor,0x01);
    ui_setOption(&option,&bar1,&bar2,Option_5_Handle);
    OptionRegister(&option);
}

void ui_ShowMenu(uint8_t redraw)
{
    u16 x=0,y=0;
    uint16_t backColor1 = LCD_COLOR_GRAY_2;
    uint16_t backColor2 = LCD_COLOR_DARK_RED;
    
    if(redraw)
    {
        LCD.FillRectangle(0,20,320,220,LCD_COLOR_WHITE);
        
        UI_Bar_t bar;
        ui_setBar(&bar,320,20,LCD_COLOR_RED,"系统设置",LCD_COLOR_WHITE,0x01);
        ui_drawBar(0,20,redraw,&bar);
    }
        
    x=8,y=45;
    for(uint8_t i=0;i<Menu.Num;i++)
    {
        backColor1 = Menu.index == i+1 ? LCD_COLOR_RED : LCD_COLOR_GRAY_2;
        backColor2 = OptionArray[i].status ? LCD_COLOR_RED : LCD_COLOR_DARK_RED;
        ui_setOptionColor(&OptionArray[i],backColor1,backColor2);
        
        ui_drawOption(x,y,redraw,&OptionArray[i]);
        
        y+=25;
    }
}

void MenuExitHandle()
{
    ui_SetAppCurrentID(DESKTOP_ID);
    Menu.redraw = 1;
}

void K1_MenuHandle()
{
    Menu.currentID = Menu.index;
    if(Menu.currentID != 0)
    {
        if(OptionArray[Menu.currentID-1].func != NULL)
            OptionArray[Menu.currentID-1].func();
    }
}

void K2_MenuHandle()
{
    MenuExitHandle();
}

void K3_MenuHandle()
{
    if(Menu.index>1)
        Menu.index--;
    else
        Menu.index = Menu.Num;
}

void K4_MenuHandle()
{
    Menu.index++;
    if(Menu.index>Menu.Num)
        Menu.index = 1;
}

void ui_MenuKeyHandle(uint8_t keyStatus)
{
    switch(keyStatus)
    {
        case 0x01:K1_MenuHandle();
            break;
        case 0x02:K2_MenuHandle();
            break;
        case 0x04:K3_MenuHandle();
            break;
        case 0x08:K4_MenuHandle();
            break;
    }  
}


PROCESS(ui_MenuProcess, "ui_MenuProcess");

PROCESS_THREAD(ui_MenuProcess, ev, data)
{
    PROCESS_BEGIN();
    
    OptionInit();
    ui_ShowMenu(0x01);
        
    while(1)
    {
        PROCESS_WAIT_EVENT();  
        if(ev == uiRefresh_event)
        {
            ui_ShowMenu(Menu.redraw);
            if(Menu.redraw)
                Menu.redraw = 0;
        }
        if(ev == uiKey_event)
        {
            ui_MenuKeyHandle(*((uint8_t*)data));
            if(*((uint8_t*)data) != 0x02)//显示细节优化
                ui_ShowMenu(0x01);
        }
    }
    PROCESS_END();
}

