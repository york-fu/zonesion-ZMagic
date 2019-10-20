/*********************************************************************************************
* 文件：fml_key.c
* 作者：fuyou
* 说明：按键的功能模块驱动文件
* 功能：提供按键驱动接口
* 修改：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "fml_key.h"

/*********************************************************************************************
* 名称：key_init
* 功能：按键管脚初始化
* 参数：无
* 返回：无
* 修改：无
*********************************************************************************************/
void key_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                          //定义一个GPIO_InitTypeDef类型的结构体
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //设置引脚模式为输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //设置引脚为上拉模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //设置引脚速率为100MHz
    
    RCC_AHB1PeriphClockCmd( K1_CLK, ENABLE);                    //开启KEY相关的GPIO外设时钟
    GPIO_InitStructure.GPIO_Pin = K1_PIN;                       //选择要控制的GPIO引脚
    GPIO_Init(K1_PORT, &GPIO_InitStructure);                    //初始化GPIO配置
    
    RCC_AHB1PeriphClockCmd( K2_CLK, ENABLE);                    //开启KEY相关的GPIO外设时钟
    GPIO_InitStructure.GPIO_Pin = K2_PIN;                       //选择要控制的GPIO引脚
    GPIO_Init(K2_PORT, &GPIO_InitStructure);                    //初始化GPIO配置
    
    RCC_AHB1PeriphClockCmd( K3_CLK, ENABLE);                    //开启KEY相关的GPIO外设时钟
    GPIO_InitStructure.GPIO_Pin = K3_PIN;                       //选择要控制的GPIO引脚
    GPIO_Init(K3_PORT, &GPIO_InitStructure);                    //初始化GPIO配置
}


/*********************************************************************************************
* 名称：key_get_status
* 功能：按键管脚初始化
* 参数：无
* 返回：key_status
* 修改：
*********************************************************************************************/
uint8_t key_GetStatus(void)
{
    uint8_t key_status = 0;
    if(GPIO_ReadInputDataBit(K1_PORT,K1_PIN) == 0)                //判断PB12引脚电平状态
        key_status |= K1_PRESSED;                                   //低电平key_status bit0位置1
    if(GPIO_ReadInputDataBit(K2_PORT,K2_PIN) == 0)                //判断PB13引脚电平状态
        key_status |= K2_PRESSED;                                   //低电平key_status bit1位置1
    if(GPIO_ReadInputDataBit(K3_PORT,K3_PIN) == 0)                //判断PB14引脚电平状态
        key_status |= K3_PRESSED;                                   //低电平key_status bit2位置1
    return key_status;
}

