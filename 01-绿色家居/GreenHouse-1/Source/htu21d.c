/*********************************************************************************************
* 文件：htu21d.c
* 作者：zonesion
* 说明：htu21d驱动程序
* 修改：Chenkm 2017.01.10 修改代码格式，增加代码注释和文件说明
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <ioCC2530.h>
#include <math.h>
#include <stdio.h>
#include "htu21d.h"
#include "iic.h"

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
unsigned char flag;

/*********************************************************************************************
* 名称：htu21d_init()
* 功能：htu21d初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void htu21d_init(void)
{
  iic_init();		                                            //I2C初始化
  iic_start();		                                            //启动I2C
  iic_write_byte(HTU21DADDR&0xfe);		                        //写HTU21D的I2C地址
  iic_write_byte(0xfe);
  iic_stop();		                                            //停止I2C
  //delay(600);		                                            //短延时
}

/*********************************************************************************************
* 名称：htu21d_read_reg()
* 功能：htu21读取寄存器
* 参数：cmd -- 寄存器地址
* 返回：data 寄存器数据
* 修改：
* 注释：
*********************************************************************************************/
unsigned char htu21d_read_reg(unsigned char cmd)
{
  unsigned char data = 0; 
  iic_start();                                                  //I2C开始
  if(iic_write_byte(HTU21DADDR & 0xfe) == 0){                   //写HTU21D的I2C地址
    if(iic_write_byte(cmd) == 0){                               //写寄存器地址
      do{
        delay(30);                                              //延时30ms
        iic_start();	                                        //开启IIC通信
      }
      while(iic_write_byte(HTU21DADDR | 0x01) == 1);            //发送读信号
      data = iic_read_byte(0);                                  //读取一个字节数据
      iic_stop();                                               //I2C停止
    }
  }
  return data;
}

/*********************************************************************************************
* 名称：htu21d_get_data()
* 功能：htu21d测量温湿度
* 参数：order -- 指令
* 返回：temperature -- 温度值  humidity -- 湿度值
* 修改：
* 注释：
*********************************************************************************************/
int htu21d_get_data(unsigned char order)
{
  float temp = 0,TH = 0;
  unsigned char MSB,LSB;
  unsigned int humidity,temperature; 
  iic_start();                                                  //I2C开始
  if(iic_write_byte(HTU21DADDR & 0xfe) == 0){                   //写HTU21D的I2C地址
    if(iic_write_byte(order) == 0){                             //写寄存器地址
      do{
        delay(30);
        iic_start();	
      }
      while(iic_write_byte(HTU21DADDR | 0x01) == 1);            //发送读信号
      MSB = iic_read_byte(0);                                   //读取数据高八位
      delay(30);                                                //延时
      LSB = iic_read_byte(0);                                   //读取数据低八位
      iic_read_byte(1);
      iic_stop();                                               //I2C停止
      LSB &= 0xfc;                                              //取出数据有效位
      temp = MSB*256+LSB;                                       //数据合并
      if (order == 0xf3){                                       //触发开启温度检测
        TH=(175.72)*temp/65536-46.85;                           //温度:T= -46.85 + 175.72 * ST/2^16
        temperature =(unsigned int)(fabs(TH)*100);
        if(TH >= 0)
          flag = 0;
        else
          flag = 1;
        return temperature;	
      }else{
        TH = (temp*125)/65536-6;
        humidity = (unsigned int)(fabs(TH)*100);                //湿度: RH%= -6 + 125 * SRH/2^16
        return humidity;
      }
    }
  }
  iic_stop(); 
  return 0;
}