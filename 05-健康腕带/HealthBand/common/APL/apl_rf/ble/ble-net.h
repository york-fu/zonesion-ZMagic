#ifndef _ble_net_h_
#define _ble_net_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"

PROCESS_NAME(ble_process);

char* bleGat_mac(void);
unsigned char bleGat_link(void);

#endif