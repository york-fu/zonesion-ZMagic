
#include "LockScreenDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_TEXT_0   (GUI_ID_USER + 0x01)
#define ID_TEXT_1   (GUI_ID_USER + 0x02)
#define ID_PROGBAR_0   (GUI_ID_USER + 0x03)
#define ID_BUTTON_0   (GUI_ID_USER + 0x04)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
char LockScreen = 0x80;//bit7:0-关闭锁屏，bit7:1-启用锁屏，bit1:0-未锁屏，bit1:1-锁屏中
short lockScreenTimeMax = 30;
short lockScreenTime = 30;
WM_HWIN LockhWin;

const char lockScreenText[][16]={
  {"Unlock>>"},
  {"滑动解锁"},
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "LockScreen", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "time", ID_TEXT_0, 20, 20, 280, 70, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "date", ID_TEXT_1, 20, 100, 280, 40, 0, 0x64, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 15, 160, 290, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 20, 160, 80, 50, 0, 0x0, 0 },
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
  Calendar_t calendar;
  char buf[64]={0};
	static short y,x[2] = {20};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'LockScreen'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, 0x00000000);
    //
    // Initialization of 'time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, 0x00FFFFFF);
    TEXT_SetFont(hItem, GUI_FONT_D64);
    TEXT_SetText(hItem, "00:00");
    //
    // Initialization of 'date'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, 0x00FFFFFF);
    TEXT_SetFont(hItem, FontList24[System.font]);
    TEXT_SetText(hItem, "0000/00/00 Week");
    //
    // Initialization of 'Progbar'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
    PROGBAR_SetFont(hItem, FontList16[System.font]);
    PROGBAR_SetText(hItem,"  ");
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, lockScreenText[System.language]);
    // USER START (Optionally insert additional code for further widget initialization)
    calendar = Calendar_Get();
    sprintf(buf,"%02u:%02u",calendar.hour,calendar.minute);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), buf);
    
    sprintf(buf,"%04u/%02u/%02u %s",calendar.year,calendar.month,calendar.day,WeekText[System.language][calendar.week]);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), buf);
    // USER END
    break;
  case WM_DELETE:
    StatusBarSet(1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
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
  case WM_TIMER://定时器消息,到时间时有效
    WM_RestartTimer(pMsg->Data.v, 25);
    calendar = Calendar_Get();
    if(calendar.second%2==0)
      sprintf(buf,"%02u:%02u",calendar.hour,calendar.minute);
    else
      sprintf(buf,"%02u %02u",calendar.hour,calendar.minute);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), buf);
    
    sprintf(buf,"%04u/%02u/%02u %s",calendar.year,calendar.month,calendar.day,WeekText[System.language][calendar.week]);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), buf);
		
    x[1] = GUI_TOUCH_X_MeasureX();
    y = GUI_TOUCH_X_MeasureY();
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    if((x[1]>=60)&&(x[1]<=260)&&(y>=180)&&(y<=230) && 
       (x[1] < (x[0]+80)))
    {
      x[0] = x[1];
      WM_MoveChildTo(hItem,x[1]-40,160);
      if(x[1]>240)
        process_post_synch(&LockScreenProcess,AppInputEvent,NULL);
    }
    else
    {
      x[0] = 20;
      WM_MoveChildTo(hItem,20,160);
    }
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
*       CreateLockScreen
*/
WM_HWIN CreateLockScreenDLG(void);
WM_HWIN CreateLockScreenDLG(void) {
  WM_HWIN hWin;

  StatusBarSet(0);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x02, 25, 0); //创建一个软件定时器
  //WM_SetStayOnTop(hWin, 1);
  //WM_MakeModal(hWin);
  return hWin;
}

// USER START (Optionally insert additional public code)
void lockScreenEnable(char s)
{
  if(s)
    LockScreen |= 0x80;
  else
    LockScreen &= ~0x80;
}

uint8_t LockScreenGet()
{
  return (((LockScreen&0x80) && (LockScreen&0x01)) ? 1 : 0);
}

void lockScreenTimeReset()
{
  lockScreenTime = lockScreenTimeMax;
}

void LockScreenTimeSet(short time)
{
  lockScreenTimeMax = time;
}

short LockScreenTimeGet()
{
  return lockScreenTimeMax;
}

void Screenlock()
{
  LockScreen |= 0x01;
  LockhWin = CreateLockScreenDLG();
}

void ScreenUnlock()
{
  lockScreenTime = lockScreenTimeMax;
  if((LockScreen&0x01))
  {
    LockScreen &= ~0x01;
    WM_DeleteWindow(LockhWin);
  }
}

void LockScreenProcessInit()
{
  if(LockScreen&0x80)
  {
    LockScreen |= 0x01;
    LockhWin = CreateLockScreenDLG();
  }
}

void LockScreenProcessPoll()
{
  if((LockScreen&0x80) && ((LockScreen&0x01) == 0))
  {
    lockScreenTime--;
    if(lockScreenTime == 0)
    {
      Screenlock();
    }
  }
}


PROCESS(LockScreenProcess, "LockScreenProcess");

PROCESS_THREAD(LockScreenProcess, ev, data)
{
  static struct etimer et_LockScreen; 
  
  PROCESS_BEGIN();
  
  TouchIrqSet(lockScreenTimeReset);
  if(LockScreen & 0x80)
  {
    Screenlock();
  }
  etimer_set(&et_LockScreen,1000);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_LockScreen,1000);
      LockScreenProcessPoll();
    }
    if(ev==AppInputEvent)
    {
      ScreenUnlock();
    }
    if(ev==AppOpenEvent)
    {
      Screenlock();
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
