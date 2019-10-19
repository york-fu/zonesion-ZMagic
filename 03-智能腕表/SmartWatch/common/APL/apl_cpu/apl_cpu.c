#include "apl_cpu.h"

#if 0
#define UART_DEBUG(...)      printf(__VA_ARGS__)
#else
#define UART_DEBUG(...)
#endif

unsigned long IdleCountMax = 0;
unsigned long IdleCount = 0;
unsigned char CPUUsage = 0;
process_event_t CPUUsageEvent;

unsigned char GetCPUUsage()
{
  return CPUUsage;
}

PROCESS(CPUUsageProcess, "CPUUsageProcess");

PROCESS_THREAD(CPUUsageProcess, ev, data)
{
  static struct etimer et_CPUUsage;
  
  PROCESS_BEGIN();

  CPUUsageEvent = process_alloc_event();
  etimer_set(&et_CPUUsage,100);
  PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_TIMER) && (etimer_expired(&et_CPUUsage)));
  IdleCountMax = IdleCount*5;
  CPUUsage = (unsigned char)((IdleCount*100.0)/(IdleCountMax/5));
  CPUUsage = 100 - CPUUsage;
  IdleCount = 0;
  process_post(PROCESS_BROADCAST,CPUUsageEvent,NULL);
  
  process_post(&CPUUsageProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT(); 
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_CPUUsage,500);
      CPUUsage = (unsigned char)((IdleCount*100.0)/IdleCountMax);
      CPUUsage = 100 - CPUUsage;
      IdleCount = 0;
      UART_DEBUG("CPU Usage: %d\r\n",CPUUsage);
    } 
  }
  
  PROCESS_END();
}