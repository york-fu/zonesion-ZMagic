#include "adc.h"
#include "delay.h"

void  AdcInit(void)
{    
  GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                  //模拟输入
  //先初始化ADC1通道0 IO口
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);         //使能GPIOC时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;                    //PC0 通道0
  GPIO_Init(GPIOC, &GPIO_InitStructure);                        //初始化  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
  ADC_TempSensorVrefintCmd(ENABLE);//使能内部温度传感器
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;//两个采样阶段之间的延迟10个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化

  ADC_Cmd(ADC1, ENABLE);//开启AD转换器
}	

unsigned short AdcGet(u8 ch)   
{
  uint16_t i=0;
  
  if (ch == 1) ch = ADC_Channel_10;                           //pc0
  else if (ch == 2) ch = ADC_Channel_11;
  else if (ch == 3) ch = ADC_Channel_12;
  else if (ch == 4) ch = ADC_Channel_13;
  else return 0;
  //设置指定ADC的规则组通道，一个序列，采样时间
  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度	
  ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ))//等待转换结束
  {
    delay_us(1);
    i++;
    if(i>499) return 0;
  }
  
  return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


unsigned short AdcGetX(unsigned char ch,unsigned short num)
{
  unsigned long AdcValue=0;
  
  for(short i=num;i>0;i--)
  {
    AdcValue += AdcGet(ch);
  }
  
  return (u16)(AdcValue/num);
}



double Get_CpuTemprateADC(void)
{
	uint16_t i=0;
  
  //设置指定ADC的规则组通道，一个序列，采样时间
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度		
  ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ))//等待转换结束
  {
    delay_us(1);
    i++;
    if(i>499) return 0;
  }
  
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


//得到温度值
//返回值:温度值(单位:℃.)
double Get_CpuTemprateX(short n)
{
  uint32_t adcx=0;
 	double temperate=0;
  
  for(short i=n;i>0;i--)
  {
    adcx+=Get_CpuTemprateADC();
  }
  adcx=(uint16_t)(adcx/n);
  temperate=(double)adcx*(3.3/4096);		//电压值
  temperate=(temperate-0.76)/0.0025 + 25; //转换为温度值 
  return temperate;
}
