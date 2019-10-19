#include <contiki.h>
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "adc.h"
#include "debug.h"
#include "apl_cpu.h"


PROCESS(StartProcess, "StartProcess");

PROCESS_THREAD(StartProcess, ev, data)
{
  PROCESS_BEGIN();
  
  process_start(&CPUUsageProcess, NULL);
  PROCESS_WAIT_EVENT_UNTIL(ev == CPUUsageEvent);
  autostart_start(autostart_processes);
  
  PROCESS_EXIT();
  PROCESS_END();
}


void main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  BspADC_Init();
  clock_init();
  
  debug_init();
  printf("\r\n Starting ");
  printf(CONTIKI_VERSION_STRING);
  printf(" on STM32F40x \r\n");
  
  process_init();
  ctimer_init();
  process_start(&etimer_process, NULL);
  process_start(&StartProcess, NULL);
  
  while(1)
  {
    do
    {
    }
    while(process_run() > 0);
    IdleCount++;
    /* Idle! */
    /* Stop processor clock */
    /* asm("wfi"::); */
  }
}