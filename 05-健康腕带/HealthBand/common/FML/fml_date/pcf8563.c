#include <stdio.h>
#include "pcf8563.h"
#include "soft_iic.h"

/*********************************************************************************************
* 名称:pcf8563_readReg()
* 功能:读取PCF8563寄存器
* 参数:regAddr-寄存器地址
* 返回:data-读取数据
* 修改:
* 注释:
*********************************************************************************************/
int pcf8563_readReg(unsigned char regAddr)
{
  unsigned char data;
  I2C_Start();
  I2C_WriteByte(PCF8563_ADDR & 0xFE);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(regAddr);
  if(I2C_WaitAck()) return -1;
  I2C_Start();
  I2C_WriteByte(PCF8563_ADDR | 0x01);
  if(I2C_WaitAck()) return -1;
  data = I2C_ReadByte();
  if(I2C_WaitAck()) return -1;
  I2C_Stop();
  return data;
}

/*********************************************************************************************
* 名称:pcf8563_writeReg()
* 功能:写入PCF8563寄存器
* 参数:regAddr-寄存器地址 data-写入的数据
* 返回: 0/-1 成功/失败
* 修改:
* 注释:
*********************************************************************************************/
int pcf8563_writeReg(unsigned char regAddr, unsigned char data)
{
  I2C_Start();
  I2C_WriteByte(0XA2 & 0xFE);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(regAddr);
  if(I2C_WaitAck()) return -1;
  I2C_WriteByte(data);
  if(I2C_WaitAck()) return -1;
  I2C_Stop();
  return 0;
}

/*********************************************************************************************
* 名称:pcf8563_readRegs()
* 功能:读取PCF8563寄存器多个数据
* 参数:regAddr-寄存器地址 readLen-读取长度 buf-数据
* 返回:buf-数据
* 修改:
* 注释:
*********************************************************************************************/
unsigned char *pcf8563_readRegs(unsigned char regAddr ,unsigned char readLen, unsigned char *buf)
{
  *buf = NULL;
  I2C_Start();                
  I2C_WriteByte(PCF8563_ADDR & 0xFE);       
  if (I2C_WaitAck()) return NULL;
  I2C_WriteByte(regAddr);        
  if ( I2C_WaitAck()) return NULL;
  I2C_Start();                
  I2C_WriteByte(PCF8563_ADDR | 0x01); 
  if (I2C_WaitAck()) return NULL;
  for(unsigned char i=0; i<readLen; i++)
  {
    buf[i] = I2C_ReadByte();
    if(i < readLen-1)             
      I2C_Ack();
  }
  I2C_NoAck();          
  I2C_Stop();
  return buf;
}

/*********************************************************************************************
* 名称:pcf8563_init()
* 功能:PCF8563初始化
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void pcf8563_init(void)
{
  I2C_GPIOInit();
  pcf8563_writeReg(0x00, 0x00);                                 // 0x00,0x01为控制/状态寄存器地址
  pcf8563_writeReg(0x01, 0x12);                                 // 脉冲,报警中断有效
  pcf8563_writeReg(0x09, 0x80);                                 // 关闭分钟报警  
  pcf8563_writeReg(0x0A, 0x80);                                 // 关闭小时报警
  pcf8563_writeReg(0x0B, 0x80);                                 // 关闭日报警
  pcf8563_writeReg(0x0C, 0x80);                                 // 关闭星期报警
  pcf8563_writeReg(0x0D, 0x00);                                 // 关闭CLKOUT输出
  pcf8563_writeReg(0x0E, 0x03);                                 // 关闭定时器
}

/*********************************************************************************************
* 名称:get_pcf8563Time()
* 功能:读取PCF8563当前时间
* 参数:无
* 返回:date-读取到的时间,已转换为实际值
* 修改:
* 注释:
*********************************************************************************************/
unsigned char get_pcf8563Time(unsigned char* date)
{
  unsigned char buf[8] = {0};
  if(pcf8563_readRegs(0x02, 7, buf))
  {
    date[0] = ((buf[0] >> 4)&0x07)*10 + (buf[0]&0x0F);       // 秒
    date[1] = ((buf[1] >> 4)&0x07)*10 + (buf[1]&0x0F);       // 分
    date[2] = ((buf[2] >> 4)&0x03)*10 + (buf[2]&0x0F);       // 时
    date[3] = ((buf[3] >> 4)&0x03)*10 + (buf[3]&0x0F);       // 日
    date[4] = buf[4] & 0x07;                                 // 周,0-6,0-星期日
    date[5] = ((buf[5]>>4)&0x01)*10 + (buf[5]&0x0F);         // 月
    date[6] = ((buf[6]>>4)&0x0F)*10 + (buf[6]&0x0F);         // 年,数值加2000
    return 0;
  }
  return 1;
}

/*********************************************************************************************
* 名称:set_pcf8563Time()
* 功能:设置PCF8563时间
* 参数:buf-需要修改的参数
* 返回:data-0/-1 成功/失败
* 修改:
* 注释:参数为BCD码
*       例如：timeBuf[7] = {0x55, 0x56, 0x20, 0x07, 0x04, 0x03, 0x19} // 秒 分 时 日 周 月 年
*********************************************************************************************/
int set_pcf8563Time(unsigned char *buf)
{
  unsigned char i=0;
  for(i=0; i<7; i++)
  {
    buf[i]+=(buf[i]/10)*6;
  }
  for(i=0; i<7; i++)
  {
    int status = pcf8563_writeReg(0x02 + i, buf[i]);
    if(status < 0)
      return -1;
  }
  return 0;
}











