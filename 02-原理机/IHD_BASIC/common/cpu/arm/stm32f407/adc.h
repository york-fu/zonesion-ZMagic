#ifndef __ADC_H
#define __ADC_H	

void Bsp_ADCInit(void);                      //ADC通道初始化
unsigned short  Get_Adc(unsigned char ch); 				//获得某个通道值 

void Adc3_Init(void); 				                        //ADC通道初始化
unsigned short  Get_Adc3(unsigned char ch); 				//获得某个通道值 
#endif 
