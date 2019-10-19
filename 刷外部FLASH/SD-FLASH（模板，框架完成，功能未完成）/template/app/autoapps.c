#include <contiki.h>

PROCESS_NAME(helloworld); 
PROCESS_NAME(PowerProcess); 
PROCESS_NAME(LedProcess); 
PROCESS_NAME(KeyProcess); 
PROCESS_NAME(rfUartProcess); 
PROCESS_NAME(AppProcess);

struct process * const autostart_processes[] = {
  &LedProcess,
  &KeyProcess,
  &AppProcess,
  NULL
};


void StartProcesslist(void)
{
  autostart_start(autostart_processes);
}
