
#include "FTDLG.h"
#include "lte_zhiyun.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0  (GUI_ID_USER + 0x01)
#define ID_TEXT_0  (GUI_ID_USER + 0x02)
#define ID_PROGBAR_0  (GUI_ID_USER + 0x03)
#define ID_TEXT_1  (GUI_ID_USER + 0x04)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
FT_Status_t FT_Status = {
  .mode = FT_RECV,
  .status = FT_IDLE,
};

FileTransmit_t* FileTransmit = NULL;

short (*FTSendData)(char* dat) = NULL;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "FT", ID_FRAMEWIN_0, 20, 40, 280, 180, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "stop", ID_BUTTON_0, 95, 120, 80, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "title", ID_TEXT_0, 0, 5, 270, 30, 0, 0x64, 0 },
  { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 15, 50, 240, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 15, 85, 240, 20, 0, 0x64, 0 },
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
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'FT'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTitleHeight(hItem, 20);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
    FRAMEWIN_SetText(hItem, "FTWindow");
    //
    // Initialization of 'stop'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "Stop");
    //
    // Initialization of 'title'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "Send/Recv : FileName");
    //
    // Initialization of 'Progbar'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
    PROGBAR_SetFont(hItem, GUI_FONT_16B_ASCII);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_TOP);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetText(hItem, "Text");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'stop'
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
*       CreateFT
*/
WM_HWIN CreateFT(void);
WM_HWIN CreateFT(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)


void FTFailHandle()
{
  FT_Status.mode = FT_RECV;
  FT_Status.result = FT_FAIL;
  FT_Status.status = FT_IDLE;
  f_close(&FileTransmit->ftFile);
  free(FileTransmit->ftDataBuffer);
  free(FileTransmit);
}


short FTSendRequest()
{
  uint16_t num;
  char buf[32] = {0};
  
  num = sprintf(buf,"{V6=1}");
  buf[num]=0;
  return FTSendData(buf);
}

void FT_Connect()
{
  if(FTSendRequest() > 0)
    FT_Status.status = FT_CONNECT;
}

void FT_SendFileInit(char* path)
{
  FRESULT result = FR_OK;  
  
  FileTransmit = (FileTransmit_t*)calloc(sizeof(FileTransmit_t),1);
  if(FileTransmit == NULL)
  {
    printf("Memory error: FileTransmit == NULL !\r\n");
    return;
  }
  FileTransmit->ftDataBuffer = (char*)calloc(FTBufferSize,1);
  if(FileTransmit->ftDataBuffer == NULL)
  {
    printf("Memory error: FileTransmit->ftDataBuffer == NULL !\r\n");
    return;
  }
  
  FileTransmit->ftName = path;
  result = f_open(&FileTransmit->ftFile, FileTransmit->ftName, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
  if (result != FR_OK)
  {
    printf("File error: open '%s' fail!\r\n",FileTransmit->ftName);
    FTFailHandle();
    return;
  }
  
  FT_Status.mode = FT_SEND;
  
  if(FileTransmit->ftFile.fsize % FTBufferSize == 0)
    FileTransmit->ftPackMax = (uint32_t)FileTransmit->ftFile.fsize / FTBufferSize;
  else
    FileTransmit->ftPackMax = (uint32_t)FileTransmit->ftFile.fsize / FTBufferSize + 1;
  
  FT_Connect();
}

void AppFTProcessInit()
{
  FTSendData = zhiyun_send;
}

void AppFTProcessPoll()
{
}


PROCESS(AppFTProcess, "AppFTProcess");

PROCESS_THREAD(AppFTProcess, ev, data)
{
  static struct etimer et_AppFT; 
  
  PROCESS_BEGIN();
  
  AppFTProcessInit();
  process_post(&AppFTProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_AppFT,20);
      AppFTProcessPoll();
    }
    if(ev==key_event)
    {
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
