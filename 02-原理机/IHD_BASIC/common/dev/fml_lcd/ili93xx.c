/*********************************************************************************************
* 文件：
* 作者：
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "fml_lcd/ili93xx.h"

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT       printf
#else
#define DEBUG_PRINT(...)
#endif


/*********************************************************************************************
* 全局变量
*********************************************************************************************/
typedef struct
{
  short wide;
  short high;
  uint32_t id;
  uint8_t dir;    //横屏还是竖屏控制：0，竖屏；1，横屏。
  uint8_t wram;//开始写gram指令	
  uint8_t rram;//开始读gram指令
  uint8_t setx;//设置x坐标指令
  uint8_t sety;//设置y坐标指令	 
}Ili93xx_dev_t;

Ili93xx_dev_t Ili93xx = {
  .wide = ILI93XX_HIGH,
  .high = ILI93XX_WIDE,
  .id = 0,
  .dir = 1,
};

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
  ILI93XX_WCMD(r);
  ILI93XX_WDATA(d);
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
  ILI93XX_WCMD(r);
  v = ILI93XX_RDATA();
  return v;
}

/*********************************************************************************************
* 名称：Ili93xx_init()
* 功能：Ili93xx初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Ili93xx_GpioInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;  
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  //reset
  // 
  //black light
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_SetBits(GPIOD, GPIO_Pin_2);
}

/*********************************************************************************************
* 名称：Ili93xx_IDGet
* 功能：Ili93xxid获取
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
uint32_t Ili93xx_IDGet()
{
  uint32_t id = ILI93xx_ReadReg(0);
  if (id == 0)
  {
    ILI93XX_WCMD(0Xd3);
    int a=ILI93XX_RDATA();	//dummy read
    int b=ILI93XX_RDATA();	//读到0X00
    int c=ILI93XX_RDATA();   	//读取93
    int d=ILI93XX_RDATA();  	//读取41
    id = (c << 8) | d;
  }
  if ((id == 0x9325) ||
      (id == 0x9328) ||
      (id == 0xD3D3) ||
      (id == 0x9341))
  {
    return id;
  }
  DEBUG_PRINT("error: Ili93xx can't find lcd ic\r\n");
  return 0;
}

/*********************************************************************************************
* 名称：Ili93xx_init()
* 功能：Ili93xx初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
uint8_t Ili93xx_init(void)
{
  Ili93xx_GpioInit();
  fsmc_init();
  
  Ili93xx.id = Ili93xx_IDGet();
  if(Ili93xx.id == 0)
    return 1;
  
  if (Ili93xx.id == 0x9341)
  {
    Ili93xx.wram = 0x2C;
    ILI93XX_WCMD(0xCF);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0xC1);
    ILI93XX_WDATA(0X30);
    ILI93XX_WCMD(0xED);
    ILI93XX_WDATA(0x64);
    ILI93XX_WDATA(0x03);
    ILI93XX_WDATA(0X12);
    ILI93XX_WDATA(0X81);
    ILI93XX_WCMD(0xE8);
    ILI93XX_WDATA(0x85);
    ILI93XX_WDATA(0x10);
    ILI93XX_WDATA(0x7A);
    ILI93XX_WCMD(0xCB);
    ILI93XX_WDATA(0x39);
    ILI93XX_WDATA(0x2C);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x34);
    ILI93XX_WDATA(0x02);
    ILI93XX_WCMD(0xF7);
    ILI93XX_WDATA(0x20);
    ILI93XX_WCMD(0xEA);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);
    ILI93XX_WCMD(0xC0);                                           //Power control
    ILI93XX_WDATA(0x1B);                                          //VRH[5:0]
    ILI93XX_WCMD(0xC1);                                           //Power control
    ILI93XX_WDATA(0x01);                                          //SAP[2:0];BT[3:0]
    ILI93XX_WCMD(0xC5);                                           //VCM control
    ILI93XX_WDATA(0x30); 	                                        //3F
    ILI93XX_WDATA(0x30); 	                                        //3C
    ILI93XX_WCMD(0xC7);                                           //VCM control2
    ILI93XX_WDATA(0XB7);
    
    ILI93XX_WCMD(0x36);                                           // Memory Access Control
    
    if(Ili93xx.dir)
      ILI93XX_WDATA((1<<7)|(1<<6)|(1<<5)|ILI93XX_RS_PIN);                 //从下到上,从左到右
    else
      ILI93XX_WDATA((0<<7)|(0<<6)|(0<<5)|ILI93XX_RS_PIN);                 //从左到右,从上到下
    
    ILI93XX_WCMD(0x3A);
    ILI93XX_WDATA(0x55);
    ILI93XX_WCMD(0xB1);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x1A);
    ILI93XX_WCMD(0xB6);                                           // Display Function Control
    ILI93XX_WDATA(0x0A);
    ILI93XX_WDATA(0xA2);
    ILI93XX_WCMD(0xF2);                                           // 3Gamma Function Disable
    ILI93XX_WDATA(0x00);
    ILI93XX_WCMD(0x26);                                           //Gamma curve selected
    ILI93XX_WDATA(0x01);
    ILI93XX_WCMD(0xE0);                                           //Set Gamma
    ILI93XX_WDATA(0x0F);
    ILI93XX_WDATA(0x2A);
    ILI93XX_WDATA(0x28);
    ILI93XX_WDATA(0x08);
    ILI93XX_WDATA(0x0E);
    ILI93XX_WDATA(0x08);
    ILI93XX_WDATA(0x54);
    ILI93XX_WDATA(0XA9);
    ILI93XX_WDATA(0x43);
    ILI93XX_WDATA(0x0A);
    ILI93XX_WDATA(0x0F);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);
    ILI93XX_WCMD(0XE1);                                           //Set Gamma
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x15);
    ILI93XX_WDATA(0x17);
    ILI93XX_WDATA(0x07);
    ILI93XX_WDATA(0x11);
    ILI93XX_WDATA(0x06);
    ILI93XX_WDATA(0x2B);
    ILI93XX_WDATA(0x56);
    ILI93XX_WDATA(0x3C);
    ILI93XX_WDATA(0x05);
    ILI93XX_WDATA(0x10);
    ILI93XX_WDATA(0x0F);
    ILI93XX_WDATA(0x3F);
    ILI93XX_WDATA(0x3F);
    ILI93XX_WDATA(0x0F);
    
    ILI93XX_WCMD(0x2B);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);                                          //ILI93XX_WDATA(0x01);
    ILI93XX_WDATA(0xef);                                          //ILI93XX_WDATA(0x3f);
    ILI93XX_WCMD(0x2A);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x01);                                          //ILI93XX_WDATA(0x00);
    ILI93XX_WDATA(0x3f);                                          //ILI93XX_WDATA(0xef);
    
    ILI93XX_WCMD(0x11);                                           //Exit Sleep
    delay_ms(120);
    ILI93XX_WCMD(0x29);                                           //display on
    
    Ili93xx_SetWindow(0, ILI93XX_WIDE, 0, ILI93XX_HIGH);              //设置窗口为整个屏幕
  }
  else if ((Ili93xx.id == 0x9325) || (Ili93xx.id == 0x9328) || (Ili93xx.id == 0xD3D3))            //ILI9328   OK
  {
    Ili93xx.wram = 0x22;
    char sm = 0;
    char ss = 1;
    char gs = 1;
    if(Ili93xx.dir)
      gs = 0;
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
    if(Ili93xx.dir)
      ILI93xx_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(1<<3));      //65K
    else
      ILI93xx_WriteReg(0x0003,(1<<12)|(0<<5)|(0<<4)|(0<<3));      //65K
    
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
    
    Ili93xx_SetWindow(0, ILI93XX_WIDE, 0, ILI93XX_HIGH);              //设置窗口为整个屏幕
  }
  
  Ili93xx_FillColor(0, 0, Ili93xx.wide-1, Ili93xx.high-1, 0x0000);
  return 0;
}


/*********************************************************************************************
* 名称：
* 功能：
* 参数：
* 返回：
* 修改：
* 注释：
//DRIVE TABLE(寄存器 03H)
//BIT3=AM BIT4:5=ID0:1
//AM      ID0     ID1               FUNCATION
// 0      0       0	                R->L  D->U
// 1      0       0	                D->U  R->L
// 0      1       0	                L->R  D->U
// 1      1       0                 D->U  L->R
// 0      0       1	                R->L  U->D
// 1      0       1                 U->D  R->L
// 0      1       1                 L->R  U->D
// 1      1       1	                U->D  L->R
*********************************************************************************************/
uint8_t Ili93xx_DirectionSet(uint8_t dir)
{
  if(dir>8) return 1;
  uint16_t regVal = 0;
  //l-r/u-d
  regVal = ILI93xx_ReadReg(0x0003);
  regVal &= ~(7<<3);
  regVal |= (dir<<3);
  ILI93xx_WriteReg(0x0003,regVal);
  return 0;
}



/*********************************************************************************************
* 名称：Ili93xx_prepare_write()
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Ili93xx_PrepareWrite()
{
  ILI93XX_WCMD(Ili93xx.wram);
}


/********************************************************************************************/
void Ili93xx_SetCursorPos(short x,short y)
{
  if (Ili93xx.id == 0x9341)
  {
    ILI93XX_WCMD(0X2A);
    ILI93XX_WDATA(x>>8);
    ILI93XX_WDATA(x&0XFF);
    ILI93XX_WCMD(0X2B);
    ILI93XX_WDATA(y>>8);
    ILI93XX_WDATA(y&0XFF);
  }
  else if ((Ili93xx.id == 0x9325) || (Ili93xx.id == 0x9328))
  {
    ILI93xx_WriteReg(0x21, x);
    ILI93xx_WriteReg(0x20, y);
  }
}

/*********************************************************************************************
* 名称：Ili93xx_set_activie_window()
* 功能：
* 参数：x1 -- 
*       y1 --
*       x2 --
*       y2 --
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Ili93xx_SetWindow(short x1, short y1, short x2, short y2)
{
  if (Ili93xx.id == 0x9341)
  {
    ILI93XX_WCMD(0X2A);
    ILI93XX_WDATA(x1>>8);
    ILI93XX_WDATA(x1&0XFF);
    ILI93XX_WDATA(x2>>8);
    ILI93XX_WDATA(x2&0XFF);
    ILI93XX_WCMD(0X2B);
    ILI93XX_WDATA(y1>>8);
    ILI93XX_WDATA(y1&0XFF);
    ILI93XX_WDATA(y2>>8);
    ILI93XX_WDATA(y2&0XFF);
  }
  else
  {
    ILI93xx_WriteReg(0x50, y1);	                                //ystart
    ILI93xx_WriteReg(0x51, y2);
    ILI93xx_WriteReg(0x52, x1);	                                //xstart
    ILI93xx_WriteReg(0x53, x2);                                 //xend
  }
}


/********************************************************************************************/
void Ili93xx_PrepareFill(short x1, short y1, short x2, short y2)
{
  Ili93xx_SetWindow(x1,y1,x2,y2);
  Ili93xx_PrepareWrite();
}


/********************************************************************************************/
void Ili93xx_DrawPoint(short x,short y,uint32_t color)
{
  Ili93xx_SetCursorPos(x,y);
  Ili93xx_PrepareWrite();		//开始写入GRAM
  ILI93XX_WDATA(color);
}

/********************************************************************************************/
uint32_t Ili93xx_ReadPoint(short x,short y)
{
  uint32_t r=0,g=0,b=0;
  Ili93xx_SetCursorPos(x,y);
  ILI93XX_WCMD(Ili93xx.rram);
  r = ILI93XX_RDATA();//dummy Read
  r = ILI93XX_RDATA();//RG
  b = ILI93XX_RDATA();//BR
  g = r&0X00FF;
  r = r&0XFF00;
  return (r|((g>>2)<<5)|(b>>11));
}

/*********************************************************************************************
* 名称：Ili93xx_FillColor
* 功能：
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Ili93xx_FillColor(short x1,short y1,short x2,short y2,uint32_t color)
{
  uint16_t x, y;
  Ili93xx_SetWindow(x1,y1,x2,y2);
  Ili93xx_PrepareWrite();
  for (y=y1; y<=y2; y++)
  {
    for (x=x1; x<=x2; x++)
    {
      ILI93XX_WDATA(color);
    }
  }
}

/*********************************************************************************************
* 名称：Ili93xx_FillData
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Ili93xx_FillData(short x1,short y1,short x2,short y2,unsigned short* dat)
{
  short x, y;
  Ili93xx_SetWindow(x1,y1,x2,y2);
  Ili93xx_PrepareWrite();
  for (y=y1; y<=y2; y++)
  {
    for (x=x1; x<=x2; x++)
    {
      ILI93XX_WDATA(*dat);
      dat++;
    }
  }
}
