#include "ui_RfInfo.h"


uint8_t* ui_RfPageIndex = NULL;


void ui_showBar_1(uint16_t y,char* text,uint16_t color,uint16_t backColor,uint8_t style,uint8_t redraw)
{
    UI_Bar_t bar;
    ui_setBar(&bar,320,20,backColor,text,color,style);
    ui_drawBar(0,y,redraw,&bar);
}

void ui_showBar_2(uint16_t y,char* text1,char* text2,uint16_t color,uint16_t backColor,uint8_t style,uint8_t redraw)
{
    UI_Bar_t bar;
    
    ui_setBar(&bar,80,20,backColor,text1,color,style);
    ui_drawBar(0,y,redraw,&bar);
    
    ui_setBar(&bar,240,20,backColor,text2,color,style);
    ui_drawBar(80,y,redraw,&bar);
}

void ui_showBar_4(uint16_t y,char* text1,char* text2,char* text3,char* text4,
                       uint16_t color,uint16_t backColor,uint8_t style,uint8_t redraw)
{
    uint16_t x=0;
    UI_Bar_t bar;
    
    ui_setBar(&bar,80,20,backColor,text1,color,style);
    ui_drawBar(x,y,redraw,&bar);
    
    x+=80;
    ui_setBar(&bar,80,20,backColor,text2,color,style);
    ui_drawBar(x,y,redraw,&bar);
    
    x+=80;
    ui_setBar(&bar,80,20,backColor,text3,color,style);
    ui_drawBar(x,y,redraw,&bar);
    
    x+=80;
    ui_setBar(&bar,80,20,backColor,text4,color,style);
    ui_drawBar(x,y,redraw,&bar);
}

void lcdPageZigbee(uint16_t _y,unsigned char redraw)
{
    u16 y=_y;
    u16 textColor = LCD_COLOR_BLACK;
    u16 backColor = LCD_COLOR_GRAY_2;
    char buf1[30]={0};
    char buf2[30]={0};
    
    sprintf(buf1,"网络类型");
    sprintf(buf2,"ZigBee");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"射频芯片");
    sprintf(buf2,"CC2530");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"PANID");
    sprintf(buf2,"%u",zigbeeGat_panid());
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"通信信道");
    sprintf(buf2,"%u",zigbeeGat_channel());
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"节点类型");
    switch(zigbeeGat_type())
    {
        case 0:sprintf(buf2,"汇集节点");
            break;
        case 1:sprintf(buf2,"路由节点");
            break;
        case 2:sprintf(buf2,"终端节点");
            break;
        default:sprintf(buf2,"未知节点");
    }
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"连接状态");
    if(zigbeeGat_link())
    {
        sprintf(buf2,"已连接");
    }
    else
    {
        sprintf(buf2,"已断开");
    }
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"MAC");
    if(zigbeeGat_mac() != NULL)
        sprintf(buf2,"%s",zigbeeGat_mac());
    else
        sprintf(buf2,"获取失败");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
}

void lcdPageBle(uint16_t _y,unsigned char redraw)
{
    u16 y=_y;
    u16 textColor = LCD_COLOR_BLACK;
    u16 backColor = LCD_COLOR_GRAY_2;
    char buf1[30]={0};
    char buf2[30]={0};
    
    sprintf(buf1,"网络类型");
    sprintf(buf2,"BLE");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"射频芯片");
    sprintf(buf2,"CC2540");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"连接状态");
    if(bleGat_link())
    {
        sprintf(buf2,"已连接");
    }
    else
    {
        sprintf(buf2,"已断开");
    }
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"MAC");
    if(bleGat_mac() != NULL)
        sprintf(buf2,"%s",bleGat_mac());
    else
        sprintf(buf2,"获取失败");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
}

void lcdPageWiFi(uint16_t _y,unsigned char redraw)
{
    u16 y=_y;
    u16 textColor = LCD_COLOR_BLACK;
    u16 backColor = LCD_COLOR_GRAY_2;
    char buf1[20]={0};
    char buf2[20]={0};
    
    sprintf(buf1,"网络类型");
    sprintf(buf2,"WiFi");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"射频芯片");
    sprintf(buf2,"CC3200");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"WiFi名称");
    ui_showBar_2(y,buf1,wifiGat_ssid(),textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"WiFi密码");
    if(wifiGat_keyType()==1)
        strcpy(buf2,wifiGat_key());
    else
        sprintf(buf2,"无加密\0");
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"IP地址");
    char* pdata = wifiGat_ip();
    if(*pdata==0)
        sprintf(buf2,"0.0.0.0");
    else
        sprintf(buf2,"%s",pdata);
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"SIP地址");
    pdata = wifiGat_sip();
    if(*pdata==0)
        sprintf(buf2,"0.0.0.0");
    else
        sprintf(buf2,"%s",pdata);
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"连接状态");
    if(wifiGat_link())
    {
        sprintf(buf2,"已连接");
    }
    else
    {
        sprintf(buf2,"已断开");
    }
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
}


void lcdPageLora(uint16_t _y,unsigned char redraw)
{
    u16 y=_y;
    u16 textColor = LCD_COLOR_BLACK;
    u16 backColor = LCD_COLOR_GRAY_2;
    char buf1[40]={0};
    char buf2[30]={0};
    char buf3[20]={0};
    char buf4[20]={0};
    const char bandwidthList[10][10]={"7.8KHz","10.4KHz","15.6KHz","20.8KHz","31.25KHz",
                                        "41.7KHz","62.58KHz","125KHz","250KHz","500KHz"};
    
    sprintf(buf1,"网络类型");
    sprintf(buf2,"LoRa");
    sprintf(buf3,"射频芯片");
    sprintf(buf4,"SX1278");
    ui_showBar_4(y,buf1,buf2,buf3,buf4,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"基频");
    sprintf(buf2,"%u",loraGet_fp());
    sprintf(buf3,"发射功率");
    sprintf(buf4,"%u",loraGet_pv());
    ui_showBar_4(y,buf1,buf2,buf3,buf4,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"扩频因子");
    sprintf(buf2,"%u",loraGet_sf());
    sprintf(buf3,"编码率");
    sprintf(buf4,"%u",loraGet_cr());
    ui_showBar_4(y,buf1,buf2,buf3,buf4,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"前导码长");
    sprintf(buf2,"%u",loraGet_ps());
    sprintf(buf3,"带宽");
    ui_showBar_4(y,buf1,buf2,buf3,(char*)bandwidthList[loraGet_bw()],textColor,backColor,0x01,redraw);
    
    y+=20;
    if(loraGet_hop())
        sprintf(buf1,"%s",loraGet_hopTab());
    else
        sprintf(buf1,"跳频功能已关闭");
    ui_showBar_1(y,buf1,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"网络ID");
    sprintf(buf2,"%u",loraGet_netID());
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
    
    y+=20;
    sprintf(buf1,"节点地址");
    uint16_t temp = loraGet_nodeID();
    sprintf(buf2,"LoRa:%X:%X",temp/256,temp%256);
    ui_showBar_2(y,buf1,buf2,textColor,backColor,0x01,redraw);
}

void lcdPageDefault(uint16_t _y,unsigned char redraw)
{
    u16 y=_y;
    u16 textColor = LCD_COLOR_BLACK;
    u16 backColor = LCD_COLOR_GRAY_2;
    char buf1[40]={0};
    
    sprintf(buf1,"无线信息获取失败");
    ui_showBar_1(y,buf1,textColor,backColor,0x01,redraw);
    sprintf(buf1,"请复位或检查设备");
    ui_showBar_1(y+=20,buf1,textColor,backColor,0x01,redraw);
    sprintf(buf1,"!!!");
    ui_showBar_1(y+=20,buf1,textColor,backColor,0x01,redraw);
}

void ui_Rf1Page(uint16_t y,uint8_t redraw)
{
    if(RF1_hwTypeGet() != 0)
    {
        switch(RF1_hwTypeGet())
        {
            case 1:lcdPageZigbee(y,redraw);
                break;
            case 2:lcdPageBle(y,redraw);
                break;
        }
    }
    else
    {
        lcdPageDefault(y,redraw);
    }
}

void ui_Rf2Page(uint16_t y,uint8_t redraw)
{
    if(RF2_hwTypeGet() != 0)
    {
        switch(RF2_hwTypeGet())
        {
            case 1:lcdPageWiFi(y,redraw);
                break;
        }
    }
    else
    {
        lcdPageDefault(y,redraw);
    }
}

void ui_Rf3Page(uint16_t y,uint8_t redraw)
{
    if(RF3_hwTypeGet() != 0)
    {
        switch(RF3_hwTypeGet())
        {
            case 1:lcdPageLora(y,redraw);
                break;
        }
    }
    else
    {
        lcdPageDefault(y,redraw);
    }
}

void ui_Rf4Page(uint16_t y,uint8_t redraw)
{
    if(RF4_hwTypeGet() != 0)
    {
        switch(RF4_hwTypeGet())
        {
            case 1:
                break;
        }
    }
    else
    {
        lcdPageDefault(y,redraw);
    }
}

void ui_RfInfoShow(uint8_t redraw)
{
    uint16_t x = 0,y = 20;
    u16 textColor = LCD_COLOR_WHITE;
    u16 backColor = LCD_COLOR_RED;
    char pbuf[20] = {0};
    
    if(redraw)
    {
        LCD.FillRectangle(0,20,320,220,LCD_COLOR_WHITE);
        ui_showBar_1(y,"无线网络",textColor,backColor,0x01,redraw);
    }
    
    y+=20;
    UI_Bar_t bar;
    ui_setBar(&bar,20,20,backColor,"<<",textColor,0x01);
    ui_drawBar(0,y,redraw,&bar);
    
    ui_setBar(&bar,20,20,backColor,">>",textColor,0x01);
    ui_drawBar(320-20,y,redraw,&bar);
    
    x=20;
    for(uint8_t i=0;i<4;i++)
    {
        backColor = (*ui_RfPageIndex)==i ? LCD_COLOR_RED : LCD_COLOR_DARK_RED;
        sprintf(pbuf,"RF%u",i+1);
        ui_setBar(&bar,70,20,backColor,pbuf,textColor,0x01);
        ui_drawBar(x,y,redraw,&bar);
        x+=70;
    }
    
    if(redraw)
    {
        LCD.FillRectangle(0,y+20,320,220,LCD_COLOR_WHITE);
    }
    
    y+=25;
    switch((*ui_RfPageIndex))
    {
        case 0:ui_Rf1Page(y,redraw);
            break;
        case 1:ui_Rf2Page(y,redraw);
            break;
        case 2:ui_Rf3Page(y,redraw);
            break;
        case 3:ui_Rf4Page(y,redraw);
            break;
    }
}

void RfInfoExitHandle()
{
    ui_SetAppCurrentID(DESKTOP_ID);
    if(ui_RfPageIndex != NULL)
        free(ui_RfPageIndex);
    process_exit(&ui_RfInfoProcess);
}

void K1_RfInfoHandle()
{
}

void K2_RfInfoHandle()
{
    RfInfoExitHandle();
}

void K3_RfInfoHandle()
{
    if((*ui_RfPageIndex)<1)
        (*ui_RfPageIndex) = 3;
    else
        (*ui_RfPageIndex)--;
}

void K4_RfInfoHandle()
{
    (*ui_RfPageIndex)++;
    if((*ui_RfPageIndex)>3)
        (*ui_RfPageIndex) = 0;
}

void ui_RfInfoKeyHandle(uint8_t keyStatus)
{
    switch(keyStatus)
    {
        case 0x01:K1_RfInfoHandle();
            break;
        case 0x02:K2_RfInfoHandle();
            break;
        case 0x04:K3_RfInfoHandle();
            break;
        case 0x08:K4_RfInfoHandle();
            break;
    }  
}


PROCESS(ui_RfInfoProcess, "ui_RfInfo process");

PROCESS_THREAD(ui_RfInfoProcess, ev, data)
{
    PROCESS_BEGIN();
    
    ui_RfPageIndex = (uint8_t*)(malloc(sizeof(uint8_t)));
    if(ui_RfPageIndex == NULL)
    {
        ui_SetWindowStatus(WIN_STA_SHOW+WIN_STA_ERROR);
        RfInfoExitHandle();
    }
    else
    {
        (*ui_RfPageIndex) = 0;
        ui_RfInfoShow(0x01);
    }
    
    while (1)
    {
        PROCESS_WAIT_EVENT();  
        if(ev == uiRefresh_event)
        {
            ui_RfInfoShow(0x00);
        }
        if(ev == uiKey_event)
        {
            ui_RfInfoKeyHandle(*((uint8_t*)data));
            if(*((uint8_t*)data) != 0x02)//显示细节优化
                ui_RfInfoShow(0x01);
        }
    }
    
    PROCESS_END();
}