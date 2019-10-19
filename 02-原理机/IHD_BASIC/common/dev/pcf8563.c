#include "pcf8563.h"
#include <sys/clock.h>
#include "stdio.h"
void clock_delay_us(unsigned int ut);
#define RTC_I2C_delay(x) clock_delay_us(x)
//#define RTC_I2C_delay(x) do{int i,a=x; for(i=0;i<a; i++){for(int j=0;j<702;j++);}}while(0)

/*********************************************************************************************
* 名称：模拟IIC总线io的配置
* 功能：初始化IO(PF0、PF1,推挽输出、上拉)
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void RTC_I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  RTC_I2C_SDA = 1;
  RTC_I2C_SCL = 1;
}


/***********************************************************
* 名称: RTC_I2C_Start()
* 功能: IIC开始函数
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/

int RTC_I2C_Start(void)
{
  RTC_SDA_OUT();                                                //SDA输出
  RTC_I2C_SDA = 1;
  RTC_I2C_SCL = 1;
  RTC_I2C_delay(5);
  RTC_I2C_SDA = 0;
  RTC_I2C_delay(5);
  RTC_I2C_SCL = 0;
  return 0;
}

/***********************************************************
* 名称: Stop()
* 功能: IIC停止函数
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/

void RTC_I2C_Stop(void)
{
  RTC_SDA_OUT();                                                //SDA输出
  RTC_I2C_SCL = 0;
  RTC_I2C_SDA = 0;
  RTC_I2C_delay(5);
  RTC_I2C_SCL = 1;
   RTC_I2C_delay(5);
  RTC_I2C_SDA = 1;
  RTC_I2C_delay(5);
}

/***********************************************************
* 名称: RTC_I2C_Ack()
* 功能: 主机发送应答信号
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/

void RTC_I2C_Ack(void)
{
  RTC_SDA_OUT();
  RTC_I2C_SCL = 0;
  RTC_I2C_delay(2);
  RTC_I2C_SDA = 0;   
  RTC_I2C_delay(3);
  RTC_I2C_SCL = 1; 
  RTC_I2C_delay(5);
  RTC_I2C_SCL = 0;

}

/***********************************************************
* 名称: RTC_I2C_NoAck()
* 功能: 不发送确认信号
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/

void RTC_I2C_NoAck(void)
{
  RTC_SDA_OUT();
  RTC_I2C_SCL = 0;
  RTC_I2C_delay(2);
  RTC_I2C_SDA = 1;
  RTC_I2C_delay(3);
  RTC_I2C_SCL = 1;
  RTC_I2C_delay(5);
  RTC_I2C_SCL = 0;
}
/***********************************************************
* 名称: RTC_I2C_WaitAck()
* 功能: 等待从设备发送应答信号
* 参数: 无
* 返回: 1为无ACK,0为有ACK
* 修改:
* 注释: 
***********************************************************/

int RTC_I2C_WaitAck(void) 	
{
  unsigned char ucErrTime=0;
  
  RTC_I2C_SDA=1;
  RTC_SDA_IN();
  //RTC_I2C_SCL=0;
  RTC_I2C_delay(5);	   
  RTC_I2C_SCL=1;
  RTC_I2C_delay(2);	
  while(RTC_SDA_read)
   {
    ucErrTime++;
	if(ucErrTime>250){
	 RTC_I2C_Stop();
	 return 1;
        }
   }
  RTC_I2C_SCL=0;//ê±?óê?3?0 	   
  return 0;  
} 

/***********************************************************
* 名称: RTC_I2C_SendByte()
* 功能: 发送八位数据
* 参数: 输入 SendByte 需要发送的字节
* 返回: 无
* 修改:
* 注释: 
***********************************************************/

void RTC_I2C_SendByte(unsigned char SendByte)                   //数据从高位到低位
{ 
    RTC_SDA_OUT(); 	    
    //RTC_I2C_SCL=0;
    for(unsigned char t=0;t<8;t++){              
        	
        RTC_I2C_delay(2);   
        RTC_I2C_SDA=(SendByte&0x80)>>7;
        SendByte<<=1; 	  
	RTC_I2C_delay(3);   
	RTC_I2C_SCL=1;
	RTC_I2C_delay(5); 
	RTC_I2C_SCL=0;
	//RTC_I2C_delay(2);
    }	 
}

/***********************************************************
* 名称: RTC_I2C_ReceiveByte()
* 功能: 读取八位数据
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/

unsigned char RTC_I2C_ReceiveByte(void)                         //数据从高位到低位//
{ 
unsigned char i,receive=0;
      RTC_I2C_SDA=1;
	RTC_SDA_IN();
    for(i=0;i<8;i++ )
	{
        
        RTC_I2C_delay(5);
	RTC_I2C_SCL=1;
        RTC_I2C_delay(3);         
        receive<<=1;
        if(RTC_SDA_read)receive++;   
        RTC_I2C_delay(2);
	 RTC_I2C_SCL=0;	
    }					 
    return receive;
}



/***********************************************************
* 名称: PCF8563_write_byte()
* 功能: 向8563时钟芯片写入一个字节
* 参数: 无
* 返回: 无
* 修改:
* 注释: addr为写的寄存器地址;dat寄存器要写入的值
***********************************************************/
void PCF8563_write_byte(unsigned char addr,unsigned char dat){
   RTC_I2C_Start();                                                 //开启iic
   RTC_I2C_SendByte(0xA2);                                          //选中PCF8563，并让其做好被写入的准备(PCF8563的写地址为0xA2)
   RTC_I2C_WaitAck();           
   RTC_I2C_SendByte(addr);                                          //写入要写的寄存器地址
   RTC_I2C_WaitAck();           
   RTC_I2C_SendByte(dat);                                           //写入要写的数据
   RTC_I2C_WaitAck();           
   RTC_I2C_Stop();                                                  //停止iic
}

/***********************************************************
* 名称: PCF8563_init()
* 功能: 初始化时钟芯片
* 参数: 无
* 返回: 无
* 修改:
* 注释: 
***********************************************************/
void PCF8563_init(void){
 RTC_I2C_GPIO_Config();
 PCF8563_write_byte(0x00,0x00);                                 //0x00,0x01为控制/状态寄存器地址
 PCF8563_write_byte(0x01,0x12);                                 //脉冲,报警中断有效
 PCF8563_write_byte(0x09,0x80);                                 //关闭分钟报警//  
 PCF8563_write_byte(0x0A,0x80);                                 //关闭小时报警
 PCF8563_write_byte(0x0B,0x80);                                 //关闭日报警
 PCF8563_write_byte(0x0C,0x80);                                 //关闭星期报警
 PCF8563_write_byte(0x0D,0x00);                                 //关闭CLKOUT输出
 PCF8563_write_byte(0x0E,0x03);                                 //关闭定时器
}
/***********************************************************
* 名称: PCF8563_read_byte()
* 功能: 从PCF8563读取一个字节
* 参数: 无
* 返回: 读出的数据
* 修改:
* 注释: addr为要读的地址，函数返回读出的数据
***********************************************************/
unsigned char PCF8563_read_byte(unsigned char addr){
 unsigned char dat;
 RTC_I2C_Start();                                               //开启iic
 RTC_I2C_SendByte(0xA2);                                        //选中PCF8563，并让其做好被写入的准备(PCF8563的写地址为0xA2)
 RTC_I2C_WaitAck();           
 RTC_I2C_SendByte(addr);                                        //写入要读的地址
 RTC_I2C_WaitAck();           
 RTC_I2C_Start();                                               //重新开始新通讯
 RTC_I2C_SendByte(0xA3);                                        //PCF8563的读地址为0xA3,准备开始读取
 RTC_I2C_WaitAck();           	
 dat=RTC_I2C_ReceiveByte();                                     //将读出来的值赋给参数
 RTC_I2C_NoAck();                                               //因为只读一个字节，所以不用发送应答信号
 RTC_I2C_Stop();                                                //停止iic
 return(dat);
}

/*********************************************************************************************
* 名称: PCF8563_Read_NByte()
* 功能: 从PCF8563读取N个字节
* 参数: 
* 返回: 
* 修改:
* 注释: 
*********************************************************************************************/
unsigned char * PCF8563_Read_NByte(unsigned char addr ,unsigned char len,unsigned char *buf){
 unsigned char i;
 RTC_I2C_Start();                
 RTC_I2C_SendByte(0xA2);       
 if (RTC_I2C_WaitAck()) {
   return NULL;
 }
 RTC_I2C_SendByte(addr);        
 if ( RTC_I2C_WaitAck()) {
   return NULL;
 }
 RTC_I2C_Start();                
 RTC_I2C_SendByte(0xA3); 
 if (RTC_I2C_WaitAck()) {
   return NULL;
 }
 for(i=0;i<len;i++){
   buf[i]=RTC_I2C_ReceiveByte();
   if(i<len-1)             
    RTC_I2C_Ack();
 }
 RTC_I2C_NoAck();          
 RTC_I2C_Stop();              
 return buf;
}

int PCF8563_gettime(char *buf)
{
  if (PCF8563_Read_NByte(0x02, 7, (unsigned char *)buf)) return 0;
  
  return -1;
}

/*********************************************************************************************
* 名称: PCF8563_settime()
* 功能: 设置时间
* 参数: 
* 返回: 
* 修改:
* 注释: 时间的格式是秒分时日周月年 BCD码
* 如：unsigned char Time[7]={0x55,0x59,0x23,0x28,0x01,0x02,0x03};
* PCF8563_settime(Time);
*********************************************************************************************/
void PCF8563_settime(u8*buf){
  for(unsigned char i=0;i<7;i++){
  PCF8563_write_byte(0x02+i,buf[i]);
  }
}

