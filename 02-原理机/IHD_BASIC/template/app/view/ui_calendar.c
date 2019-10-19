#include "ui_calendar.h"

static Calendar_t Calendar = {
    .second = 0,
    .minute = 0,
    .hour = 0,
};

uint8_t Calendar_GetSecond()
{
    return Calendar.second;
}

uint8_t Calendar_GetMinute()
{
    return Calendar.minute;
}

uint8_t Calendar_GetHour()
{
    return Calendar.hour;
}

void CalendarRule()
{
    Calendar.second++;
    if(Calendar.second>59)
    {
        Calendar.second = 0;
        Calendar.minute++;
        if(Calendar.minute>59)
        {
            Calendar.minute = 0;
            Calendar.hour++;
            if(Calendar.hour>23)
                Calendar.hour = 0;
        }
    }
}

PROCESS(CalendarProcess, "CalendarProcess");

PROCESS_THREAD(CalendarProcess, ev, data)
{
    static struct etimer calendarSec_etimer;
    
    PROCESS_BEGIN();
    
    etimer_set(&calendarSec_etimer,1000);
    
    while(1)
    {
        PROCESS_WAIT_EVENT();  
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&calendarSec_etimer);
            CalendarRule();
        }
    }
    
    PROCESS_END();
}
