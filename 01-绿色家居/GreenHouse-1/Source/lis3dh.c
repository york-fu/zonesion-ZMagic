/*********************************************************************************************
* 文件：lis3dh.c
* 作者：zonesion
* 说明：lis3dh驱动程序
* 修改：Chenkm 2017.01.10 修改代码格式，增加代码注释和文件说明
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <ioCC2530.h>
#include <math.h>
#include <stdio.h>
#include "lis3dh.h"
#include "iic.h"

/*********************************************************************************************
* 名称：lis3dh_init()
* 功能：lis3dh初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char lis3dh_init(void)
{
  iic_init();		                                        //I2C初始化
  delay(600);		                                        //短延时
  if(LIS3DH_ID != lis3dh_read_reg(LIS3DH_IDADDR))               //读取设备ID
    return 1;
  delay(600);                                                   //短延时
  if(lis3dh_write_reg(LIS3DH_CTRL_REG1,0x97))                   //1.25kHz，x,y,z输出使能
    return 1;
  delay(600);                                                   //短延时
  if(lis3dh_write_reg(LIS3DH_CTRL_REG4,0x10))                   //4G量程
    return 1;
  return 0;
}

/*********************************************************************************************
* 名称：lis3dh_read_reg()
* 功能：htu21读取寄存器
* 参数：cmd -- 寄存器地址
* 返回：data 寄存器数据
* 修改：
* 注释：
*********************************************************************************************/
unsigned char lis3dh_read_reg(unsigned char cmd)
{
  unsigned char data = 0; 				        //定义数据
  iic_start();						        //启动总线
  if(iic_write_byte(LIS3DHADDR & 0xfe) == 0){		        //地址设置
    if(iic_write_byte(cmd) == 0){			        //命令输入
     // do{
     //   delay(300);					        //延时
        iic_start();					        //启动总线
     // }
     // while(iic_write_byte(LIS3DHADDR | 0x01) == 1);	        //等待数据传输完成
        iic_write_byte(LIS3DHADDR | 0x01);
      data = iic_read_byte(1);				        //读取数据
      iic_stop();					        //停止总线传输
    }
  }
  return data;						        //返回数据
}

/*********************************************************************************************
* 名称：lis3dh_write_reg()
* 功能：lis3dh写寄存器
* 参数：cmd -- 寄存器地址data 寄存器数据
* 返回：0-寄存器写入成功，1-写失败
* 修改：
* 注释：
*********************************************************************************************/
unsigned char lis3dh_write_reg(unsigned char cmd,unsigned char data)
{
  iic_start();						        //启动总线
  if(iic_write_byte(LIS3DHADDR & 0xfe) == 0){		        //地址设置
    if(iic_write_byte(cmd) == 0){			        //命令输入
      if(iic_write_byte(data) == 0){			        //数据输入
        iic_stop();                                             //停止总线传输
        return 0;                                               //返回结果
      }
    }
  }
  iic_stop();
  return 1;						        //返回结果
}

/*********************************************************************************************
* 名称：lis3dh_read_data()
* 功能：lis3dh读数据
* 参数：accx：x轴加速度 accy：y轴加速度 accz：z轴加速度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void lis3dh_read_data(float *accx,float *accy,float *accz)
{
  char accxl,accxh,accyl,accyh,acczl,acczh;
  accxl = lis3dh_read_reg(LIS3DH_OUT_X_L);
  accxh = lis3dh_read_reg(LIS3DH_OUT_X_H);
  if(accxh & 0x80){
    *accx = (float)(((int)accxh << 4 | (int)accxl >> 4)-4096)/2048*9.8*4;
  }
  else{
    *accx = (float)((int)accxh << 4 | (int)accxl >> 4)/2048*9.8*4;
  }
  
  accyl = lis3dh_read_reg(LIS3DH_OUT_Y_L);
  accyh = lis3dh_read_reg(LIS3DH_OUT_Y_H);
  if(accyh & 0x80){
    *accy = (float)(((int)accyh << 4 | (int)accyl >> 4)-4096)/2048*9.8*4;
  }
  else{
    *accy = (float)((int)accyh << 4 | (int)accyl >> 4)/2048*9.8*4;
  }
   
  acczl = lis3dh_read_reg(LIS3DH_OUT_Z_L);
  acczh = lis3dh_read_reg(LIS3DH_OUT_Z_H);
  if(acczh & 0x80){
    *accz = (float)(((int)acczh << 4 | (int)acczl >> 4)-4096)/2048*9.8*4;
  }
  else{
    *accz = (float)((int)acczh << 4 | (int)acczl >> 4)/2048*9.8*4;
  }
}

