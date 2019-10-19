#include "fml_VibratingMotor/VibratingMotor.h"

void VibratingMotor_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
	RCC_AHB1PeriphClockCmd(VIBRATING_MOTOR_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = VIBRATING_MOTOR_PIN;  
	GPIO_Init(VIBRATING_MOTOR_GPIO, &GPIO_InitStructure);  
  
  GPIO_ResetBits(VIBRATING_MOTOR_GPIO,VIBRATING_MOTOR_PIN);
}


void VibratingMotor_Set(uint8_t s)
{
  if(s)
    GPIO_SetBits(VIBRATING_MOTOR_GPIO,VIBRATING_MOTOR_PIN);
  else
    GPIO_ResetBits(VIBRATING_MOTOR_GPIO,VIBRATING_MOTOR_PIN);
}