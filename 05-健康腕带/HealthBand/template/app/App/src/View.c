#include "View.h"

char GUI_ExecEnable = 1;


const GUI_FONT* FontList16[2]={
  GUI_FONT_16_ASCII,
  &FONT_XINSONGTI_16,
};


const GUI_FONT* FontList24[2]={
  GUI_FONT_24_ASCII,
  &FONT_XINSONGTI_24,
};


const char WeekText[][7][16]={
  {"Sun.","Mon.","Tue.","Wed.","Thu.","Fri.","Sat."},
  {"星期日","星期一","星期二","星期三","星期四","星期五","星期六"}
};
  

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
}


PROCESS(ViewProcess, "ViewProcess");

PROCESS_THREAD(ViewProcess, ev, data)
{
  static struct etimer et_View1; 
  static struct etimer et_View2; 
  
  PROCESS_BEGIN();
  
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
  }
  
  PROCESS_END();
}