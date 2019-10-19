#include "usart.h"	
#include "string.h"
#include "stm32f4xx.h"


/*********************************************************************************************
* 名称：usart5_init
* 功能：usart5初始化
* 参数：bound波特率
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void uart5_init(unsigned int bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);          //使能GPIOC时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);          //使能GPIOD时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);          //使能UART5时钟
 
  //串口5对应引脚复用映射
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);       //GPIOC12复用为UART5
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);        //GPIOD2复用为UART5
	
  //UART5端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                    //GPIOC12
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	        //速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOC,&GPIO_InitStructure);                         //初始化PC12
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                     //GPIOD2
  GPIO_Init(GPIOD,&GPIO_InitStructure);                         //初始化PD2(配置同上)
  
  //UART5 初始化设置
  USART_InitStructure.USART_BaudRate = bound;                   //波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;           //无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
  USART_Init(UART5, &USART_InitStructure);                      //根据上述配置初始化串口5
	
  //Uart5 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;              //串口5中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;       //抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器、

  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);                 //开启串口5接收中断
  USART_Cmd(UART5, ENABLE);                                     //使能串口5 	
}



void uart5_putc(unsigned char x)
{
  USART_SendData(UART5, x);
    /* Loop until the end of transmission */
  while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
}

static int (*input_call)(unsigned char )  = NULL;
void uart5_set_input(int(*fun)(unsigned char))
{
  input_call = fun;
}

/*********************************************************************************************
* 名称：UART5_IRQHandler
* 功能：串口中断处理函数
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void UART5_IRQHandler(void)                	           
{
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) {
    unsigned char x;
    x = USART_ReceiveData(UART5);
    if(input_call != NULL) input_call(x);
  }
}
