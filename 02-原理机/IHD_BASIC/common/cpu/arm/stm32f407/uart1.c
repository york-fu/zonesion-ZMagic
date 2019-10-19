#include <stdio.h>
#include "stm32f4xx.h"



void uart1_init(unsigned int bpr)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
        
  /* Configure USARTx_Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度10MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
  USART_InitStructure.USART_BaudRate = bpr;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);
  
    /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Enable USARTy Receive and Transmit interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
   /* Enable the USARTy */
  USART_Cmd(USART1, ENABLE);
  
}

void uart1_putc( char x)
{
  USART_SendData(USART1, x);
    // Loop until the end of transmission 
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*PUTCHAR_PROTOTYPE
{
  uart1_putc(ch);
  return ch;
}*/
static int (*input_call)( char x) = NULL;
void  uart1_set_input(int (*fun)( char ) )
{
 
  input_call = fun;
}

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    unsigned char x;
    x = USART_ReceiveData(USART1);
    if(input_call != NULL) input_call(x);
  }
  
}