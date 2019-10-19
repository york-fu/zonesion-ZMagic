#include "stm32f4xx.h"
#include "extend.h"
#include "delay.h"

//IO方向设置
#define SDA_IN()  {GPIOC->MODER&=~(3<<(5*2));GPIOC->MODER|=0<<5*2;}	//Pc9输入模式
#define SDA_OUT() {GPIOC->MODER&=~(3<<(5*2));GPIOC->MODER|=1<<5*2;}     //Pc9输出模式
//IO操作函数
#define IIC_SCL    PCout(4) //SCL
#define IIC_SDA    PCout(5) //SDA	 
#define READ_SDA   PCin(5)  //输入SDA 

//初始化IIC
void IIC3_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOB时钟

    //GPIOB8,B9初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化

    IIC_SCL=1;
    IIC_SDA=1;
}
//产生IIC起始信号
void IIC3_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA=1;
    IIC_SCL=1;
    delay_us(4);
    IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC3_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL=1;
    delay_us(2);
    IIC_SDA=1;//发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC3_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA=1;
    delay_us(1);
    IIC_SCL=1;
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC3_Stop();
            return 1;
        }
    }
    IIC_SCL=0;//时钟输出0
    return 0;
}
//产生ACK应答
void IIC3_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}
//不产生ACK应答
void IIC3_NAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC3_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL=1;
        delay_us(2);
        IIC_SCL=0;
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC3_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0; i<8; i++ )
    {
        IIC_SCL=0;
        delay_us(2);
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC3_NAck();//发送nACK
    else
        IIC3_Ack(); //发送ACK
    return receive;
}



int IIC3_Read(char addr, char r, char *buf, int len)
{
    int i;

    IIC3_Start();
    IIC3_Send_Byte(addr<<1);
    if (IIC3_Wait_Ack()) return -1;
    IIC3_Send_Byte(r);
    if (IIC3_Wait_Ack()) return -1;
    IIC3_Start();
    IIC3_Send_Byte((addr<<1)|1);
    if (IIC3_Wait_Ack()) return -1;
    for (i=0; i<len-1; i++)
    {
        buf[i] = IIC3_Read_Byte(1);
    }
    buf[i++] = IIC3_Read_Byte(0);
    IIC3_Stop();
    return i;
}
int IIC3_Write(char addr, char r, char *buf, int len)
{
    int i;

    IIC3_Start();
    IIC3_Send_Byte(addr<<1);
    if (IIC3_Wait_Ack()) return -1;
    IIC3_Send_Byte(r);
    if (IIC3_Wait_Ack()) return -1;
    for (i=0; i<len; i++)
    {
        IIC3_Send_Byte(buf[i]);
        if (IIC3_Wait_Ack()) return -1;
    }
    IIC3_Stop();
    return i;
}