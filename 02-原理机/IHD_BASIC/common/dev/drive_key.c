/*********************************************************************************************
* 文件：key.c
* 作者：chennian 2017.11.02
* 说明：K1 对应引脚 PE12
*       K2 对应管脚 PE13
*       K3 对应引脚 PE14
*       K4 对应管脚 PE15
* 功能：按键驱动代码
* 修改：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "drive_key.h"

/*********************************************************************************************
* 名称：key_init
* 功能：按键管脚初始化
* 参数：无
* 返回：无
* 修改：无
*********************************************************************************************/
void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                          //定义一个GPIO_InitTypeDef类型的结构体
    RCC_AHB1PeriphClockCmd( K1_CLK  | K2_CLK|K3_CLK | K4_CLK, ENABLE);             //开启KEY相关的GPIO外设时钟
    
    GPIO_InitStructure.GPIO_Pin = K1_PIN | K2_PIN |K3_PIN | K4_PIN;                //选择要控制的GPIO引脚
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //设置引脚模式为输出模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //设置引脚为上拉模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //设置引脚速率为2MHz
    
    GPIO_Init(K1_PORT, &GPIO_InitStructure);                      //初始化GPIO配置
    GPIO_Init(K2_PORT, &GPIO_InitStructure);                      //初始化GPIO配置
    GPIO_Init(K3_PORT, &GPIO_InitStructure);                      //初始化GPIO配置
    GPIO_Init(K4_PORT, &GPIO_InitStructure);                      //初始化GPIO配置
}
void KeyExitxConfig(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);//PB12 连接到中断线12
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);//PB13 连接到中断线13
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);//PB14 连接到中断线14
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);//PB15 连接到中断线15
    /* 配置EXTI_Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13|EXTI_Line14 | EXTI_Line15;//LINE/3
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE2/3
    EXTI_Init(&EXTI_InitStructure);//配置
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断1
    //NVIC_Init(&NVIC_InitStructure);//配置
}
/*********************************************************************************************
* 名称：get_key_status
* 功能：按键管脚初始化
* 参数：无
* 返回：key_status
* 修改：
*********************************************************************************************/
char get_key_status(void)
{
    char key_status = 0;
    if(GPIO_ReadInputDataBit(K1_PORT,K1_PIN) == 0)                //判断PB12引脚电平状态
        key_status |= K1_PREESED;                                   //低电平key_status bit0位置1
    if(GPIO_ReadInputDataBit(K2_PORT,K2_PIN) == 0)                //判断PB13引脚电平状态
        key_status |= K2_PREESED;                                   //低电平key_status bit1位置1
    if(GPIO_ReadInputDataBit(K3_PORT,K3_PIN) == 0)                //判断PB14引脚电平状态
        key_status |= K3_PREESED;                                   //低电平key_status bit2位置1
    if(GPIO_ReadInputDataBit(K4_PORT,K4_PIN) == 0)                //判断PB15引脚电平状态
        key_status |= K4_PREESED;                                   //低电平key_status bit3位置1
    return key_status;
}

static void (*call)(int) = NULL;

void KeySetCallback(void (*f)(int k))
{
    call = f;
}
void EXTI15_10_IRQHandler(void)
{
    if (call != NULL) call(1);
    if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
        EXTI_ClearITPendingBit(EXTI_Line12);                        //清除LINE14上的中断标志位
    if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
        EXTI_ClearITPendingBit(EXTI_Line13);                        //清除LINE15上的中断标志位
    if(EXTI_GetITStatus(EXTI_Line14)!=RESET)
        EXTI_ClearITPendingBit(EXTI_Line14);                        //清除LINE14上的中断标志位
    if(EXTI_GetITStatus(EXTI_Line15)!=RESET)
        EXTI_ClearITPendingBit(EXTI_Line15);                        //清除LINE15上的中断标志位
}
