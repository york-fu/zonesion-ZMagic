
#include "CameraDLG.h"
#include "MusicDLG.h"

// USER START (Optionally insert additional public code)
char CameraStatus = 0;

void CameraLoadingUI(char index)
{
  short x = 0;
  char text[][2][128] = {
    {
      {"Camera loading..."},
      {"相机启动中..."},
    },
    {
      {"Camera error,please retry or check!"},
      {"相机初始化失败，请退出重试或检查硬件！"},
    },
  };
  
  if(index == 0)
  {
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
  }
  else
  {
    GUI_SetColor(GUI_RED);
    GUI_SetBkColor(GUI_YELLOW);
  }
  GUI_SetFont(FontList16[System.font]);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  x = 8;
  GUI_DispStringAt(text[index][System.language],x,112);
}

void CameraRunUI()
{
  short x = 0;
  char text[][64] = {
    {"K1:Exit  K2:Photo"},
    {"K1:退出  K2:拍照"},
  };
  
  GUI_SetColor(GUI_WHITE);
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetFont(FontList16[System.font]);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  x = 160 - strlen(text[System.language])*4;
  GUI_DispStringAt(text[System.language],x,224);
}

void OV2640_Config()
{
  OV2640_Auto_Exposure(1);
  OV2640_Light_Mode(0);
  OV2640_Color_Saturation(2);
  OV2640_Brightness(2);
  OV2640_Contrast(2);
  OV2640_Special_Effects(0);  
}

void CameraClose()
{
  if(CameraStatus == 1)
  {
    DCMI_Stop();
    OV2640_Close();
    CameraStatus = 0;
  }
}

void Camera_FrameIRQHandle()
{
  DCMI_Stop();
  CameraRunUI();
  LCD.PrepareFill(0,0,lcd_dev.screen->wide-1,lcd_dev.screen->high-1);
  DCMI_Start();
}


void Camera_KeyHandle(uint8_t val)
{
  switch(val)
  {
  case 0x01:process_post(&CameraProcess,AppCloseEvent,NULL);
    break;
  case 0x02:
    break;
  case 0x04:
    break;
  }
}


void CameraProcessInit()
{
  if(ProcessIsRun(&MusicProcess))
  {
    process_post_synch(&MusicProcess,AppCloseEvent,"EXIT");
  }
  
  GUI_SetExecEnable(0);
  lockScreenEnable(0);
  DeleteStatusBar();
  GUI_Exec();
  
  GUI_SetBkColor(GUI_WHITE);
  GUI_FillRect(0,0,320,240);
  CameraLoadingUI(CAMERA_UI_INIT);
  
  for(short i=5;i>0;i--)
  {
    if(OV2640_Init() == 0)
    {
      CameraStatus = 1;
      break;
    }
    delay_ms(100);
  }
  
  if(CameraStatus == 0)
  {
    CameraLoadingUI(CAMERA_UI_DEV_ERROR);
    CameraRunUI();
  }
  else
  {
    OV2640_RGB565_Mode();//rgb565 mode
    ov2640_speed_ctrl(0,4);
    OV2640_OutSize_Set(lcd_dev.screen->wide,lcd_dev.screen->high); 
    
    OV2640_DCMI_Init();
    DCMI_DMA_Init((u32)&ST7789_DAT,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置 
    DcmiFrameIrq_Set(Camera_FrameIRQHandle);
         
    LCD.PrepareFill(0,0,lcd_dev.screen->wide-1,lcd_dev.screen->high-1);
    DCMI_Start();//启动传输
  }
}


PROCESS(CameraProcess, "CameraProcess");

PROCESS_THREAD(CameraProcess, ev, data)
{
  //static struct etimer et_Camera; 
  
  PROCESS_BEGIN();
  
  CameraProcessInit();
  process_post(&CameraProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      //etimer_set(&et_Camera,100);
    }
    if(ev == ViewKeyEvent)
    {
      Camera_KeyHandle(*((uint8_t*)data));
    }
    if(ev == AppInputEvent)
    {
    }
    if(ev == AppCloseEvent)
    {
      CameraClose();
      CreateStatusBar();
      CreateDesktop();
      lockScreenEnable(1);
      GUI_SetExecEnable(1);
      PROCESS_EXIT();
    }
  }
  
  PROCESS_END();
}

// USER END

/*************************** End of file ****************************/
