#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"


//IO方向设置
#define SCCB_SDA_IN()  {GPIOD->MODER&=~(3<<(13*2));GPIOD->MODER|=0<<13*2;}	//PD7 输入
#define SCCB_SDA_OUT() {GPIOD->MODER&=~(3<<(13*2));GPIOD->MODER|=1<<13*2;} 	//PD7 输出


//IO操作函数	 
#define SCCB_SCL    		PDout(11)	 	//SCL
#define SCCB_SDA    		PDout(13) 		//SDA	 

#define SCCB_READ_SDA    	PDin(13)  		//输入SDA    
#define SCCB_ID   			0X60  			//OV2640的ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif













