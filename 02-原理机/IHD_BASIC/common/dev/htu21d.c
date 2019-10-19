/*********************************************************************************************
* 文件: htu21d.c
* 作者：zonesion 2016.12.22
* 说明：板载温湿度相关函数
* 修改：
* 注释：
*********************************************************************************************/
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "htu21d.h"

//条件编译 1:使用软件模拟I2C
#define I2C_GPIO  GPIOA
#define PIN_SCL   GPIO_Pin_1
#define PIN_SDA   GPIO_Pin_0

#define SDA_IN     do{I2C_GPIO->MODER &= ~(3<<(0*2)); I2C_GPIO->MODER |= (0<<(0*2));}while(0)
#define SDA_OUT    do{I2C_GPIO->MODER &= ~(3<<(0*2)); I2C_GPIO->MODER |= (1<<(0*2));}while(0)

#define   SCL_L         (I2C_GPIO->BSRRH=PIN_SCL)
#define   SCL_H         (I2C_GPIO->BSRRL=PIN_SCL)
#define   SDA_L         (I2C_GPIO->BSRRH=PIN_SDA)
#define   SDA_H         (I2C_GPIO->BSRRL=PIN_SDA)

#define   SDA_R         (I2C_GPIO->IDR&PIN_SDA)


/*********************************************************************************************
* 名称:HTU21DGPIOInit()
* 功能:初始化HTU21D
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void HTU21DGPIOInit(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SDA_H;
    SCL_H;
}
/*********************************************************************************************
* 名称:I2C_Start()
* 功能:I2C初始信号
* 参数:无
* 返回:0
* 修改:
* 注释:
*********************************************************************************************/
static int I2C_Start(void)
{
    SDA_OUT;
    SDA_H;
    SCL_H;
    delay_us(2);
    SDA_L;
    delay_us(2);
    SCL_L;
    return 0;
}

/*********************************************************************************************
* 名称:I2C_Stop()
* 功能:I2C停止信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
static void I2C_Stop(void)
{
    SDA_OUT;
    SCL_L;
    SDA_L;
    delay_us(2);
    SCL_H;
    SDA_H;
    //delay_us(2);
}

/*********************************************************************************************
* 名称:I2C_Ack()
* 功能:I2C应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
static void I2C_Ack(void)
{
    SCL_L;
    SDA_OUT;
    SDA_L;
    delay_us(2);
    SCL_H;
    delay_us(2);
    SCL_L;
}

/*********************************************************************************************
* 名称:I2C_NoAck()
* 功能:I2C非应答信号
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
static void I2C_NoAck(void)
{
    SCL_L;
    SDA_OUT;
    SDA_H;
    delay_us(2);
    SCL_H;
    delay_us(2);
    SCL_L;
}

/*********************************************************************************************
* 名称:I2C_WaitAck()
* 功能:I2C等待应答信号
* 参数:无
* 返回:返回为:=1有ACK,=0无ACK
* 修改:
* 注释:
*********************************************************************************************/
static int I2C_WaitAck(void)
{
    //int r = 0;
    // int t = 0;
    SDA_IN;
    SDA_H;
    delay_us(2);
    SCL_H;
    delay_us(2);
    if (SDA_R)
    {
        I2C_Stop();
        return 1;
    }
    SCL_L;
    return 0;
}

/*********************************************************************************************
* 名称:I2C_SendByte()
* 功能:I2C发送字节数据
* 参数:char SendByte -- 发送数据
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
static void I2C_SendByte(char SendByte) //数据从高位到低位//
{
    u8 i=8;
    SDA_OUT;
    while(i--)
    {

        if(SendByte&0x80) SDA_H;
        else SDA_L;
        SendByte<<=1;
        delay_us(1);
        SCL_H;
        delay_us(2);
        SCL_L;
        delay_us(1);
    }
}

/*********************************************************************************************
* 名称:I2C_ReceiveByte()
* 功能:I2C接收字节数据，数据从高位到低位
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
static int I2C_ReceiveByte(void)
{
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_IN;
    SDA_H;
    while(i--)
    {
        ReceiveByte<<=1;
        SCL_L;
        delay_us(2);
        SCL_H;
        delay_us(2);
        if(SDA_R)
        {
            ReceiveByte|=0x01;
        }
        //SCL_L;
        //delay_us(1);
    }
    SCL_L;
    return (ReceiveByte&0xff);
}

/*********************************************************************************************
* 名称:bh1750_i2c_write()
* 功能:I2C写
* 参数:char addr -- 地址, char *buf -- 发送数据, int len -- 发送数据长度
* 返回:0/-1
* 修改:
* 注释:
*********************************************************************************************/

static int i2c_write(char addr, char *buf, int len)
{
    if (I2C_Start() < 0)
    {
        I2C_Stop();
        return -1;
    }
    I2C_SendByte(addr<<1);
    if (I2C_WaitAck())
    {
        I2C_Stop();
        return -1;
    }
    for (int i=0; i<len; i++)
    {
        I2C_SendByte(buf[i]);
        if (I2C_WaitAck())
        {
            I2C_Stop();
            return -1;
        }
    }
    I2C_Stop();
    return 0;
}

/*********************************************************************************************
* 名称:i2c_read()
* 功能:I2C读
* 参数:char addr -- 地址, char *buf -- 发送数据, int len -- 发送数据长度
* 返回:数据长度/-1
* 修改:
* 注释:
*********************************************************************************************/
static int i2c_read(char addr, char *buf, int len)
{
    int i;

    if (I2C_Start() < 0)
    {
        I2C_Stop();
        return -1;
    }
    I2C_SendByte((addr<<1)|1);
    if (I2C_WaitAck())
    {
        I2C_Stop();
        return -1;
    }

    for (i=0; i<len-1; i++)
    {
        buf[i] = I2C_ReceiveByte();
        I2C_Ack();
    }
    buf[i] = I2C_ReceiveByte();
    I2C_NoAck();
    I2C_Stop();
    return len;
}
#define HTU21D_ADDR 0x40

/*********************************************************************************************
* 名称:htu21d_reset()
* 功能:htu21d重启
* 参数:无
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void htu21d_reset(void)
{
    char cmd = 0xfe;
    i2c_write(HTU21D_ADDR, &cmd, 1); 								//reset
}
/*********************************************************************************************
* 名称:htu21d_mesure_t()
* 功能:发送读取温度指令
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void htu21d_mesure_t(void)
{
    char cmd = 0xf3;
    i2c_write(HTU21D_ADDR, &cmd, 1);
}

/*********************************************************************************************
* 名称:htu21d_mesure_h()
* 功能:发送读取湿度命令
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void htu21d_mesure_h(void)
{
    char cmd = 0xf5;
    i2c_write(HTU21D_ADDR, &cmd, 1);
}

/*********************************************************************************************
* 名称:htu21d_init()
* 功能:htu21d初始化
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void htu21d_init(void)
{
    char cmd = 0xfe;
    HTU21DGPIOInit();
    i2c_write(HTU21D_ADDR, &cmd, 1); 								//reset
    delay_ms(10);
}

/*********************************************************************************************
* 名称:htu21d_t()
* 功能:读取温度
* 参数:无
* 返回:-1/float t -- 处理后的温度值
* 修改:
* 注释:
*********************************************************************************************/
float htu21d_t(void)
{
    char cmd = 0xf3;
    char dat[4];
    i2c_write(HTU21D_ADDR, &cmd, 1);
    delay_ms(50);
    if (i2c_read(HTU21D_ADDR, dat, 2) == 2)
    {
        if ((dat[1]&0x02) == 0)
        {
            float t = -46.85f + 175.72f * ((dat[0]<<8 | dat[1])&0xfffc) / (1<<16);
            return t;
        }
    }
    return -1;
}

/*********************************************************************************************
* 名称:htu21d_h()
* 功能:读取湿度
* 参数:无
* 返回:-1/float h -- 处理后的湿度值
* 修改:
* 注释:
*********************************************************************************************/
float htu21d_h(void)
{
    char cmd = 0xf5;
    char dat[4];

    i2c_write(HTU21D_ADDR, &cmd, 1);
    delay_ms(50);
    if (i2c_read(HTU21D_ADDR, dat, 2) == 2)
    {
        if ((dat[1]&0x02) == 0x02)
        {
            float h = -6 + 125 * ((dat[0]<<8 | dat[1])&0xfffc) / (1<<16);
            return h;
        }
    }
    return -1;
}


/*********************************************************************************************
htu21d温度值和湿度值
*********************************************************************************************/
float htu21dValue_t = 0;
float htu21dValue_h = 0;

/*********************************************************************************************
* 名称: Htu21dTemperature_Get
* 功能: 获取温度
* 参数: 无
* 返回: 温度值
* 修改:
* 注释:
*********************************************************************************************/
float Htu21dTemperature_Get()
{
    return htu21dValue_t;
}

/*********************************************************************************************
* 名称: Htu21dHumidity_Get
* 功能: 获取湿度
* 参数: 无
* 返回: 湿度值
* 修改:
* 注释:
*********************************************************************************************/
float Htu21dHumidity_Get()
{
    return htu21dValue_h;
}

//申明进程
PROCESS(htu21d_update,"htu21d_update");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程
* 参数:
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(htu21d_update, ev, data)
{
    static struct etimer htu21d_time;
    static char htu21d_command = 0;
    static char htu21d_buf[4]= {0};

    PROCESS_BEGIN();

    htu21d_init();

    while(1)
    {
        /*更新温度值*/
        for(unsigned char i = 0; i<4; i++)
        {
            htu21d_buf[i] = 0;                                  //清htu21d_buf
        }
        htu21d_command = 0xf3;
        i2c_write(HTU21D_ADDR, &htu21d_command, 1);
        etimer_set(&htu21d_time,50);                            //设置etimer定时器
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        if (i2c_read(HTU21D_ADDR, htu21d_buf, 2) == 2)
        {
            if ((htu21d_buf[1]&0x02) == 0)
            {
                htu21dValue_t = -46.85f + 175.72f * ((htu21d_buf[0]<<8 | htu21d_buf[1])&0xfffc) / (1<<16);
            }
        }

        /*更新湿度值*/
        for(unsigned char i = 0; i<4; i++)
        {
            htu21d_buf[i] = 0;                                  //清htu21d_buf
        }
        htu21d_command = 0xf5;
        i2c_write(HTU21D_ADDR, &htu21d_command, 1);
        etimer_set(&htu21d_time,50);                            //设置etimer定时器
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        if (i2c_read(HTU21D_ADDR, htu21d_buf, 2) == 2)
        {
            if ((htu21d_buf[1]&0x02) == 0x02)
            {
                htu21dValue_h = -6 + 125 * ((htu21d_buf[0]<<8 | htu21d_buf[1])&0xfffc) / (1<<16);
            }
        }

        etimer_set(&htu21d_time,200);                           //设置etimer定时器
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    }

    PROCESS_END();
}