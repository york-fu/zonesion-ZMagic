
#include "App.h"
#include "apl_key.h"

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


void SysDeviceInit()
{
  LCD_DriverInit();
  FRESULT f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive -- SDcard */
	if(f_result!=FR_OK)
    bsp_result |= BSP_SD;
  else 
    bsp_result &=~BSP_SD;  
}

void AppProcessInit()
{
  SysDeviceInit();
  process_start(&ViewProcess,NULL);
}


PROCESS(AppProcess, "AppProcess");

PROCESS_THREAD(AppProcess, ev, data)
{
  //static struct etimer et_App; 
  
  PROCESS_BEGIN();
  
  AppOpenEvent = process_alloc_event();
  AppCloseEvent = process_alloc_event();
  AppInputEvent = process_alloc_event();
  
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
