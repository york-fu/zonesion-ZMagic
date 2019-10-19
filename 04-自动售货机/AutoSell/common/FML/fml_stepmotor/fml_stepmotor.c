#include "fml_stepmotor.h"

uint32_t STEPMOTOR_TIMES=0;
Stepmotor_t Stepmotor[3]={
  {
    .step=0,
    .speed=6,
  },
  {
    .step=0,
    .speed=6,
  },
  {
    .step=0,
    .speed=6,
  },
};


short Stepmotor_SetEnable(uint8_t index,uint8_t status)
{
  if(index>2) return 1;
  switch(index)
  {
  case 0:
    if(status) STEPMOTOR1_ENABLE_H;
    else STEPMOTOR1_ENABLE_L;
    break;
  case 1:
    if(status) STEPMOTOR2_ENABLE_H;
    else STEPMOTOR2_ENABLE_L;
    break;
  case 2:
    if(status) STEPMOTOR3_ENABLE_H;
    else STEPMOTOR3_ENABLE_L;
    break;
  }
  return 0;
}


short Stepmotor_SetDirection(uint8_t index,uint8_t status)
{
  if(index>2) return 1;
  switch(index)
  {
  case 0:
    if(status) STEPMOTOR1_DIR_H;
    else STEPMOTOR1_DIR_L;
    break;
  case 1:
    if(status) STEPMOTOR2_DIR_H;
    else STEPMOTOR2_DIR_L;
    break;
  case 2:
    if(status) STEPMOTOR3_DIR_H;
    else STEPMOTOR3_DIR_L;
    break;
  }
  return 0;
}


short Stepmotor_SetStepIO(uint8_t index,uint8_t status)
{
  if(index>2) return 1;
  switch(index)
  {
  case 0:
    if(status) STEPMOTOR1_STEP_H;
    else STEPMOTOR1_STEP_L;
    break;
  case 1:
    if(status) STEPMOTOR2_STEP_H;
    else STEPMOTOR2_STEP_L;
    break;
  case 2:
    if(status) STEPMOTOR3_STEP_H;
    else STEPMOTOR3_STEP_L;
    break;
  }
  return 0;
}


void StepmotorInit()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
  //stepmotot1 enable
  RCC_AHB1PeriphClockCmd(STEPMOTOR1_ENABLE_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR1_ENABLE_PIN;
  GPIO_Init(STEPMOTOR1_ENABLE_GPIO, &GPIO_InitStruct);
  //stepmotot1 dir
  RCC_AHB1PeriphClockCmd(STEPMOTOR1_DIR_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR1_DIR_PIN;
  GPIO_Init(STEPMOTOR1_DIR_GPIO, &GPIO_InitStruct);
  //stepmotot1 step
  RCC_AHB1PeriphClockCmd(STEPMOTOR1_STEP_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR1_STEP_PIN;
  GPIO_Init(STEPMOTOR1_STEP_GPIO, &GPIO_InitStruct);
  
  //stepmotot2 enable
  RCC_AHB1PeriphClockCmd(STEPMOTOR2_ENABLE_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR2_ENABLE_PIN;
  GPIO_Init(STEPMOTOR2_ENABLE_GPIO, &GPIO_InitStruct);
  //stepmotot2 dir
  RCC_AHB1PeriphClockCmd(STEPMOTOR2_DIR_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR2_DIR_PIN;
  GPIO_Init(STEPMOTOR2_DIR_GPIO, &GPIO_InitStruct);
  //stepmotot2 step
  RCC_AHB1PeriphClockCmd(STEPMOTOR2_STEP_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR2_STEP_PIN;
  GPIO_Init(STEPMOTOR2_STEP_GPIO, &GPIO_InitStruct);
  
  //stepmotot3 enable
  RCC_AHB1PeriphClockCmd(STEPMOTOR3_ENABLE_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR3_ENABLE_PIN;
  GPIO_Init(STEPMOTOR3_ENABLE_GPIO, &GPIO_InitStruct);
  //stepmotot3 dir
  RCC_AHB1PeriphClockCmd(STEPMOTOR3_DIR_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR3_DIR_PIN;
  GPIO_Init(STEPMOTOR3_DIR_GPIO, &GPIO_InitStruct);
  //stepmotot3 step
  RCC_AHB1PeriphClockCmd(STEPMOTOR3_STEP_RCC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = STEPMOTOR3_STEP_PIN;
  GPIO_Init(STEPMOTOR3_STEP_GPIO, &GPIO_InitStruct);
  
  Stepmotor_SetEnable(0,STEPMOTOR_DISABLE);
  Stepmotor_SetEnable(1,STEPMOTOR_DISABLE);
  Stepmotor_SetEnable(2,STEPMOTOR_DISABLE);
  
  TIMx_Configuration(83,99);
  TimeIrq_Set(StepmotorIrq);
}

void StepmotorIrq()
{
  static uint8_t status[3]={0};
  for(short i=0;i<3;i++)
  {
    if(STEPMOTOR_TIMES%Stepmotor[i].speed==0)
    {
      if(Stepmotor[i].step>1)
      {
        Stepmotor[i].step--;
        if(Stepmotor[i].speed>0)
        {
          Stepmotor_SetStepIO(i,status[i]);
          status[i]=!status[i];
        }
      }
      else if(Stepmotor[i].step==1)
      {
        Stepmotor[i].step--;
        Stepmotor_SetEnable(i,STEPMOTOR_DISABLE);
      }
    }
  }
  STEPMOTOR_TIMES++;
}


short Stepmotor_SetStepCount(uint8_t index,uint16_t step)
{
  if(index>2) return 1;
  Stepmotor[index].step=step*2;
  return 0;
}


short Stepmotor_SetSpeed(uint8_t index,uint16_t speed)
{
  if(index>2) return 1;
  if(speed>10) speed=10;
  if(speed==0)
    Stepmotor[index].speed=0;
  else
    Stepmotor[index].speed=11-speed;
  return 0;
}

short Stepmotor_GetRunStatus(uint8_t index)
{
  if(index>3 || index==0) return -1;
  return Stepmotor[index-1].step;
}


short Stepmotor_Run(uint8_t index,uint8_t dir,uint16_t step)
{
  if(index>3 || index==0) return -1;
  Stepmotor_SetEnable(index-1,STEPMOTOR_ENABLE);
  Stepmotor_SetDirection(index-1,dir);
  Stepmotor_SetStepCount(index-1,step);
  return 0;
}

void Stepmotor_Stop(uint8_t index)
{
  Stepmotor_SetEnable(index-1,STEPMOTOR_DISABLE);
  Stepmotor_SetStepCount(index-1,0);
}

