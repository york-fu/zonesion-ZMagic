#ifndef _drive_led_h_
#define _drive_led_h_

#include "stm32f4xx_gpio.h"
#include "extend.h"

#define LED1_RCC    RCC_AHB1Periph_GPIOE
#define LED1_GPIO   GPIOE
#define LED1_PIN    GPIO_Pin_0

#define LED2_RCC    RCC_AHB1Periph_GPIOE
#define LED2_GPIO   GPIOE
#define LED2_PIN    GPIO_Pin_1

#define LED3_RCC    RCC_AHB1Periph_GPIOE
#define LED3_GPIO   GPIOE
#define LED3_PIN    GPIO_Pin_2

#define LED4_RCC    RCC_AHB1Periph_GPIOE
#define LED4_GPIO   GPIOE
#define LED4_PIN    GPIO_Pin_3

#define LEDR_RCC	RCC_AHB1Periph_GPIOB
#define LEDR_GPIO	GPIOB
#define LEDR_PIN	GPIO_Pin_0

#define LEDG_RCC	RCC_AHB1Periph_GPIOB
#define LEDG_GPIO	GPIOB
#define LEDG_PIN	GPIO_Pin_1

#define LEDB_RCC	RCC_AHB1Periph_GPIOB
#define LEDB_GPIO	GPIOB
#define LEDB_PIN	GPIO_Pin_2

#define LED1 		PEout(0)
#define LED2 		PEout(1)
#define LED3 		PEout(2)
#define LED4 		PEout(3)
#define RGB_R		PBout(0)
#define RGB_G		PBout(1)
#define RGB_B		PBout(2)

#define LED1_NUM      	0X01                               
#define LED2_NUM      	0X02                                
#define LED3_NUM      	0X04                                 
#define LED4_NUM      	0X08                               
#define LEDR_NUM    	0X10                                   
#define LEDG_NUM    	0X20                                 
#define LEDB_NUM    	0X40                                   

void led_init(void);
void turn_off(unsigned char led);
void turn_on(unsigned char led);
unsigned char get_led_status(void);

#endif