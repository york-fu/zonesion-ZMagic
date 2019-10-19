/*********************************************************************************************
* 文件：rfUart.h
* 作者：fuyou 
* 说明：原理机与无线的串口通信驱动
* 修改：
* 注释：
*********************************************************************************************/
#ifndef __rfUart_h_
#define __rfUart_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "contiki.h"

/*********************************************************************************************
rfConfigMacro
*********************************************************************************************/
//串口接收BUF大小
#define RF1_BUF_SIZE 128
#define RF2_BUF_SIZE 128

//串口调试信息宏
#define RF_UART_DEBUG   1

//xlabTools使能
#define XLABTOOLS_ON    0

//xlabTools调试时串口打印不可用
#if XLABTOOLS_ON
#define UART_INFO_DEBUG   0
#else
#define UART_INFO_DEBUG   RF_UART_DEBUG
#endif

#if UART_INFO_DEBUG
#define RF_PRINT(...)    printf(__VA_ARGS__)
#else
#define RF_PRINT(...)
#endif

extern process_event_t uart_command_event;
PROCESS_NAME(rfUartProcess);

void rfUartSendByte(uint8_t index,char ch);
//向无线发送字符串
void rfUartSendString(uint8_t index,char *buf, int len);
//向无线发送透传数据
uint8_t rfUartSendData(uint8_t index, uint8_t* DataACK, char *dat);

uint8_t RF1_SendData(char* dat);
uint8_t RF2_SendData(char* dat);
void RFSendData(char* dat);

#endif
