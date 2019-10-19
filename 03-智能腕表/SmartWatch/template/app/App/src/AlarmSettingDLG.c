
#include "DIALOG.h"
#include "AlarmClockDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x0A)
#define ID_LISTBOX_0   (GUI_ID_USER + 0x0C)
#define ID_CHECKBOX_0   (GUI_ID_USER + 0x0D)
#define ID_CHECKBOX_1   (GUI_ID_USER + 0x0E)
#define ID_CHECKBOX_2   (GUI_ID_USER + 0x0F)
#define ID_CHECKBOX_3   (GUI_ID_USER + 0x10)
#define ID_LISTWHEEL_0   (GUI_ID_USER + 0x11)
#define ID_LISTWHEEL_1   (GUI_ID_USER + 0x12)
#define ID_BUTTON_0   (GUI_ID_USER + 0x13)
#define ID_LISTVIEW_0   (GUI_ID_USER + 0x0A)


// USER START (Optionally insert additional defines)
extern WM_HWIN hWinCalendar;
static short AlarmSetIndex = 0;
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "AlamrSetting", ID_WINDOW_0, 0, 0, 320, 200, 0, 0x0, 0 },
  { LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_0, 10, 30, 210, 82, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "Checkbox1", ID_CHECKBOX_0, 230, 30, 80, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "Checkbox2", ID_CHECKBOX_1, 230, 50, 80, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "Checkbox3", ID_CHECKBOX_2, 230, 70, 80, 20, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "Checkbox4", ID_CHECKBOX_3, 230, 90, 80, 20, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "Listwheel hour", ID_LISTWHEEL_0, 10, 115, 100, 60, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "Listwheel minute", ID_LISTWHEEL_1, 120, 115, 100, 60, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "save", ID_BUTTON_0, 280, 0, 40, 25, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 230, 115, 80, 60, 0, 0x0, 0 },
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
  char Week[][7][6]={
    {"Sun.","Mon.","Tue.","Wed.","Thu.","Fri.","Sat.",},
    {"日","一","二","三","四","五","六",},
  };
  char buf[32];
  short i;
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'AlamrSetting'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, 0x00C0C0C0);
    //
    // Initialization of 'Listbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
    LISTBOX_SetItemSpacing(hItem,4);
    LISTBOX_SetTextAlign(hItem,GUI_TA_VCENTER);
    LISTBOX_SetFont(hItem, FontList16[System.font]);
    for(short i=0;i<ALARMCLOCK_NUM;i++)
    {
      sprintf(buf,"Alarm %d",i+1);
      LISTBOX_AddString(hItem, buf);
    }
    LISTBOX_SetSel(hItem,0);
    //
    // Initialization of 'Checkbox1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
    CHECKBOX_SetText(hItem, "ON");
    CHECKBOX_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'Checkbox2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
    CHECKBOX_SetText(hItem, "ON");
    CHECKBOX_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'Checkbox3'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
    CHECKBOX_SetText(hItem, "ON");
    CHECKBOX_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'Checkbox4'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
    CHECKBOX_SetText(hItem, "ON");
    CHECKBOX_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'Listwheel hour'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
    LISTWHEEL_SetFont(hItem, FontList20[System.font]);
    LISTWHEEL_SetSnapPosition(hItem,20);
    LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTWHEEL_SetTextColor(hItem,LISTWHEEL_CI_SEL,GUI_GREEN);
    LISTWHEEL_SetTextColor(hItem,LISTWHEEL_CI_UNSEL,GUI_GRAY);
    for(i=0;i<24;i++)
    {
      sprintf(buf,"%02u",i);
      LISTWHEEL_AddString(hItem, buf);
    }
    //
    // Initialization of 'Listwheel minute'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
    LISTWHEEL_SetFont(hItem,FontList20[System.font]);
    LISTWHEEL_SetSnapPosition(hItem,20);
    LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTWHEEL_SetTextColor(hItem,LISTWHEEL_CI_SEL,GUI_GREEN);
    LISTWHEEL_SetTextColor(hItem,LISTWHEEL_CI_UNSEL,GUI_GRAY);
    for(i=0;i<60;i++)
    {
      sprintf(buf,"%02u",i);
      LISTWHEEL_AddString(hItem, buf);
    }
    //
    // Initialization of 'save'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "Save");
    //
    // Initialization of 'Listview'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    LISTVIEW_AddColumn(hItem, 20, "1", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 20, "2", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 20, "3", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 20, "4", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_SetGridVis(hItem, 1);
    LISTVIEW_SetHeaderHeight(hItem, 0);
    LISTVIEW_SetRowHeight(hItem, 15);
    LISTVIEW_SetFont(hItem, FontList12[System.font]);
    for(short y=0;y<4;y++)
    {
      LISTVIEW_DisableRow(hItem,y);
    }
    // USER START (Optionally insert additional code for further widget initialization)
    for(short i=0;i<ALARMCLOCK_NUM;i++)
    {
      hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0+i);
      CHECKBOX_SetState(hItem,Alarm[i].uc_switch);
    }
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
    LISTWHEEL_SetSel(hItem,Alarm[AlarmSetIndex].hour);
    LISTWHEEL_MoveToPos(hItem,Alarm[AlarmSetIndex].hour);
  
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
    LISTWHEEL_SetSel(hItem,Alarm[AlarmSetIndex].minute);
    LISTWHEEL_MoveToPos(hItem,Alarm[AlarmSetIndex].minute);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    for(short i=0;i<7;i++)
    {
      if(Alarm[AlarmSetIndex].week & (1<<i))
        sprintf(buf,"ON");
      else
        sprintf(buf,"  ");
      
      if(i<4)
      {
        LISTVIEW_SetItemBkColor(hItem,i,0,LISTVIEW_CI_DISABLED,GUI_DARKGRAY);
        LISTVIEW_SetItemText(hItem, i, 0, Week[System.language][i]);
        LISTVIEW_SetItemText(hItem, i, 1, buf);
      }
      else
      {
        LISTVIEW_SetItemBkColor(hItem,i-4,2,LISTVIEW_CI_DISABLED,GUI_DARKGRAY);
        LISTVIEW_SetItemText(hItem, i-4, 2, Week[System.language][i]);
        LISTVIEW_SetItemText(hItem, i-4, 3, buf);
      }
    }
    LISTVIEW_SetItemBkColor(hItem,3,2,LISTVIEW_CI_DISABLED,GUI_DARKGRAY);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
    WM_SetFocus(hItem);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_LISTBOX_0: // Notifications sent by 'Listbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				AlarmSetIndex = LISTBOX_GetSel(WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0));
        
        if((AlarmSetIndex >= 0) && (AlarmSetIndex < ALARMCLOCK_NUM))
        {
          for(short i=0;i<ALARMCLOCK_NUM;i++)
          {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0+i);
            CHECKBOX_SetState(hItem,Alarm[i].uc_switch);
          }
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
          LISTWHEEL_SetSel(hItem,Alarm[AlarmSetIndex].hour);
          LISTWHEEL_MoveToPos(hItem,Alarm[AlarmSetIndex].hour);
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
          LISTWHEEL_SetSel(hItem,Alarm[AlarmSetIndex].minute);
          LISTWHEEL_MoveToPos(hItem,Alarm[AlarmSetIndex].minute);
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
          for(short i=0;i<7;i++)
          {
            if(Alarm[AlarmSetIndex].week & (1<<i))
              sprintf(buf,"ON");
            else
              sprintf(buf,"  ");
            
            if(i<4)
              LISTVIEW_SetItemText(hItem, i, 1, buf);
            else
              LISTVIEW_SetItemText(hItem, i-4, 3, buf);
          }
        }
        WM_InvalidateWindow(pMsg->hWin);
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
    case ID_CHECKBOX_0: // Notifications sent by 'Checkbox1'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
				Alarm[0].uc_switch=CHECKBOX_GetState(hItem);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_1: // Notifications sent by 'Checkbox2'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
				Alarm[1].uc_switch=CHECKBOX_GetState(hItem);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_2: // Notifications sent by 'Checkbox3'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
				Alarm[2].uc_switch=CHECKBOX_GetState(hItem);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_3: // Notifications sent by 'Checkbox4'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
				Alarm[3].uc_switch=CHECKBOX_GetState(hItem);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTWHEEL_0: // Notifications sent by 'Listwheel hour'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
        LISTWHEEL_SetSel(hItem,LISTWHEEL_GetPos(hItem));
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTWHEEL_1: // Notifications sent by 'Listwheel minute'
      WM_SetFocus(WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0));
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
        LISTWHEEL_SetSel(hItem,LISTWHEEL_GetPos(hItem));
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_0: // Notifications sent by 'save'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
        Alarm[AlarmSetIndex].hour = LISTWHEEL_GetPos(hItem);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
        Alarm[AlarmSetIndex].minute = LISTWHEEL_GetPos(hItem);
        
        Alarm[AlarmSetIndex].status = 0;
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_LISTVIEW_0: // Notifications sent by 'Listview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
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
    WM_SetFocus(WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0));
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
*       CreateAlamrSetting
*/
WM_HWIN CreateAlarmSetting(void);
WM_HWIN CreateAlarmSetting(void) {
  WM_HWIN hWin;

  AlarmSetIndex = 0;
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
