#include "fml_audio/audioAmplifier.h"


void TCS8642_EN_Set(uint8_t status)
{
  if(status)
    GPIO_SetBits(GPIOE,GPIO_Pin_2);
  else
    GPIO_ResetBits(GPIOE,GPIO_Pin_2);
}


void TCS8642_EN_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
  /*使用上面的结构体初始化按键*/
	GPIO_Init(GPIOE, &GPIO_InitStructure);  
  
  TCS8642_EN_Set(0);
}



void LoudspeakerInit()
{
  TCS8642_EN_Init();
}

void LoudspeakerSet(uint8_t status)
{
  TCS8642_EN_Set(status);
}



uint8_t EarphoneGet()
{
  return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4);
}


void EarphoneInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
  /*使用上面的结构体初始化按键*/
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
}

