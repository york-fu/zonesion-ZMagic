#include "exti.h"

void exti9_5_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //对应引脚
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);//PA7 连接到中断线7
  
	/* 配置EXTI_Line7 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;//LINE
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //下降沿触发 
  //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE
	EXTI_Init(&EXTI_InitStructure);//配置
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//外部中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
}


void (*EXTI9_5_CALL[5])(void)={NULL};

uint8_t EXTI9_5_CALL_Set(uint8_t index,void (*func)(void))
{
  if(index>5) return 1;
  EXTI9_5_CALL[index-1]=func;
  return 1;
}

//外部中断服务程序
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志位  
    
    if(EXTI9_5_CALL[0] != NULL)
      EXTI9_5_CALL[0]();
  }
  if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line6);//清除中断标志位  
    
    if(EXTI9_5_CALL[1] != NULL)
      EXTI9_5_CALL[1]();
  }
  if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
  {
    
    if(EXTI9_5_CALL[2] != NULL)
      EXTI9_5_CALL[2]();
    EXTI_ClearITPendingBit(EXTI_Line7);//清除中断标志位  
  }
  if(EXTI_GetITStatus(EXTI_Line8)!=RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line8);//清除中断标志位  
    
    if(EXTI9_5_CALL[3] != NULL)
      EXTI9_5_CALL[3]();
  }
  if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line9);//清除中断标志位  
    
    if(EXTI9_5_CALL[4] != NULL)
      EXTI9_5_CALL[4]();
  }
}




