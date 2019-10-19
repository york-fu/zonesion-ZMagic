#ifndef __SYN6288_H__
#define __SYN6288_H__
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <stm32f4xx.h>
#include "usart.h"
#include "string.h"
#include <stdio.h>

void syn6288_init();
int syn6288_busy(void);
void syn6288_play(char *s);
char *hex2unicode(char *str);
void syn6288_play_unicode(uint16_t *s, int len);
void syn6288_stop(void);
void syn6288_suspend(void);
void syn6288_continue(void);
void syn6288_playMusic(unsigned char musicNum, char *str);
void syn6288_playHint(unsigned char hintNum);
void syn6288_setVolume(unsigned char vValue, unsigned char mValue);

#endif
