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

#define ZIGBEE_CONFIG_RADIO_TYPE        1

#define BLE_CONFIG_RADIO_TYPE           4
#define BLE_CONFIG_DEVICE_TYPE          2       //end device

#define WIFI_CONFIG_RADIO_TYPE          3
#define WIFI_CONFIG_DEVICE_TYPE         2       //end device

#define LORA_CONFIG_RADIO_TYPE          8
#define LORA_CONFIG_DEVICE_TYPE         2       //end device
    
#define LTE_CONFIG_RADIO_TYPE            6
#define LTE_CONFIG_DEVICE_TYPE           2       //end device

#define NB_CONFIG_RADIO_TYPE            7
#define NB_CONFIG_DEVICE_TYPE           2       //end device


void zxbeeBegin(void);
int zxbeeAdd(char* tag, char* val);
char* zxbeeEnd(void);
void _zxbee_onrecv_fun(uint8_t index,char *pkg, int len);

#endif