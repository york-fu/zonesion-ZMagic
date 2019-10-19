#ifndef _lteinf_h_
#define _lteinf_h_

#include "rfUart.h"
#include "lib/ringbuf.h"

int gsm_recv_ch(char ch);
void gsm_inf_init(void);
void gsm_uart_write(char *buf, int len);
int gsm_uart_read_ch(void);

#endif