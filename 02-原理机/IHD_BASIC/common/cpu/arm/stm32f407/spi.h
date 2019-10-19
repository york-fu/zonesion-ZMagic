#ifndef __SPI_H
#define __SPI_H
#include "stm32f4xx.h" 	
 	    													  
void SPI1_Init(void);			 			//初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet); 				//设置SPI1速度   
u8 SPI1_ReadWriteByte(u8 TxData);				//SPI1总线读写一个字节

void SPI2_Init(void);			 			//初始化SPI2口
void SPI2_SetSpeed(u8 SpeedSet); 				//设置SPI2速度   
u8 SPI2_ReadWriteByte(u8 TxData);				//SPI2总线读写一个字节
		 
void SPI3_Init(void);			 			//初始化SPI3口
void SPI3_SetSpeed(u8 SpeedSet); 				//设置SPI3速度   
u8 SPI3_ReadWriteByte(u8 TxData);				//SPI3总线读写一个字节
		 
#endif

