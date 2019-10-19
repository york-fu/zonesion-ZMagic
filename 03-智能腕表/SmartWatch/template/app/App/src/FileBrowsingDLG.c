
#include "FileBrowsingDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x03)
#define ID_BUTTON_0   (GUI_ID_USER + 0x04)
#define ID_TREEVIEW_0   (GUI_ID_USER + 0x05)
#define ID_TEXT_0   (GUI_ID_USER + 0x06)
#define ID_BUTTON_1   (GUI_ID_USER + 0x0A)


// USER START (Optionally insert additional defines)
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
  { WINDOW_CreateIndirect, "FileBrowsing", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "back", ID_BUTTON_0, 0, 0, 40, 25, 0, 0x0, 0 },
  { TREEVIEW_CreateIndirect, "Treeview", ID_TREEVIEW_0, 5, 30, 310, 165, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 45, 0, 270, 25, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "share", ID_BUTTON_1, 130, 200, 60, 20, 0, 0x0, 0 },
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

#define FILE_PATH_MAX   512

typedef enum
{
  ALLFILE = 0,
  TEXTFILE,
  IMAGEFILE,
}FILE_TYPE;

/************************************************************************************
*函数:  ScanFiles
*参数:  path:遍历路径名
        hFile:记录文件路径的文件指针结构体
*返回:  None
*描述:  遍历此文件夹及子文件夹下所有文件
************************************************************************************/
static FRESULT ScanFiles(char* path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,FILE_TYPE fileType,u32 *fileNum)
{
  FRESULT res;
  FILINFO fno;
  DIR dir;
  int i;
  char *fn;   /* This function is assuming non-Unicode cfg. */
  TREEVIEW_ITEM_Handle hItem;

#if _USE_LFN
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持 /* Buffer to store the LFN */
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif

  res = f_opendir(&dir, (const TCHAR*)path);                       /* Open the directory */
  if (res == FR_OK)
  {
    i = strlen((const char*)path);
    for (;;)
    {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      if (*fn == '.') continue;             /* Ignore dot entry */ //忽略上级目录
      if (fno.fattrib & AM_DIR) /* It is a directory */
      {
        //if(hTree != NULL &&	hNode != NULL)
        {
          //目录，创建结点
          hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);
          //把结点加入到目录树中
          TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);
        }
        
        //在路径最后添加文件夹路径名
        sprintf(&path[i], "/%s", fn);
        //遍历此新文件夹下的文件
        res = ScanFiles(path,hTree,hItem,fileType,fileNum);
        if (res != FR_OK) break;//打开失败，跳出循环
        path[i] = 0;//末尾添加结束字符
      }
      else /* It is a file. */
      {
        if(fileType == TEXTFILE )
        {
          //判断如果不是txt文件，跳出本函数
          if(!(strstr(fn,".txt")||strstr(fn,".TXT")
               ||strstr(fn,".c")||strstr(fn,".c")
               ||strstr(fn,".cpp")||strstr(fn,".CPP")
               ||strstr(fn,".h")||strstr(fn,".h")))
          {
            return res;
          }
        }
        //判断如果不是Image文件，跳出本函数
        else if(fileType ==IMAGEFILE)
        {
          if(!(strstr(fn,".bmp")||strstr(fn,".BMP")||
               strstr(fn,".jpg")||strstr(fn,".JPG")||
               strstr(fn,".gif")||strstr(fn,".GIF")||
               strstr(fn,".png")||strstr(fn,".PNG")))
          {
            return res;
          }
        }
        /* 根据要求是否创建目录树 */
        //if(hTree != NULL &&	hNode != NULL)//创建目录树
        {
          hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);//文件，创建树叶
          TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);//把树叶添加到目录树
//                    (*fileNum)++;//记录文件数目
        }
        //else
        {
          (*fileNum)++;//记录文件数目
        }
      }

    }
    f_closedir(&dir);
  }
  return res;
}

void Fill_FileList(char *path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,FILE_TYPE fileType,u32 *fileNum,char flags)
{
  TREEVIEW_ITEM_Handle hTreeItem_Flash;
  TREEVIEW_ITEM_Handle hTreeItemCur;
  char p_path[FILE_PATH_MAX]= {0}; //目录名 指针

  hTreeItemCur=hNode;
  strcpy(p_path,path);    //复制目录名到指针
  ScanFiles(p_path,hTree,hNode,fileType,fileNum);    //递归扫描文件

  if(flags&0x01)
  {
    hTreeItem_Flash = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "Flash[1:]", 0);
    TREEVIEW_AttachItem(hTree, hTreeItem_Flash, hTreeItemCur, TREEVIEW_INSERT_BELOW);
    strcpy(p_path,"1:");    //复制目录名到指针
    ScanFiles(p_path,hTree,hTreeItem_Flash,fileType,fileNum);    //递归扫描文件
  }
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  TREEVIEW_ITEM_Handle   hTreeItemCur;
  TREEVIEW_ITEM_Handle   hTreeItemSDCard;
  WM_HWIN                hItem;
  int                    NCode;
  int                    Id;
  // USER START (Optionally insert additional variables)
  uint32_t FileNum = 0;
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'back'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    //
    // Initialization of 'Treeview'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
    TREEVIEW_SetAutoScrollV(hItem, 1);
    TREEVIEW_SetAutoScrollH(hItem, 1);
    TREEVIEW_SetSelMode(hItem, TREEVIEW_SELMODE_ROW);
    TREEVIEW_SetFont(hItem,FontList24[System.font]);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetText(hItem, "FileBrowsing");
    //
    // Initialization of 'share'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "Share");
    // USER START (Optionally insert additional code for further widget initialization)
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
    hTreeItemCur = TREEVIEW_GetItem(hItem, 0, TREEVIEW_GET_LAST);
    hTreeItemSDCard = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "SDCard[0:]", 0);
    TREEVIEW_AttachItem(hItem, hTreeItemSDCard, hTreeItemCur, TREEVIEW_INSERT_BELOW);
    
    Fill_FileList("0:",hItem,hTreeItemSDCard,ALLFILE,&FileNum,1);
    // USER END
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
        WM_DeleteWindow(pMsg->hWin);
        process_post_synch(&AppFileBrowsingProcess,AppCloseEvent,NULL);
          
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_TREEVIEW_0: // Notifications sent by 'Treeview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
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
    case ID_BUTTON_1: // Notifications sent by 'share'
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
  case WM_PAINT:
    WM_SetFocus(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0));
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
*       CreateFileBrowsing
*/
WM_HWIN CreateFileBrowsingDLG(void);
WM_HWIN CreateFileBrowsingDLG(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

void AppFileBrowsingProcessInit()
{
  if(bsp_result & BSP_SD)
  {
    /* 挂载文件系统 -- SD卡 */
    FRESULT f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive */
    if(f_result!=FR_OK)
    {
      ErrorDialog("FileBrowsing","SD card or FatFs error!");
      process_post(&AppFileBrowsingProcess,AppCloseEvent,NULL);
      return;
    }
    else 
      bsp_result &= ~BSP_SD;
  }
  
  CreateFileBrowsingDLG();
}

PROCESS(AppFileBrowsingProcess, "AppFileBrowsingProcess");

PROCESS_THREAD(AppFileBrowsingProcess, ev, data)
{
  //static struct etimer et_AppFileBrowsing; 
  
  PROCESS_BEGIN();
  
  AppFileBrowsingProcessInit();
  process_post(&AppFileBrowsingProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      //etimer_set(&et_AppFileBrowsing,100);
    }
    if(ev==AppCloseEvent)
    {
      CreateAppList();
      PROCESS_EXIT();
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
