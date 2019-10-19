
#include "AppListDLG.h"
#include "MusicDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x03)
#define ID_BUTTON_0   (GUI_ID_USER + 0x04)
#define ID_ICONVIEW_0   (GUI_ID_USER + 0x05)


// USER START (Optionally insert additional defines)
extern GUI_CONST_STORAGE GUI_BITMAP bmapp_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmSetting_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmClock_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmCalendar_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusic_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmFile_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmPhotoAlbum_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmLock_60X60;
extern GUI_CONST_STORAGE GUI_BITMAP bmScan_60X60;

extern WM_HWIN CreateScanDevice(void);
extern WM_HWIN CreateClock(void);
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

const Icon_t IconList[][8]={
  {
    {1,&bmSetting_60X60,"Setting"},
    {1,&bmClock_60X60,"Clock"},
    {1,&bmCalendar_60X60,"Calendar"},
    {1,&bmMusic_60X60,"Music"},
    {1,&bmFile_60X60,"File"},
    {1,&bmPhotoAlbum_60X60,"Picture"},
    {1,&bmLock_60X60,"Lock"},
    {1,&bmScan_60X60,"Binding"},
  },
  {
    {1,&bmSetting_60X60,"设置"},
    {1,&bmClock_60X60,"时钟"},
    {1,&bmCalendar_60X60,"日历"},
    {1,&bmMusic_60X60,"音乐"},
    {1,&bmFile_60X60,"文件"},
    {1,&bmPhotoAlbum_60X60,"图片"},
    {1,&bmLock_60X60,"锁屏"},
    {1,&bmScan_60X60,"设备绑定"},
  },
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "AppList", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "<<", ID_BUTTON_0, 0, 0, 40, 25, 0, 0x0, 0 },
  { ICONVIEW_CreateIndirect, "Iconview", ID_ICONVIEW_0, 10, 30, 300, 180, WM_CF_HASTRANS, 0x00500040, 0 },
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
  short index = -1;
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'AppList'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_LIGHTGRAY);
    //
    // Initialization of '<<'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_0);
    ICONVIEW_SetIconAlign(hItem, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
    ICONVIEW_SetTextColor(hItem,ICONVIEW_CI_UNSEL,GUI_DARKGRAY);
    ICONVIEW_SetTextColor(hItem,ICONVIEW_CI_SEL,GUI_BLACK);
    ICONVIEW_SetFont(hItem, FontList16[System.font]);
    ICONVIEW_SetFrame(hItem, GUI_COORD_X, 0);//设置图标到IconView边框的间距
    ICONVIEW_SetFrame(hItem, GUI_COORD_Y, 0);
    ICONVIEW_SetSpace(hItem, GUI_COORD_X, (300-64*4)/3);//设置图标和图标之间的间距
    ICONVIEW_SetSpace(hItem, GUI_COORD_Y, (180-80*2));
    for(short i=0;i<(GUI_COUNTOF(IconList[System.language]));i++)
    {
      if(IconList[System.language][i].activate == 1)
        ICONVIEW_AddBitmapItem(hItem, IconList[System.language][i].pBitmap, IconList[System.language][i].pText);//添加图标项
    }
    ICONVIEW_SetSel(hItem,-1);
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
        CreateDesktop();
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
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
          process_start(&SettingProcess,NULL);
          break;
        case 1:
          WM_DeleteWindow(pMsg->hWin);
          CreateClock();
          break;
        case 2:
          WM_DeleteWindow(pMsg->hWin);
          CreateCalendar();
          break;
        case 3:
          WM_DeleteWindow(pMsg->hWin);
          if(ProcessIsRun(&MusicProcess))
            process_post(&MusicProcess,AppOpenEvent,NULL);
          else
            process_start(&MusicProcess,NULL);
          break;
        case 4:
          WM_DeleteWindow(pMsg->hWin);
          process_start(&AppFileBrowsingProcess,NULL);
          break;
        case 5:
          WM_DeleteWindow(pMsg->hWin);
          process_start(&ImageViewerProcess,NULL);
          break;
        case 6:
          process_post_synch(&LockScreenProcess,AppOpenEvent,NULL);
          break;
        case 7:
          WM_DeleteWindow(pMsg->hWin);
          CreateScanDevice();
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
*       CreateAppList
*/
WM_HWIN CreateAppList(void);
WM_HWIN CreateAppList(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
