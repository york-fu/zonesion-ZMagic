#ifndef _alarmclockdlg_h_
#define _alarmclockdlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"

#define ALARMCLOCK_NUM        4
#define ALARMCLOCK_TIMEOUT    10000

typedef struct
{
  char minute;
  char hour;
  uint8_t week;
  uint8_t uc_switch;
  uint8_t status;
}Alarm_t;

extern short AlarmIndex;
extern Alarm_t Alarm[ALARMCLOCK_NUM*2];
PROCESS_NAME(AlarmProcess);


void Alarm_ZxbeeGet(char* buf);
void Alarm_ZxbeeSet(char* str);

#endif