#include "SysCalendar.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

Calendar_t SysCalendar={0};


Calendar_t Calendar_Get()
{
  return SysCalendar;
}


short Calendar_Set(Calendar_t dat)
{
  uint8_t date[7]={0};
  
  date[0]=dat.second;
  date[1]=dat.minute;
  date[2]=dat.hour;
  date[3]=dat.day;
  date[4]=dat.week;
  date[5]=dat.month;
  
  if(dat.year>=2000)
    date[6]=dat.year-2000;
  else
    date[6]=0;
  
  return set_pcf8563Time(date);
}

char CountWeek(int iY, int iM, int iD) 
{
  int iWeekDay = -1; 
  if (1 == iM || 2 == iM) 
  {   
    iM += 12; 
    iY--;
  }   
  iWeekDay = (iD + 1 + 2 * iM + 3 * (iM + 1) / 5 + iY + iY / 4 - iY / 100 + iY / 400) % 7;
#if 0
  switch(iWeekDay)
  {   
  case 0 : return "Sunday"; break;
  case 1 : return "Monday"; break;
  case 2 : return "Tuesday"; break;
  case 3 : return "Wednesday"; break;
  case 4 : return "Thursday"; break;
  case 5 : return "Friday"; break;                                                             
  case 6 : return "Saturday"; break;
  default : return NULL; break;
  }  
#endif
  return iWeekDay;
}

void Calender_ZxbeeGet(char* str)
{
  Calendar_t calendar = Calendar_Get();
  sprintf(str,"%d/%d/%d/%d/%d/%d/%d",
          calendar.year,calendar.month,calendar.day,calendar.week,
          calendar.hour,calendar.minute,calendar.second);
}

void Calender_ZxbeeSet(char* str)
{
  Calendar_t calendar;
  char* ptr = str;
  
  calendar.year = atoi(ptr);
  
  ptr = strstr((const char*)ptr,"/");
  calendar.month = atoi(++ptr);
  
  ptr = strstr((const char*)ptr,"/");
  calendar.day = atoi(++ptr);
  
  ptr = strstr((const char*)ptr,"/");
  calendar.hour = atoi(++ptr);
  
  ptr = strstr((const char*)ptr,"/");
  calendar.minute = atoi(++ptr);
  
  ptr = strstr((const char*)ptr,"/");
  calendar.second = atoi(++ptr);
  
  calendar.week = CountWeek(calendar.year,calendar.month,calendar.day);
  Calendar_Set(calendar);
}

void SysCalendarInit()
{
  unsigned char date[8] = {0};
  pcf8563_init();
  get_pcf8563Time(date);

  if(date[0]>59)
    date[0]=0;
  SysCalendar.second = date[0];

  if(date[1]>59)
    date[1]=0;
  SysCalendar.minute = date[1];

  if(date[2]>23)
    date[2]=0;
  SysCalendar.hour = date[2];

  if((date[3]==0)||(date[3]>31))
    date[3]=1;
  SysCalendar.day = date[3];

  if((date[5]==0)||(date[5]>12))
    date[5]=1;
  SysCalendar.month = date[5];

  SysCalendar.year = 2000+date[6];
  
  SysCalendar.week = CountWeek(SysCalendar.year,SysCalendar.month,SysCalendar.day);

  set_pcf8563Time(date);
}

PROCESS(SysCalendarProcess, "SysCalendar");

PROCESS_THREAD(SysCalendarProcess, ev, data)
{
  static struct etimer et_Calendar; 
  
  PROCESS_BEGIN();
  
  SysCalendarInit();
  process_post(&SysCalendarProcess,PROCESS_EVENT_TIMER,NULL);
  
  while (1)
  {
    PROCESS_WAIT_EVENT();  
    if(ev == PROCESS_EVENT_TIMER)
    {
      etimer_set(&et_Calendar,100);
      
      unsigned char Date[8] = {0};
      get_pcf8563Time(Date);
      SysCalendar.second = Date[0];
      SysCalendar.minute = Date[1];
      SysCalendar.hour = Date[2];
      SysCalendar.day = Date[3];
      SysCalendar.week = Date[4];
      SysCalendar.month = Date[5];
      SysCalendar.year = 2000+Date[6];
    }
  }
  
  PROCESS_END();
}
