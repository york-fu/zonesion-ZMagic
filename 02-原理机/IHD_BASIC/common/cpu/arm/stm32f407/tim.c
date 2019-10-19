#include "stm32f4xx.h"
#include "tim.h"

void time_init(void)
  {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure ;
    
    NVIC_InitTypeDef NVIC_InitStructure;
    
      //打开TIM2外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
                          
   /*//打开TIM3外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
   //打开TIM4外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);*/

   //**************************************************************************
   //     定时器2设置： 7分频，20ms中断一次，向上计数
   //**************************************************************************
    TIM_TimeBaseStructure.TIM_Period = 1999;
    TIM_TimeBaseStructure.TIM_Prescaler = 7;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化定时器
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //开定时器中断
    
    NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器 2 中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //响应优先级 3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);// ④初始化 NVIC
    
    TIM_Cmd(TIM2, ENABLE); //使能定时器
    
}