
#include "SettingDLG.h"
#include "FingerprintSetDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0   (GUI_ID_USER + 0x01)
#define ID_LISTBOX_0   (GUI_ID_USER + 0x02)
#define ID_TEXT_0   (GUI_ID_USER + 0x03)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
WM_HWIN hWinSetting;

const char SetItemList[][6][40]={
  {
    " System Info ",
    " Time Setup ",
    " Date Setup ",
    " Language Setup ",
    " Display Setup ",
    " Finger Setup ",
  },
  {
    " 系统信息 ",
    " 时间设置 ",
    " 日期设置 ",
    " 语言设置 ",
    " 显示设置 ",
    " 指纹设置 ",
  },
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Setting", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "<<", ID_BUTTON_0, 0, 0, 40, 25, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "title", ID_TEXT_0, 40, 0, 240, 25, 0, 0x64, 0 },
  { LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_0, 0, 25, 320, 195, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
#define ITEM_SIZEX  290
#define ITEM_SIZEY  30

static int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  unsigned char colorIndex = 0;
  char strBuf[100] = {0};
  int FontDistY = 0;
  GUI_COLOR aColor[2] = {GUI_BLACK, GUI_WHITE};
  GUI_COLOR bkColor[2] = {GUI_WHITE, GUI_BLUE};
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_GET_XSIZE:
    return ITEM_SIZEX;
    break;
  case WIDGET_ITEM_GET_YSIZE:
    return ITEM_SIZEY;
    break;
  case WIDGET_ITEM_DRAW:
    if(LISTBOX_GetSel(pDrawItemInfo->hWin) == pDrawItemInfo->ItemIndex)
      colorIndex = 1;
    else
      colorIndex = 0;
    GUI_SetBkColor(bkColor[colorIndex]);
    GUI_Clear();
    LISTBOX_GetItemText(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex, strBuf, sizeof(strBuf));
    FontDistY  = GUI_GetFontDistY();
    GUI_SetColor(aColor[colorIndex]);
    GUI_DispStringAt(strBuf, pDrawItemInfo->x0 + 0, pDrawItemInfo->y0 + (ITEM_SIZEY - FontDistY) / 2);
    GUI_SetColor(GUI_GRAY);
    if(LISTBOX_GetNumItems(pDrawItemInfo->hWin)-1 != pDrawItemInfo->ItemIndex)
      GUI_DrawHLine(pDrawItemInfo->y0 + ITEM_SIZEY - 1, pDrawItemInfo->x0 + 10, ITEM_SIZEX - 10);
    GUI_DispStringAt(">", pDrawItemInfo->x0 + ITEM_SIZEX - 2 * 10, pDrawItemInfo->y0 + (ITEM_SIZEY - FontDistY) / 2);
    break;
  default:
    LISTBOX_OwnerDraw(pDrawItemInfo);
    break;
  }
  return 0;
}
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
  short SettingIndex = -1;
  // USER END
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Setting'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, 0x00E9E9E9);
    //
    // Initialization of '<<'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    //
    // Initialization of 'title'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetText(hItem, "System Setup");
    //
    // Initialization of 'Listbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
    LISTBOX_SetFont(hItem, FontList16[System.font]);
    LISTBOX_SetTextAlign(hItem,GUI_TA_VCENTER);
    LISTBOX_SetAutoScrollV(hItem,1);
    LISTBOX_SetScrollbarWidth(hItem,20);
    LISTBOX_SetItemSpacing(hItem,14);
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
    for(short i=0;i<GUI_COUNTOF(SetItemList[System.language]);i++)
    {
      LISTBOX_AddString(hItem, SetItemList[System.language][i]);
    }
    LISTBOX_SetSel(hItem,-1);
    LISTBOX_SetOwnerDraw(hItem, _OwnerDraw);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by '<<'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_DeleteWindow(pMsg->hWin);
        process_post_synch(&SettingProcess,AppCloseEvent,NULL);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTBOX_0: // Notifications sent by 'Listbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
        SettingIndex = LISTBOX_GetSel(hItem);
        LISTBOX_SetSel(hItem,-1);
        switch(SettingIndex)
        {
        case 0:CreateSystemInfo();
          break;
        case 1:CreateTimeSetDLG();
          break;
        case 2:CreateDateSetDLG();
          break;
        case 3:CreateLanguageSet();
          break;
        case 4:CreateLockScreenSet();
          break;
        case 5:process_post_synch(&FingerprintProcess,AppOpenEvent,NULL);
          break;
        }
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
*       CreateSetting
*/
WM_HWIN CreateSetting(void);
WM_HWIN CreateSetting(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

void SettingProcessInit()
{
  hWinSetting = CreateSetting();
}


PROCESS(SettingProcess, "SettingProcess");

PROCESS_THREAD(SettingProcess, ev, data)
{
  //static struct etimer et_Setting; 
  
  PROCESS_BEGIN();
  
  SettingProcessInit();
  process_post(&SettingProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      //etimer_set(&et_Setting,100);
    }
    if(ev == AppInputEvent)
    {
      if(memcmp(data,"SetLanguage",strlen("SetLanguage")) == 0)
      {
        WM_DeleteWindow(hWinSetting);
        hWinSetting = CreateSetting();
      }
    }
    if(ev == AppCloseEvent)
    {
      CreateAppList();
      PROCESS_EXIT();
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
