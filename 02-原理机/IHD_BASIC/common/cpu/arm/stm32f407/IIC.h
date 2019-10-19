#ifndef _IIC_H
#define _IIC_H

//#include "systick_if.h"
#include "stm32f4xx.h"
//#include "hw_memmap.h"
//#include "pin.h"
//#include "prcm.h"
typedef unsigned char u8;
#define G14_UCPINS  (1<<(14%8))  //p01
#define G16_UCPINS  (1<<(16%8))  //p05
#define G17_UCPINS  (1<<(17%8))  //p06
#define G03_UCPINS  (1<<(3%8))  //replace G14
#define H 0xFF
#define L 0X00

#define SDA(x)    GPIOPinWrite(GPIOA0_BASE, G03_UCPINS, (x))
#define SCL(x)    GPIOPinWrite(GPIOA2_BASE, G17_UCPINS, (x))
//#define SDA_read  (((unsigned char)GPIOPinRead(GPIOA1_BASE, G14_UCPINS) & 0x40) >>6)
#define SDA_read  ((unsigned char)GPIOPinRead(GPIOA0_BASE, G03_UCPINS))
#define SCL_read  ((unsigned char)GPIOPinRead(GPIOA2_BASE, G17_UCPINS))   

#define delay_us(x)  UTUtilsDelay(x)   //—” ±Œ¢√Î
#define SDA_IN()     GPIODirModeSet(GPIOA0_BASE, G03_UCPINS, GPIO_DIR_MODE_IN)
#define SDA_OUT()    GPIODirModeSet(GPIOA0_BASE, G03_UCPINS, GPIO_DIR_MODE_OUT)                 
//#endif


void I2C_GPIO_Config(void);
void I2C_Start(void);
void I2C_Stop(void);
void RepeatStart();
unsigned char ChkACK(void);
void Send8bit(unsigned char Data);
extern unsigned char RegWrite(unsigned char address, unsigned char reg,unsigned char val);
unsigned char RegWriteN(unsigned char address, unsigned char reg1,unsigned char N,unsigned char *array);
void SendAck(void);
unsigned char Read8bit(unsigned char ack);
extern unsigned char RegRead(unsigned char address, unsigned char reg);
unsigned char RegReadN(unsigned char address, unsigned char reg1,unsigned char N);


#endif