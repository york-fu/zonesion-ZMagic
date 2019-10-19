/*********************************************************************************************
* 文件：lis3dh.h
* 作者：zonesion
* 说明：lis3dh头文件
* 修改：Chenkm 2017.01.10 修改代码格式，增加代码注释和文件说明
* 注释：
*********************************************************************************************/
#ifndef __LIS3DH_H__
#define __LIS3DH_H__

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define	LIS3DHADDR              0x32		                    //三轴的IIC地址
#define	LIS3DH_IDADDR           0x0F		                    //三轴的ID地址
#define	LIS3DH_ID               0x33		                    //三轴的ID
#define LIS3DH_CTRL_REG1        0x20
#define LIS3DH_CTRL_REG2        0x21
#define LIS3DH_CTRL_REG3        0x22
#define LIS3DH_CTRL_REG4        0x23
#define LIS3DH_OUT_X_L		0x28
#define LIS3DH_OUT_X_H		0x29
#define LIS3DH_OUT_Y_L		0x2A
#define LIS3DH_OUT_Y_H		0x2B
#define LIS3DH_OUT_Z_L		0x2C
#define LIS3DH_OUT_Z_H		0x2D
/*********************************************************************************************
* 外部原型函数
*********************************************************************************************/
unsigned char lis3dh_init(void);
unsigned char lis3dh_read_reg(unsigned char cmd);
unsigned char lis3dh_write_reg(unsigned char cmd,unsigned char data);
void lis3dh_read_data(float *accx,float *accy,float *accz);
#endif //__LIS3DH_H__