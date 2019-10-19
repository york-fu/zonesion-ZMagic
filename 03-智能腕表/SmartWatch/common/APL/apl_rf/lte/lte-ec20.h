#ifndef _lteEc20_h_
#define _lteEc20_h_

#include <contiki.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/ctimer.h"
#include "dev/leds.h"
#include "lte-inf.h"

#define _RESPONSE_BUF_SIZE 256  //at指令接收缓存长度

#define PHONE_ST_IDLE   0       //空闲
#define PHONE_ST_COUT   1       //拨出中
#define PHONE_ST_TALK   2       //接听中
#define PHONE_ST_RING   3       //响铃中（来电）

typedef struct {
  int signal;
  //0:power off, 1: power on,sim card not ready, 2:sim card ready, 3 no sim card
  int gsm_status;
  int simcard_status;
  int phone_status;
  char phone_number[16];
  char network[8];
  char imei[16];
  int ppp_status;
  
  char lac[8];
  char ci[8];
  char act[8];
  
  char cced[7][32];
  unsigned int cced_tm[7];
  unsigned int SignalStrength;

}gsm_info_t;

extern gsm_info_t gsm_info;

#define gsm_request_at  _request_at_3


int _request_at_3(char *at, unsigned int to, void (*fun)(char*));
void gsm_send_message(char *num, char *msg);
int gsm_request_call(char *pnum);
void gsm_call_accept(void);
void gsm_call_hangup(void);

void _rs_gsm_call_hangup(char *rsp);

int gsm_tcp_recv_register(int id, void (*fun)(char *buf, int len));
void gsm_tcp_write(char *buf, int len);

gsm_info_t lte_GetInof(void);


PROCESS_NAME(gsm_process);


#endif
