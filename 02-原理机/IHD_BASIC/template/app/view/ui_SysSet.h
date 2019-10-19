#ifndef _ui_sysset_h_
#define _ui_sysset_h_

#include "ui.h"

#define UI_OPTION_MAX 10


typedef struct
{
    uint8_t index;
    uint8_t currentID;
    uint8_t Num;
    uint8_t redraw;
}Menu_t;

PROCESS_NAME(ui_MenuProcess);

#endif