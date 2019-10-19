#ifndef __LIS3DH_H_
#define __LIS3DH_H_
#include "stm32f4xx.h"

#define	LIS3DH_ADDR             0x32		                    //三轴的IIC地址
#define	LIS3DH_IDADDR           0x0F		                    //三轴的ID地址
#define	LIS3DH_ID               0x33		                    //三轴的ID
#define LIS3DH_CTRL_REG1        0x20
#define LIS3DH_CTRL_REG2        0x21
#define LIS3DH_CTRL_REG3        0x22
#define LIS3DH_CTRL_REG4        0x23
#define LIS3DH_OUT_X_L		    0x28
#define LIS3DH_OUT_X_H		    0x29
#define LIS3DH_OUT_Y_L		    0x2A
#define LIS3DH_OUT_Y_H		    0x2B
#define LIS3DH_OUT_Z_L		    0x2C
#define LIS3DH_OUT_Z_H		    0x2D

#define N       64                                              //采样个数
#define Fs      10                                              //采样频率
#define F_P     (((float)Fs)/N)

typedef struct{
  unsigned char tumbleEnableStatus : 1;
  unsigned char stepEnableStatus : 1;
  unsigned char SedentaryEnableStatus : 1;
  unsigned char tumbleStatus : 1;
  unsigned char SedentaryStatus : 1;
  unsigned short stepCount;
  unsigned char SedentaryInterval;
  unsigned short SedentaryTime;
}lis3dh;

int lis3dh_init(void);
void get_lis3dhInfo(float *accX, float *accY, float *accZ);
int lis3dh_tumble(float x, float y, float z);
int run_lis3dh_arithmetic(void);
void set_lis3dh_enableStatus(unsigned char cmd);
unsigned char get_lis3dh_enableStatus(void);
int get_lis3dh_tumbleStatus(void);
int get_lis3dh_stepCount(void);
void del_lis3dh_stepCount(void);
void set_lis3dh_SedentaryTime(unsigned short interval, unsigned short time);
int get_lis3dh_SedentaryStatus(void);

#endif    // end __LIS3DH_H_
