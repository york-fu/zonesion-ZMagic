/*********************************************************************************************
* 文件: ili93xx.c
* 作者：zonesion 2016.12.22
* 说明：LCD显示相关函数
* 修改：
* 注释：
*********************************************************************************************/
#include "ili93xx.h"
#include "font.h"
#include "gbk_st16.h"


LCDdev_t lcddev;


/*********************************************************************************************
* 名称：LCD_WR_REG
* 功能：写寄存器函数
* 参数：regval:寄存器值
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_WR_REG(vu16 regval)
{
    regval=regval;		                                //使用-O2优化的时候,必须插入的延时
    ILI93xx_REG=regval;                                           //写入要写的寄存器序号
}

/*********************************************************************************************
* 名称：LCD_WR_DATA
* 功能：写LCD数据
* 参数：data:要写入的值
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_WR_DATA(vu16 data)
{
    data=data;			                                //使用-O2优化的时候,必须插入的延时
    ILI93xx_DAT=data;
}

/*********************************************************************************************
* 名称：LCD_RD_DATA
* 功能：读LCD数据
* 参数：无
* 返回：读到的值
* 修改：
* 注释：
*********************************************************************************************/
u16 LCD_RD_DATA(void)
{
    vu16 ram;			                                //防止被优化
    ram=ILI93xx_DAT;
    return ram;
}

/*********************************************************************************************
* 名称：ILI93xx_WriteReg
* 功能：向LCD指定寄存器写入数据
* 参数：r:寄存器地址; d:要写入的值
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void ILI93xx_WriteReg(uint16_t r, uint16_t d)
{
    ILI93xx_REG = r;
    ILI93xx_DAT = d;
}

/*********************************************************************************************
* 名称：ILI93xx_ReadReg
* 功能：读取寄存器的值
* 参数：r:寄存器地址
* 返回：读到的值
* 修改：
* 注释：
*********************************************************************************************/
uint16_t ILI93xx_ReadReg(uint16_t r)
{
    uint16_t v;
    ILI93xx_REG = r;
    v = ILI93xx_DAT;
    return v;
}


/*********************************************************************************************
* 名称：ILI9325_ScanDir
* 功能：设置ILI9325的GRAM自增方向
* 参数：st：1，开启背光；0，关闭背光
* 返回：无
* 修改：
* 注释：
    //DRIVE TABLE(寄存器 03H)
    //BIT3=AM BIT4:5=ID0:1
    //AM      ID0     ID1               FUNCATION
    // 0      0       0	                R->L    D->U
    // 1      0       0	                D->U	R->L
    // 0      1       0	                L->R    D->U
    // 1      1       0                 D->U	L->R
    // 0      0       1	                R->L    U->D
    // 1      0       1                 U->D	R->L
    // 0      1       1                 L->R    U->D 正常就用这个.
    // 1      1       1	                U->D	L->R
*********************************************************************************************/
void ILI9325_ScanDir(uint8_t dir)
{
    uint16_t regVal = 0;
    //l-r/u-d
    regVal = ILI93xx_ReadReg(0x0003);
    regVal &= ~(7<<3);
    regVal |= (dir<<3);
    ILI93xx_WriteReg(0x0003,regVal);
}


/*********************************************************************************************
* 名称：BLOnOff
* 功能：开启或关闭LCD背光
* 参数：st：1，开启背光；0，关闭背光
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void BLOnOff(int st)
{                                          //zxbeepluse
    if (st)
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_2);                            //开启背光
    }
    else
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    }
}

/*********************************************************************************************
* 名称：BLInit
* 功能：背光IO口初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void BLInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;           //zxbeepluse
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*********************************************************************************************
* 名称：LCDREST_Init
* 功能：复位IO口初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCDREST_Init(void)
{
    //硬件复位
}

/*********************************************************************************************
* 名称：ILI93xxInit
* 功能：LCD初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void ILI93xxInit(void)
{
    fsmc_init();
    delay_ms(50);
    
    BLInit();
    BLOnOff(1);        //9341要先开启背光才能读取寄存器值
    
    lcddev.id = ILI93xx_ReadReg(0);
    if (lcddev.id == 0)
    {
        LCD_WR_REG(0Xd3);
        int a=LCD_RD_DATA();	//dummy read
        int b=LCD_RD_DATA();	//读到0X00
        int c=LCD_RD_DATA();   	//读取93
        int d=LCD_RD_DATA();  	//读取41
        lcddev.id = (c << 8) | d;
    }

    if (lcddev.id == 0x72)
    {
        lcddev.RAM_W_CMD = 0x2C;
        //****信利2.6寸IPS 240*400 BOE+HX8352-C**///
        //Power Voltage Setting
        ILI93xx_WriteReg(0x001A, 0x0002);                           //BT
        ILI93xx_WriteReg(0x001B, 0x0088);                           //VRH

        //****VCOM offset**///
        ILI93xx_WriteReg(0x0023, 0x0000);                           //SEL_VCM
        ILI93xx_WriteReg(0x0024, 0x005F);                           //VCM
        ILI93xx_WriteReg(0x0025, 0x0015);                           //VDV
        ILI93xx_WriteReg(0x002D, 0x0003);                           //NOW[2:0]=011

        //Power on Setting
        ILI93xx_WriteReg(0x0018, 0x0004);                           //Frame rate 72Hz
        ILI93xx_WriteReg(0x0019, 0x0001);                           //OSC_EN='1', start Osc
        ILI93xx_WriteReg(0x0001, 0x0000);                           //DP_STB='0', out deep sleep

        //262k/65k color selection
        ILI93xx_WriteReg(0x0017, 0x0005);                           //default 0x06 262k color // 0x05 65k color
        //SET PANEL
        ILI93xx_WriteReg(0x0036, 0x0013);                           //REV_P, SM_P, GS_P, BGR_P, SS_P

        //Gamma 2.2 Setting
        ILI93xx_WriteReg(0x0040,0x0000);
        ILI93xx_WriteReg(0x0041,0x0045);
        ILI93xx_WriteReg(0x0042,0x0045);
        ILI93xx_WriteReg(0x0043,0x0004);
        ILI93xx_WriteReg(0x0044,0x0000);
        ILI93xx_WriteReg(0x0045,0x0008);
        ILI93xx_WriteReg(0x0046,0x0023);
        ILI93xx_WriteReg(0x0047,0x0023);
        ILI93xx_WriteReg(0x0048,0x0077);
        ILI93xx_WriteReg(0x0049,0x0040);

        ILI93xx_WriteReg(0x004A,0x0004);
        ILI93xx_WriteReg(0x004B,0x0000);
        ILI93xx_WriteReg(0x004C,0x0088);
        ILI93xx_WriteReg(0x004D,0x0088);
        ILI93xx_WriteReg(0x004E,0x0088);

        ILI93xx_WriteReg(0x001F,0x00D0);                            //VCOMG=1

        //Display ON Setting
        ILI93xx_WriteReg(0x0028,0x003F);                            //GON=1, DTE=1, D=11
        //
        ILI93xx_WriteReg(0x016,0x00);  //
        //////////////////////////////////////////////////////
        //Set GRAM Area
        unsigned int xStar = 0, xEnd=240, yStar = 0, yEnd=400;
        ILI93xx_WriteReg(0x02, xStar>>8);
        ILI93xx_WriteReg(0x03, xStar);
        ILI93xx_WriteReg(0x04, xEnd>>8);
        ILI93xx_WriteReg(0x05, xEnd);

        ILI93xx_WriteReg(0x06, yStar>>8);
        ILI93xx_WriteReg(0x07, yStar);
        ILI93xx_WriteReg(0x08, yEnd>>8);
        ILI93xx_WriteReg(0x09, yEnd);

        ILI93xx_REG = 0x22;
        for (int i=0; i<240; i++)
        {
            for (int j=0; j<400; j++)
            {
                ILI93xx_DAT = 0xf800;
            }
        }
    }
    else if ((lcddev.id == 0x9325) || (lcddev.id == 0x9328) || (lcddev.id == 0xD3D3))            //ILI9328   OK
    {
        lcddev.RAM_W_CMD = 0x22;
        char sm = 0;
        char ss = 1;
        char gs = 1;
#if SCREEN_ORIENTATION_LANDSCAPE
        gs = 0;
#endif
        ILI93xx_WriteReg(0x00EC,0x108F);                            // internal timeing
        ILI93xx_WriteReg(0x00EF,0x1234);                            // ADD
        ILI93xx_WriteReg(0x0001,0x0000|(ss<<8) | (sm<<10));
        ILI93xx_WriteReg(0x0002,0x0700);                            //电源开启
        //DRIVE TABLE(寄存器 03H)
        //BIT3=AM BIT4:5=ID0:1
        //AM      ID0     ID1               FUNCATION
        // 0      0       0	                R->L    D->U
        // 1      0       0	                D->U	R->L
        // 0      1       0	                L->R    D->U
        // 1      1       0                 D->U	L->R
        // 0      0       1	                R->L    U->D
        // 1      0       1                 U->D	R->L
        // 0      1       1                 L->R    U->D 正常就用这个.
        // 1      1       1	                U->D	L->R
#if SCREEN_ORIENTATION_LANDSCAPE
        ILI93xx_WriteReg(0x0003,(1<<12)|(0<<5)|(1<<4)|(1<<3));      //65K
#else
        ILI93xx_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3));      //65K
#endif

        ILI93xx_WriteReg(0x0004,0x0000);
        ILI93xx_WriteReg(0x0008,0x0202);
        ILI93xx_WriteReg(0x0009,0x0000);
        ILI93xx_WriteReg(0x000a,0x0000);                            //display setting
        ILI93xx_WriteReg(0x000c,0x0001);                            //display setting
        ILI93xx_WriteReg(0x000d,0x0000);                            //0f3c
        ILI93xx_WriteReg(0x000f,0x0000);
        //电源配置
        ILI93xx_WriteReg(0x0010,0x0000);
        ILI93xx_WriteReg(0x0011,0x0007);
        ILI93xx_WriteReg(0x0012,0x0000);
        ILI93xx_WriteReg(0x0013,0x0000);
        ILI93xx_WriteReg(0x0007,0x0001);
        delay_ms(50);
        ILI93xx_WriteReg(0x0010,0x1490);
        ILI93xx_WriteReg(0x0011,0x0227);
        delay_ms(50);
        ILI93xx_WriteReg(0x0012,0x008A);
        delay_ms(50);
        ILI93xx_WriteReg(0x0013,0x1a00);
        ILI93xx_WriteReg(0x0029,0x0006);
        ILI93xx_WriteReg(0x002b,0x000d);
        delay_ms(50);
        ILI93xx_WriteReg(0x0020,0x0000);
        ILI93xx_WriteReg(0x0021,0x0000);
        delay_ms(50);
        //伽马校正
        ILI93xx_WriteReg(0x0030,0x0000);
        ILI93xx_WriteReg(0x0031,0x0604);
        ILI93xx_WriteReg(0x0032,0x0305);
        ILI93xx_WriteReg(0x0035,0x0000);
        ILI93xx_WriteReg(0x0036,0x0C09);
        ILI93xx_WriteReg(0x0037,0x0204);
        ILI93xx_WriteReg(0x0038,0x0301);
        ILI93xx_WriteReg(0x0039,0x0707);
        ILI93xx_WriteReg(0x003c,0x0000);
        ILI93xx_WriteReg(0x003d,0x0a0a);
        delay_ms(50);
        ILI93xx_WriteReg(0x0050,0x0000);                            //水平GRAM起始位置
        ILI93xx_WriteReg(0x0051,0x00ef);                            //水平GRAM终止位置

        ILI93xx_WriteReg(0x0052,0x0000);                            //垂直GRAM起始位置
        ILI93xx_WriteReg(0x0053,0x013f);                            //垂直GRAM终止位置

        ILI93xx_WriteReg(0x0060,(gs<<15)|0x2700);                   //320 line

        ILI93xx_WriteReg(0x0061,0x0001);
        ILI93xx_WriteReg(0x006a,0x0000);
        ILI93xx_WriteReg(0x0080,0x0000);
        ILI93xx_WriteReg(0x0081,0x0000);
        ILI93xx_WriteReg(0x0082,0x0000);
        ILI93xx_WriteReg(0x0083,0x0000);
        ILI93xx_WriteReg(0x0084,0x0000);
        ILI93xx_WriteReg(0x0085,0x0000);

        ILI93xx_WriteReg(0x0090,0x0010);
        ILI93xx_WriteReg(0x0092,0x0600);
        //开启显示设置
        ILI93xx_WriteReg(0x0007,0x0133);
        
#if (SCREEN_ORIENTATION_LANDSCAPE == 1)        
        ILI9325_ScanDir(7);
#endif
        LCDSetWindow(0, LCD_WIDTH, 0, LCD_HEIGHT);              //设置窗口为整个屏幕
    }
    else if (lcddev.id == 0x9341)
    {
        lcddev.RAM_W_CMD = 0x2C;
        LCD_WR_REG(0xCF);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC1);
        LCD_WR_DATA(0X30);
        LCD_WR_REG(0xED);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0X12);
        LCD_WR_DATA(0X81);
        LCD_WR_REG(0xE8);
        LCD_WR_DATA(0x85);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x7A);
        LCD_WR_REG(0xCB);
        LCD_WR_DATA(0x39);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x02);
        LCD_WR_REG(0xF7);
        LCD_WR_DATA(0x20);
        LCD_WR_REG(0xEA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xC0);                                           //Power control
        LCD_WR_DATA(0x1B);                                          //VRH[5:0]
        LCD_WR_REG(0xC1);                                           //Power control
        LCD_WR_DATA(0x01);                                          //SAP[2:0];BT[3:0]
        LCD_WR_REG(0xC5);                                           //VCM control
        LCD_WR_DATA(0x30); 	                                        //3F
        LCD_WR_DATA(0x30); 	                                        //3C
        LCD_WR_REG(0xC7);                                           //VCM control2
        LCD_WR_DATA(0XB7);

        LCD_WR_REG(0x36);                                           // Memory Access Control

#if SCREEN_ORIENTATION_LANDSCAPE
        LCD_WR_DATA((1<<7)|(0<<6)|(1<<5)|SWAP_RGB);                 //从下到上,从左到右
#else
        LCD_WR_DATA((0<<7)|(0<<6)|(0<<5)|SWAP_RGB);                 //从左到右,从上到下
#endif

        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);
        LCD_WR_REG(0xB1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1A);
        LCD_WR_REG(0xB6);                                           // Display Function Control
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0xA2);
        LCD_WR_REG(0xF2);                                           // 3Gamma Function Disable
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0x26);                                           //Gamma curve selected
        LCD_WR_DATA(0x01);
        LCD_WR_REG(0xE0);                                           //Set Gamma
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x2A);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x0E);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0XA9);
        LCD_WR_DATA(0x43);
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0XE1);                                           //Set Gamma
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x15);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x11);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x2B);
        LCD_WR_DATA(0x56);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x0F);

        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);                                          //LCD_WR_DATA(0x01);
        LCD_WR_DATA(0xef);                                          //LCD_WR_DATA(0x3f);
        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);                                          //LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3f);                                          //LCD_WR_DATA(0xef);

        LCD_WR_REG(0x11);                                           //Exit Sleep
        delay_ms(120);
        LCD_WR_REG(0x29);                                           //display on
        
        LCDSetWindow(0, LCD_WIDTH, 0, LCD_HEIGHT);              //设置窗口为整个屏幕
    }
    LCDClear(LCD_COLOR_BLACK);
}

/*********************************************************************************************
* 名称：LCDSetWindow
* 功能：设置窗口
* 参数：x:窗口起始横坐标，xe：窗口终点横坐标，y:窗口起始纵坐标，ye：窗口终点纵坐标
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCDSetWindow(int x, int xe, int y, int ye)
{
    if (lcddev.id == 0x9341)
    {
        LCD_WR_REG(0X2A);
        LCD_WR_DATA(x>>8);
        LCD_WR_DATA(x&0XFF);
        LCD_WR_DATA(xe>>8);
        LCD_WR_DATA(xe&0XFF);
        LCD_WR_REG(0X2B);
        LCD_WR_DATA(y>>8);
        LCD_WR_DATA(y&0XFF);
        LCD_WR_DATA(ye>>8);
        LCD_WR_DATA(ye&0XFF);
    }
    else
    {
#if SCREEN_ORIENTATION_LANDSCAPE
        ILI93xx_WriteReg(0x52, x);	                                //xstart
        ILI93xx_WriteReg(0x53, xe);                                 //xend

        ILI93xx_WriteReg(0x50, y);	                                //ystart
        ILI93xx_WriteReg(0x51, ye);
#else
        ILI93xx_WriteReg(0x52, y);	                                //xstart
        ILI93xx_WriteReg(0x53, ye);                                 //xend

        ILI93xx_WriteReg(0x50, x);	                                //ystart
        ILI93xx_WriteReg(0x51, xe);
#endif
    }
}

/*********************************************************************************************
* 名称：LCDSetCursor
* 功能：设置坐标
* 参数：x,y：坐标
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCDSetCursor(int x, int y)
{
    if (lcddev.id == 0x9341)
    {
        LCD_WR_REG(0X2A);
        LCD_WR_DATA(x>>8);
        LCD_WR_DATA(x&0XFF);
        LCD_WR_REG(0X2B);
        LCD_WR_DATA(y>>8);
        LCD_WR_DATA(y&0XFF);
    }
    if ((lcddev.id == 0x9325) || (lcddev.id == 0x9328))
    {
#if SCREEN_ORIENTATION_LANDSCAPE
        ILI93xx_WriteReg(0x21, x);
        ILI93xx_WriteReg(0x20, y);
#else
        ILI93xx_WriteReg(0x21, y);
        ILI93xx_WriteReg(0x20, x);
#endif
    }
}


void LCD_WriteRAM_Prepare()	
{
    ILI93xx_REG = lcddev.RAM_W_CMD;                                       //显示命令
}

/*********************************************************************************************
* 名称：LCDWriteLenData
* 功能：写固定长度数据
* 参数：dat:数据，len：数据长度
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCDWriteLenData(uint16_t *dat, int len)
{
    ILI93xx_REG = lcddev.RAM_W_CMD;
    for (int i=0; i<len; i++)
    {
        ILI93xx_DAT = dat[i];
    }
}

/*********************************************************************************************
* 名称：LCDClear
* 功能：LCD清屏
* 参数：color:清屏颜色
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCDClear(uint16_t color)
{
    LCDSetCursor(0,0);
    ILI93xx_REG = lcddev.RAM_W_CMD;
    for (long i=0; i<LCD_WIDTH*LCD_HEIGHT; i++)
    {
        ILI93xx_DAT = color;
    }
}

/*********************************************************************************************
* 名称：TFTLCD_DrawPixel
* 功能：画点
* 参数：x,y：坐标，color：点的颜色
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void TFTLCD_DrawPixel(int x, int y, uint16_t color)
{
    LCDSetCursor(x, y);
    ILI93xx_REG = lcddev.RAM_W_CMD;
    ILI93xx_DAT = color;
}


/*********************************************************************************************
* 名称：TFTLCD_ReadPixel
* 功能：读点
* 参数：x,y：坐标
* 返回：点的颜色
* 修改：
* 注释：
*********************************************************************************************/
uint16_t TFTLCD_ReadPixel(int x, int y)
{
    uint16_t color = 0;
    LCDSetCursor(x, y);
    ILI93xx_REG = lcddev.RAM_W_CMD;
    color = ILI93xx_DAT;
    return color;
}


/*********************************************************************************************
* 名称：LCD_Clear
* 功能：清指定大小的屏幕
* 参数：x1,y1:起始坐标值，x2,y2:终点坐标值， color：屏幕颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_FillColor(int x1,int y1,int x2,int y2,uint16_t color)
{
    LCDSetWindow(x1,x2,y1,y2);
    LCDSetCursor(x1, y1);
    ILI93xx_REG = lcddev.RAM_W_CMD;                                       //显示命令
    for(int i=x1; i<=x2; i++)
    {
        for(int j=y1; j<=y2; j++)
        {
            ILI93xx_DAT = color;
        }
    }
    
    LCDSetWindow(0, LCD_WIDTH, 0, LCD_HEIGHT);              //设置窗口为整个屏幕
}



/*********************************************************************************************
* 名称：LCD_FillData
* 功能：在指定区域显示指定颜色块
* 参数：(sx,xy)(ex,ey)：填充矩形对角坐标，color：填充颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_FillData(u16 sx,u16 sy,u16 ex,u16 ey,u16* color)
{
    u16 height,width;
    u16 x,y;
    width=ex-sx; 			//填充宽度
    height=ey-sy;			//填充高度

    LCDSetWindow(sx,ex,sy,ey);
    LCDSetCursor(sx, sy);
    ILI93xx_REG = lcddev.RAM_W_CMD;                                       //显示命令
    for(y=0; y<height; y+=1)
    {
        for(x=0; x<width; x+=1)
        {
            ILI93xx_DAT = color[y*width+x];//画点
        }
    }
    LCDSetWindow(0, LCD_WIDTH, 0, LCD_HEIGHT);              //设置窗口为整个屏幕
}


/*********************************************************************************************
* 名称：LCD_DrawLine
* 功能：lcd画线函数
* 参数：x1，y1：起点坐标，x2，y2：终点坐标，color：线的颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;

    if(delta_x>0)incx=1; //设置单步方向
    else if(delta_x==0)incx=0;//垂直线
    else
    {
        incx=-1;
        delta_x=-delta_x;
    }

    if(delta_y>0)incy=1;
    else if(delta_y==0)incy=0;//水平线
    else
    {
        incy=-1;
        delta_y=-delta_y;
    }

    if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0; t<=distance+1; t++ ) //画线输出
    {
        TFTLCD_DrawPixel(uRow,uCol,color);//画点
        xerr+=delta_x ;
        yerr+=delta_y ;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}


//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
    int a,b;
    int di;
    a=0;
    b=r;
    di=3-(r<<1);             //判断下个点位置的标志
    while(a<=b)
    {
        TFTLCD_DrawPixel(x0+a,y0-b,color);             //5
        TFTLCD_DrawPixel(x0+b,y0-a,color);             //0
        TFTLCD_DrawPixel(x0+b,y0+a,color);             //4
        TFTLCD_DrawPixel(x0+a,y0+b,color);             //6
        TFTLCD_DrawPixel(x0-a,y0+b,color);             //1
        TFTLCD_DrawPixel(x0-b,y0+a,color);
        TFTLCD_DrawPixel(x0-a,y0-b,color);             //2
        TFTLCD_DrawPixel(x0-b,y0-a,color);             //7
        a++;
        //使用Bresenham算法画圆
        if(di<0)di +=4*a+6;
        else
        {
            di+=10+4*(a-b);
            b--;
        }
    }
}


//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode,u16 color,u16 backColor)
{
    u8 temp,t1,t;
    u16 y0=y;
    u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(t=0; t<csize; t++)
    {
        if(size==12)temp=tft_asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=tft_asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=tft_asc2_2412[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0; t1<8; t1++)
        {
            if(temp&0x80)TFTLCD_DrawPixel(x,y,color);
            else if(mode==0)TFTLCD_DrawPixel(x,y,backColor);
            temp<<=1;
            y++;
            if(y>=LCD_HEIGHT)return;		//超区域了
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=LCD_WIDTH)return;	//超区域了
                break;
            }
        }
    }
}


//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p,u16 color,u16 backColor)
{
    u8 x0=x;
    width+=x;
    height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width)
        {
            x=x0;
            y+=size;
        }
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0,color,backColor);
        x+=size/2;
        p++;
    }
}


/*********************************************************************************************
* 名称：LCD_PutChar16
* 功能：16*16汉字字符显示（文件字库版）
* 参数：x，y：显示坐标 *data：字符串名Color：汉字颜色 BColor：背景颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_PutChar16(u16 x,u16 y,u8 *data,u16 Color,u16 BColor)
{
    unsigned int i,j,b=1,index;
    u8 tmp_char;
    for(b=0; b<1; b++)
    {
        index=(94*(data[b*2] - 0xa1)+(data[b*2+1] - 0xa1));	    //计算区位
        for(i=0; i<16; i++)
        {
            for(j=0; j<8; j++)
            {
                tmp_char=gbk_st16[index*32+i*2];
                if ( (tmp_char << j) & 0x80)
                {
                    TFTLCD_DrawPixel(x+j,y+i,Color);
                }
                else
                {
                    TFTLCD_DrawPixel(x+j,y+i,BColor);
                }
            }
            for(j=0; j<8; j++)
            {
                tmp_char=gbk_st16[index*32+i*2+1];
                if ( (tmp_char << j) & 0x80)
                {
                    TFTLCD_DrawPixel(x+j+8,y+i,Color);
                }
                else
                {
                    TFTLCD_DrawPixel(x+j+8,y+i,BColor);
                }
            }
        }
    }
}


/*********************************************************************************************
* 名称：lcdShowFont16
* 功能：lcd显示16x16汉字（文件字库版）
* 参数：x1,y1:起始坐标值，color：字体颜色，Bcolor：背景颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCDShowFont16(u16 x,u16 y,char* p,u16 width,u16 color,u16 Bcolor)
{
    u8* temp=(u8*)p;
    u16 x1=x;

    while(x1<(x+width))
    {
        if(*temp!='\0')
        {
            if (*temp & 0x80)
            {
                LCD_PutChar16(x1,y,temp,color,Bcolor);
                temp+=2;
                x1+=16;
            }
            else
            {
                LCD_ShowChar(x1,y,*temp,16,0,color,Bcolor);
                temp ++;
                x1 += 8;
            }
        }
        else
        {
            LCD_ShowChar(x1,y,' ',16,0,color,Bcolor);
            x1 += 8;
        }
    }
}

