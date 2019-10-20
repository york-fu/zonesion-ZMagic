#ifndef _Appview_h_
#define _Appview_h_

#include "stm32f4xx.h"
#include "contiki.h"
#include "stdio.h"
#include "string.h"
#include "GUIFont_Port.h"
#include "fml_lcd/fml_lcd.h"

#include "DesktopDLG.h"
#include "StatusBarDLG.h"
#include "AppListDLG.h"

extern const GUI_FONT* FontList12[2];
extern const GUI_FONT* FontList16[2];
extern const GUI_FONT* FontList20[2];
extern const GUI_FONT* FontList24[2];
extern const char WeekText[][7][16];

extern WM_HWIN ErrorDialog(char *string1,char *string2);
extern WM_HWIN CreatePupup(char *text);

void GUI_SetExecEnable(char s);
PROCESS_NAME(ViewProcess);

#endif