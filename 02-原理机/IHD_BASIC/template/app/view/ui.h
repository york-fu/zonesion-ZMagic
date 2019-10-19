#ifndef _ui_h_
#define _ui_h_

#include <stdio.h>
#include <string.h>
#include <contiki.h>
#include "key.h"
#include "fml_lcd/fml_lcd.h"
#include "ui_core.h"
#include "ui_RfInfo.h"
#include "camera_app.h"
#include "ui_SysSet.h"
#include "ui_calendar.h"

#define APP_ID_MAX 8

#define DESKTOP_ID 0

#define WIN_STA_SHOW        0x80
#define WIN_STA_HINT        0x01
#define WIN_STA_ERROR       0x02

typedef struct
{
    uint8_t id;
    uint8_t status;
    char* name;
    UI_Icon_t icon;
    struct process* process;
}APP_t;


extern process_event_t uiKey_event;
extern process_event_t uiRefresh_event;

PROCESS_NAME(ui_process);

void ui_SetAppCurrentID(uint8_t id);
void ui_SetWindowStatus(uint8_t status);
void ui_showDesktop(uint8_t refresh);


#endif