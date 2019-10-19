
#include "MusicDLG.h"
#include "fml_audio/audioAmplifier.h"

// USER START (Optionally insert additional includes)
AppMusic_t* Music={NULL};
GUI_HMEM hMemMusic;
MusicStatus_t MusicStatus = STA_IDLE;
// USER END


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0   (GUI_ID_USER + 0x01)
#define ID_LISTBOX_0   (GUI_ID_USER + 0x02)
#define ID_LISTWHEEL_0   (GUI_ID_USER + 0x04)
#define ID_SLIDER_0   (GUI_ID_USER + 0x05)
#define ID_BUTTON_1   (GUI_ID_USER + 0x06)
#define ID_BUTTON_2   (GUI_ID_USER + 0x07)
#define ID_BUTTON_3   (GUI_ID_USER + 0x08)
#define ID_TEXT_0   (GUI_ID_USER + 0x09)
#define ID_TEXT_1   (GUI_ID_USER + 0x0A)
#define ID_BUTTON_4   (GUI_ID_USER + 0x0B)
#define ID_SLIDER_1   (GUI_ID_USER + 0x0D)
#define ID_TEXT_2   (GUI_ID_USER + 0x0F)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 20, 320, 220, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 0, 0, 40, 25, 0, 0x0, 0 },
  { LISTBOX_CreateIndirect, "Listbox", ID_LISTBOX_0, 5, 30, 155, 180, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "Listwheel", ID_LISTWHEEL_0, 165, 30, 150, 60, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_0, 165, 100, 150, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 165, 150, 30, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 220, 145, 40, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 285, 150, 30, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 165, 125, 40, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 275, 125, 40, 20, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 165, 190, 20, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "Slider", ID_SLIDER_1, 190, 190, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_2, 290, 190, 20, 20, 0, 0x64, 0 },
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
  short i;
  short temp;
  char buf[128];
  // USER END
  
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Window'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, 0x00C0C0C0);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    //
    // Initialization of 'Listbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
    LISTBOX_SetFont(hItem,FontList16[System.font]);
    LISTBOX_SetAutoScrollV(hItem,1);
    LISTBOX_SetAutoScrollH(hItem,1);
    LISTBOX_SetScrollbarWidth(hItem,16);
    LISTBOX_SetItemSpacing(hItem,9);
    LISTBOX_SetTextAlign(hItem,GUI_TA_VCENTER);
    //
    // Initialization of 'Listwheel'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
    LISTWHEEL_SetFont(hItem,FontList16[System.font]);
    LISTWHEEL_SetSnapPosition(hItem,16);
    LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTWHEEL_SetTextColor(hItem,LISTWHEEL_CI_SEL,GUI_GREEN);
    LISTWHEEL_SetTextColor(hItem,LISTWHEEL_CI_UNSEL,GUI_GRAY);
    LISTWHEEL_AddString(hItem, "lyrics");
    //
    // Initialization
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0);
    SLIDER_SetRange(hItem,1,0xff);
    SLIDER_SetValue(hItem,0);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "<<");
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "|>");
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, ">>");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, FontList12[System.font]);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "00:00");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, FontList12[System.font]);
    TEXT_SetText(hItem, "00:00");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, FontList16[System.font]);
    BUTTON_SetText(hItem, "))");
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
    SLIDER_SetRange(hItem,1,63);
    SLIDER_SetValue(hItem,1);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList12[System.font]);
    TEXT_SetText(hItem, "0%");
    // USER START (Optionally insert additional code for further widget initialization)
    if(Music!=NULL)
    {
      hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
      if(Music->file.num)
      {
        for(i=0;i<Music->file.num;i++)
        {
          GBK2UTF8((char*)Music->file.nameList[i],buf);
          LISTBOX_AddString(hItem,buf);
        }
        LISTBOX_SetSel(hItem,0);
      }
      else
      {
        LISTBOX_AddString(hItem,"no music file");
        LISTBOX_SetSel(hItem,-1);
      }
      
      SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1),Music->device.ucVolume);
      
      sprintf(buf,"%2d%%",Music->device.ucVolume*100/63);
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), buf);
    }
    // USER END
    break;
  case WM_DELETE:
    lockScreenEnable(1);
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
        WM_DeleteWindow(pMsg->hWin);
        process_post_synch(&MusicProcess,AppCloseEvent,NULL);
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
        temp = LISTBOX_GetSel(hItem);
        if(Music->play.index != temp)
        {
          Music->play.index = temp;
          LISTBOX_SetSel(hItem,Music->play.index);
          if(MusicStatus == STA_PLAYING)
          {
            MusicStatus = STA_SWITCH;
          }
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
    case ID_LISTWHEEL_0: // Notifications sent by 'Listwheel'
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
    case ID_SLIDER_0: // Notifications sent by 'Slider'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        Music->play.flags |= (1<<14);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        Music->play.flags &= ~(1<<14);
        Music->play.flags |= SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0));
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
    case ID_BUTTON_1: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(Music->play.index>0)
          Music->play.index--;
        else 
          Music->play.index=Music->file.num-1;
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
        LISTBOX_SetSel(hItem,Music->play.index);
        
        if(MusicStatus != STA_IDLE) 
        {
          MusicStatus = STA_SWITCH;
        }
        // USER END
        break;
        // USER START (Optionally insert additional code for further notification handling)
        // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(MusicStatus == STA_PLAYING)
          MusicStatus = STA_PAUSE;
        else
          MusicStatus = STA_PLAYING;
        process_post(&MusicProcess,AppInputEvent,NULL);
        // USER END
        break;
        // USER START (Optionally insert additional code for further notification handling)
        // USER END
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(Music->play.index<(Music->file.num-1))
          Music->play.index++;
        else 
          Music->play.index=0;
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
        LISTBOX_SetSel(hItem,Music->play.index);
        
        if(MusicStatus != STA_IDLE) 
        {
          MusicStatus = STA_SWITCH;
        }
        // USER END
        break;
        // USER START (Optionally insert additional code for further notification handling)
        // USER END
      }
      break;
    case ID_BUTTON_4: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
        Music->device.ucMute = !Music->device.ucMute;
        if(Music->device.ucMute == 0)
          BUTTON_SetText(hItem, "))");
        else
          BUTTON_SetText(hItem, "X");
        wm8978_OutMute(Music->device.ucMute);
        // USER END
        break;
        // USER START (Optionally insert additional code for further notification handling)
        // USER END
      }
      break;
    case ID_SLIDER_1: // Notifications sent by 'Slider'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1);
        Music->device.ucVolume = SLIDER_GetValue(hItem);
        
        sprintf(buf,"%2d%%",Music->device.ucVolume*100/63);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2),buf);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
        if(Music->device.ucVolume == 1)
        {
          BUTTON_SetText(hItem, "X");
          Music->device.ucMute=1;
          wm8978_OutMute(1);
        }
        else
        {
          BUTTON_SetText(hItem, "))");
          Music->device.ucMute=0;
          wm8978_OutMute(0);
          /* 调节音量，左右相同音量 */
          wm8978_SetOUT1Volume(Music->device.ucVolume);
          wm8978_SetOUT2Volume(Music->device.ucVolume);
        }
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
      // USER START (Optionally insert additional code for further Ids)
      // USER END
    }
    break;
  case WM_USERMSG_PLAYSTART:
    sprintf(buf,"%02d:%02d",Music->play.allTime/60,Music->play.allTime%60);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1),buf);
    break;
  case WM_USERMSG_PLAYOVER:
    SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0),0);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0),"00:00");
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1),"00:00"); 
    break;
  case WM_USERMSG_KEYEVENT:
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    if(Music->device.ucMute == 0)
      BUTTON_SetText(hItem, "))");
    else
      BUTTON_SetText(hItem, "X");
    SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, ID_SLIDER_1),Music->device.ucVolume);
    sprintf(buf,"%2d%%",Music->device.ucVolume*100/63);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), buf);
    break;
  case WM_PAINT:
    WM_SetFocus(WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0));
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_0);
    LISTBOX_SetSel(hItem,Music->play.index);
    break;
  case WM_TIMER:
    WM_RestartTimer(pMsg->Data.v, 50);
    if(Music!=NULL)
    {
      if((Music->play.flags&(1<<14))==0)
      {
        sprintf(buf,"%02d:%02d",Music->play.currTime/60,Music->play.currTime%60);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0),buf);                  //更新时间显示 
        SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, ID_SLIDER_0),Music->play.currTime*255/Music->play.allTime); //更新时间进度条
      }
      if(MusicStatus == STA_PLAYING)
      {
        BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2), "||");
      }
      else
      {
        BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2), "|>");
      }
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
WM_HWIN CreateMusicDLG(void);
WM_HWIN CreateMusicDLG(void) {
  WM_HWIN hWin;
  
  lockScreenEnable(0);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x05, 50, 0); //创建一个软件定时器
  return hWin;
}



/**************************************************************************************************************************/

/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT ScanMusicFiles (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
	char filePath[FILE_NAME_LEN];	
	
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
      if(strstr(path,"recorder")!=NULL)continue;       //逃过录音文件
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录名
        res = ScanMusicFiles(path);									//递归遍历 
        if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
        path[i] = 0; 
      } 
			else 
			{ 
				//printf("%s/%s\r\n", path, fn);								//输出文件名
				if(strstr(fn,".wav")||strstr(fn,".WAV")/*||strstr(fn,".mp3")||strstr(fn,".MP3")*/)//判断是否mp3或wav文件
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(Music->file.num<MUSIC_MAX_NUM))
					{
						sprintf(filePath, "%s/%s", path, fn);						
						memcpy(Music->file.pathList[Music->file.num],filePath,strlen(filePath));
						memcpy(Music->file.nameList[Music->file.num],fn,strlen(fn));						
						Music->file.num++;//记录文件个数
					}
				}//if mp3||wav
      }//else
    } //for
  } 
  return res; 
}



void MUSIC_I2S_DMA_TX_Callback(void)
{
	if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //当前读取Memory1数据
	{
		Music->play.bufFlag=0;
	}
	else
	{
		Music->play.bufFlag=1;
	}
	Music->play.IsReady=1;
}


void MusicIrqHandle()
{
//	if((MusicStatus==STA_PLAYING))
//	{
//    if(Music->play.IsReady)
//    {
//      Music->play.IsReady=0;
//      if(Music->play.bufFlag==0)
//      {
//        Music->file.fs_result = f_read(&Music->file.fs_file,Music->play.dataBuffer[0],BUFFER_SIZE,&Music->file.fs_num);
//      }
//      else
//      {
//        Music->file.fs_result = f_read(&Music->file.fs_file,Music->play.dataBuffer[1],BUFFER_SIZE,&Music->file.fs_num);
//      }
//      
//      if((Music->file.fs_result!=FR_OK)||(Music->file.fs_file.fptr==Music->file.fs_file.fsize))
//      {
//        I2S_Play_Stop();
//        if(Music->hWin != 0)
//          WM_SendMessageNoPara(Music->hWin,WM_USERMSG_PLAYOVER);
//        
//        printf("播放完或者读取出错退出...\r\n");
//        if(Music->play.index<Music->file.num)
//          Music->play.index++;
//        else 
//          Music->play.index=0;
//        
//        MusicStatus = STA_SWITCH;
//        Music->play.type=0;
//        Music->play.flags=0;
//        
//        Music->file.fs_result=FR_OK;
//        Music->file.fs_file.fptr=0;
//        f_close(&Music->file.fs_file);
//        I2S_Stop();		/* 停止I2S录音和放音 */
//        wm8978_Reset();	/* 复位WM8978到复位状态 */
//        return ;
//      }
//    }
//  }
  process_poll(&MusicProcess);
}


/**
  * @brief   WAV格式音频播放主程序
	* @note   
  * @param  无
  * @retval 无
  */
void wavplayer(const char *wavfile,WM_HWIN hWin)
{
	WavHead wav;
  DWORD pos;
  static short count=0;

  if((Music->play.flags&(1<<15))==0)
  {
    Music->play.flags|=1<<15;
    count=0;
    
    Music->file.fs_result=f_open(&Music->file.fs_file,wavfile,FA_READ);
    if(Music->file.fs_result!=FR_OK)
    {
      printf("Open wavfile fail!!!->%d\r\n",Music->file.fs_result);
      Music->file.fs_result = f_close (&Music->file.fs_file);
      return;
    }
    Music->file.fs_result = f_read(&Music->file.fs_file,&wav,sizeof(WavHead),&Music->file.fs_num);//读取WAV文件头
    
    /* 初始化全局变量 */
    Music->device.ucFreq =  wav.dwSamplesPerSec;
    Music->device.ucbps =  Music->device.ucFreq*32;
    
    MusicStatus = STA_PLAYING;		/* 放音状态 */
    Music->play.allTime = Music->file.fs_file.fsize*8/Music->device.ucbps;
    Music->play.currTime = 0;
    Music->play.IsReady = 0;
    Music->play.bufFlag = 0;
    
    if(hWin != 0)
      WM_SendMessageNoPara(hWin,WM_USERMSG_PLAYSTART);
    
    wm8978_Reset();		/* 复位WM8978到复位状态 */		
    delay_ms(10);
    /* 配置WM8978芯片，输入为DAC，输出为喇叭 */
    wm8978_CfgAudioPath(DAC_ON, SPK_ON);
    /* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
    wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
    /* 调节音量，左右相同音量 */
    wm8978_SetOUT1Volume(Music->device.ucVolume);
    wm8978_SetOUT2Volume(Music->device.ucVolume);
    
    I2S_GPIO_Config();
    I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Music->device.ucFreq);
    I2S_DMA_TX_Callback=MUSIC_I2S_DMA_TX_Callback;			//回调函数指wav_i2s_dma_callback
    I2S_Play_Stop();
    
    Music->file.fs_result = f_read(&Music->file.fs_file,Music->play.dataBuffer[0],BUFFER_SIZE,&Music->file.fs_num);
    Music->file.fs_result = f_read(&Music->file.fs_file,Music->play.dataBuffer[1],BUFFER_SIZE,&Music->file.fs_num);
    
    I2Sx_TX_DMA_Init((const uint16_t*)Music->play.dataBuffer[0],(const uint16_t*)Music->play.dataBuffer[1],BUFFER_SIZE/2);
    I2S_Play_Start();
  }
	/* 进入主程序循环体 */
	if((MusicStatus == STA_PLAYING))
	{
    if(Music->play.IsReady)
    {
      Music->play.IsReady=0;
      if(Music->play.bufFlag==0)
      {
        Music->file.fs_result = f_read(&Music->file.fs_file,Music->play.dataBuffer[0],BUFFER_SIZE,&Music->file.fs_num);
      }
      else
      {
        Music->file.fs_result = f_read(&Music->file.fs_file,Music->play.dataBuffer[1],BUFFER_SIZE,&Music->file.fs_num);
      }
      
      if((Music->file.fs_result!=FR_OK)||(Music->file.fs_file.fptr==Music->file.fs_file.fsize))
      {
        I2S_Play_Stop();
        if(Music->hWin != 0)
          WM_SendMessageNoPara(Music->hWin,WM_USERMSG_PLAYOVER);
        
        printf("播放完或者读取出错退出...\r\n");
        if(Music->play.index<Music->file.num)
          Music->play.index++;
        else 
          Music->play.index=0;
        
        MusicStatus = STA_SWITCH;
        Music->play.type=0;
        Music->play.flags=0;
        
        Music->file.fs_result=FR_OK;
        Music->file.fs_file.fptr=0;
        f_close(&Music->file.fs_file);
        I2S_Stop();		/* 停止I2S录音和放音 */
        wm8978_Reset();	/* 复位WM8978到复位状态 */
        return ;
      }
    }
    else
    {
      if((Music->play.flags&0xff)==0) 
      { 
        if(count>9)
        {
          count=0;
          Music->play.currTime=Music->file.fs_file.fptr*8/Music->device.ucbps;                                //获取当前播放进度(单位：s)
        }
      }
      else
      {
        uint8_t temp=Music->play.flags&0xff;
        pos=Music->file.fs_file.fsize/255*temp;
        if(pos<sizeof(WavHead))pos=sizeof(WavHead);
        if(wav.wBitsPerSample==24)temp=12;
        else temp=8;
        if((pos-sizeof(WavHead))%temp)
        {
          pos+=temp-(pos-sizeof(WavHead))%temp;
        }        
        f_lseek(&Music->file.fs_file,pos);
        Music->play.flags &= ~0xff;
      }
    }
    count++;
	}
}



void MusicProcessExit()
{
  APP_TRACE_DBG(("MusicDLG delete\n"));
  I2S_Stop();		/* 停止I2S录音和放音 */
  wm8978_Reset();	/* 复位WM8978到复位状态 */
#if 0
  GUI_ALLOC_Free(hMemMusic);
#else
  free(Music);
#endif
  Music = NULL;
}

void MusicProcessInit()
{
  char str[128]={0};
  char path[512]="0:";	
  FRESULT f_result=FR_OK;

  APP_TRACE_DBG(("MusicDLG create\n"));
    
  if(bsp_result & BSP_SD)
  {
    /* 挂载文件系统 -- SD卡 */
    f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive */
    if(f_result == FR_OK)
      bsp_result &= ~BSP_SD;
  }
    
  /* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
  if (wm8978_Init()!=0)
    bsp_result|=BSP_WM8978;
  else
    bsp_result&=~BSP_WM8978;  
#if 0
  hMemMusic = NULL;
	hMemMusic = GUI_ALLOC_AllocZero(sizeof(AppMusic_t));
  if(hMemMusic == NULL)
    bsp_result|=BSP_ALLOC;
  else
  {
    Music = GUI_ALLOC_h2p(hMemMusic);
    if(Music == NULL)
      bsp_result|=BSP_ALLOC;
    else
      bsp_result&=~BSP_ALLOC;
  }
#else
  Music = (AppMusic_t*)calloc(sizeof(AppMusic_t),1);
  if(Music == NULL)
  {
    bsp_result|=BSP_ALLOC;
  }
  else
  {
    bsp_result&=~BSP_ALLOC;
  }
#endif
  if((bsp_result&BSP_SD)||(bsp_result&BSP_WM8978)||(bsp_result&BSP_ALLOC))
  {		
    if(bsp_result&BSP_WM8978)
      sprintf(str,"The WM8978 drive cannot work!");
    else if(bsp_result&BSP_SD)
      sprintf(str,"The SD drive cannot work!");
    else if(bsp_result&BSP_ALLOC)
      sprintf(str,"The insufficient memory!");
    
    ErrorDialog("Music error",str);
    process_post(&MusicProcess,AppCloseEvent,NULL);
  }
  else
  {
    Music->device.ucMute = 0;
    Music->device.ucVolume = 31;  /* 缺省音量 */
    Music->device.ucSpk = 1;
    MusicStatus = STA_IDLE;   /* 待机状态 */
    
    ScanMusicFiles(path);
    
    Music->hWin = CreateMusicDLG();
  }
}


void MusicProcessPoll()
{
  if(MusicStatus == STA_PLAYING)
  {  
    if(Music->play.type!=0)
    {
      switch(Music->play.type)
      {
      case 1:wavplayer((char const*)Music->play.file,Music->hWin);
        break;
      case 2://mp3 play
        break;
      }
    }
    else
    {    
      strcpy((char*)Music->play.file, Music->file.pathList[Music->play.index]);
      
      if(strstr((char const*)Music->play.file,".wav")||strstr((char const*)Music->play.file,".WAV"))
      {
        Music->play.type=1;
      }
      else if(strstr((char const*)Music->play.file,".mp3")||strstr((char const*)Music->play.file,".MP3"))
      {
        Music->play.type=2;
      }
      else
      {
        Music->play.type=0;
      }
      Music->play.flags=0;
    }
  }
  else if(MusicStatus == STA_SWITCH)
  {	
    MusicStatus = STA_PLAYING;
    Music->play.type=0;
    Music->play.flags=0;
  }
  if(EarphoneGet())
  {
    Music->device.ucSpk = 1;
    wm8978_CfgAudioPath(DAC_ON, SPK_ON);
  }
  else
  {
    Music->device.ucSpk = 0;
    wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
  }
}

void MusicKeyHandle(uint8_t val)
{
  switch(val)
  {
  case 0x02:
    {
      if(Music->device.ucVolume < 57)
        Music->device.ucVolume += 6;
      else
        Music->device.ucVolume = 63;
    }
    break;
  case 0x04:
    {
      if(Music->device.ucVolume > 6)
        Music->device.ucVolume -= 6;
      else
        Music->device.ucVolume = 1;
    }
    break;
  }
  if((val == 0x02) || (val == 0x04))
  {
    if(Music->device.ucVolume==1)
    {
      Music->device.ucMute=1;
      wm8978_OutMute(1);
    }
    else
    {
      Music->device.ucMute=0;
      wm8978_OutMute(0);
      /* 调节音量，左右相同音量 */
      wm8978_SetOUT1Volume(Music->device.ucVolume);
      wm8978_SetOUT2Volume(Music->device.ucVolume);
    }
  }
}

void MusicStatusSet(MusicStatus_t status)
{
  MusicStatus = status;
  process_post_synch(&MusicProcess,AppInputEvent,NULL);
}


PROCESS(MusicProcess, "MusicProcess");

PROCESS_THREAD(MusicProcess, ev, data)
{ 
  static struct etimer et_Music; 
  PROCESS_BEGIN();
  
  MusicProcessInit();
  process_post(&MusicProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_Music,1);
      MusicProcessPoll();
    }
    if(ev==AppInputEvent)
    {
      if(MusicStatus == STA_PAUSE)
      {
        I2S_Play_Stop();
      }
      else if(MusicStatus == STA_PLAYING)
      {
        I2S_Play_Start();
      }
    }
    if(ev==AppOpenEvent)
    {
      Music->hWin = CreateMusicDLG();
    }
    if(ev==AppCloseEvent)
    {
      if(memcmp(data,"EXIT",4) == 0)
      {
        MusicStatusSet(STA_PAUSE);
        MusicProcessExit();
        PROCESS_EXIT();
      }
      else
      {
        Music->hWin = 0;
        CreateAppList();
        if((MusicStatus == STA_PAUSE) || (MusicStatus == STA_IDLE))
        {
          MusicProcessExit();
          PROCESS_EXIT();
        }
      }
    }
    if(ev == ViewKeyEvent)
    {
      MusicKeyHandle(*((uint8_t*)data));
      if(Music->hWin != 0)
        WM_SendMessageNoPara(Music->hWin,WM_USERMSG_KEYEVENT);
    }
  }
  
  PROCESS_END();
}

/*************************** End of file ****************************/
