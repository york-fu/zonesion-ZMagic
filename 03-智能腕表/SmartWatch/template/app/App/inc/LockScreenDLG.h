#ifndef _lockscreendlg_h_
#define _lockscreendlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"

void lockScreenEnable(char s);
uint8_t LockScreenGet(void);
void lockScreenTimeReset();
void LockScreenTimeSet(short time);
short LockScreenTimeGet(void);
void ScreenUnlock(void);

PROCESS_NAME(LockScreenProcess);

#endif