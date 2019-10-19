#include "soft_iic.h"


/*********************************************************************************************
* 名称:I2C2_GPIOInit()
* 功能:初始化I2C2
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C2_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(I2C2_SCL_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = I2C2_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C2_SCL_GPIO, &GPIO_InitStructure);
  
  RCC_AHB1PeriphClockCmd(I2C2_SDA_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = I2C2_SDA_PIN;
  GPIO_Init(I2C2_SDA_GPIO, &GPIO_InitStructure);
  
  SDA2_H;
  SCL2_H;
}
/*********************************************************************************************
* 名称:I2C2_Start()
* 功能:I2C2初始信号
* 参数:无
* 返回:0
* 修改:
* 注释:
*********************************************************************************************/
void I2C2_Start(void)
{
  SDA2_OUT;
  SDA2_H;
  SCL2_H;
  IIC_DelayUs(2);
  SDA2_L;
  IIC_DelayUs(2);
  SCL2_L;
}

/*********************************************************************************************
* 名称:I2C2_Stop()
* 功能:I2C2停止信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C2_Stop(void)
{
  SDA2_OUT;
  SCL2_L;
  SDA2_L;
  IIC_DelayUs(2);
  SCL2_H;
  SDA2_H;
  IIC_DelayUs(2);
}

/*********************************************************************************************
* 名称:I2C2_Ack()
* 功能:I2C2应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C2_Ack(void)
{
  SCL2_L;
  SDA2_OUT;
  SDA2_L;
  IIC_DelayUs(2);
  SCL2_H;
  IIC_DelayUs(2);
  SCL2_L;
}

/*********************************************************************************************
* 名称:I2C2_NoAck()
* 功能:I2C2非应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C2_NoAck(void)
{
  SCL2_L;
  SDA2_OUT;
  SDA2_H;
  IIC_DelayUs(2);
  SCL2_H;
  IIC_DelayUs(2);
  SCL2_L;
}

/*********************************************************************************************
* 名称:I2C2_WaitAck()
* 功能:I2C2等待应答信号
* 参数:无
* 返回:返回为:=1有ACK,=0无ACK
* 修改:
* 注释:
*********************************************************************************************/
int I2C2_WaitAck(void)
{
  uint8_t ucErrTime=0;
  SDA2_H;
  IIC_DelayUs(2);
  SDA2_IN;
  SCL2_H;
  IIC_DelayUs(2);
  while (SDA2_R)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      I2C2_Stop();
      return 1;
    }
  }
  SCL2_L;
  return 0;
}

/*********************************************************************************************
* 名称:I2C2_WriteByte()
* 功能:I2C2写字节数据
* 参数:char SendByte -- 发送数据
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C2_WriteByte(uint8_t SendByte) //数据从高位到低位//
{
  uint8_t i=8;
  SDA2_OUT;
  while(i--)
  {
    if(SendByte&0x80) SDA2_H;
    else SDA2_L;
    SendByte<<=1;
    IIC_DelayUs(1);
    SCL2_H;
    IIC_DelayUs(2);
    SCL2_L;
    IIC_DelayUs(1);
  }
  SDA2_H;
}

/*********************************************************************************************
* 名称:I2C2_ReadByte()
* 功能:I2C2读字节数据，数据从高位到低位
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
uint8_t I2C2_ReadByte(void)
{
  uint8_t i=8;
  uint8_t ReceiveByte=0;
  
  SDA2_H;
  SDA2_IN;
  while(i--)
  {
    ReceiveByte<<=1;
    SCL2_L;
    IIC_DelayUs(2);
    SCL2_H;
    IIC_DelayUs(2);
    if(SDA2_R)
    {
      ReceiveByte|=0x01;
    }
  }
  SCL2_L;
  return ReceiveByte;
}

