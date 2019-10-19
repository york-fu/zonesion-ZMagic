#ifndef _lte_config_h_
#define _lte_config_h_

#include <contiki.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lte-ec20.h"
#include "lte-tcp.h"
#include "w25qxx.h"
#include "zxbee.h"
#include "sensor.h"

#define CONFIG_RADIO_TYPE       6       //lte
#define CONFIG_DEV_TYPE         2       //end dev

#define CONFIG_ZHIYUN_IP        "api.zhiyun360.com"
#define CONFIG_ZHIYUN_PORT      28082

#define AID                     "2191629613"
#define AKEY                    "bpCH5rdI0EhFs5PTycaRi88yvmKsz6L0"

typedef struct {
    char id[16]; //ID
    char key[64]; //KEY
    char ip[32]; //
    int  port;
}lte_config_t;


extern lte_config_t lteConfig;
PROCESS_NAME(u_zhiyun);

int lteConfig_init(void);
void lteConfig_save(void);
void zhiyun_send(char *pkg);

#endif
