/*********************************************************************************************
* 文件：
* 作者：
* 说明：
* 功能：
* 修改：
*********************************************************************************************/
#ifndef __LedProcess_H__
#define __LedProcess_H__

#include "contiki.h"

PROCESS_NAME(key);
extern process_event_t key_event;
extern process_event_t keyUp_event;

#endif