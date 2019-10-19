#ifndef _IIC1_H
#define _IIC1_H

#include "stm32f4xx.h"
//#include "delay.h"
#include <sys/clock.h>

#define delay_us(x) clock_delay_us(x)


#define IIC_Write  0x98
#define IIC_Read   0x99
#define ERR      (0x01)
#define SUCCESS  (0x00)

#define ACK	  0
#define	NACK      1


#define BIT10   0x400
#define BIT11   0x800

#define SCL_1()         GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define SCL_0()         GPIO_ResetBits(GPIOB,GPIO_Pin_4)
#define SDA_1()         GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define SDA_0()         GPIO_ResetBits(GPIOC,GPIO_Pin_5)
#define SCL_read()      GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define SDA_read()      GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)

#define SDA_OUT()       {GPIOC->MODER &= ~BIT11;GPIOC->MODER |= BIT10;} 
#define SDA_IN()        GPIOC->MODER &= ~(BIT11|BIT10)
 				                        //GPIO≥ı ºªØ
//**************************************************************

void clock_delay_us(unsigned int ut);


void  GPIO_IIC_init(void);
void START_bit(void);
void STOP_bit(void);
unsigned char TX_byte(unsigned char Tx_buffer);
unsigned char RX_byte(unsigned char ack_nack);
void send_bit(unsigned char bit_out);
unsigned char Receive_bit(void);




#endif