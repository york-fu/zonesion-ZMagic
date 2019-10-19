#ifndef _camera_app_h_
#define _camera_app_h_

#include <stdio.h>
#include <contiki.h>
#include "dcmi.h" 
#include "ov2640.h"
#include "drive_led.h"
#include "key.h"
#include "ui.h"

#define CAMERA_UI_INIT      0
#define CAMERA_UI_ERROR     1

PROCESS_NAME(cameraApp_process);

#endif