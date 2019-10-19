#include "soft_iic.h"


/*********************************************************************************************
* 名称:I2C1_GPIOInit()
* 功能:初始化I2C1
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C1_GPIOInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(I2C1_SCL_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C1_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(I2C1_SCL_GPIO, &GPIO_InitStructure);
  
    RCC_AHB1PeriphClockCmd(I2C1_SDA_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C1_SDA_PIN;
    GPIO_Init(I2C1_SDA_GPIO, &GPIO_InitStructure);

    SDA1_H;
    SCL1_H;
}
/*********************************************************************************************
* 名称:I2C1_Start()
* 功能:I2C1初始信号
* 参数:无
* 返回:0
* 修改:
* 注释:
*********************************************************************************************/
void I2C1_Start(void)
{
  SDA1_OUT;
  SDA1_H;
  SCL1_H;
  IIC_DelayUs(2);
  SDA1_L;
  IIC_DelayUs(2);
  SCL1_L;
}

/*********************************************************************************************
* 名称:I2C1_Stop()
* 功能:I2C1停止信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C1_Stop(void)
{
  SDA1_OUT;
  SCL1_L;
  SDA1_L;
  IIC_DelayUs(2);
  SCL1_H;
  SDA1_H;
  IIC_DelayUs(2);
}

/*********************************************************************************************
* 名称:I2C1_Ack()
* 功能:I2C1应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C1_Ack(void)
{
  SCL1_L;
  SDA1_OUT;
  SDA1_L;
  IIC_DelayUs(2);
  SCL1_H;
  IIC_DelayUs(2);
  SCL1_L;
}

/*********************************************************************************************
* 名称:I2C1_NoAck()
* 功能:I2C1非应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C1_NoAck(void)
{
  SCL1_L;
  SDA1_OUT;
  SDA1_H;
  IIC_DelayUs(2);
  SCL1_H;
  IIC_DelayUs(2);
  SCL1_L;
}

/*********************************************************************************************
* 名称:I2C1_WaitAck()
* 功能:I2C1等待应答信号
* 参数:无
* 返回:返回为:=1有ACK,=0无ACK
* 修改:
* 注释:
*********************************************************************************************/
int I2C1_WaitAck(void)
{
  uint8_t ucErrTime=0;
  SDA1_H;
  SDA1_IN;
  IIC_DelayUs(2);
  SCL1_H;
  IIC_DelayUs(2);
  while (SDA1_R)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      I2C1_Stop();
      return 1;
    }
  }
  SCL1_L;
  return 0;
}

/*********************************************************************************************
* 名称:I2C1_WriteByte()
* 功能:I2C1写字节数据
* 参数:char SendByte -- 发送数据
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C1_WriteByte(uint8_t SendByte) //数据从高位到低位//
{
  uint8_t i=8;
  SDA1_OUT;
  while(i--)
  {
    if(SendByte&0x80) SDA1_H;
    else SDA1_L;
    SendByte<<=1;
    IIC_DelayUs(1);
    SCL1_H;
    IIC_DelayUs(2);
    SCL1_L;
    IIC_DelayUs(1);
  }
  SDA1_H;
}

/*********************************************************************************************
* 名称:I2C1_ReadByte()
* 功能:I2C1读字节数据，数据从高位到低位
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
uint8_t I2C1_ReadByte(void)
{
  uint8_t i=8;
  uint8_t ReceiveByte=0;
  
  SDA1_H;
  SDA1_IN;
  while(i--)
  {
    ReceiveByte<<=1;
    SCL1_L;
    IIC_DelayUs(2);
    SCL1_H;
    IIC_DelayUs(2);
    if(SDA1_R)
    {
      ReceiveByte|=0x01;
    }
  }
  SCL1_L;
  return ReceiveByte;
}

