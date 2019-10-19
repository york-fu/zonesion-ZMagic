/*********************************************************************************************
* 文件：fml_key.h
* 作者：fuyou
* 说明：按键的应用文件
* 功能：
* 修改：
*********************************************************************************************/
#ifndef __APL_KEY_H__
#define __APL_KEY_H__

#include "fml_key.h"
#include "contiki.h"

PROCESS_NAME(key);
extern process_event_t key_event;
extern process_event_t keyUp_event;

#endif