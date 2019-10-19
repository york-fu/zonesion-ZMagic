#ifndef _PCF8563_H
#define _PCF8563_H
#include "stm32f4xx.h" 
#include "extend.h"


 /*//////////////////////////old
#define RTC_SCL_H         GPIOF->BSRRL = GPIO_Pin_1
#define RTC_SCL_L         GPIOF->BSRRH  = GPIO_Pin_1   
#define RTC_SDA_H         GPIOF->BSRRL = GPIO_Pin_0
#define RTC_SDA_L         GPIOF->BSRRH  = GPIO_Pin_0
#define RTC_SCL_read      GPIOF->IDR  & GPIO_Pin_1
#define RTC_SDA_read      GPIOF->IDR  & GPIO_Pin_0
*////////////////////////////old

//IO方向配置
#define RTC_SDA_IN()  {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=0<<(11*2);}	//PF0输入模式
#define RTC_SDA_OUT() {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=1<<(11*2);} //PF0输出模式
//IO操作函数
#define RTC_I2C_SCL    PBout(10) //SCL
#define RTC_I2C_SDA    PBout(11) //SDA	 
#define RTC_SDA_read   PBin(11)  //SDA电平


extern void RTC_I2C_GPIO_Config(void);
int RTC_I2C_Start(void);
void RTC_I2C_Stop(void);
void RTC_I2C_Ack(void);
void RTC_I2C_NoAck(void);
int RTC_I2C_WaitAck(void) ;
void RTC_I2C_SendByte(unsigned char SendByte);
unsigned char RTC_I2C_ReceiveByte(void);
extern unsigned char * PCF8563_Read_NByte(unsigned char addr ,unsigned char len,unsigned char *buf);
extern void PCF8563_settime(u8*buf);
extern void PCF8563_settime(u8*buf);
extern void PCF8563_init(void);
int PCF8563_gettime(char *buf);
void PCF8563_settime(u8*buf);


#endif