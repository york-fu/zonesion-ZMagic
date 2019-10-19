#include "View.h"
#include "PhoneDLG.h"
#include "CameraDLG.h"
#include "MusicDLG.h"
#include "FingerprintSetDLG.h"

char GUI_ExecEnable = 1;

const GUI_FONT* FontList12[2]={
  GUI_FONT_13_ASCII,
  &FONT_XINSONGTI_12,
};

const GUI_FONT* FontList16[2]={
  GUI_FONT_16_ASCII,
  &FONT_XINSONGTI_16,
};

const GUI_FONT* FontList20[2]={
  GUI_FONT_20_ASCII,
  GUI_FONT_20_ASCII,
};

const GUI_FONT* FontList24[2]={
  GUI_FONT_24_ASCII,
  &FONT_XINSONGTI_24,
};


const char WeekText[][7][16]={
  {"Sun.","Mon.","Tue.","Wed.","Thu.","Fri.","Sat."},
  {"星期日","星期一","星期二","星期三","星期四","星期五","星期六"}
};
  

process_event_t ViewKeyEvent;

void GUI_SetExecEnable(char s)
{
  if(s)
    GUI_ExecEnable = 1;
  else
    GUI_ExecEnable = 0;
}
  
void ViewProcessInit()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//时钟使能 
  GUI_Init();//初始化emWin/ucGUI
  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_UC_SetEncodeUTF8();//使用UTF8编码
  
  if(Creat_XBF_Font() != 0)//创建XBF字体
  {
    System.font = 0;
    System.language = 0;
  }
  
  CreateStatusBar();
  CreateDesktop();
  process_start(&LockScreenProcess,NULL);
}


PROCESS(ViewProcess, "ViewProcess");

PROCESS_THREAD(ViewProcess, ev, data)
{
  static struct etimer et_View1; 
  static struct etimer et_View2; 
  
  PROCESS_BEGIN();
  
  ViewKeyEvent = process_alloc_event();
  ViewProcessInit();
  etimer_set(&et_View1,1);
  etimer_set(&et_View2,100);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      if(etimer_expired(&et_View1))
      {
        etimer_set(&et_View1,25);
        if(GUI_ExecEnable)
          GUI_Exec();
      }
      if(etimer_expired(&et_View2))
      {
        etimer_set(&et_View2,10);
        LCD.TouchScan();
      }
    }
    if(ev == key_event)
    {
      if(*((uint8_t*)data) == 0x01)
      {
        if(ProcessIsRun(&CameraProcess))
        {
          process_post(&CameraProcess,ViewKeyEvent,data);
        }
        else
        {
          PhoneOpenContacts();
        }
      }
      if(ProcessIsRun(&MusicProcess))
      {
        process_post(&MusicProcess,ViewKeyEvent,data);
      }
    }
  }
  
  PROCESS_END();
}