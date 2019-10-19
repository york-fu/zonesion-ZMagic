/*********************************************************************************************
* 文件：iic.c
* 作者：zonesion
* 说明：iic驱动程序
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <ioCC2530.h>
#include <math.h>
#include <stdio.h>

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define    SCL	                P0_7       	                	//IIC时钟引脚定义
#define    SDA	                P0_6       	                	//IIC数据引脚定义

/*********************************************************************************************
* 名称：iic_delay_us()
* 功能：延时函数
* 参数：i -- 延时设置
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void  iic_delay_us(unsigned int i)
{
    while(i--)
    {
        //    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    }
}

/*********************************************************************************************
* 名称：iic_init()
* 功能：I2C初始化函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_init(void)
{
    P0SEL &= ~(1<<6);                                             //设置为普通IO模式
    P0DIR |= (1<<6);                                              //设置为输出模式
    P0SEL &= ~(1<<7);                                             //设置为普通IO模式
    P0DIR |= (1<<7);                                              //设置为输出模式
    SDA = 1;                                                      //拉高数据线
    //  iic_delay_us(2);                                             //延时10us
    SCL = 1;                                                      //拉高时钟线
    //  iic_delay_us(2);                                             //延时10us
}

void SDA_output()
{
    P0DIR |= (1<<6); 
}

void SDA_input()
{
    P0DIR &= ~(1<<6); 
}

/*********************************************************************************************
* 名称：iic_start()
* 功能：I2C起始信号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_start(void)
{
    SDA_output();
    SDA = 1;                                                      //拉高数据线
    SCL = 1;                                                      //拉高时钟线
    iic_delay_us(2);                                              //延时
    SDA = 0;                                                      //产生下降沿
    iic_delay_us(2);                                              //延时
    SCL = 0;                                                      //拉低时钟线
}

/*********************************************************************************************
* 名称：iic_stop()
* 功能：I2C停止信号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_stop(void)
{
    SDA_output();
    SDA =0;                                                       //拉低数据线
    SCL =1;                                                       //拉高时钟线
    iic_delay_us(2);                                              //延时5us
    SDA=1;                                                        //产生上升沿
    iic_delay_us(2);                                              //延时5us
}

/*********************************************************************************************
* 名称：iic_send_ack()
* 功能：I2C发送应答
* 参数：ack -- 应答信号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic_send_ack(int ack)
{
    SDA = ack;                                                    //写应答信号
    SCL = 1;                                                      //拉高时钟线
    //  iic_delay_us(2);                                              //延时
    SCL = 0;                                                      //拉低时钟线
    //  iic_delay_us(2);                                              //延时
}

/*********************************************************************************************
* 名称：iic_recv_ack()
* 功能：I2C接收应答
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int iic_recv_ack(void)
{
    SCL = 1;                                                      //拉高时钟线
    //  iic_delay_us(2);                                              //延时
    CY = SDA;                                                     //读应答信号
    SCL = 0;                                                      //拉低时钟线
    //  iic_delay_us(2);                                              //延时
    return CY;
}

/*********************************************************************************************
* 名称：iic_write_byte()
* 功能：I2C写一个字节数据，返回ACK或者NACK，从高到低，依次发送
* 参数：data -- 要写的数据
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic_write_byte(unsigned char data)
{
    unsigned char i;
    SDA_output();
    SCL = 0;                                                      //拉低时钟线
    iic_delay_us(2);                                              //延时2us
    for(i = 0;i < 8;i++){                                         
        if(data & 0x80){                                            //判断数据最高位是否为1
            SDA = 1;
            //iic_delay_us(2);                                            //延时5us
        }
        else{
            SDA = 0;
            //iic_delay_us(2);                                            //延时5us
        }
        // iic_delay_us(2);                                            //延时5us
        SCL = 1;	                                                //输出SDA稳定后，拉高SCL给出上升沿，从机检测到后进行数据采样
        iic_delay_us(2);                                            //延时5us
        SCL = 0;                                                    //拉低时钟线
        //iic_delay_us(2);                                            //延时5us
        data <<= 1;                                                 //数组左移一位
    } 
    //iic_delay_us(2);                                              //延时2us
    SDA = 1;                                                      //拉高数据线
    //iic_delay_us(2);                                              //延时2us
    SCL = 1;                                                      //拉高时钟线
    SDA_input();
    //iic_delay_us(2);                                              //延时2us，等待从机应答
    if(SDA == 1){			                                //SDA为高，收到NACK
        return 1;	
    }else{ 				                        //SDA为低，收到ACK
        SCL = 0;
        //iic_delay_us(2);
        return 0;
    }
}

/*********************************************************************************************
* 名称：iic_read_byte()
* 功能：I2C写一个字节数据，返回ACK或者NACK，从高到低，依次发送
* 参数：data -- 要写的数据
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic_read_byte(unsigned char ack)
{
    unsigned char i,data = 0;
    SDA_output();
    SCL = 0;
    SDA = 1;			                                //释放总线	
    SDA_input();
    for(i = 0;i < 8;i++){
        SCL = 0;		                                        //给出上升沿
        //    iic_delay_us(2);	                                        //延时等待信号稳定
        SCL = 1;		                                        //给出上升沿
        //    iic_delay_us(2);	                                        //延时等待信号稳定
        data <<= 1;	
        if(SDA == 1){ 		                                //采样获取数据
            data |= 0x01;
        }else{
            data &= 0xfe;
        }
        iic_delay_us(2);
    }   
    SDA_output();
    SDA = ack;	                                                //应答状态
    //  iic_delay_us(2);
    SCL = 1;                         
    //  iic_delay_us(2);          
    SCL = 0;
    //  iic_delay_us(2);
    return data;
}

/*********************************************************************************************
* 名称：delay()
* 功能：延时
* 参数：t -- 设置时间
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void delay(unsigned int t)
{
    unsigned char i;
    while(t--){
        for(i = 0;i < 200;i++);
    }					   
}
