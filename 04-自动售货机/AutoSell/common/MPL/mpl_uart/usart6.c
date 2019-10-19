#include "usart.h"	
#include "string.h"
#include "stm32f4xx.h"



/*********************************************************************************************
* 名称：usart1_init
* 功能：usart1初始化
* 参数：bound波特率
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void uart6_init(unsigned int bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);     //使能GPIOC时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);    //使能USART6时钟
 
  //串口6对应引脚复用映射
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);  //GPIOC6复用为USART6
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOAC7复用为USART6
	
  //USART6端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //GPIOA9与GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             //复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //上拉
  GPIO_Init(GPIOC,&GPIO_InitStructure);                    //初始化PC6，PC7

  //USART6 初始化设置
  USART_InitStructure.USART_BaudRate = bound;                      //波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;           //一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;              //无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //收发模式
  USART_Init(USART6, &USART_InitStructure);                        //根据上述配置初始化串口6
	
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;               //串口1中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;         //抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		  //子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		  //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);	                          //根据指定的参数初始化VIC寄存器、

  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);                  //开启串口6接收中断
  USART_Cmd(USART6, ENABLE);                                      //使能串口6	
}


static int (*input_call)( char )  = NULL;
void uart6_set_input(int(*fun)( char))
{
  input_call = fun;
}
/*********************************************************************************************
* 名称：USART6_IRQHandler
* 功能：串口中断处理函数
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void USART6_IRQHandler(void)                	           
{
  char c;
  if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET){  
    c = USART_ReceiveData(USART6);
    if (input_call != NULL) {
      input_call(c);
    }
  }
}


/*********************************************************************************************
* 名称：usart6_send
* 功能：串口6发送数据
* 参数：s待发送的数据指针，len待发送的数据长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/ 
void uart6_putc( char x)
{
  USART_SendData(USART6, x);
  // Loop until the end of transmission
  while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
}


