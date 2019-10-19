#include "wk2xxx.h"

/*串口一扩四驱动*/

static uint8_t Wk2114_IrqStatus = 0;
static char Wk2114_Buffer = 0;

int Wk2114_UartIrqCallback(char byte)
{
  Wk2114_IrqStatus = 1;
  Wk2114_Buffer = byte;
  return 0;
}

//读取串口数据 
int Wk2114_ReadByte(void) 
{  
  uint16_t timeout = 0;
  
  while(Wk2114_IrqStatus == 0)
  {
    delay_us(1);
    timeout++;
    if(timeout > 899)
      return 0;
  }
  Wk2114_IrqStatus = 0;
  return Wk2114_Buffer;
}

//通过串口写入 1 个字节的数据，byte 为数据 
void Wk2114_WriteByte(unsigned char byte) 
{ 
  uart2_putc(byte);
  delay_us(50);//延时比较重要
} 


void WK2114_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //上下拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIO时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //对应引脚
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
  
  Exti2_Init(EXTI_PortSourceGPIOD);
  Exti2IrqCall_Set(Wk2114_ExIrqCallback);
  
  uart2_init(460800);//wk2114主串口,波特率要高一点
  uart2_set_input(Wk2114_UartIrqCallback);
  
	Wk2114_config();
}

void Wk2114_Rest()
{
  WK2114_REST_L;
  delay_ms(300);
  WK2114_REST_H;
  delay_ms(20);
}

void Wk2114_RestInit()
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  RCC_AHB1PeriphClockCmd(WK2114_REST_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = WK2114_REST_PIN;
  GPIO_Init(WK2114_REST_GPIO, &GPIO_InitStructure);
  
  WK2114_REST_H;
}


/********************************************************************************************/

/**************************************Wk2114WriteReg***********************************/
//函数功能：写寄存器函数（前提是该寄存器可写，某些寄存器如果你写1，可能会自动置1，具体见数据手册)
//参数：port:为子串口的数(C0C1)
//      reg:为寄存器的地址(A3A2A1A0)
//      byte:为写入寄存器的数据
//注意：在子串口被打通的情况下，向FDAT写入的数据会通过TX引脚输出
//*************************************************************************/
static void Wk2114WriteReg(unsigned char port,unsigned char reg,unsigned char byte)
{	 
  Wk2114_WriteByte(((port-1)<<4)+reg);	//写指令，对于指令的构成见数据手册
  Wk2114_WriteByte(byte);//写数据
}


/*************************************Wk2114ReadReg************************************/
//函数功能：读寄存器函数
//参数：port:为子串口的数(C0C1)
//      reg:为寄存器的地址(A3A2A1A0)
//      rec_bytea:为读取到的寄存器值
//注意：在子串口被打通的情况下，读FDAT，实际上就是读取uart的rx接收的数据
/*************************************************************************/
static unsigned char Wk2114ReadReg(unsigned char port,unsigned char reg)
{	 
  unsigned char rec_bytea=0;
  Wk2114_WriteByte(0x40+((port-1)<<4)+reg); //写指令，对于指令的构成见数据手册
  rec_bytea=Wk2114_ReadByte();	 //接收返回的寄存器值									
  return rec_bytea;
}



/**************************************Wk2114writeFIFO***********************************/
//函数功能：写FIFO函数（该函数写入的数据会通过uart的TX发送出去)
//参数：port:为子串口的数(C0C1)
//      *byte：为写入数据指针
//      num：为写入数据的个数，不超过16个字节（N3N2N1N0）
/*************************************************************************/
//void Wk2114writeFIFO(unsigned char port,unsigned char *send_da,unsigned char num)
//{	 
//    unsigned char i;
//    Wk2114_WriteByte(0x80+((port-1)<<4)+(num-1)); //写指令,对于指令构成见数据手册
//    for(i=0;i<num;i++)
//    {
//        Wk2114_WriteByte( *(send_da+i));//写数据
//    }
//}

/***************************************Wk2114readFIFO**********************************/
//函数功能：读FIFO函数（该函数读取的数据是FIFO缓存中的数据，实际上是uart的rx接收的数据)
//参数：port:为子串口的数(C0C1)
//      *byte：为读到数据指针
//      num：为读出数据的个数，不超过16个字节（N3N2N1N0）
/*************************************************************************/
//unsigned char *Wk2114readFIFO(unsigned char port,unsigned char num)
//{
//    unsigned char n;
//    unsigned char byte[256];
//    Wk2114_WriteByte(0xc0+((port-1)<<4)+(num-1));
//    for(n=0;n<num;n++)
//    {	
//        byte[n]=Wk2114_ReadByte();	
//    }
//    return byte;
//}

/******************************Wk2114PortInit*******************************************/
//函数功能：本函数主要会初始化一些芯片基本寄存器；
/*********************************************************************************/
void Wk2114PortInit(unsigned char port)
{
  unsigned char gena,grst,gier,sier,scr;
	//使能子串口时钟
  gena=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
  {
  case 1://使能子串口1的时钟
    gena|=WK2XXX_UT1EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  case 2://使能子串口2的时钟
    gena|=WK2XXX_UT2EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  case 3://使能子串口3的时钟
    gena|=WK2XXX_UT3EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  case 4://使能子串口4的时钟
    gena|=WK2XXX_UT4EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  }	
	//软件复位子串口
	grst=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
  {
  case 1://软件复位子串口1
    grst|=WK2XXX_UT1RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  case 2://软件复位子串口2
    grst|=WK2XXX_UT2RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  case 3://软件复位子串口3
    grst|=WK2XXX_UT3RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  case 4://软件复位子串口4
    grst|=WK2XXX_UT4RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  }	
  //使能子串口中断，包括子串口总中断和子串口内部的接收中断，和设置中断触点
	gier=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GIER); 
	switch (port)
  {
  case 1:
    gier|=WK2XXX_UT1RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
    break;
  case 2:
    gier|=WK2XXX_UT2RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
    break;
  case 3:
    gier|=WK2XXX_UT3RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
    break;
  case 4:
    gier|=WK2XXX_UT4RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
    break;
  }	 
  //使能子串口接收触点中断和超时中断
  sier=Wk2114ReadReg(port,WK2XXX_SIER); 
  sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
  sier |= WK2XXX_RFTRIG_IEN;
  Wk2114WriteReg(port,WK2XXX_SIER,sier);
  // 初始化FIFO和设置固定中断触点
  Wk2114WriteReg(port,WK2XXX_FCR,0XFF);
  //设置任意中断触点，如果下面的设置有效，那么上面FCR寄存器中断的固定中断触点将失效
  Wk2114WriteReg(port,WK2XXX_SPAGE,1);//切换到page1
  Wk2114WriteReg(port,WK2XXX_RFTL,0X01);//设置接收触点1个字节
  Wk2114WriteReg(port,WK2XXX_TFTL,0xff);//设置发送触点为256个字节
  Wk2114WriteReg(port,WK2XXX_SPAGE,0);//切换到page0 
  //使能子串口的发送和接收使能
  scr=Wk2114ReadReg(port,WK2XXX_SCR); 
  scr|=WK2XXX_TXEN|WK2XXX_RXEN;
  Wk2114WriteReg(port,WK2XXX_SCR,scr);
}
/******************************Wk2114Close*******************************************/
//函数功能：本函数会关闭当前子串口，和复位初始值；
/*********************************************************************************/

void Wk2114Close(unsigned char port)
{
  unsigned char gena,grst;
	//复位子串口
	grst=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
  {
  case 1://软件复位子串口1
    grst&=~WK2XXX_UT1RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  case 2://软件复位子串口2
    grst&=~WK2XXX_UT2RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  case 3://软件复位子串口3
    grst&=~WK2XXX_UT3RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  case 4://软件复位子串口4
    grst&=~WK2XXX_UT4RST;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
    break;
  }	
	//关闭子串口时钟
  gena=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
  {
  case 1://使能子串口1的时钟
    gena&=~WK2XXX_UT1EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  case 2://使能子串口2的时钟
    gena&=~WK2XXX_UT2EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  case 3://使能子串口3的时钟
    gena&=~WK2XXX_UT3EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  case 4://使能子串口4的时钟
    gena&=~WK2XXX_UT4EN;
    Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
    break;
  }	
}
/**************************Wk2114SetBaud*******************************************************/
//函数功能：设置子串口波特率函数、此函数中波特率的匹配值是根据11.0592Mhz下的外部晶振计算的
// port:子串口号
// baud:波特率大小.波特率表示方式，
//
/**************************Wk2114SetBaud*******************************************************/
void Wk2114SetBaud(unsigned char port,int baud)
{  
	unsigned char baud1,baud0,pres,scr;
	//如下波特率相应的寄存器值，是在外部时钟为11.0592的情况下计算所得，如果使用其他晶振，需要重新计算
	switch (baud) 
	{
  case 600:
    baud1=0x4;
    baud0=0x7f;
    pres=0;
    break;
  case 1200:
    baud1=0x2;
    baud0=0x3F;
    pres=0;
    break;
  case 2400:
    baud1=0x1;
    baud0=0x1f;
    pres=0;
    break;
  case 4800:
    baud1=0x00;
    baud0=0x8f;
    pres=0;
    break;
  case 9600:
    baud1=0x00;
    baud0=0x47;
    pres=0;
    break;
  case 19200:
    baud1=0x00;
    baud0=0x23;
    pres=0;
    break;
  case 38400:
    baud1=0x00;
    baud0=0x11;
    pres=0;
    break;
  case 76800:
    baud1=0x00;
    baud0=0x08;
    pres=0;
    break; 
    
  case 1800:
    baud1=0x01;
    baud0=0x7f;
    pres=0;
    break;
  case 3600:
    baud1=0x00;
    baud0=0xbf;
    pres=0;
    break;
  case 7200:
    baud1=0x00;
    baud0=0x5f;
    pres=0;
    break;
  case 14400:
    baud1=0x00;
    baud0=0x2f;
    pres=0;
    break;
  case 28800:
    baud1=0x00;
    baud0=0x17;
    pres=0;
    break;
  case 57600:
    baud1=0x00;
    baud0=0x0b;
    pres=0;
    break;
  case 115200:
    baud1=0x00;
    baud0=0x05;
    pres=0;
    break;
  case 230400:
    baud1=0x00;
    baud0=0x02;
    pres=0;
    break;
  default:
    baud1=0x00;
    baud0=0x00;
    pres=0;
  }
	//关掉子串口收发使能
	scr=Wk2114ReadReg(port,WK2XXX_SCR); 
	Wk2114WriteReg(port,WK2XXX_SCR,0);
	//设置波特率相关寄存器
	Wk2114WriteReg(port,WK2XXX_SPAGE,1);//切换到page1
	Wk2114WriteReg(port,WK2XXX_BAUD1,baud1);
	Wk2114WriteReg(port,WK2XXX_BAUD0,baud0);
	Wk2114WriteReg(port,WK2XXX_PRES,pres);
	Wk2114WriteReg(port,WK2XXX_SPAGE,0);//切换到page0 
	//使能子串口收发使能
	Wk2114WriteReg(port,WK2XXX_SCR,scr);
}


void Wk2114BaudAdaptive()
{
  Wk2114_Rest();
  Wk2114_WriteByte(0x55);
  delay_ms(200);
  
}

void Wk2114_config()
{
	//uint32_t baud = B38400;
  
  Wk2114_RestInit();
  Wk2114BaudAdaptive();
  
//	Wk2114PortInit(1);          //初始化设置
//	Wk2114SetBaud(1,baud);  //设置子串口波特率
  
//	Wk2114PortInit(2);
//	Wk2114SetBaud(2,baud);
  
//	Wk2114PortInit(3);
//	Wk2114SetBaud(3,baud);
  
//	Wk2114PortInit(4);
//	Wk2114SetBaud(4,baud);	
  
	delay_ms(10);
}



/*WK2114子串口发送数据*/
void Wk2114_Uart1SendByte(char byte)
{
  Wk2114WriteReg(1,WK2XXX_FDAT,byte);	
}

void Wk2114_Uart2SendByte(char byte)
{
  Wk2114WriteReg(2,WK2XXX_FDAT,byte);	
}

void Wk2114_Uart3SendByte(char byte)
{
  Wk2114WriteReg(3,WK2XXX_FDAT,byte);	
}

void Wk2114_Uart4SendByte(char byte)
{
  Wk2114WriteReg(4,WK2XXX_FDAT,byte);	
}



int (*Wk2114_SlaveRecv[4])(char byte) = {NULL};

uint8_t Wk2114_SlaveRecv_Set(uint8_t index,int (*func)(char byte))
{
  if((index==0) || (index>4))
    return 1;
  Wk2114_SlaveRecv[index-1] = func;
  return 0;
}


void Wk2114_ExIrqCallback()
{
  uint8_t status=0;
  char byte1,byte2,byte3,byte4;
  
	do
	{
    status=Wk2114ReadReg(1,WK2XXX_GIFR);//判断中断源
    
		if(status&WK2XXX_UT1INT)//判断子串口1
		{
			while((Wk2114ReadReg(1,WK2XXX_FSR)&WK2XXX_RDAT))//判断接收fifo中是否有数据，直到把FIFO读空
			{
				byte1 = Wk2114ReadReg(1,WK2XXX_FDAT);
        if(Wk2114_SlaveRecv[0] != NULL)
          Wk2114_SlaveRecv[0](byte1);
			}	
		}
    
		if(status&WK2XXX_UT2INT)//判断子串口2
		{
			while((Wk2114ReadReg(2,WK2XXX_FSR)&WK2XXX_RDAT))
			{
				byte2=Wk2114ReadReg(2,WK2XXX_FDAT);
        if(Wk2114_SlaveRecv[1] != NULL)
          Wk2114_SlaveRecv[1](byte2);
			}	
		} 
    
		if(status&WK2XXX_UT3INT)  //判断子串口3
		{
			while((Wk2114ReadReg(3,WK2XXX_FSR)&WK2XXX_RDAT))
			{
				byte3 = Wk2114ReadReg(3,WK2XXX_FDAT);
        if(Wk2114_SlaveRecv[2] != NULL)
          Wk2114_SlaveRecv[2](byte3);
			}	
		}
    
		if(status&WK2XXX_UT4INT) //判断子串口4
		{
			while((Wk2114ReadReg(4,WK2XXX_FSR)&WK2XXX_RDAT))
			{
				byte4 = Wk2114ReadReg(4,WK2XXX_FDAT);
        if(Wk2114_SlaveRecv[3] != NULL)
          Wk2114_SlaveRecv[3](byte4);
			}	
		}
	}
	while((0x0f&Wk2114ReadReg(1,WK2XXX_GIFR))!=0);  //判断是否还有中断，没有中断再退出
}

