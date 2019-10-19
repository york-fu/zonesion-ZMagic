/*********************************************************************************************
* 文件：fml_key.h
* 作者：fuyou
* 说明：按键的功能模块驱动头文件
* 功能：
* 修改：
*********************************************************************************************/
#ifndef __FML_KEY_H__
#define __FML_KEY_H__

#include "stm32f4xx.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define K1_PIN                GPIO_Pin_3                        //宏定义K1管脚为KEY1_PIN
#define K1_PORT               GPIOC                             //宏定义K1通道为KEY1_PORT
#define K1_CLK                RCC_AHB1Periph_GPIOC              //宏定义KY1时钟为KEY1_CLK

#define K2_PIN                GPIO_Pin_3                        //宏定义K2管脚为KEY2_PIN
#define K2_PORT               GPIOE                             //宏定义K2通道为KEY2_PORT
#define K2_CLK                RCC_AHB1Periph_GPIOE              //宏定义K2时钟为KEY2_CLK

#define K3_PIN                GPIO_Pin_1                        //宏定义K3管脚为KEY3_PIN
#define K3_PORT               GPIOA                             //宏定义K3通道为KEY3_PORT
#define K3_CLK                RCC_AHB1Periph_GPIOA              //宏定义K3时钟为KEY3_CLK

#define K1_PRESSED            0x01                              //宏定义K1数字编号
#define K2_PRESSED            0x02                              //宏定义K2数字编号
#define K3_PRESSED            0x04                              //宏定义K3数字编号
/********************************************************************************************/

void key_GpioInit(void);
uint8_t key_GetStatus(void);

#endif
