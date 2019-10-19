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
#include "fml_lcd/st7789.h"

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT       printf
#else
#define DEBUG_PRINT(...)
#endif


/*********************************************************************************************
* 全局变量
*********************************************************************************************/
short St7789_wide = ST7789_WIDE;
short St7789_high = ST7789_HIGH;
uint8_t St7789_dir = 1;//横屏还是竖屏控制：0，竖屏；1，横屏。

  
/*********************************************************************************************
* 名称：St7789_init()
* 功能：St7789初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void St7789_GpioInit(void)
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
* 名称：St7789_IDGet
* 功能：St7789id获取
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
uint32_t St7789_IDGet()
{
  uint32_t id = 0;
  uint16_t v3, v4;  
  
  St7789_WCMD(0x04);
  v3 = St7789_RDATA();
  v4 = St7789_RDATA();  
  if ((0x85==v3 && 0x52 == v4) || (v3==0x04 && v4==0x85))
  {
    id = (v3<<8)|v4;
    return id;
  }
  DEBUG_PRINT("error: St7789 can't find lcd ic\r\n");
  return 0;
}

/*********************************************************************************************
* 名称：St7789_init()
* 功能：St7789初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
uint8_t St7789_init(void)
{
  St7789_GpioInit();
  fsmc_init();
  
  if(St7789_IDGet() == 0)
    return 1;
  
  St7789_WCMD(0x11); 
  delay_ms(120);                                                //Delay 120ms 
  St7789_WCMD(0x3A);
  St7789_WDATA(0x05);
  
  St7789_WCMD(0x36);
  if(St7789_dir)
    St7789_WDATA(0xA0);
  else
    St7789_WDATA(0x00);
  
#if 0
  //ST7789S Frame rate setting
  St7789_WCMD(0xb2);
  St7789_WDATA(0x00);
  St7789_WDATA(0x00);
  St7789_WDATA(0x00);
  St7789_WDATA(0x33);
  St7789_WDATA(0x33);
  St7789_WCMD(0xb7);
  St7789_WDATA(0x35);
  //ST7789S Power setting
  St7789_WCMD(0xb8);
  St7789_WDATA(0x2f);
  St7789_WDATA(0x2b);
  St7789_WDATA(0x2f);
  St7789_WCMD(0xbb);
  St7789_WDATA(0x24);                                             //vcom
  St7789_WCMD(0xc0);
  St7789_WDATA(0x2C);
  St7789_WCMD(0xc3);
  St7789_WDATA(0x10);                                             //0B调深浅 
  St7789_WCMD(0xc4);
  St7789_WDATA(0x20);
  St7789_WCMD(0xc6);
  St7789_WDATA(0x11);
  St7789_WCMD(0xd0);
  St7789_WDATA(0xa4);
  St7789_WDATA(0xa1);
  St7789_WCMD(0xe8);
  St7789_WDATA(0x03);
  St7789_WCMD(0xe9);
  St7789_WDATA(0x0d);
  St7789_WDATA(0x12);
  St7789_WDATA(0x00);
  //ST7789S gamma setting
  St7789_WCMD(0xe0);
  St7789_WDATA(0xd0);
  St7789_WDATA(0x00);
  St7789_WDATA(0x00);
  St7789_WDATA(0x08);
  St7789_WDATA(0x11);
  St7789_WDATA(0x1a);
  St7789_WDATA(0x2b);
  St7789_WDATA(0x33);
  St7789_WDATA(0x42);
  St7789_WDATA(0x26);
  St7789_WDATA(0x12);
  St7789_WDATA(0x21);
  St7789_WDATA(0x2f);
  St7789_WDATA(0x11);
  St7789_WCMD(0xe1);
  St7789_WDATA(0xd0);
  St7789_WDATA(0x02);
  St7789_WDATA(0x09);
  St7789_WDATA(0x0d);
  St7789_WDATA(0x0d);
  St7789_WDATA(0x27);
  St7789_WDATA(0x2b);
  St7789_WDATA(0x33);
  St7789_WDATA(0x42);
  St7789_WDATA(0x17);
  St7789_WDATA(0x12);
  St7789_WDATA(0x11);
  St7789_WDATA(0x2f);
  St7789_WDATA(0x31);
  St7789_WCMD(0x21);                                          //反显  
  St7789_WCMD(0x29);                                          //display on
#else
  //-------------------------------ST7789S Frame rate setting----------------------------------// 
  St7789_WCMD(0xb2); 
  St7789_WDATA(0x0c); 
  St7789_WDATA(0x0c); 
  St7789_WDATA(0x00); 
  St7789_WDATA(0x33); 
  St7789_WDATA(0x33); 

  St7789_WCMD(0xb7); 
  St7789_WDATA(0x35); 
  //---------------------------------ST7789S Power setting--------------------------------------// 
  St7789_WCMD(0xbb); 
  St7789_WDATA(0X35);//vcom flick

  St7789_WCMD(0xc2);
  St7789_WDATA(0x01); 
   
  St7789_WCMD(0xc3);//gvdd
  St7789_WDATA(0x10); //调深浅12
                                                                                      
  St7789_WCMD(0xc4); 
  St7789_WDATA(0x20); 

  St7789_WCMD(0xc6); 
  St7789_WDATA(0x0f); 

  St7789_WCMD(0xd0); 
  St7789_WDATA(0xa4); 
  St7789_WDATA(0xa2); 
  //--------------------------------ST7789S gamma setting---------------------------------------// 
  St7789_WCMD(0xe0); 
  St7789_WDATA(0xD0); 
  St7789_WDATA(0x00); 
  St7789_WDATA(0x02); 
  St7789_WDATA(0x07); 
  St7789_WDATA(0x0A); 
  St7789_WDATA(0x28); 
  St7789_WDATA(0x32); 
  St7789_WDATA(0x44); 
  St7789_WDATA(0x42); 
  St7789_WDATA(0x06); 
  St7789_WDATA(0x0E); 
  St7789_WDATA(0x12); 
  St7789_WDATA(0x14); 
  St7789_WDATA(0x17);

  St7789_WCMD(0xe1); 
  St7789_WDATA(0xD0); 
  St7789_WDATA(0x00); 
  St7789_WDATA(0x02); 
  St7789_WDATA(0x07); 
  St7789_WDATA(0x0A); 
  St7789_WDATA(0x28); 
  St7789_WDATA(0x31); 
  St7789_WDATA(0x54); 
  St7789_WDATA(0x47); 
  St7789_WDATA(0x0E); 
  St7789_WDATA(0x1C); 
  St7789_WDATA(0x17); 
  St7789_WDATA(0x1B); 
  St7789_WDATA(0x1E); 

  St7789_WCMD(0x3a); 
  St7789_WDATA(0x55);

  St7789_WCMD(0x2A); //Frame rate control
  St7789_WDATA(0x00);
  St7789_WDATA(0x00);
  St7789_WDATA(0x00);
  St7789_WDATA(0xEF);

  St7789_WCMD(0x2B); //Display function control
  St7789_WDATA(0x00);
  St7789_WDATA(0x00);
  St7789_WDATA(0x01);
  St7789_WDATA(0x3F);
  //**********************
  St7789_WCMD(0x11); 
  delay_ms(120);      //Delay 120ms 
  St7789_WCMD(0x29); //display on
  delay_ms(25);
#endif
  
  St7789_FillColor(0, 0, St7789_wide-1, St7789_high-1, 0x0000);
  return 0;
}


/********************************************************************************************/
uint8_t St7789_DirectionSet(uint8_t dir)
{
  if(dir>1) return 1;
  St7789_WCMD(0x36);
  if(dir)
  {
    St7789_WDATA(0xA0);
    St7789_wide = ST7789_WIDE;
    St7789_high = ST7789_HIGH;
  }
  else
  {
    St7789_WDATA(0x00);
    St7789_wide = ST7789_HIGH;
    St7789_high = ST7789_WIDE;
  }
  St7789_dir = dir;
  return 0;
}


/*********************************************************************************************
* 名称：St7789_prepare_write()
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void St7789_PrepareWrite()
{
  St7789_WCMD(ST7789_WRAM);
}


/********************************************************************************************/
void St7789_SetCursorPos(short x,short y)
{
  St7789_WCMD(ST7789_SETX);                                          //Frame rate control
  St7789_WDATA(x>>8);
  St7789_WDATA(x&0xff);  
  St7789_WCMD(ST7789_SETY);                                          //Display function control
  St7789_WDATA(y>>8);
  St7789_WDATA(y&0xff);
}

/*********************************************************************************************
* 名称：St7789_set_activie_window()
* 功能：
* 参数：x1 -- 
*       y1 --
*       x2 --
*       y2 --
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void St7789_SetWindow(short x1, short y1, short x2, short y2)
{
  St7789_WCMD(ST7789_SETX);                                          //Frame rate control
  St7789_WDATA(x1>>8);
  St7789_WDATA(x1);  
  St7789_WDATA(x2>>8);
  St7789_WDATA(x2);
  St7789_WCMD(ST7789_SETY);                                          //Display function control
  St7789_WDATA(y1>>8);
  St7789_WDATA(y1);
  St7789_WDATA(y2>>8);
  St7789_WDATA(y2);
}


/********************************************************************************************/
void St7789_PrepareFill(short x1, short y1, short x2, short y2)
{
  St7789_SetWindow(x1,y1,x2,y2);
  St7789_PrepareWrite();
}


/********************************************************************************************/
void St7789_DrawPoint(short x,short y,uint32_t color)
{
#if 0
  St7789_SetCursorPos(x,y);
  St7789_PrepareWrite();		//开始写入GRAM
  St7789_WDATA(color);
#else
  St7789_WCMD(ST7789_SETX);                                          //Frame rate control
  St7789_WDATA(x>>8);
  St7789_WDATA(x&0xff);  
  St7789_WCMD(ST7789_SETY);                                          //Display function control
  St7789_WDATA(y>>8);
  St7789_WDATA(y&0xff);
  
  St7789_WCMD(0x2C);
  
  St7789_WDATA(color);
#endif
}

/********************************************************************************************/
uint32_t St7789_ReadPoint(short x,short y)
{
  uint32_t r=0,g=0,b=0;
  St7789_SetCursorPos(x,y);
  St7789_WCMD(ST7789_RRAM);
  r = St7789_RDATA();//dummy Read
  r = St7789_RDATA();//RG
  b = St7789_RDATA();//BR
  g = r&0X00FF;
  r = r&0XFF00;
  return (r|((g>>2)<<5)|(b>>11));
}

/*********************************************************************************************
* 名称：St7789_FillColor
* 功能：
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void St7789_FillColor(short x1,short y1,short x2,short y2,uint32_t color)
{
  uint16_t x, y;
  St7789_SetWindow(x1,y1,x2,y2);
  St7789_PrepareWrite();
  for (y=y1; y<=y2; y++)
  {
    for (x=x1; x<=x2; x++)
    {
      St7789_WDATA(color);
    }
  }
}

/*********************************************************************************************
* 名称：St7789_FillData
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void St7789_FillData(short x1,short y1,short x2,short y2,unsigned short* dat)
{
  short x, y;
  St7789_SetWindow(x1,y1,x2,y2);
  St7789_PrepareWrite();
  for (y=y1; y<=y2; y++)
  {
    for (x=x1; x<=x2; x++)
    {
      St7789_WDATA(*dat);
      dat++;
    }
  }
}
