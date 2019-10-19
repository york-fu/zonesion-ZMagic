
#include <contiki.h>
#include "drive_led.h"

PROCESS(led, "led");

PROCESS_THREAD(led, ev, data)
{
    static struct etimer led_et;

    PROCESS_BEGIN();
    
    led_init();
    etimer_set(&led_et, 1000);
    
    while (1)
    {
        LED1 = !LED1;
        etimer_set(&led_et, 1000);
        PROCESS_WAIT_EVENT_UNTIL(ev==PROCESS_EVENT_TIMER);
    }
    PROCESS_END();
}