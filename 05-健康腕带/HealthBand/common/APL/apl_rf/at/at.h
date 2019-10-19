#ifndef __AT_H__
#define __AT_H__

#define ATOK "OK\r\n"
#define ATERROR "ERR: Bad command\r\n"

void at_init(void);
void at_proc(void);
void at_response_buf(char *s, int len);
void at_response(char *s);

void at_notify_data(char *buf, int len);
int8_t user_at_proc(char *msg);

PROCESS_NAME(AtProcess);

//#define at_printf        Report

#endif