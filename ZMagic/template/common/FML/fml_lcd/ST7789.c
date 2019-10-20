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

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
screen_dev_t screen_dev = {
  .wide = LCD_HIGH,
  .high = LCD_WIDE,
  .id = 0,
  .dir = 1,//横屏
  .wramcmd = 0X2C,
  .rramcmd = 0X2E,
  .setxcmd = 0x2A,
  .setycmd = 0x2B,
};


  
/*********************************************************************************************
* 名称：st7789_init()
* 功能：st7789初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void st7789_GpioInit(void)
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
* 名称：st7789_init()
* 功能：st7789初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
uint8_t st7789_init(void)
{
  st7789_GpioInit();
  fsmc_init();
  
  uint16_t v3, v4;  
  write_command(0x04);
  v3 = ST7789_DAT;
  v4 = ST7789_DAT;  
  if (!((0x85==v3 && 0x52 == v4) || (v3==0x04 && v4==0x85))){
    printf("error: can't fond lcd ic\r\n");
    return 1;
  }
  write_command(0x11); 
  delay_ms(120);                                                //Delay 120ms 
  write_command(0x3A);
  write_data(0x05);
  
  write_command(0x36);
  if(screen_dev.dir)
    write_data(0xA0);
  else
    write_data(0x00);
  
#if 0
  //ST7789S Frame rate setting
  write_command(0xb2);
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0x33);
  write_data(0x33);
  write_command(0xb7);
  write_data(0x35);
  //ST7789S Power setting
  write_command(0xb8);
  write_data(0x2f);
  write_data(0x2b);
  write_data(0x2f);
  write_command(0xbb);
  write_data(0x24);                                             //vcom
  write_command(0xc0);
  write_data(0x2C);
  write_command(0xc3);
  write_data(0x10);                                             //0B调深浅 
  write_command(0xc4);
  write_data(0x20);
  write_command(0xc6);
  write_data(0x11);
  write_command(0xd0);
  write_data(0xa4);
  write_data(0xa1);
  write_command(0xe8);
  write_data(0x03);
  write_command(0xe9);
  write_data(0x0d);
  write_data(0x12);
  write_data(0x00);
  //ST7789S gamma setting
  write_command(0xe0);
  write_data(0xd0);
  write_data(0x00);
  write_data(0x00);
  write_data(0x08);
  write_data(0x11);
  write_data(0x1a);
  write_data(0x2b);
  write_data(0x33);
  write_data(0x42);
  write_data(0x26);
  write_data(0x12);
  write_data(0x21);
  write_data(0x2f);
  write_data(0x11);
  write_command(0xe1);
  write_data(0xd0);
  write_data(0x02);
  write_data(0x09);
  write_data(0x0d);
  write_data(0x0d);
  write_data(0x27);
  write_data(0x2b);
  write_data(0x33);
  write_data(0x42);
  write_data(0x17);
  write_data(0x12);
  write_data(0x11);
  write_data(0x2f);
  write_data(0x31);
  write_command(0x21);                                          //反显  
  write_command(0x29);                                          //display on
#else
  //-------------------------------ST7789S Frame rate setting----------------------------------// 
  write_command(0xb2); 
  write_data(0x0c); 
  write_data(0x0c); 
  write_data(0x00); 
  write_data(0x33); 
  write_data(0x33); 

  write_command(0xb7); 
  write_data(0x35); 
  //---------------------------------ST7789S Power setting--------------------------------------// 
  write_command(0xbb); 
  write_data(0X35);//vcom flick

  write_command(0xc2);
  write_data(0x01); 
   
  write_command(0xc3);//gvdd
  write_data(0x10); //调深浅12
                                                                                      
  write_command(0xc4); 
  write_data(0x20); 

  write_command(0xc6); 
  write_data(0x0f); 

  write_command(0xd0); 
  write_data(0xa4); 
  write_data(0xa2); 
  //--------------------------------ST7789S gamma setting---------------------------------------// 
  write_command(0xe0); 
  write_data(0xD0); 
  write_data(0x00); 
  write_data(0x02); 
  write_data(0x07); 
  write_data(0x0A); 
  write_data(0x28); 
  write_data(0x32); 
  write_data(0x44); 
  write_data(0x42); 
  write_data(0x06); 
  write_data(0x0E); 
  write_data(0x12); 
  write_data(0x14); 
  write_data(0x17);

  write_command(0xe1); 
  write_data(0xD0); 
  write_data(0x00); 
  write_data(0x02); 
  write_data(0x07); 
  write_data(0x0A); 
  write_data(0x28); 
  write_data(0x31); 
  write_data(0x54); 
  write_data(0x47); 
  write_data(0x0E); 
  write_data(0x1C); 
  write_data(0x17); 
  write_data(0x1B); 
  write_data(0x1E); 

  write_command(0x3a); 
  write_data(0x55);

  write_command(0x2A); //Frame rate control
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0xEF);

  write_command(0x2B); //Display function control
  write_data(0x00);
  write_data(0x00);
  write_data(0x01);
  write_data(0x3F);
  //**********************
  write_command(0x11); 
  delay_ms(120);      //Delay 120ms 
  write_command(0x29); //display on
  delay_ms(25);
  write_command(0x2c); 
#endif
  
  st7789_FillColor(0, 0, screen_dev.wide-1, screen_dev.high-1, 0x0000);
  return 0;
}


/********************************************************************************************/
uint8_t st7789_DirectionSet(uint8_t dir)
{
  if(dir>1) return 1;
  write_command(0x36);
  if(dir)
  {
    write_data(0xA0);
    screen_dev.wide = LCD_HIGH;
    screen_dev.high = LCD_WIDE;
  }
  else
  {
    write_data(0x00);
    screen_dev.wide = LCD_WIDE;
    screen_dev.high = LCD_HIGH;
  }
  screen_dev.dir = dir;
  return 0;
}


/*********************************************************************************************
* 名称：st7789_prepare_write()
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void st7789_PrepareWrite()
{
  write_command(screen_dev.wramcmd);
}


/********************************************************************************************/
void st7789_SetCursorPos(short x,short y)
{
  write_command(screen_dev.setxcmd);                                          //Frame rate control
  write_data(x>>8);
  write_data(x&0xff);  
  write_command(screen_dev.setycmd);                                          //Display function control
  write_data(y>>8);
  write_data(y&0xff);
}

/*********************************************************************************************
* 名称：st7789_set_activie_window()
* 功能：
* 参数：x1 -- 
*       y1 --
*       x2 --
*       y2 --
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void st7789_SetWindow(int x1, int y1, int x2, int y2)
{
  write_command(0x2A);                                          //Frame rate control
  write_data(x1>>8);
  write_data(x1);  
  write_data(x2>>8);
  write_data(x2);
  write_command(0x2B);                                          //Display function control
  write_data(y1>>8);
  write_data(y1);
  write_data(y2>>8);
  write_data(y2);
}


/********************************************************************************************/
void st7789_PrepareFill(short x1, short y1, short x2, short y2)
{
  st7789_SetWindow(x1,y1,x2,y2);
  st7789_PrepareWrite();
}


/********************************************************************************************/
void st7789_DrawPoint(short x,short y,uint32_t color)
{
#if 0
  st7789_SetCursorPos(x,y);
  st7789_PrepareWrite();		//开始写入GRAM
  write_data(color);
#else
  write_command(0x2A);                                          //Frame rate control
  write_data(x>>8);
  write_data(x&0xff);  
  write_command(0x2B);                                          //Display function control
  write_data(y>>8);
  write_data(y&0xff);
  
  write_command(0x2C);
  
  write_data(color);
#endif
}

/********************************************************************************************/
uint32_t LCD_ReadPoint(short x,short y)
{
  uint32_t r=0,g=0,b=0;
  st7789_SetCursorPos(x,y);
  write_command(screen_dev.rramcmd);
  r = read_data();//dummy Read
  r = read_data();//RG
  b = read_data();//BR
  g = r&0X00FF;
  r = r&0XFF00;
  return (r|((g>>2)<<5)|(b>>11));
}

/*********************************************************************************************
* 名称：st7789_FillColor
* 功能：
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void st7789_FillColor(short x1,short y1,short x2,short y2,uint16_t color)
{
  uint16_t x, y;
  st7789_SetWindow(x1,y1,x2,y2);
  st7789_PrepareWrite();
  for (y=y1; y<=y2; y++)
  {
    for (x=x1; x<=x2; x++)
    {
      write_data(color);
    }
  }
}

/*********************************************************************************************
* 名称：st7789_FillData
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void st7789_FillData(short x1,short y1,short x2,short y2,unsigned short* dat)
{
  short x, y;
  st7789_SetWindow(x1,y1,x2,y2);
  st7789_PrepareWrite();
  for (y=y1; y<=y2; y++)
  {
    for (x=x1; x<=x2; x++)
    {
      write_data(*dat);
      dat++;
    }
  }
}
