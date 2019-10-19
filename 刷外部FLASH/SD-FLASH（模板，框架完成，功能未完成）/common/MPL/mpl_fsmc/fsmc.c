/*********************************************************************************************
* 文件：fsmc.c
* 作者：
* 说明：
* 修改：
* 注释：
*
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "fsmc.h"
/* zonesion zxbeepluse 接法*/                                          
/*  fsmc io
PD12 -- RS // lcd  A17                                              
PD7 -- CS // lcd  当前电路PD13无片选功能，需要换到PD7脚 NE1功能    
---------------------
PD4 --- NOE
PD5 --- NWE
PD14 -- D0
PD15 -- D1
PD0 --- D2
PD1 --- D3

PE7 --- D4
PE8     D5
.      .
.      .
PE15 -- D12
PD8 --- D13
PD9 --- D14
PD10 -- D15
*/

/*********************************************************************************************
* 名称:fsmc_init()
* 功能:fsmc初始化
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void fsmc_init(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
  FSMC_NORSRAMTimingInitTypeDef  writeTiming;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //输出速度
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //无上下拉
  
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);                      //使能FSMC时钟
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIO时钟         
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | 
                                GPIO_Pin_7 |
                                GPIO_Pin_4 |
                                GPIO_Pin_5 ;//选中相应的引脚
  GPIO_Init(GPIOD, &GPIO_InitStructure);//按上述参数初始化
        
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | 
                                GPIO_Pin_15 |
                                GPIO_Pin_0 |
                                GPIO_Pin_1 |
                                GPIO_Pin_8 |
                                GPIO_Pin_9 |
                                GPIO_Pin_10;//选中相应的引脚
  GPIO_Init(GPIOD, &GPIO_InitStructure);//按上述参数初始化
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIO时钟   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
    GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15;//选中相应的引脚
  GPIO_Init(GPIOE, &GPIO_InitStructure);                        //按上述参数初始化(PE)
  //复用配置，将下列引脚复用为FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);
  //读配置
  readWriteTiming.FSMC_AddressSetupTime = 2;                  //地址建立时间为2个HCLK 1/168M=6ns*2=12ns	
  readWriteTiming.FSMC_AddressHoldTime = 0;                     //地址保持时间 模式A未用到	
  readWriteTiming.FSMC_DataSetupTime = 10;                       //数据保持时间为10个HCLK =6*10=60ns
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	        //模式A
  //写配置
  writeTiming.FSMC_AddressSetupTime = 2;                        //地址建立时间为2个HCLK =12ns 
  writeTiming.FSMC_AddressHoldTime = 0;	                        //地址保持时间
  writeTiming.FSMC_DataSetupTime = 6;                           //数据保持时间为6ns*6个HCLK=36ns
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	        //模式A
  //配置FSMC
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;    //使用NE1
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器宽度16bit   
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; //读写使用不同时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序
  
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                 //初始化fsmc配置
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);                 // 使能bank1,sram1
}
