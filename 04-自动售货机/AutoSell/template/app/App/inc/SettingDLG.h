#ifndef _settingdlg_h_
#define _settingdlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"

extern WM_HWIN CreateSystemInfo(void);
extern WM_HWIN CreateTimeSetDLG(void);
extern WM_HWIN CreateDateSetDLG(void);
extern WM_HWIN CreateLanguageSet(void);
extern WM_HWIN CreateVoiceSet(void);

extern WM_HWIN hWinSetting;

PROCESS_NAME(SettingProcess);

#endif