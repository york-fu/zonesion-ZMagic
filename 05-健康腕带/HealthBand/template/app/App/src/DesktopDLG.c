
#include "DesktopDLG.h"

#include "SysCalendar.h"
#include "apl_picture.h"
#include "BatteryVoltage.h"
#include "fml_imu/lis3dh.h"


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_TEXT_0   (GUI_ID_USER + 0x01)
#define ID_TEXT_1   (GUI_ID_USER + 0x04)
#define ID_TEXT_2   (GUI_ID_USER + 0x05)
#define ID_TEXT_3   (GUI_ID_USER + 0x06)
#define ID_ICONVIEW_0   (GUI_ID_USER + 0x07)


// USER START (Optionally insert additional defines)
extern GUI_CONST_STORAGE GUI_BITMAP bmTmep_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmHeartRate_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmMore_60X60;

extern WM_HWIN CreateTemp(void);
extern WM_HWIN CreateBloodPressure(void);
extern WM_HWIN CreateHeartRate(void);
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)

typedef struct
{
  unsigned char activate;
  const GUI_BITMAP * pBitmap;
  const char * pText;
}Icon_t;

const Icon_t DesktopList[][4]={
  {
    {1,&bmTmep_60X60,"Temp"},
    {1,&bmHeartRate_60X60,"Heart"},
    {1,&bmMore_60X60,"Blood"},
    {1,&bmMore_60X60,"More"},
  },
  {
    {1,&bmTmep_60X60,"体温"},
    {1,&bmHeartRate_60X60,"心率"},
    {1,&bmMore_60X60,"血压"},
    {1,&bmMore_60X60,"更多"},
  },
};

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Desktop", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "time", ID_TEXT_0, 5, 20, 170, 60, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "date", ID_TEXT_1, 5, 80, 170, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "temp", ID_TEXT_2, 185, 30, 130, 30, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "step", ID_TEXT_3, 185, 65, 130, 30, 0, 0x64, 0 },
  { ICONVIEW_CreateIndirect, "Iconview", ID_ICONVIEW_0, 10, 130, 300, 80, WM_CF_HASTRANS, 0x0050003C, 0 },
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
  static short tick = 0;
  Calendar_t calendar;
  short index = -1;
  char buf[64] = {0};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Desktop'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00FF8080));
    //
    // Initialization of 'time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_D48);
    TEXT_SetText(hItem, "00:00");
    //
    // Initialization of 'date'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetText(hItem, "0000/00/00 week");
    //
    // Initialization of 'temp'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList24[System.font]);
    TEXT_SetText(hItem, "TEMP 000.0");
    //
    // Initialization of 'step'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList24[System.font]);
    TEXT_SetText(hItem, "STEP 00000");
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0);
    ICONVIEW_SetIconAlign(hItem, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
    ICONVIEW_SetBkColor(hItem,ICONVIEW_CI_BK,0xFF000000|GUI_LIGHTBLUE);
    ICONVIEW_SetTextColor(hItem,ICONVIEW_CI_UNSEL,GUI_DARKGRAY);
    ICONVIEW_SetTextColor(hItem,ICONVIEW_CI_SEL,GUI_BLACK);
    ICONVIEW_SetFrame(hItem, GUI_COORD_X, 0);//设置图标到IconView边框的间距
    ICONVIEW_SetFrame(hItem, GUI_COORD_Y, 0);
    ICONVIEW_SetSpace(hItem, GUI_COORD_X, (300-60*4)/3);//设置图标和图标之间的间距
    ICONVIEW_SetSpace(hItem, GUI_COORD_Y, 0);
    ICONVIEW_SetFont(hItem, FontList16[System.font]);
    for(short i=0;i<(GUI_COUNTOF(DesktopList[System.language]));i++)
    {
      if(DesktopList[System.language][i].activate == 1)
        ICONVIEW_AddBitmapItem(hItem, DesktopList[System.language][i].pBitmap, DesktopList[System.language][i].pText);//添加图标项
    }
    ICONVIEW_SetSel(hItem,-1);
		
    calendar = Calendar_Get();
    sprintf(buf,"%02u:%02u",calendar.hour,calendar.minute);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), buf);
    
    sprintf(buf,"%04u/%02u/%02u %s",calendar.year,calendar.month,calendar.day,WeekText[System.language][calendar.week]);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), buf);
    
    sprintf(buf,"TEMP %05.1f",CpuTemperature_Get());
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), buf);
    
    sprintf(buf,"STEP %05d",get_lis3dh_stepCount());
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), buf);
    // USER END
    break;
  case WM_DELETE:
    StatusBarSet(1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_ICONVIEW_0: // Notifications sent by 'Iconview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        index = ICONVIEW_GetSel(WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0));
        ICONVIEW_SetSel(WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0),-1);
        switch(index)
        {
        case 0:
          WM_DeleteWindow(pMsg->hWin);
          CreateTemp();
          break;
        case 1:
          WM_DeleteWindow(pMsg->hWin);
          CreateHeartRate();
          break;
        case 2:
          WM_DeleteWindow(pMsg->hWin);
          CreateBloodPressure();
          break;
        case 3:
          WM_DeleteWindow(pMsg->hWin);
          CreateAppList();
          break;
        }
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SCROLL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
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
  case WM_PAINT:
    break;
  case WM_TIMER:
    WM_RestartTimer(pMsg->Data.v, 50);
    calendar = Calendar_Get();
    if(calendar.second%2==0)
      sprintf(buf,"%02u:%02u",calendar.hour,calendar.minute);
    else
      sprintf(buf,"%02u %02u",calendar.hour,calendar.minute);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), buf);
    
    sprintf(buf,"%04u/%02u/%02u %s",calendar.year,calendar.month,calendar.day,WeekText[System.language][calendar.week]);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), buf);
    
    if(tick>9)
    {
      tick=0;
      sprintf(buf,"TEMP %5.1f",CpuTemperature_Get());
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), buf);
      
      sprintf(buf,"STEP %05d",get_lis3dh_stepCount());
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), buf);
    }
    tick++;
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
*       CreateDesktop
*/
WM_HWIN CreateDesktop(void);
WM_HWIN CreateDesktop(void) {
  WM_HWIN hWin;

  StatusBarSet(0);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x01, 50, 0); //创建一个软件定时器
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
