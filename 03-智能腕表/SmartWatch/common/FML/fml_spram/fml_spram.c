#include "fml_spram.h"


void SPRAM_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
	GPIO_Init(GPIOE, &GPIO_InitStructure);  
  
  GPIO_SetBits(GPIOE,GPIO_Pin_0);
}
