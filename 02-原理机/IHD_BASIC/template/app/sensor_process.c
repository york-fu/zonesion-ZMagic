#include "sensor_process.h"

PROCESS(sensor_process, "sensor_process");

PROCESS_THREAD(sensor_process, ev, data)
{
    PROCESS_BEGIN();
    
    static struct etimer etimer_sensorCheck,etimer_sensorPoll;
    static unsigned short checkTime = 0,tick = 0;
    
    sensor_init();
    etimer_set(&etimer_sensorCheck,100);
    etimer_set(&etimer_sensorPoll,1000);
    
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&etimer_sensorCheck))
        {
            checkTime = sensor_check();
            etimer_set(&etimer_sensorCheck,checkTime);
        }
        if(etimer_expired(&etimer_sensorPoll))
        {
            etimer_set(&etimer_sensorPoll,1000);
            sensor_poll(++tick);
            if(tick>59999)
                tick=0;
        }
    }
    
    PROCESS_END();
}