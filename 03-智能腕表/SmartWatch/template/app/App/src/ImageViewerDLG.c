
#include "ImageViewerDLG.h"

#include "apl_picture.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x05)
#define ID_BUTTON_0   (GUI_ID_USER + 0x06)
#define ID_BUTTON_1   (GUI_ID_USER + 0x07)
#define ID_BUTTON_2   (GUI_ID_USER + 0x08)
#define ID_BUTTON_3   (GUI_ID_USER + 0x09)
#define ID_TEXT_0   (GUI_ID_USER + 0x0A)
#define ID_TEXT_1   (GUI_ID_USER + 0x0B)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
ImageViewer_t* ImageViewer = NULL;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "ImageViewer", ID_WINDOW_0, 0, 0, 320, 240, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "<<", ID_BUTTON_0, 0, 0, 40, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "<", ID_BUTTON_1, 5, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, ">", ID_BUTTON_2, 290, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "share", ID_BUTTON_3, 140, 210, 40, 25, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "title", ID_TEXT_0, 50, 0, 180, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 240, 0, 80, 25, 0, 0x64, 0 },
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
	FRESULT fs_result = FR_OK;
	FIL fs_file;
	UINT ReadBytesNum;
  BitmapInfoHeader_t BitmapInfoHeader;
  short x,y;
  char buf[100] = {0};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'ImageViewer'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00C0C0C0));
    //
    // Initialization of '<<'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    //
    // Initialization of '<'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of '>'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    //
    // Initialization of 'share'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "Share");
    //
    // Initialization of 'title'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetText(hItem, "ImageViewer/File name");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetText(hItem, "0/0");
    // USER START (Optionally insert additional code for further widget initialization)
    if(ImageViewer->num > 0)
    {
      hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
      TEXT_SetText(hItem, ImageViewer->nameList[ImageViewer->index]);
      
      hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
      sprintf(buf,"%d/%d",ImageViewer->index,ImageViewer->num-1);
      TEXT_SetText(hItem, buf);
      WM_InvalidateWindow(pMsg->hWin);
    }
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
        lockScreenEnable(1);
        process_post_synch(&ImageViewerProcess,AppCloseEvent,NULL);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by '<'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(ImageViewer->num > 0)
        {
          if(ImageViewer->index > 0)
            ImageViewer->index--;
          else
            ImageViewer->index = ImageViewer->num-1;
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
          TEXT_SetText(hItem, ImageViewer->nameList[ImageViewer->index]);
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
          sprintf(buf,"%d/%d",ImageViewer->index,ImageViewer->num-1);
          TEXT_SetText(hItem, buf);
          WM_InvalidateWindow(pMsg->hWin);
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by '>'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(ImageViewer->num > 0)
        {
          if(ImageViewer->index < (ImageViewer->num-1))
            ImageViewer->index++;
          else
            ImageViewer->index = 0;
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
          TEXT_SetText(hItem, ImageViewer->nameList[ImageViewer->index]);
          
          hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
          sprintf(buf,"%d/%d",ImageViewer->index,ImageViewer->num-1);
          TEXT_SetText(hItem, buf);
          WM_InvalidateWindow(pMsg->hWin);
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'share'
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
  case WM_PAINT:
    x = 0,y = 0;
    fs_result = f_open(&fs_file, ImageViewer->pathList[ImageViewer->index], FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
    if(fs_result == FR_OK)
    {
      fs_result = f_lseek(&fs_file, 14);
      fs_result = f_read(&fs_file, &BitmapInfoHeader, 40, &ReadBytesNum);
      fs_result = f_close(&fs_file);
      
      if(BitmapInfoHeader.biWidth < PIC_WINSOW_WIDTH)
      {
        x = (PIC_WINSOW_WIDTH - BitmapInfoHeader.biWidth)/2;
      }
      
      if(BitmapInfoHeader.biHeight < 0)
        BitmapInfoHeader.biHeight = -BitmapInfoHeader.biHeight;
      if(BitmapInfoHeader.biHeight < PIC_WINSOW_HEIGHT)
      {
        y = (PIC_WINSOW_HEIGHT - BitmapInfoHeader.biHeight)/2;
      }
    }
    
    _ShowBMPEx((const char*)ImageViewer->pathList[ImageViewer->index],x,y);
    break;
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
*       CreateImageViewer
*/
WM_HWIN CreateImageViewer(void);
WM_HWIN CreateImageViewer(void) {
  WM_HWIN hWin;

  lockScreenEnable(0);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT ScanImageFiles (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
	char filePath[PICTURE_NAME_LEN];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 

  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
      if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //if(strstr(path,"recorder")!=NULL)continue;       //逃过录音文件
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录名
        res = ScanImageFiles(path);											//递归遍历 
        if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
        path[i] = 0; 
      } 
			else 
			{ 
				//printf("%s/%s\r\n", path, fn);								//输出文件名
				if(strstr(fn,".bmp")||strstr(fn,".BMP"))//判断文件
				{
					if ((strlen(path)+strlen(fn) < PICTURE_NAME_LEN) && (ImageViewer->num < PICTURE_NUM_MAX))
					{
						sprintf(filePath, "%s/%s", path, fn);						
						memcpy(ImageViewer->pathList[ImageViewer->num],filePath,strlen(filePath));
						memcpy(ImageViewer->nameList[ImageViewer->num],fn,strlen(fn));						
						ImageViewer->num++;//记录文件个数
					}
				}//if mp3||wav
      }//else
    } //for
  } 
  return res; 
}

void ImageViewerProcessInit()
{
  char path[256] = "0:";	
  FRESULT f_result = FR_OK;
  
  if(bsp_result & BSP_SD)
  {
    /* 挂载文件系统 -- SD卡 */
    f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive */
    if(f_result != FR_OK)
    {
      ErrorDialog("ImageViewer error","The SD drive cannot work!");
      process_post(&ImageViewerProcess,AppCloseEvent,NULL);
    }
    else
      bsp_result &= ~BSP_SD;
  }
  
  ImageViewer = (ImageViewer_t*)calloc(sizeof(ImageViewer_t),1);
  if(ImageViewer == NULL)
  {
    bsp_result |= BSP_ALLOC;
    ErrorDialog("ImageViewer error","The insufficient memory!");
    process_post(&ImageViewerProcess,AppCloseEvent,NULL);
  }
  else
    bsp_result &= ~BSP_ALLOC;
  
  if(!(bsp_result&BSP_SD) && !(bsp_result&BSP_ALLOC))
  {
    ImageViewer->index = 0;
    ScanImageFiles(path);
    DeleteStatusBar();
    CreateImageViewer();
  }
}


PROCESS(ImageViewerProcess, "ImageViewerProcess");

PROCESS_THREAD(ImageViewerProcess, ev, data)
{
  PROCESS_BEGIN();
  
  ImageViewerProcessInit();
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == AppCloseEvent)
    {
      free(ImageViewer);
      CreateStatusBar();
      CreateAppList();
      PROCESS_EXIT();
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
