#include "soft_iic.h"


void IIC_DelayUs(uint16_t t)
{
  unsigned int n = 0;
  while(t--)
  {
    for(n=11;n>0;n--);//168MHz，约1us
  }
}

/*********************************************************************************************
* 名称:I2C_GPIOInit()
* 功能:初始化I2C
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(I2C_SCL_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);
  
  RCC_AHB1PeriphClockCmd(I2C_SDA_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
  GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
  
  SDA_H;
  SCL_H;
}
/*********************************************************************************************
* 名称:I2C_Start()
* 功能:I2C初始信号
* 参数:无
* 返回:0
* 修改:
* 注释:
*********************************************************************************************/
void I2C_Start(void)
{
  SDA_OUT;
  SDA_H;
  SCL_H;
  IIC_DelayUs(2);
  SDA_L;
  IIC_DelayUs(2);
  SCL_L;
}

/*********************************************************************************************
* 名称:I2C_Stop()
* 功能:I2C停止信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C_Stop(void)
{
  SDA_OUT;
  SCL_L;
  SDA_L;
  IIC_DelayUs(2);
  SCL_H;
  SDA_H;
  IIC_DelayUs(2);
}

/*********************************************************************************************
* 名称:I2C_Ack()
* 功能:I2C应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C_Ack(void)
{
  SCL_L;
  SDA_OUT;
  SDA_L;
  IIC_DelayUs(2);
  SCL_H;
  IIC_DelayUs(2);
  SCL_L;
}

/*********************************************************************************************
* 名称:I2C_NoAck()
* 功能:I2C非应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C_NoAck(void)
{
  SCL_L;
  SDA_OUT;
  SDA_H;
  IIC_DelayUs(2);
  SCL_H;
  IIC_DelayUs(2);
  SCL_L;
}

/*********************************************************************************************
* 名称:I2C_WaitAck()
* 功能:I2C等待应答信号
* 参数:无
* 返回:返回为:=1有ACK,=0无ACK
* 修改:
* 注释:
*********************************************************************************************/
int I2C_WaitAck(void)
{
  uint8_t ucErrTime=0;
  SDA_H;
  IIC_DelayUs(2);
  SDA_IN;
  SCL_H;
  IIC_DelayUs(2);
  while (SDA_R)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      I2C_Stop();
      return 1;
    }
  }
  SCL_L;
  return 0;
}

/*********************************************************************************************
* 名称:I2C_WriteByte()
* 功能:I2C写字节数据
* 参数:char SendByte -- 发送数据
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void I2C_WriteByte(uint8_t SendByte) //数据从高位到低位//
{
  uint8_t i=8;
  SDA_OUT;
  while(i--)
  {
    if(SendByte&0x80) SDA_H;
    else SDA_L;
    SendByte<<=1;
    IIC_DelayUs(1);
    SCL_H;
    IIC_DelayUs(2);
    SCL_L;
    IIC_DelayUs(1);
  }
  SDA_H;
}

/*********************************************************************************************
* 名称:I2C_ReadByte()
* 功能:I2C读字节数据，数据从高位到低位
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
uint8_t I2C_ReadByte(void)
{
  uint8_t i=8;
  uint8_t ReceiveByte=0;
  
  SDA_H;
  SDA_IN;
  while(i--)
  {
    ReceiveByte<<=1;
    SCL_L;
    IIC_DelayUs(2);
    SCL_H;
    IIC_DelayUs(2);
    if(SDA_R)
    {
      ReceiveByte|=0x01;
    }
  }
  SCL_L;
  return ReceiveByte;
}

