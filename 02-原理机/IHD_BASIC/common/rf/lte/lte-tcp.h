#ifndef __LTE_TCP_H__
#define __LTE_TCP_H__

#include <contiki.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stimer.h>
#include "lte-ec20.h"

typedef struct {
  struct process *p;
  char *pdat;
  int datlen;
  int status;
  int id;
} tcp_t;
#define TCP_STATUS_CLOSED    0
#define TCP_STATUS_OPEN      1
#define TCP_STATUS_CONNECTED 2
#define TCP_STATUS_CLOSEING  3


extern process_event_t evt_tcp;

tcp_t* tcp_alloc();
void tcp_free(tcp_t *ptcp);
tcp_t* gsm_tcp_open(tcp_t *ptcp, char *sip, int sport);
int gsm_tcp_send(tcp_t *ptcp, int len);
char *gsm_tcp_buf(void);
  
#endif