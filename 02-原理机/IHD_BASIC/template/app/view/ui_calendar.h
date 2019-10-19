#ifndef _ui_calendar_h_
#define _ui_calendar_h_

#include "ui.h"

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
}Calendar_t;

PROCESS_NAME(CalendarProcess);

uint8_t Calendar_GetSecond(void);

uint8_t Calendar_GetMinute(void);

uint8_t Calendar_GetHour(void);

#endif