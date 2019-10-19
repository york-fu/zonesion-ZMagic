#include "test_ui.h"
#include "fml_lcd/fml_lcd.h"

void testUIupdate(uint8_t refresh)
{
    uint16_t x=8,y=2;
    uint16_t PEN_COLOR=LCD_COLOR_WHITE;
    char pbuf[50]={0};
    
    if(refresh&0x80)
    {
        LCD.FillRectangle(0,0,319,19,LCD_COLOR_DARK_BLUE);
        sprintf(pbuf,"Plus节点测试程序");
        LCDShowFont16(160-strlen(pbuf)*4,y,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_DARK_BLUE);
        sprintf(pbuf,"测试说明");
        LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
        sprintf(pbuf,"LED/RGB周期性闪烁/变色。");
        LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
        sprintf(pbuf,"按键K1--K4首次按下会显示状态；");
        LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
        sprintf(pbuf,"K2再次按下重新测试；");
        LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
        sprintf(pbuf,"K3再次按下开/关继电器一；");
        LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
        sprintf(pbuf,"K4再次按下开/关继电器二。");
        LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
    }
    y=122;
    sprintf(pbuf,"测试结果");
    LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
    if(testFlag_key1)
    {
        sprintf(pbuf,"K1:    功能正常");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"K1:    按下测试");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);
    if(testFlag_key2)
    {
        sprintf(pbuf,"K2:    功能正常");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"K2:    按下测试");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x+160,y,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);
    if(testFlag_key3)
    {
        sprintf(pbuf,"K3:    功能正常");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"K3:    按下测试");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);
    if(testFlag_key4)
    {
        sprintf(pbuf,"K4:    功能正常");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"K4:    按下测试");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x+160,y,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);
    
    if(testFlag_uart1&0x02)
    {
        sprintf(pbuf,"RF串口:通信失败   ");
        PEN_COLOR=LCD_COLOR_RED;
    }
    else if(testFlag_uart1&0x01)
    {
        sprintf(pbuf,"RF串口:通信正常 ");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"串口1: 测试中...");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);
    
    if(testFlag_flash&0x02)
    {
        sprintf(pbuf,"flash: 读写失败 ");
        PEN_COLOR=LCD_COLOR_RED;
    }
    else if(testFlag_flash&0x01)
    {
        sprintf(pbuf,"flash: 读写正常 ");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"flash: 测试中...");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x+160,y,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);

    if(testFlag_ethernet&0x02)
    {
        sprintf(pbuf,"以太网:通信失败 ");
        PEN_COLOR=LCD_COLOR_RED;
    }
    else if(testFlag_ethernet&0x01)
    {
        sprintf(pbuf,"以太网:通信正常 ");
        PEN_COLOR=LCD_COLOR_GREEN;
    }
    else
    {
        sprintf(pbuf,"以太网:测试中...");
        PEN_COLOR=LCD_COLOR_WHITE;
    }
    LCDShowFont16(x,y+=20,pbuf,strlen(pbuf)*8,PEN_COLOR,LCD_COLOR_BLACK);
}

//test ui process
PROCESS(test_ui, "test_ui");

PROCESS_THREAD(test_ui, ev, data)
{
    PROCESS_BEGIN();
    
    static struct etimer etimer_testUI;
    
    LCD_DriverInit();
    LCD_Clear(LCD_COLOR_BLACK);
    testUIupdate(0x80);
    process_post(&test_ui,PROCESS_EVENT_TIMER,NULL);
    
    while (1)
    {
        PROCESS_WAIT_EVENT(); 
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_set(&etimer_testUI,100);
            testUIupdate(0x00);
        }
    }
    
    PROCESS_END();
}