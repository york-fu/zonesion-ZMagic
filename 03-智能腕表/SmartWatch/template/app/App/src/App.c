
#include "App.h"
#include "wk2xxx.h"
#include "fml_VibratingMotor/VibratingMotor.h"
#include "fml_spram/fml_spram.h"
#include "QR_Encode.h"
#include "apl_key.h"
#include "fml_audio/audioAmplifier.h"
#include "PhoneDLG.h"
#include "AlarmClockDLG.h"
#include "FingerprintSetDLG.h"

extern struct process *process_list;

uint32_t bsp_result = BSP_OK;
FATFS fs[2];											/* Work area (file system object) for logical drives */	

SystemSet_t System = {
  .font = 1,
  .language = 1,
};

process_event_t AppOpenEvent;
process_event_t AppCloseEvent;
process_event_t AppInputEvent;

char ProcessIsRun(struct process *p)
{
  struct process *q;

  /* First make sure that we don't try to start a process that is
  already running. */
  for(q = process_list; q != p && q != NULL; q = q->next);
  
  /* If we found the process on the process list, we bail out. */
  if(q == p) 
  {
    return 1;
  }
  return 0;
}


void DISPLAY_RENCODE_TO_TFT(short xs,short ys,uint8_t *qrcode_data)
{
  short i,j;
  short x,y,p;
  char buf[64];
  
  GUI_SetBkColor(GUI_WHITE);
  GUI_FillRect(xs,ys,xs+QrEncode_WindowSize-1,ys+QrEncode_WindowSize-1);
  
  
  m_byModule = (m_byModuleData_t*)calloc(sizeof(m_byModuleData_t),1);
  if(m_byModule == NULL)
  {
    return;
  }
  EncodeData((char *)qrcode_data);
  
  if((m_nSymbleSize>QrEncode_WindowSize)||(m_nSymbleSize < 1))	
  {
    GUI_SetColor(GUI_RED);
    sprintf(buf, "QR Data Error!");
    GUI_DispStringAt(buf,xs,ys);
    free(m_byModule);
    return;
  }
  
  for(i=2;i<10;i++)
  {
    if((m_nSymbleSize*i)>QrEncode_WindowSize)	break;
  }
  p=i-1;//µ„¥Û–°
  x=xs+(QrEncode_WindowSize-m_nSymbleSize*p)/2;
  y=ys+(QrEncode_WindowSize-m_nSymbleSize*p)/2;
  
  GUI_SetColor(GUI_BLACK);
  for(i=0;i<m_nSymbleSize;i++)
  {
    for(j=0;j<m_nSymbleSize;j++)
    {
      if(m_byModule->Data[i][j]==1)
        GUI_FillRect(x+p*i,y+p*j,x+p*(i+1),y+p*(j+1));
    }
    
  }
  free(m_byModule);
}



void SysDeviceInit()
{
  SPRAM_Init();
  LCD_DriverInit();
  LoudspeakerInit();
  EarphoneInit();
  VibratingMotor_Init();
  WK2114_init();
  FRESULT f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive -- SDcard */
	if(f_result!=FR_OK)
    bsp_result |= BSP_SD;
  else 
    bsp_result &=~BSP_SD;  
}

void AppProcessInit()
{
  SysDeviceInit();
  process_start(&SysCalendarProcess,NULL);
  process_start(&ViewProcess,NULL);
  process_start(&PhoneProcess,NULL);
  process_start(&AlarmProcess,NULL);
  process_start(&FingerprintProcess,NULL);
  process_start(&sensor_process,NULL);
}


PROCESS(AppProcess, "AppProcess");

PROCESS_THREAD(AppProcess, ev, data)
{
  //static struct etimer et_App; 
  
  PROCESS_BEGIN();
  
  AppOpenEvent=process_alloc_event();
  AppCloseEvent=process_alloc_event();
  AppInputEvent=process_alloc_event();
  
  AppProcessInit();
  process_post(&AppProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      //etimer_set(&et_App,100);
    }
  }
  
  PROCESS_END();
}
