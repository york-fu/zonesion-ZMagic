#ifndef _SysCalendar_h_
#define _SysCalendar_h_

#include "stm32f4xx.h"
#include "contiki.h"
#include "pcf8563.h"

typedef enum
{
  CALENDAR_SECOND=0,
  CALENDAR_MINUTE,
  CALENDAR_HOUR,
  CALENDAR_DAY,
  CALENDAR_WEEK,
  CALENDAR_MONTH,
  CALENDAR_YEAR,
}CalendarIndex_t;

typedef struct
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t week;
  uint8_t month;
  uint16_t year;
}Calendar_t;


PROCESS_NAME(SysCalendarProcess);
Calendar_t Calendar_Get();
short Calendar_Set(Calendar_t dat);
char CountWeek(int iY, int iM, int iD);

void Calender_ZxbeeGet(char* str);
void Calender_ZxbeeSet(char* str);


#endif