#include "ui.h"
#include "image.h"


static uint8_t WindowStatus = 0;
static uint8_t AppIndex = 0;
static uint8_t AppCurrentID = 0;
static uint8_t AppNum = 0;
APP_t AppArray[APP_ID_MAX] = {NULL};
process_event_t uiKey_event;
process_event_t uiRefresh_event;
process_event_t uiError_event;

void ui_SetAppCurrentID(uint8_t id)
{
  AppCurrentID = id;
}

uint8_t ui_GetAppCurrentID()
{
  return AppCurrentID;
}


void ui_SetWindowStatus(uint8_t status)
{
  WindowStatus = status;
}

uint8_t ui_GetWindowStatus()
{
  return WindowStatus;
}

void WindowManage()
{
  static uint8_t tick = 0;
  if(WindowStatus&0x80)
  {
    Window_t window;
    if(WindowStatus&0x01)
    {
      ui_setWindow(&window,140,80,LCD_COLOR_LIGHT_BLUE,LCD_COLOR_GRAY_2,"提示","请选择应用！",0x00);
      ui_drawWindow(80,80,0x01,&window);
      WindowStatus &= ~(0x01);
      tick = 10*3;
    }
    else if(WindowStatus&0x02)
    {
      ui_setWindow(&window,140,80,LCD_COLOR_LIGHT_BLUE,LCD_COLOR_GRAY_2,"应用错误","内存分配失败！",0x00);
      ui_drawWindow(80,80,0x01,&window);
      WindowStatus &= ~(0x02);
      tick = 10*3;
    }
    if(tick>0)
      tick--;
    else
    {
      WindowStatus &= ~(0x80);
      ui_showDesktop(0x01);
    }
  }
}

uint8_t AppRegister(char* name, UI_Icon_t* icon, struct process* p)
{
  if(AppNum<=APP_ID_MAX)
  {
    APP_t App;
    App.id = AppNum;
    App.status = 0;
    App.name = name;
    App.icon = *icon;
    App.process = p;
    AppArray[AppNum] = App;
    AppNum++;
    return 0;
  }
  return 1;
}

void AppInit()
{
  UI_Icon_t icon;
  uint16_t wide = 70;
  uint16_t high = 70;
  
  AppNum = 0;
  ui_setIcon(&icon,wide,high,(uint8_t*)gImage_settings_60X60);
  AppRegister("设置",&icon,&ui_MenuProcess);
  
  ui_setIcon(&icon,wide,high,(uint8_t*)gImage_rf_60X60);
  AppRegister("无线信息",&icon,&ui_RfInfoProcess);
  
  ui_setIcon(&icon,wide,high,(uint8_t*)gImage_camera_60X60);
  AppRegister("相机",&icon,&cameraApp_process);
  
//  AppRegister("App4",&icon,NULL);
//  AppRegister("App5",&icon,NULL);
//  AppRegister("App6",&icon,NULL);
}

void ui_showApp(uint16_t x,uint16_t y,APP_t* app,uint8_t style)
{
  ui_setIconStyle(&app->icon,style);
  ui_drawIcon(x,y,&app->icon);
  LCDShowFont16(x+(app->icon.wide/2)-(strlen(app->name)*4),y+app->icon.high+2,
                app->name,strlen(app->name)*8,LCD_COLOR_BLACK,LCD_COLOR_WHITE);
}


void ui_drawSignal(uint16_t x,uint16_t y,uint8_t size,uint8_t status)
{
  if(size<10) return;
  uint16_t color = LCD_COLOR_WHITE;
  
  if(status)
  {
    LCD.FillRectangle(x,y,x+size-1,y+size-1,LCD_COLOR_BLACK);
    LCD.FillRectangle(x,y+(size/3*2),x+(size/5),y+size-1,color);
    LCD.FillRectangle(x+(size/5*2),y+(size/3),x+(size/5*3),y+size-1,color);
    LCD.FillRectangle(x+(size/5*4),y,x+size,y+size-1,color);
  }
  else
  {
    LCD_DrawLine(x,y,x+size-1,y+size-1,color);
    LCD_DrawLine(x+size-1,y,x,y+size-1,color);
  }
}

void ui_DrawBatteryIcon(uint16_t x,uint16_t y,uint8_t size,uint8_t quantity)
{
  if(size<6) return;
  uint16_t color = LCD_COLOR_WHITE;
  
  LCD_DrawRectangle(x,y,x+(size*2)-3,y+size-1,color);
  LCD_DrawRectangle(x+(size*2)-2,y+(size/3),x+(size*2)-1,y+(size/3*2),color);
  
  if(quantity > 100)
    quantity = 100;
  quantity = (size*2-6)*quantity/100;
  
  LCD.FillRectangle(x+2,y+2,x+quantity+2,y+size-3,color);
  LCD.FillRectangle(x+quantity+2,y+2,x+size*2-5,y+size-3,LCD_COLOR_BLACK);
}


//算术平均滤波
#define VOLTAGE_NUM 20
void ui_ShowBattery(uint16_t x,uint16_t y,uint8_t redraw)
{
  char pbuf[40] = {0};
  uint8_t quantity = 0;
  static float voltage = 0;
  static float voltageBuf[VOLTAGE_NUM] = {0};
  static float V_Max = 0,V_Min = 999;
  static uint8_t count = 0;
  
  if(V_Min == 999)
  {
    voltage = BatteryVotage_Get() - 8.6;
    quantity = (uint8_t)(voltage*100.0/4.0);
    
    sprintf(pbuf,"%3u",quantity);
    LCDShowFont16(x,y,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
    ui_DrawBatteryIcon(x+26,y+2,12,quantity);
  }
  else if(redraw)
  {
    quantity = (uint8_t)(voltage*100.0/4.0);
    
    sprintf(pbuf,"%3u",quantity);
    LCDShowFont16(x,y,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
    ui_DrawBatteryIcon(x+26,y+2,12,quantity);
  }
  
  voltageBuf[count] = BatteryVotage_Get() - 8.6;
  
  if(voltageBuf[count]<V_Min)
    V_Min = voltageBuf[count];
  if(voltageBuf[count]>V_Max)
    V_Max = voltageBuf[count];
  
  count++;
  if(count>VOLTAGE_NUM)
  {
    count = 0;
    
    voltage = 0;
    for(uint8_t i=0;i<VOLTAGE_NUM;i++)
    {
      voltage += voltageBuf[i];
    }
    voltage -= (V_Min+V_Max);
    voltage /= (VOLTAGE_NUM-2); 
    quantity = (uint8_t)(voltage*100.0/4.0);
    
    sprintf(pbuf,"%3u",quantity);
    LCDShowFont16(x,y,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
    ui_DrawBatteryIcon(x+26,y+2,12,quantity);
  }
}

void ui_showStatusBar(uint8_t redraw)
{
  uint16_t x=0;
  u16 textColor = LCD_COLOR_WHITE;
  u16 backColor = LCD_COLOR_BLACK;
  char pbuf[40] = {0};
  
  if(redraw)
  {
    LCD.FillRectangle(0,0,320,20,backColor);
  }
  
  ui_drawSignal(x+=1,3,15,RF1_hwTypeGet());
  ui_drawSignal(x+=20,3,15,RF2_hwTypeGet());
  ui_drawSignal(x+=20,3,15,RF3_hwTypeGet());
  ui_drawSignal(x+=20,3,15,RF4_hwTypeGet());
  
  sprintf(pbuf,"%02u:%02u:%02u",Calendar_GetHour(),Calendar_GetMinute(),Calendar_GetSecond());
  LCDShowFont16(160-(strlen(pbuf)*4),2,pbuf,strlen(pbuf)*8,textColor,backColor);
  
  ui_ShowBattery(270,2,redraw);
}

void ui_showHint(uint8_t redraw)
{
  u16 textColor = LCD_COLOR_YELLOW;
  u16 backColor = LCD_COLOR_BLACK;
  char pbuf[40] = {0};
  
  if(redraw)
  {
    UI_Bar_t bar;
    sprintf(pbuf,"K1:确认   K2:返回   K3:前移   K4:后移");
    ui_setBar(&bar,320,20,backColor,pbuf,textColor,0);
    ui_drawBar(0,220,redraw,&bar);
  }
}

void ui_showDesktop(uint8_t redraw)
{
  uint16_t x = 0;
  uint16_t y = 0;
  
  ui_showStatusBar(redraw);
  ui_showHint(redraw);
  
  if(redraw)
  {
    LCD.FillRectangle(0,20,320,220,LCD_COLOR_WHITE);
    x = 5;
    y = 25;
    for(uint8_t i=0;i<AppNum;i++)
    {
      ui_showApp(x,y,&AppArray[i],(i+1)==AppIndex?1:0);
      x+=80;
      if(x>310)
      {
        x = 10;
        y = 130;
      }
    }
  }
}


void ui_K1_Handle()
{
  AppCurrentID = AppIndex;
  if(AppCurrentID != 0)
    process_start(AppArray[AppCurrentID-1].process,NULL);
  else
  {
    ui_SetWindowStatus(WIN_STA_SHOW+WIN_STA_HINT);
  }
}

void ui_K2_Handle()
{
  AppIndex = 0;
}

void ui_K3_Handle()
{
  if(AppIndex>1)
    AppIndex--;
  else
    AppIndex = AppNum;
}

void ui_K4_Handle()
{
  AppIndex++;
  if(AppIndex>AppNum)
    AppIndex = 1;
}

void ui_KeyHandle(uint8_t keyStatus)
{
  switch(keyStatus)
  {
  case 0x01:ui_K1_Handle();
    break;
  case 0x02:ui_K2_Handle();
    break;
  case 0x04:ui_K3_Handle();
    break;
  case 0x08:ui_K4_Handle();
    break;
  }  
}


void uiDirverInit()
{
  LCD_DriverInit();
  LCD_Clear(LCD_COLOR_WHITE);
}

PROCESS(ui_process, "ui_process");

PROCESS_THREAD(ui_process, ev, data)
{
  static struct etimer uiPeriod_etimer; 
  static uint8_t redraw = 0;
  
  PROCESS_BEGIN();
  
  uiDirverInit();
  AppInit();
  ui_showDesktop(0x01);
  
  uiKey_event = process_alloc_event();
  uiRefresh_event = process_alloc_event();
  
  process_start(&CalendarProcess,NULL);
  
  process_post(&ui_process,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&uiPeriod_etimer,100);
      
      if(AppCurrentID != 0)
      {
        if(AppArray[AppCurrentID-1].process != NULL)
          process_post(AppArray[AppCurrentID-1].process,uiRefresh_event,data);
        else
          AppCurrentID = 0;
      }
      else
      {
        ui_showDesktop(redraw);
        if(redraw)
          redraw = 0;
      }
      WindowManage();
    }
    if(ev == key_event)
    {   
      if(AppCurrentID != 0)
      {
        if(AppArray[AppCurrentID-1].process != NULL)
          process_post(AppArray[AppCurrentID-1].process,uiKey_event,data);
        else
          AppCurrentID = 0;
      }
      else
      {
        ui_KeyHandle(*((uint8_t*)data));
        redraw = 1;
      }
    }
    if(ev == uiError_event)
    {
    }
  }
  
  PROCESS_END();
}

