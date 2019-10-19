#ifndef _IIC3_H
#define _IIC3_H
#include "extend.h"
#include "stm32f4xx.h"
void IIC3_Init(void);
void IIC3_Start(void);
void IIC3_Stop(void);
u8 IIC3_Wait_Ack(void);
void IIC3_Ack(void);
void IIC3_NAck(void);
void IIC3_Send_Byte(u8 txd);
u8 IIC3_Read_Byte(unsigned char ack);

int IIC3_Read(char addr, char r, char *buf, int len);
int II2_Write(char addr, char r, char *buf, int len);

#endif