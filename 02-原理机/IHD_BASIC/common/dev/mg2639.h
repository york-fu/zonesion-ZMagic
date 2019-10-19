#ifndef _MG2639_H_
#define _MG2639_H_

void mg2639_write(char* cmd, int len);
void mg2639_init(void);
int mg2639_get_ch(void);
int mg2639_put_ch(char ch);
#endif