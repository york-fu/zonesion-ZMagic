//#include "mma7660.h"
//#include "hw_types.h"
#include "IIC.h"


//模拟IIC引脚初始化
void I2C_GPIO_Config(void)
{
    PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);      //使能时钟
    PinTypeGPIO(PIN_58,PIN_MODE_0,false);                         //选择引脚为GPIO模式（gpio14）
    GPIODirModeSet(GPIOA0_BASE, G03_UCPINS, GPIO_DIR_MODE_OUT);   //设置GPIO14为输出模式

    PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);      //使能时钟
    PinTypeGPIO(PIN_08,PIN_MODE_0,false);                         //选择引脚为GPIO模式（gpio17）
    GPIODirModeSet(GPIOA2_BASE, G17_UCPINS, GPIO_DIR_MODE_OUT);   //设置GPIO16为输出模式

    PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);      //使能时钟
    PinTypeGPIO(PIN_07,PIN_MODE_0,false);                         //选择引脚为GPIO模式（gpio16）
    GPIODirModeSet(GPIOA2_BASE, G16_UCPINS, GPIO_DIR_MODE_OUT);   //设置GPIO16为输出模式
    GPIOPinWrite(GPIOA2_BASE, G16_UCPINS, 0x0);                  //使能
}




void I2C_Start(void)
{
    SDA_OUT();     //sda线输出
    SDA(H);
    delay_us(1);
    SCL(H);
    delay_us(4);
    SDA(L);
    delay_us(4);
    SCL(L);
    delay_us(4);
}

void RepeatStart()                //without stop signal before
{
    SDA_OUT();
    SDA(H);
    delay_us(1);
    SCL(H);
    delay_us(4);                   //delay about 5us
    SDA(L);
    delay_us(4);
    SCL(L);
    delay_us(4);
}


void I2C_Stop(void)
{
    SDA_OUT();
    SDA(L);
    delay_us(1);
    SCL(H);
    delay_us(4);
    SDA(H);
    delay_us(4);
    SCL(L);
    delay_us(4);
}


unsigned char ChkACK(void)        //返回0表示有ack，1表示无ack
{
    SDA_OUT();
    SDA(H);
    delay_us(4);
    SCL(H);
    delay_us(4);
    SDA_IN();
    if(!SDA_read)               //如果SDA有低电平
    {
        SCL(L);
        delay_us(4);
        return 0;
    }
    else
    {
        SCL(L);
        delay_us(4);
        return 1;
    }
}

void SendAck(void)
{
    SDA_OUT();
    SDA(L);
    delay_us(1);
    SCL(H);
    delay_us(4);
    SCL(L);
    delay_us(4);
}

void ReadSDA(void)
{

}

void SendNoAck(void)
{
    SDA_OUT();
    SCL(L);
    delay_us(4);
    SDA(H);
    delay_us(1);
    SCL(H);
    delay_us(4);
}

void Send8bit(unsigned char AData)//发送八位字节，先发送高位，在传输低位
{
    unsigned char i=8;
    SDA_OUT();
    for(i=8; i>0; i--)               //详细研究
    {
        if(AData&0x80)
            SDA(H);
        else
            SDA(L);
        delay_us(1);
        SCL(H);
        delay_us(4);
        SCL(L);
        AData<<=1;
        delay_us(4);                   //mannual
    }
}

unsigned char Read8bit(unsigned char ack)        //ack=1表示需要向MMA传输停止位，为0则表示不需要向MMA传输停止位
{
    unsigned char temp=0;
    unsigned char  i;
    unsigned char label;
    label=ack;
    SDA_OUT();
    SDA(H);
    delay_us(4);
    for(i=8; i>0; i--)
    {
        //ActiveTime();                 //保险起见，回头可以删掉
        SDA_OUT();
        SCL(H);
        delay_us(4);
        SDA_IN();
        if(SDA_read)
            temp+=0x01;
        else
            temp&=0xfe;
        SCL(L);
        delay_us(4);
        if(i>1)
            temp<<=1;
    }
    if(label)
    {
        SendAck();
    }
    else
    {
        SendNoAck();
    }                               //改成输出模式
    return temp;
}


#define ERR      1
#define SUCCESS  0
unsigned char RegWrite(unsigned char address, unsigned char reg,unsigned char val)
{
    I2C_Start();                        //Send Start
    Send8bit(address);              //Send IIC "Write" Address
    if(ChkACK())
        return ERR;
    Send8bit(reg);                  // Send Register
    if(ChkACK())
        return ERR;
    Send8bit(val);                  // Send Value
    if(ChkACK())
        return ERR;
    I2C_Stop();                         // Send Stop
    return SUCCESS;
}

/***********************************************************
重复启动情况：常用语主设备与从设备在不同模式之间切换的场合
比如前面一次传输结束过后，无需stop信号，而转而进入接收状态
这样子操作的话，不释放总线，一直处于busy状态
**************************************************************/

unsigned char RegRead(unsigned char address, unsigned char reg)
{
    unsigned char  b=0;
    I2C_Start();
    Send8bit(address);
    if(ChkACK())
        return ERR;
    Send8bit(reg);
    if(ChkACK())
        return ERR;
    RepeatStart();                  //从MMA里面读取数据必须采用这种方式
    Send8bit(address+1);            //重新发送设备地址位，并且该读写属性为读取
    if(ChkACK())
        return ERR;
    b=Read8bit(0);                  //不带应答信号
    I2C_Stop();
    return b;
}

/*********************************************************************************************
* 名称：MMA7660_GetResult()
* 功能：获取三轴结果
* 参数：Regs_Addr
* 返回：ret
* 修改：
* 注释：
*********************************************************************************************/
/*uint8 MMA7660_GetResult(uint8 Regs_Addr)
{
  uint8 ret ;
  if(Regs_Addr>MMA7660_ZOUT)
    return 0;
   ret=RegRead(IIC_Read,Regs_Addr);
  while(ret&0x40)
  {
    RegRead(IIC_Read,Regs_Addr);
  }
  return ret;

}*/