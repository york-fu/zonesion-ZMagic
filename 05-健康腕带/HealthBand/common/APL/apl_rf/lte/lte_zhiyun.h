#ifndef _lte_zhiyun_h_
#define _lte_zhiyun_h_

#include <contiki.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lte-ec20.h"
#include "lte-tcp.h"

#define CONFIG_ZHIYUN_IP        "api.zhiyun360.com"
#define CONFIG_ZHIYUN_PORT      28082

#define AID                     "4078338416"
#define AKEY                    "XpRfpZibpJ0k1mW7FoX0dAo3pPBORKwv"

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
short zhiyun_send(char *pkg);

uint8_t Lte_GetTcpConnect(void);
lte_config_t Lte_GetConfig();

#endif
