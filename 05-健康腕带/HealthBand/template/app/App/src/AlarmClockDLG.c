
#include "AlarmClockDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0   (GUI_ID_USER + 0x01)
#define ID_BUTTON_1   (GUI_ID_USER + 0x02)
#define ID_TEXT_0   (GUI_ID_USER + 0x03)
#define ID_TEXT_1   (GUI_ID_USER + 0x04)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
short AlarmIndex = -1;
Alarm_t Alarm[ALARMCLOCK_NUM*2]={
  {
    .hour = 8,
    .minute = 0,
    .uc_switch = 1,
    .week = 0x7f,
    .status = 0,
  },
  {
    .hour = 21,
    .minute = 0,
    .uc_switch = 1,
    .week = 0x7f,
    .status = 0,
  },
  {
    .hour = 12,
    .minute = 0,
    .uc_switch = 0,
    .week = 0x7f,
    .status = 0,
  },
  {
    .hour = 0,
    .minute = 0,
    .uc_switch = 0,
    .week = 0x00,
    .status = 0,
  },
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "AlarmClock", ID_FRAMEWIN_0, 20, 40, 280, 180, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "delay", ID_BUTTON_0, 20, 115, 80, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "close", ID_BUTTON_1, 175, 115, 80, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "time", ID_TEXT_0, 20, 10, 230, 40, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 20, 60, 230, 30, 0, 0x64, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  Calendar_t Calendar;
  char buf[32] = {0};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'AlarmClock'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTitleHeight(hItem, 20);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'delay'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'close'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList24[System.font]);
    TEXT_SetText(hItem, "00:00  week");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00000000));
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    Calendar = Calendar_Get();
    sprintf(buf,"%02d:%02d  %s",
            Alarm[AlarmIndex].hour,Alarm[AlarmIndex].minute,WeekText[System.language][Calendar.week]);
    TEXT_SetText(hItem, buf);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    sprintf(buf,"Alarm %d",AlarmIndex);
    TEXT_SetText(hItem, buf);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'delay'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_DeleteWindow(pMsg->hWin);
        if(AlarmIndex < ALARMCLOCK_NUM)
        {
          Calendar = Calendar_Get();
          Alarm[AlarmIndex*2].week = (1<<Calendar.week);
          Alarm[AlarmIndex*2].uc_switch = 1;
          Alarm[AlarmIndex*2].status = 0;
          
          Alarm[AlarmIndex*2].minute = Alarm[AlarmIndex].minute + 5;
          if(Alarm[AlarmIndex*2].minute > 59)
          {
            Alarm[AlarmIndex*2].minute -= 60;
            Alarm[AlarmIndex*2].hour = Alarm[AlarmIndex].hour + 1;
            if(Alarm[AlarmIndex*2].hour > 23)
            {
              Alarm[AlarmIndex*2].hour -= 24;
            }
          }
        }
        else
        {
          Alarm[AlarmIndex].week = (1<<Calendar.week);
          Alarm[AlarmIndex].uc_switch = 1;
          Alarm[AlarmIndex].status = 0;
          
          Alarm[AlarmIndex].minute = Alarm[AlarmIndex].minute + 5;
          if(Alarm[AlarmIndex].minute > 59)
          {
            Alarm[AlarmIndex].minute -= 60;
            Alarm[AlarmIndex].hour = Alarm[AlarmIndex].hour + 1;
            if(Alarm[AlarmIndex].hour > 23)
            {
              Alarm[AlarmIndex].hour -= 24;
              Alarm[AlarmIndex].week <<= 1;
              if(Alarm[AlarmIndex].week > 127)
              {
                Alarm[AlarmIndex].week = 1;
              }
            }
          }
        }
        process_post_synch(&AlarmProcess,AppCloseEvent,NULL);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'close'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_DeleteWindow(pMsg->hWin);
        process_post_synch(&AlarmProcess,AppCloseEvent,NULL);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  case WM_TIMER:
    WM_DeleteWindow(pMsg->hWin);
    process_post_synch(&AlarmProcess,AppCloseEvent,NULL);
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateAlarmClock
*/
WM_HWIN CreateAlarmClock(void);
WM_HWIN CreateAlarmClock(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x04, ALARMCLOCK_TIMEOUT, 0); //创建一个软件定时器
  FRAMEWIN_SetMoveable(hWin,1);
  WM_SetStayOnTop(hWin, 1);
  //WM_MakeModal(hWin);
  return hWin;
}

// USER START (Optionally insert additional public code)

uint8_t Alarm_SetSwitch(uint8_t index,uint8_t status)
{
  if(index > ALARMCLOCK_NUM) return 1;
  Alarm[index].uc_switch = status;
  return 0;
}

uint8_t Alarm_SetTime(uint8_t index,uint8_t hour,uint8_t minute)
{
  if(index> ALARMCLOCK_NUM ) return 1;
  Alarm[index].hour = hour;
  Alarm[index].minute = minute;
  return 0;
}

char Alarm_GetSwitch(uint8_t index)
{
  if(index > ALARMCLOCK_NUM) return -1;
  return (Alarm[index].uc_switch);
}

void Alarm_ZxbeeGet(char* buf)
{
  short len = 0;
  char buffer[64] = {0};
  char tempbuf[ALARMCLOCK_NUM][32] = {0};
  for(short i=0;i<ALARMCLOCK_NUM;i++)
  {
    sprintf(tempbuf[i],"%d/%d/%d/%d/%d/",
            i+1,Alarm[i].uc_switch,Alarm[i].week,Alarm[i].hour,Alarm[i].minute);
    strcat(buffer,tempbuf[i]);
  }
  len = strlen(buffer);
  for(short i=0;i<len;i++)
    buf[i] = buffer[i];
}

void Alarm_ZxbeeSet(char* str)
{
  short index = -1;
  Alarm_t alarm;
  char* ptr = str;
  
  index = atoi(ptr);
  if((index > 0) && (index <= ALARMCLOCK_NUM))
  {
    index -= 1;
    
    ptr = strstr((const char*)ptr,"/");
    alarm.uc_switch = atoi(++ptr);
    
    ptr = strstr((const char*)ptr,"/");
    alarm.week = atoi(++ptr);
    
    ptr = strstr((const char*)ptr,"/");
    alarm.hour = atoi(++ptr);
    
    ptr = strstr((const char*)ptr,"/");
    alarm.minute = atoi(++ptr);
    
    alarm.status = 0;
    Alarm[index] = alarm;
  }
}

static uint8_t CompareTime(uint8_t index,Calendar_t calendar)
{
  if((Alarm[index].week & (1<<calendar.week)) && 
     (Alarm[index].hour == calendar.hour) && 
     (Alarm[index].minute == calendar.minute))
  {
    return 1;
  }
  return 0;
}

static void AlarmStatusClean(uint8_t index,Calendar_t calendar)
{
  if((Alarm[index].week & (1<<calendar.week)) && 
     (Alarm[index].hour <= calendar.hour) && 
     ((Alarm[index].minute+1) <= calendar.minute))
  {
    Alarm[index].status = 0;
  }
}

static void AlarmPoll()
{
  Calendar_t Calendar = Calendar_Get();
  if(AlarmIndex == -1)
  {
    for(uint8_t i=0;i<(ALARMCLOCK_NUM*2);i++)
    {
      AlarmStatusClean(i,Calendar);
      if((Alarm[i].uc_switch) &&
         (Alarm[i].status == 0))
      {
        if(CompareTime(i,Calendar))
        {
          AlarmIndex = i;
          CreateAlarmClock();
        }
      }
    }
  }
}

PROCESS(AlarmProcess, "AlarmProcess");

PROCESS_THREAD(AlarmProcess, ev, data)
{
  static struct etimer et_Alarm; 
  
  PROCESS_BEGIN();
  process_post(&AlarmProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_Alarm,100);
      AlarmPoll();
    }
    if(ev == AppCloseEvent)
    {
      Alarm[AlarmIndex].status = 1;
      AlarmIndex = -1;
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
