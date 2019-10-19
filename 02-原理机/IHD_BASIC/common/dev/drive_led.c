#include "drive_led.h"

void led_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;                        //定义一个GPIO_InitTypeDef类型的结构体


    RCC_AHB1PeriphClockCmd(LED1_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LED1_PIN;                     //选择要控制的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;               //输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;            //无上下拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;           //设置引脚速率为2MHz
    GPIO_Init(LED1_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    RCC_AHB1PeriphClockCmd(LED2_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LED2_PIN;                     //选择要控制的GPIO引脚
    GPIO_Init(LED2_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    RCC_AHB1PeriphClockCmd(LED3_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LED3_PIN;                     //选择要控制的GPIO引脚
    GPIO_Init(LED3_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    RCC_AHB1PeriphClockCmd(LED4_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LED4_PIN;                     //选择要控制的GPIO引脚
    GPIO_Init(LED4_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    RCC_AHB1PeriphClockCmd(LEDR_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LEDR_PIN;                     //选择要控制的GPIO引脚
    GPIO_Init(LEDR_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    RCC_AHB1PeriphClockCmd(LEDG_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LEDG_PIN;                     //选择要控制的GPIO引脚
    GPIO_Init(LEDG_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    RCC_AHB1PeriphClockCmd(LEDB_RCC, ENABLE);                   //使能时钟
    GPIO_InitStructure.GPIO_Pin = LEDB_PIN;                     //选择要控制的GPIO引脚
    GPIO_Init(LEDB_GPIO, &GPIO_InitStructure);                  //初始化GPIO配置

    turn_off(0xFF);
}

/*********************************************************************************************
* 名称：Turn_off
* 功能：置引脚高电平，关闭LED
* 参数：led
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_off(unsigned char led)
{
    if(led & LED1_NUM)                                          //判断LED选择
        GPIO_SetBits(LED1_GPIO, LED1_PIN);                      //PE0置引脚高电平，关闭D1
    if(led & LED2_NUM)
        GPIO_SetBits(LED2_GPIO, LED2_PIN);                      //PE1置引脚高电平，关闭D2
    if(led & LED3_NUM)
        GPIO_SetBits(LED3_GPIO, LED3_PIN);                      //PE2置引脚高电平，关闭D3
    if(led & LED4_NUM)
        GPIO_SetBits(LED4_GPIO, LED4_PIN);                      //PE3置引脚高电平，关闭D4
    if(led & LEDR_NUM)
        GPIO_SetBits(LEDR_GPIO, LEDR_PIN);                      //PB0置引脚高电平，关闭RGB灯的红灯
    if(led & LEDG_NUM)
        GPIO_SetBits(LEDG_GPIO, LEDG_PIN);                      //PB1置引脚高电平，关闭RGB灯的绿灯
    if(led & LEDB_NUM)
        GPIO_SetBits(LEDB_GPIO, LEDB_PIN);                      //PB2置引脚高电平，关闭RGB灯的蓝灯
}

/*********************************************************************************************
* 名称：turn_on
* 功能：置引脚低电平，打开LED
* 参数：led
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_on(unsigned char led)
{
    if(led & LED1_NUM)                                                  //判断LED选择
        GPIO_ResetBits(LED1_GPIO, LED1_PIN);                          //PE0置引脚低电平，打开D1
    if(led & LED2_NUM)
        GPIO_ResetBits(LED2_GPIO, LED2_PIN);                          //PE1置引脚低电平，打开D2
    if(led & LED3_NUM)
        GPIO_ResetBits(LED3_GPIO, LED3_PIN);                          //PE2置引脚低电平，打开D3
    if(led & LED4_NUM)
        GPIO_ResetBits(LED4_GPIO, LED4_PIN);                          //PE3置引脚低电平，打开D4
    if(led & LEDR_NUM)
        GPIO_ResetBits(LEDR_GPIO, LEDR_PIN);                          //PB0置引脚低电平，打开RGB灯的红灯
    if(led & LEDG_NUM)
        GPIO_ResetBits(LEDG_GPIO, LEDG_PIN);                          //PB1置引脚低电平，打开RGB灯的绿灯
    if(led & LEDB_NUM)
        GPIO_ResetBits(LEDB_GPIO, LEDB_PIN);                          //PB2置引脚低电平，打开RGB灯的蓝灯
}
/*********************************************************************************************
* 名称：get_led_status
* 功能：获取LED状态
* 参数：
* 返回：led_status--bit0-bit3分别表示4路LED灯的状态，bit4-bit6分别表示RGB灯的状态
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_led_status(void)
{
    unsigned char led_status = 0;
    if(GPIO_ReadOutputDataBit(LED1_GPIO, LED1_PIN))                 //判断PE0引脚电平
        led_status |= LED1_NUM;                                           //高电平将led_status bit0置1
    else
        led_status &= ~LED1_NUM;                                          //低电平将led_status bit0置0

    if(GPIO_ReadOutputDataBit(LED2_GPIO, LED2_PIN))                 //判断PE1引脚电平
        led_status |= LED2_NUM;                                           //高电平将led_status bit1置1
    else
        led_status &= ~LED2_NUM;                                          //低电平将led_status bit1置0

    if(GPIO_ReadOutputDataBit(LED3_GPIO, LED3_PIN))                 //判断PE2引脚电平
        led_status |= LED3_NUM;                                           //高电平将led_status bit2置1
    else
        led_status &= ~LED3_NUM;                                          //低电平将led_status bit2置0

    if(GPIO_ReadOutputDataBit(LED4_GPIO, LED4_PIN))                 //判断PE3引脚电平
        led_status |= LED4_NUM;                                           //高电平将led_status bit3置1
    else
        led_status &= ~LED4_NUM;                                          //低电平将led_status bit3置0

    if(GPIO_ReadOutputDataBit(LEDR_GPIO, LEDR_PIN))                 //判断PB0引脚电平
        led_status |= LEDR_NUM;                                         //高电平将led_status bit4置1
    else
        led_status &= ~LEDR_NUM;                                        //低电平将led_status bit4置0

    if(GPIO_ReadOutputDataBit(LEDG_GPIO, LEDG_PIN))                 //判断PB1引脚电平
        led_status |= LEDG_NUM;                                         //高电平将led_status bit5置1
    else
        led_status &= ~LEDG_NUM;                                        //低电平将led_status bit5置0

    if(GPIO_ReadOutputDataBit(LEDB_GPIO, LEDB_PIN))                 //判断PB2引脚电平
        led_status |= LEDB_NUM;                                         //高电平将led_status bit6置1
    else
        led_status &= ~LEDB_NUM;                                        //低电平将led_status bit6置0
    return led_status;                                            //返回led_status
}


