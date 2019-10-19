/*********************************************************************************************
* 文件：fbm320.h
* 作者：zonesion
* 说明：大气压力传感器头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _FBM320_H_
#define _FBM320_H_

#define         FBM320_ADDR     0xD8 
#define         FBM320_ID_ADDR  0x6B
#define         FBM320_ID       0x42
#define         FBM320_CONFIG   0XF4
#define         FBM320_RESET    0XE0
#define         FBM320_DATAM    0xF6 
#define         FBM320_DATAC    0xF7 
#define         FBM320_DATAL    0xF8 
#define         OSR1024         0x34
#define         OSR2048         0x74
#define         OSR4096         0xB4
#define         OSR8192         0xF4
#define         TEMPERATURE     0x2E
#define         RESET_DATA      0xB6


/*********************************************************************************************
* 外部原型函数
*********************************************************************************************/
unsigned char fbm320_read_id(void);
unsigned char fbm320_read_reg(unsigned char reg);
void fbm320_write_reg(unsigned char reg,unsigned char data);
long fbm320_read_data(void);
void Coefficient(void);
void Calculate(long UP, long UT);
unsigned char fbm320_init(void);
int fbm320_data_get(float *temperature,long *pressure);
#endif 