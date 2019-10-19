#include <contiki.h>

PROCESS_NAME(helloworld); 
PROCESS_NAME(KeyProcess); 
PROCESS_NAME(PowerProcess); 
PROCESS_NAME(rfUartProcess); 
PROCESS_NAME(AppProcess);

struct process * const autostart_processes[] = {
  &PowerProcess,
  &KeyProcess,
  &rfUartProcess,
  &AppProcess,
  NULL
};


void StartProcesslist(void)
{
  autostart_start(autostart_processes);
}
