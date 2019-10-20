#ifndef __ADC_H
#define __ADC_H	

void AdcInit(void); 				                        //ADC通道初始化
unsigned short  AdcGet(unsigned char ch); 				//获得某个通道值 
unsigned short AdcGetX(unsigned char ch,unsigned short num);

void Adc3_Init(void); 				                        //ADC通道初始化
unsigned short  Get_Adc3(unsigned char ch); 				//获得某个通道值 


double Get_CpuTemprate(void);
double Get_CpuTemprateX(short n);

#endif 
