#ifndef _wifi_net_h_
#define _wifi_net_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "contiki.h"
#include "rfUart.h"
#include "sensor_process.h"

#if 0

#define WIFI_CONFIG_SSID        "AndroidAP"
#define WIFI_CONFIG_KEYTYPE     0
#define WIFI_CONFIG_KEY         "123456789"
#define WIFI_CONFIG_SIP         "192.168.43.1"

#endif

PROCESS_NAME(wifi_process);

unsigned char wifiGat_link(void);
char wifiGat_keyType(void);
char* wifiGat_mac(void);
char* wifiGat_ssid(void);
char* wifiGat_key(void);
char* wifiGat_ip(void);
char* wifiGat_sip(void);

#endif