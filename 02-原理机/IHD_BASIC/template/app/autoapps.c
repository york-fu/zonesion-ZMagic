#include <contiki.h>

PROCESS_NAME(helloworld); 
PROCESS_NAME(led);
PROCESS_NAME(key);
PROCESS_NAME(onboard_sensors);
PROCESS_NAME(ui_process);
PROCESS_NAME(rfUart_process);
PROCESS_NAME(cameraApp_process);

struct process * const autostart_processes[] = {
    &led,
    &key,
    &onboard_sensors,
    &rfUart_process,
    &ui_process,
    NULL
};

void StartProcesslist(void)
{
    autostart_start(autostart_processes);
}
