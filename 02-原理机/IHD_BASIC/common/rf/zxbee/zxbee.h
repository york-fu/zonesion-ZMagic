#ifndef __ZXBEE_H__
#define __ZXBEE_H__

#include "contiki.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stimer.h>
#include "rfUart.h"
#include "hw.h"
#include "sensor.h"
#include "zigbee-net.h"

//PLUS_NODE Defind Device types definitions
#ifdef PLUS_ZIGBEE_NODE

#define ZG_DEVICETYPE_ROUTER        0x01                        
#define ZG_DEVICETYPE_ENDDEVICE     0x02    
//Device Types NODE_ROUTER
#define NODE_ROUTER                 ZG_DEVICETYPE_ROUTER
//Device Types NODE_ENDDEVICE
#define NODE_ENDDEVICE              ZG_DEVICETYPE_ENDDEVICE

#endif

void zxbeeBegin(void);
int zxbeeAdd(char* tag, char* val);
char* zxbeeEnd(void);
void _zxbee_onrecv_fun(uint8_t index,char *pkg, int len);
int SysProcess_command_call(uint8_t index,char* ptag, char* pval, char* obuf);
uint8_t ResponseType_Set(uint8_t index,char* obuf);

#endif