/*********************************************************************************************
* 文件：
* 作者：
* 说明：
* 功能：
* 修改：
*********************************************************************************************/
#ifndef __FML_LED_H__
#define __FML_LED_H__

#include "stm32f4xx.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define LED1_RCC                RCC_AHB1Periph_GPIOE              //宏定义LED1时钟
#define LED1_CPIO               GPIOE                             //宏定义LED1 GPIO
#define LED1_PIN                GPIO_Pin_0                        //宏定义LED1管脚

#define LED2_RCC                RCC_AHB1Periph_GPIOE              //宏定义LED1时钟
#define LED2_CPIO               GPIOE                             //宏定义LED1 GPIO
#define LED2_PIN                GPIO_Pin_1                        //宏定义LED1管脚

#define LED3_RCC                RCC_AHB1Periph_GPIOE              //宏定义LED1时钟
#define LED3_CPIO               GPIOE                             //宏定义LED1 GPIO
#define LED3_PIN                GPIO_Pin_2                        //宏定义LED1管脚

#define LED4_RCC                RCC_AHB1Periph_GPIOE              //宏定义LED1时钟
#define LED4_CPIO               GPIOD                             //宏定义LED1 GPIO
#define LED4_PIN                GPIO_Pin_3                        //宏定义LED1管脚

/********************************************************************************************/

void Bsp_LedInit(void);
void Bsp_LedOn(uint8_t index);
void Bsp_LedOff(uint8_t index);
void Bsp_LedTurn(uint8_t index);

#endif
