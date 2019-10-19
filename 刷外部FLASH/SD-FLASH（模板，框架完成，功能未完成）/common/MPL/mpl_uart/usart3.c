#include "usart.h"	
#include "string.h"
#include "stm32f4xx.h"
#include <stdio.h>


/*********************************************************************************************
* 名称：usart3_init
* 功能：usart3初始化
* 参数：bound波特率
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void uart3_init(unsigned int bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);     //使能GPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);    //使能USART1时钟
 
  //串口3对应引脚复用映射
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_USART3);  //GPIOB10复用为USART3
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_USART3);  //GPIOB11复用为USART3
	
  //USART3端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;  //GPIOB10与GPIOB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             //复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //上拉
  GPIO_Init(GPIOC,&GPIO_InitStructure);                    //初始化PB10 PB11

  //USART3 初始化设置
  USART_InitStructure.USART_BaudRate = bound;                      //波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;           //一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;              //无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //收发模式
  USART_Init(USART3, &USART_InitStructure);                        //根据上述配置初始化串口3
	
	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;               //串口3中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;         //抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		  //子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		  //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);	                          //根据指定的参数初始化VIC寄存器、

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                  //开启串口3接收中断
  USART_Cmd(USART3, ENABLE);                                      //使能串口3 	
}

static int (*input_call)( char x) = NULL;
void  uart3_set_input(int (*fun)( char ) )
{
 
  input_call = fun;
}

/*********************************************************************************************
* 名称：USART3_IRQHandler
* 功能：串口中断处理函数
* 参数： 
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void USART3_IRQHandler(void)                	           
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){         //如果收到数据(接收中断) 
       unsigned char x;
      x = USART_ReceiveData(USART3);
      if(input_call != NULL) input_call(x);   
  } 
}



void uart3_putc( char x)
{
  USART_SendData(USART3, x);
    /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}
