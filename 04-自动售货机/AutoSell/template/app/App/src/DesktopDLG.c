
#include "DesktopDLG.h"
#include "SysCalendar.h"
#include "SellDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_ICONVIEW_0   (GUI_ID_USER + 0x01)
#define ID_TEXT_0   (GUI_ID_USER + 0x02)
#define ID_TEXT_1   (GUI_ID_USER + 0x03)
#define ID_BUTTON_0   (GUI_ID_USER + 0x04)
#define ID_BUTTON_1   (GUI_ID_USER + 0x05)


// USER START (Optionally insert additional defines)
extern GUI_CONST_STORAGE GUI_BITMAP bmpic01;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic02;
extern GUI_CONST_STORAGE GUI_BITMAP bmpic03;

extern WM_HWIN CreateScanDevice(void);
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
  const GUI_BITMAP * pBitmap;
  const char * pText;
}GoodsIcon_t;

const char TitleList[][50]={
  "Vending machine",
  "欢迎使用小智售货机"
};

const char Button1_Text[][10]={
  "bind",
  "绑定"
};

const char Button2_Text[][10]={
  "More",
  "更多"
};
  
const GoodsIcon_t GoodsListIcon[][3]={
  {
    {&bmpic01,"Drinks"},
    {&bmpic02,"Cola"},
    {&bmpic03,"Water"},
  },
  {
    {&bmpic01,"饮料"},
    {&bmpic02,"可乐"},
    {&bmpic03,"纯净水"},
  },
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Desktop", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { ICONVIEW_CreateIndirect, "Iconview ", ID_ICONVIEW_0, 10, 90, 300, 120, WM_CF_HASTRANS, 0x00640054, 0 },
  { TEXT_CreateIndirect, "title", ID_TEXT_0, 20, 0, 280, 30, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "time", ID_TEXT_1, 20, 40, 280, 20, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "bind", ID_BUTTON_0, 0, 0, 50, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "more", ID_BUTTON_1, 270, 0, 50, 30, 0, 0x0, 0 },
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
  char buf[64] = {0};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Desktop'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_LIGHTCYAN);
    //
    // Initialization of 'title'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList24[System.font]);
    TEXT_SetText(hItem, TitleList[System.language]);
    //
    // Initialization of 'time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList24[System.font]);
    TEXT_SetText(hItem, "00:00:00");
    //
    // Initialization of 'bind'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, Button1_Text[System.language]);
    //
    // Initialization of 'more'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, Button2_Text[System.language]);
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0);
    ICONVIEW_SetIconAlign(hItem, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
    ICONVIEW_SetBkColor(hItem,ICONVIEW_CI_BK,GUI_TRANSPARENT);
    ICONVIEW_SetTextColor(hItem,ICONVIEW_CI_UNSEL,GUI_BLACK);
    ICONVIEW_SetFont(hItem, FontList16[System.font]);
    ICONVIEW_SetFrame(hItem, GUI_COORD_X, 5);
    ICONVIEW_SetFrame(hItem, GUI_COORD_Y, (120-100)/2);
    ICONVIEW_SetSpace(hItem, GUI_COORD_X, (290-84*3)/2);
    ICONVIEW_SetSpace(hItem, GUI_COORD_Y, 0);
    for(short i=0;i<(GUI_COUNTOF(GoodsListIcon[System.language]));i++)
    {
      ICONVIEW_AddBitmapItem(hItem, GoodsListIcon[System.language][i].pBitmap, GoodsListIcon[System.language][i].pText);
    }
    ICONVIEW_SetSel(hItem,-1);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_ICONVIEW_0: // Notifications sent by 'Iconview '
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0);
        Sell.index = ICONVIEW_GetSel(hItem);
        ICONVIEW_SetSel(hItem,-1);
        if(Sell.index > -1)
        {
          WM_DeleteWindow(pMsg->hWin);
          process_post_synch(&SellProcess,AppOpenEvent,NULL);
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
    case ID_BUTTON_0: // Notifications sent by 'bind'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_DeleteWindow(pMsg->hWin);
        CreateScanDevice();
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'more'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_DeleteWindow(pMsg->hWin);
        CreateAppList();
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
  case WM_DELETE:
    StatusBarSet(1);
    break;
  case WM_TIMER:
    WM_RestartTimer(pMsg->Data.v,100);
    Calendar = Calendar_Get();
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    sprintf(buf,"%02u:%02u:%02u",Calendar.hour,Calendar.minute,Calendar.second);
    TEXT_SetText(hItem, buf);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0);
    for(short i=0;i<(GUI_COUNTOF(GoodsListIcon[System.language]));i++)
    {
      sprintf(buf,"%s(%d)",GoodsListIcon[System.language][i].pText,GoodsList[i].num);
      ICONVIEW_SetItemText(hItem,i,buf);
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
*       CreateDesktop
*/
WM_HWIN CreateDesktop(void);
WM_HWIN CreateDesktop(void) {
  WM_HWIN hWin;

  StatusBarSet(0);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x01, 100, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
