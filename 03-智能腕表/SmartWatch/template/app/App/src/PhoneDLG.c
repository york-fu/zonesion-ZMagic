
#include "PhoneDLG.h"
#include "fml_audio/audioAmplifier.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0   (GUI_ID_USER + 0x01)
#define ID_EDIT_0   (GUI_ID_USER + 0x02)
#define ID_BUTTON_1   (GUI_ID_USER + 0x03)
#define ID_BUTTON_2   (GUI_ID_USER + 0x04)
#define ID_BUTTON_3   (GUI_ID_USER + 0x05)
#define ID_BUTTON_4   (GUI_ID_USER + 0x06)
#define ID_BUTTON_5   (GUI_ID_USER + 0x07)
#define ID_BUTTON_6   (GUI_ID_USER + 0x08)
#define ID_BUTTON_7   (GUI_ID_USER + 0x09)
#define ID_BUTTON_8   (GUI_ID_USER + 0x0A)
#define ID_BUTTON_9   (GUI_ID_USER + 0x0B)
#define ID_BUTTON_10   (GUI_ID_USER + 0x0C)
#define ID_BUTTON_11   (GUI_ID_USER + 0x0D)
#define ID_BUTTON_12   (GUI_ID_USER + 0x0E)
#define ID_BUTTON_13   (GUI_ID_USER + 0x0F)
#define ID_BUTTON_14   (GUI_ID_USER + 0x10)
#define ID_TEXT_0   (GUI_ID_USER + 0x11)
#define ID_BUTTON_15   (GUI_ID_USER + 0x12)


// USER START (Optionally insert additional defines)
extern WM_HWIN CreateContacts(void);
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
static uint8_t PhoneWM = 0;
static uint8_t second = 0;
static uint8_t minute = 0;
static uint8_t hour = 0;
static uint8_t PhoneNumberIndex = 0;
char PhoneNumber[NUMBUF_SIZE] = {0};
PhoneContacts_t ContactsList[PHONE_CONTACTS_MAX] = {0};

const char PhoneCallText[][6][20]={
  {
    {"CALL"},
    {"HANG"},
    {"CONNECT"},
    {"C"},
    {"Call out..."},
    {"Call in..."},
  },
  {
    {"拨号"},
    {"挂断"},
    {"接通"},
    {"清除"},
    {"拨出..."},
    {"来电..."},
  },
};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "back", ID_BUTTON_0, 0, 0, 40, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "list", ID_BUTTON_15, 280, 0, 40, 25, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_0, 50, 5, 220, 40, 0, 0x32, 0 },
  { BUTTON_CreateIndirect, "k9-1", ID_BUTTON_1, 10, 70, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-2", ID_BUTTON_2, 85, 70, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-3", ID_BUTTON_3, 160, 70, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-4", ID_BUTTON_4, 10, 105, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-5", ID_BUTTON_5, 85, 105, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-6", ID_BUTTON_6, 160, 105, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-7", ID_BUTTON_7, 10, 140, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-8", ID_BUTTON_8, 85, 140, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k9-9", ID_BUTTON_9, 160, 140, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "*", ID_BUTTON_10, 235, 70, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "0", ID_BUTTON_11, 235, 105, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "#", ID_BUTTON_12, 235, 140, 75, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Phone", ID_BUTTON_13, 10, 175, 150, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "C", ID_BUTTON_14, 160, 175, 150, 35, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 80, 45, 160, 20, 0, 0x64, 0 },
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

static uint8_t PhoneNumber_Add(char value)
{
  if(PhoneNumberIndex<NUMBUF_SIZE)
  {
    PhoneNumber[PhoneNumberIndex] = value;
    PhoneNumberIndex++;
    PhoneNumber[PhoneNumberIndex] = 0;
    return 0;
  }
  return 1;
}

static uint8_t PhoneNumber_Delete()
{
  if(PhoneNumberIndex>0)
  {
    PhoneNumberIndex--;
    PhoneNumber[PhoneNumberIndex] = 0;
    return 0;
  }
  return 1;
}


static void CountTime()
{
  static uint8_t tick=0;
  tick++;
  if(tick>19)
  {
    tick=0;
    second++;
    if(second>59)
    {
      second = 0;
      minute++;
      if(minute>59)
      {
        minute = 0;
        hour++;
        if(hour>23)
          hour = 0;
      }
    }
  }
}

void ClearTime()
{
  second=0;
  minute=0;
  hour=0;
}


/*
static int _SkinDrawGreen(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
  char btnStrBuf[10] = {0};
  GUI_RECT rect;
  BUTTON_SKINFLEX_PROPS pProps;
  switch(pDrawItemInfo->Cmd)
  {
  case WIDGET_ITEM_DRAW_BACKGROUND:
    BUTTON_GetSkinFlexProps(&pProps, pDrawItemInfo->ItemIndex);
    if(pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
    {
      GUI_SetColor(0x00B000);
      GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, pProps.Radius);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, pDrawItemInfo->y0+2, pDrawItemInfo->x1-4, pDrawItemInfo->y1/2, 0xBBFBBB, 0x00F000);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, (pDrawItemInfo->y1-pDrawItemInfo->y0)/2, pDrawItemInfo->x1-4, pDrawItemInfo->y1-2, 0x00F000, 0x009000);
    }
    else
    {
      GUI_SetColor(0x00B000);
      GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, pProps.Radius);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, pDrawItemInfo->y0+2, pDrawItemInfo->x1-4, pDrawItemInfo->y1/2, 0xBBFBBB, 0x00F500);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, (pDrawItemInfo->y1-pDrawItemInfo->y0)/2, pDrawItemInfo->x1-4, pDrawItemInfo->y1-2, 0x00F000, 0x00B000);
    }
    GUI_SetColor(pProps.aColorFrame[1]);
    GUI_DrawRoundedRect(pDrawItemInfo->x0+1, pDrawItemInfo->y0+1, pDrawItemInfo->x1-1, pDrawItemInfo->y1-1, pProps.Radius);
    GUI_SetColor(pProps.aColorFrame[0]);
    GUI_DrawRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, pProps.Radius);
    break;
  case WIDGET_ITEM_DRAW_TEXT:
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    BUTTON_GetText(pDrawItemInfo->hWin, btnStrBuf, sizeof(btnStrBuf));
    rect.x0 = pDrawItemInfo->x0;
    rect.y0 = pDrawItemInfo->y0;
    rect.x1 = pDrawItemInfo->x1;
    rect.y1 = pDrawItemInfo->y1;
    GUI_SetColor(BUTTON_GetTextColor(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex));
    GUI_SetFont(BUTTON_GetFont(pDrawItemInfo->hWin));
    GUI__DispStringInRect(btnStrBuf, &rect, BUTTON_GetTextAlign(pDrawItemInfo->hWin), sizeof(btnStrBuf));
    break;
  default:
    return BUTTON_DrawSkinFlex(pDrawItemInfo);
  }
  return 0;
}

static int _SkinDrawRed(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
  char btnStrBuf[10] = {0};
  GUI_RECT rect;
  BUTTON_SKINFLEX_PROPS pProps;
  switch(pDrawItemInfo->Cmd)
  {
  case WIDGET_ITEM_DRAW_BACKGROUND:
    BUTTON_GetSkinFlexProps(&pProps, pDrawItemInfo->ItemIndex);
    if(pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
    {
      GUI_SetColor(0x0000B0);
      GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, pProps.Radius);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, pDrawItemInfo->y0+2, pDrawItemInfo->x1-4, pDrawItemInfo->y1/2, 0xBBBBFB, 0x0000F0);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, (pDrawItemInfo->y1-pDrawItemInfo->y0)/2, pDrawItemInfo->x1-4, pDrawItemInfo->y1-2, 0x0000F0, 0x000090);
    }
    else
    {
      GUI_SetColor(0x0000B0);
      GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, pProps.Radius);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, pDrawItemInfo->y0+2, pDrawItemInfo->x1-4, pDrawItemInfo->y1/2, 0xBBBBFB, 0x0000F5);
      GUI_DrawGradientV(pDrawItemInfo->x0+4, (pDrawItemInfo->y1-pDrawItemInfo->y0)/2, pDrawItemInfo->x1-4, pDrawItemInfo->y1-2, 0x0000F0, 0x0000B0);
    }
    GUI_SetColor(pProps.aColorFrame[1]);
    GUI_DrawRoundedRect(pDrawItemInfo->x0+1, pDrawItemInfo->y0+1, pDrawItemInfo->x1-1, pDrawItemInfo->y1-1, pProps.Radius);
    GUI_SetColor(pProps.aColorFrame[0]);
    GUI_DrawRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, pProps.Radius);
    break;
  case WIDGET_ITEM_DRAW_TEXT:
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    BUTTON_GetText(pDrawItemInfo->hWin, btnStrBuf, 10);
    rect.x0 = pDrawItemInfo->x0;
    rect.y0 = pDrawItemInfo->y0;
    rect.x1 = pDrawItemInfo->x1;
    rect.y1 = pDrawItemInfo->y1;
    GUI_SetColor(BUTTON_GetTextColor(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex));
    GUI_SetFont(BUTTON_GetFont(pDrawItemInfo->hWin));
    GUI__DispStringInRect(btnStrBuf, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER, 10);
    break;
  default:
    return BUTTON_DrawSkinFlex(pDrawItemInfo);
  }
  return 0;
}
*/

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
  static gsm_info_t phoneInfo;
  char buf[20];
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Window'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, 0x00C0C0C0);
    //
    // Initialization of 'back'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    //
    // Initialization of 'list'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_15);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "List");
    //
    // Initialization of 'Edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
    EDIT_SetText(hItem, "");
    EDIT_SetFont(hItem, FontList24[System.font]);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'k9-1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "1");
    //
    // Initialization of 'k9-2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "2");
    //
    // Initialization of 'k9-3'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "3");
    //
    // Initialization of 'k9-4'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "4");
    //
    // Initialization of 'k9-5'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "5");
    //
    // Initialization of 'k9-6'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "6");
    //
    // Initialization of 'k9-7'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "7");
    //
    // Initialization of 'k9-8'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_8);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "8");
    //
    // Initialization of 'k9-9'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_9);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "9");
    //
    // Initialization of '*'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_10);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "*");
    //
    // Initialization of '0'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_11);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "0");
    //
    // Initialization of '#'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_12);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "#");
    //
    // Initialization of 'Phone'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, " ");
    //
    // Initialization of 'C'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14);
    BUTTON_SetFont(hItem, FontList24[System.font]);
    BUTTON_SetText(hItem, "C");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, " ");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_DELETE:
    PhoneWM = 0;
    lockScreenEnable(1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'back'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(phoneInfo.phone_status == PHONE_ST_IDLE)
        {
          PhoneNumber[0]=0;
          PhoneNumberIndex=0;
          WM_DeleteWindow(pMsg->hWin);
          process_post_synch(&PhoneProcess,AppCloseEvent,NULL);
        }
        else
        {
          ErrorDialog("Phone","In the communication!");
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_15: // Notifications sent by 'list'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(PhoneOpenContacts() == 0)
        {
          WM_DeleteWindow(pMsg->hWin);
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_0: // Notifications sent by 'Edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
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
    case ID_BUTTON_1: // Notifications sent by 'k9-1'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('1');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'k9-2'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('2');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'k9-3'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('3');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_4: // Notifications sent by 'k9-4'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('4');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_5: // Notifications sent by 'k9-5'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('5');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_6: // Notifications sent by 'k9-6'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('6');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_7: // Notifications sent by 'k9-7'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('7');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_8: // Notifications sent by 'k9-8'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('8');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_9: // Notifications sent by 'k9-9'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('9');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_10: // Notifications sent by '*'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('*');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_11: // Notifications sent by '0'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('0');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_12: // Notifications sent by '#'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        PhoneNumber_Add('#');
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_13: // Notifications sent by 'Phone'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        switch(phoneInfo.phone_status)
        {
        case PHONE_ST_IDLE:
          if(PhoneNumberIndex>2)
          {
            gsm_request_call(PhoneNumber);
          }
          break;
        case PHONE_ST_COUT:
        case PHONE_ST_TALK:
          gsm_call_hangup();
          break;
        case PHONE_ST_RING:
          gsm_call_accept();
          break;
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_14: // Notifications sent by 'C'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(phoneInfo.phone_status==PHONE_ST_RING)
          gsm_call_hangup();
        else
          PhoneNumber_Delete();
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
    hItem = WM_GetDialogItem(WM_HBKWIN, ID_EDIT_0);
    WM_SetFocus(hItem);
    if(phoneInfo.phone_status==PHONE_ST_IDLE)
    {
      EDIT_SetText(hItem, PhoneNumber);
    }
    else
    {
      EDIT_SetText(hItem, phoneInfo.phone_number);
    }
    break;
  case WM_TIMER:
    WM_RestartTimer(pMsg->Data.v, 50);
    phoneInfo = lte_GetInof();
    if(phoneInfo.phone_status==PHONE_ST_TALK)
    {
      CountTime();
      sprintf(buf,"%02u:%02u:%02u",hour,minute,second);
    }
    else
      ClearTime();
    
    switch(phoneInfo.phone_status)
    {
    case PHONE_ST_IDLE:
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), " ");
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13), PhoneCallText[System.language][0]);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14), PhoneCallText[System.language][3]);
      break;
    case PHONE_ST_COUT:
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), PhoneCallText[System.language][4]);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13), PhoneCallText[System.language][1]);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14), PhoneCallText[System.language][3]);
      break;
    case PHONE_ST_TALK:
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), buf);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13), PhoneCallText[System.language][1]);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14), PhoneCallText[System.language][3]);
      break;
    case PHONE_ST_RING:
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), PhoneCallText[System.language][5]);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13), PhoneCallText[System.language][2]);
      BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14), PhoneCallText[System.language][1]);
      break;
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
*       CreateWindow
*/
WM_HWIN CreatePhone(void);
WM_HWIN CreatePhone(void) {
  WM_HWIN hWin;

  PhoneWM = 1;
  lockScreenEnable(0);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x03, 50, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

void Phone_ZxbeeGet(char* buf)
{
  short len = 0;
  char buffer[128] = {0};
  char tempbuf[PHONE_CONTACTS_MAX][32] = {0};
  for(short i=0;i<PHONE_CONTACTS_MAX;i++)
  {
    sprintf(tempbuf[i],"%d/%s/%s/",
            i+1,ContactsList[i].name,ContactsList[i].number);
    strcat(buffer,tempbuf[i]);
  }
  len = strlen(buffer);
  for(short i=0;i<len;i++)
    buf[i] = buffer[i];
}

void Phone_ZxbeeSet(char* str)
{
  short i=0,index = -1;
  PhoneContacts_t Contacts;
  char* ptr1 = str;
  char* ptr2 = str;
  
  index = atoi(ptr1);
  if((index > 0) && (index <= PHONE_CONTACTS_MAX))
  {
    index -= 1;
    ptr1 = strstr((const char*)ptr1,"/");
    ptr1++;
    
    ptr2 = strstr((const char*)ptr1,"/");
    while(ptr1 != ptr2)
    {
      Contacts.name[i++] = *ptr1++;
    }
    Contacts.name[i++] = 0;
    
    strcpy(Contacts.number,++ptr2);
    
    ContactsList[index] = Contacts;
  }
}

short PhoneOpenContacts()
{
  gsm_info_t phoneInfo = lte_GetInof();
  if(phoneInfo.phone_status == PHONE_ST_IDLE)
  {
    if(phoneInfo.simcard_status != 1)
    {
      ErrorDialog("Phone","SIM card error!");
    }
    ScreenUnlock();
    CreateContacts();
    return 0;
  }
  ErrorDialog("Phone","In the communication!");
  return 1;
}

void PhoneProcessInit()
{
  for(short i=0;i<PHONE_CONTACTS_MAX;i++)
  {
    sprintf(ContactsList[i].name,"name%d",i+1);
  }
}

void PhoneProcessPoll()
{
  gsm_info_t PhoneInfo = lte_GetInof();
  if(PhoneInfo.phone_status==PHONE_ST_RING)
  {
    if(!PhoneWM)
    {
      ScreenUnlock();
      CreatePhone();
    }
  }
  if(PhoneInfo.phone_status != PHONE_ST_IDLE)
  {
    if(EarphoneGet()==1)
    {
      LoudspeakerSet(1);
    }
    else
    {
      LoudspeakerSet(0);
    }
  }
  else
  {
    LoudspeakerSet(0);
  }
}


PROCESS(PhoneProcess, "PhoneProcess");

PROCESS_THREAD(PhoneProcess, ev, data)
{
  static struct etimer et_Phone; 
  
  PROCESS_BEGIN();
  
  PhoneProcessInit();
  process_post(&PhoneProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_Phone,100);
      PhoneProcessPoll();
    }
    if(ev==AppOpenEvent)
    {
      gsm_info_t PhoneInfo = lte_GetInof();
      if(PhoneInfo.simcard_status != 1)
      {
        ErrorDialog("Phone","SIM card error!");
      }
      CreatePhone();
    }
    if(ev==AppCloseEvent)
    {
      CreateDesktop();
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
