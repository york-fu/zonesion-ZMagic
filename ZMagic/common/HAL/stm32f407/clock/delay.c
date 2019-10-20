#include "delay.h"

//不支持contiki
#if SYSTEM_SUPPORT_CONTIKI!=1

static unsigned char  fac_us=0;//us延时倍乘数
static unsigned short fac_ms=0;//ms延时倍乘数
/*********************************************************************************************
* 名称：delay_init
* 功能：初始化延时函数
* 参数：SYSCLK（系统时钟频率，单位MHz）
* 返回：无
* 描述：使用systick定时器作为延时定时器
*       stm32f407systick定时器工作频率为HCLK/8
*       HCKL=SYSCLK
* 修改：
* 注释：
*********************************************************************************************/
void delay_init(unsigned char SYSCLK)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us=SYSCLK/8;
    fac_ms=(unsigned short)fac_us*1000;
}


/*********************************************************************************************
* 名称：delay_us
* 功能：us级延时函数
* 参数：nus，延时时长，单位us
* 返回：无
* 描述：fac_us=SYSCLK/8;，即等于其工作频率，单位MHz,SYSCLK取值168MHz，fac_us=21
*       那么定时器定时计数fac_us次，历时1微秒。nus*fac_us次，则历时nus微秒
*       systick定时器为24位，最大计数次数为0xFFFFFF = MAXus * fac_us，可以得到其最长延时时间MAX = 798915微秒
* 修改：
* 注释：
*********************************************************************************************/
void delay_us(unsigned short nus)
{
    unsigned int temp;
    SysTick->LOAD=nus*fac_us;                         //时间加载
    SysTick->VAL=0x00;                                //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
    do
    {
        temp=SysTick->CTRL;                           //取CTRL(Control and Status Register)值
    }
    while((temp&0x01)&&!(temp&(1<<16)));              //等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;          //关闭计数器
    SysTick->VAL =0X00;                               //清空计数器
}

/*********************************************************************************************
* 名称：delay_xms
* 功能：ms级延时函数
* 参数：nms，延时时长，单位ms
* 返回：无
* 描述：fac_ms = 10*fac_us，单位MHz,SYSCLK取值168MHz，fac_ms=21000
*       那么定时器定时计数fac_ms次，历时1毫秒。nms*fac_ms次，则历时nms毫秒
*       systick定时器为24位，最大计数次数为0xFFFFFF = MAXms * fac_ms，可以得到其最长延时时间MAX = 798毫秒(取整)
* 修改：
* 注释：
*********************************************************************************************/
void delay_xms(unsigned short nms)
{
    unsigned int temp;
    SysTick->LOAD=(unsigned int)nms*fac_ms;           //时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;                               //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
    do
    {
        temp=SysTick->CTRL;                            //取CTRL(Control and Status Register)值
    }
    while((temp&0x01)&&!(temp&(1<<16)));              //等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;          //关闭计数器
    SysTick->VAL =0X00;                               //清空计数器
}

/*********************************************************************************************
* 名称：delay_ms
* 功能：ms级延时函数
* 参数：nms，延时时长，单位ms
* 返回：无
* 描述：delay_xms延时是有时限的，当stm32F407超频到248M的时候，delay_xms只能延时541ms左右
*       delay_ms延时函数将延时时间分割成若干个delay_xms函数可以达到的延时时长，进行延时。延时时间最高65535ms
* 修改：
* 注释：
*********************************************************************************************/
#define XMS_MAX 540    //delay_xms能延时的最长时间（单位ms）
void delay_ms(unsigned short nms)
{
    unsigned char repeat=nms/XMS_MAX;	   //分割次数
    unsigned short remain=nms%XMS_MAX;      //分割剩余
    while(repeat)
    {
        delay_xms(XMS_MAX);
        repeat--;
    }
    if(remain)
        delay_xms(remain);
}

#endif




































