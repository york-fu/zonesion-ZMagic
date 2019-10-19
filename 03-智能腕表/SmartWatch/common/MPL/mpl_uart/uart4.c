#include "usart.h"

void uart4_init(unsigned int bpr)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  
    //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOA10复用为USART1
  
  /* Configure USARTx_Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度10MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = bpr;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(UART4, &USART_InitStructure);
  
    /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Enable USARTy Receive and Transmit interrupts */
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
  
   /* Enable the USARTy */
  USART_Cmd(UART4, ENABLE);
  
}

void uart4_putc( char x)
{
  USART_SendData(UART4, x);
    /* Loop until the end of transmission */
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
}

static int (*input_call)( char )  = NULL;
void uart4_set_input(int(*fun)( char))
{
  input_call = fun;
}

void UART4_IRQHandler(void)
{
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
    unsigned char x;
    x = USART_ReceiveData(UART4);
    if(input_call != NULL) input_call(x);
  }
}