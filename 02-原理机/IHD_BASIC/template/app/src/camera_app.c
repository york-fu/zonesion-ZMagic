#include "camera_app.h"
#include "fml_lcd/fml_lcd.h"
#include "fml_lcd/st7789.h"
#include "fml_lcd/ili93xx.h"

uint8_t* cameraStatus = NULL;

uint8_t camera_GetStatus()
{
  return *cameraStatus;
}

void camera_SetStatus(uint8_t status)
{
  if(status==0)
    *cameraStatus = status;
  else
    *cameraStatus |= status;
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

void Camera_UI(uint8_t status)
{
  char pbuf[40] = {0};
  
  if(status == CAMERA_UI_INIT)
  {
    LCD.FillRectangle(0,20,320,220,LCD_COLOR_WHITE);
    sprintf(pbuf,"相机启动中...");
    LCDShowFont16(160-(strlen(pbuf)*4),112,pbuf,strlen(pbuf)*8,LCD_COLOR_BLACK,LCD_COLOR_WHITE);
  }
  if(status == CAMERA_UI_ERROR)
  {
    sprintf(pbuf,"相机初始化失败，请退出重试或检查硬件!");
    LCDShowFont16(160-(strlen(pbuf)*4),112,pbuf,strlen(pbuf)*8,LCD_COLOR_RED,LCD_COLOR_YELLOW);
  }
}

void CameraExitHandle()
{
  if(cameraStatus != NULL)
  {
    if(camera_GetStatus()&0x80)
    {
      DCMI_Stop();
      OV2640_Close();
      LED2 = 1;//OFF
    }
    free(cameraStatus);
  }
  ui_SetAppCurrentID(DESKTOP_ID);
  process_exit(&cameraApp_process);
}

void Camera_K1_Handle()
{
}
void Camera_K2_Handle()
{
  CameraExitHandle();
}
void Camera_K3_Handle()
{
}
void Camera_K4_Handle()
{
}

void CameraKeyHandle(uint8_t keyStatus)
{
  switch(keyStatus)
  {
  case 0x01:Camera_K1_Handle();
  break;
  case 0x02:Camera_K2_Handle();
  break;
  case 0x04:Camera_K3_Handle();
  break;
  case 0x08:Camera_K4_Handle();
  break;
  }  
}

void Camera_FrameIRQHandle()
{
  LED2 = !LED2;
  
  char pbuf[10] = {0};
  strcat(pbuf,"K2退出");
  LCDShowFont16(319-strlen(pbuf)*8,239-16,pbuf,strlen(pbuf)*8,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
  
  LCD.PrepareFill(0,0,lcd_dev.screen->wide-1,lcd_dev.screen->high-1);
}

PROCESS(cameraApp_process, "cameraApp_process");

PROCESS_THREAD(cameraApp_process, ev, data)
{
  PROCESS_BEGIN();
  
  cameraStatus = malloc(sizeof(uint8_t));
  if(cameraStatus == NULL)
  {
    ui_SetWindowStatus(WIN_STA_SHOW+WIN_STA_ERROR);
    CameraExitHandle();
  }
  else
  {
    Camera_UI(CAMERA_UI_INIT);
    camera_SetStatus(0);
    for(uint8_t i=0;i<5;i++)
    {
      if(!OV2640_Init())
      {
        camera_SetStatus(0x80);
        break;
      }
      delay_ms(100);
    }
    
    if(camera_GetStatus()&0x80)
    {
      OV2640_RGB565_Mode();       //rgb565 mode 
      ov2640_speed_ctrl(0,4);
      OV2640_OutSize_Set(lcd_dev.screen->wide,lcd_dev.screen->high); 
      
      OV2640_DCMI_Init();
      
      uint32_t ptr = NULL;
      if(lcd_dev.screen->id == 0x7789)
        ptr = (uint32_t)&ST7789_DAT;
      else
        ptr = (uint32_t)&ILI93XX_DAT;
      DCMI_DMA_Init((u32)ptr,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置 
      
      LCD.PrepareFill(0,0,lcd_dev.screen->wide-1,lcd_dev.screen->high-1);
      DCMI_Start(); 		//启动传输
    }
    else
    {
      Camera_UI(CAMERA_UI_ERROR);
    }
  }
  
  while(1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == uiKey_event)
    {
      CameraKeyHandle(*(uint8_t*)data);
    }
  }
  
  PROCESS_END();
}

