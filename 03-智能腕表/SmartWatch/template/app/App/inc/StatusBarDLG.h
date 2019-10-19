#ifndef _statusbardlg_h_
#define _statusbardlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"
#include "apl_cpu.h"

extern WM_HWIN WMStatusBar;

void StatusBarSet(char style);
void CreateStatusBar(void);
void DeleteStatusBar(void);

#endif