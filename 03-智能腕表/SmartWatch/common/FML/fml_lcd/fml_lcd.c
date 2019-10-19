#include "fml_lcd/fml_lcd.h"

#if LCD_FONT_EN
#include "font.h"
#include "gbk_st16.h"
#endif


lcd_dev_t lcd_dev = {
  &screen_dev,
  &TouchDev,
};

lcd_phy_t LCD = {
  .PrepareFill = st7789_PrepareFill,
  .DrawPoint = st7789_DrawPoint,
  .ReadPoint = LCD_ReadPoint,
  .FillRectangle = st7789_FillColor,
  .FillData = st7789_FillData,
  .TouchScan = ft6x36_TouchScan,
};


uint8_t LCD_DriverInit()
{
  int ret = 0;
  ret = st7789_init();
  if(ret!=0)
    return ret;
  ret = ft6x36_init();
  if(ret!=0)
    return ret;
  return 0;
}


/*==========================================================================================*/


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
    LCD.DrawPoint(uRow,uCol,color);//画点
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
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color)
{
  int a,b;
  int di;
  a=0;
  b=r;
  di=3-(r<<1);             //判断下个点位置的标志
  while(a<=b)
  {
    LCD.DrawPoint(x0+a,y0-b,color);             //5
    LCD.DrawPoint(x0+b,y0-a,color);             //0
    LCD.DrawPoint(x0+b,y0+a,color);             //4
    LCD.DrawPoint(x0+a,y0+b,color);             //6
    LCD.DrawPoint(x0-a,y0+b,color);             //1
    LCD.DrawPoint(x0-b,y0+a,color);
    LCD.DrawPoint(x0-a,y0-b,color);             //2
    LCD.DrawPoint(x0-b,y0-a,color);             //7
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


#if LCD_FONT_EN

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
      if(temp&0x80)LCD.DrawPoint(x,y,color);
      else if(mode==0)LCD.DrawPoint(x,y,backColor);
      temp<<=1;
      y++;
      if(y>=LCD_HEIGHT_MAX)return;		//超区域了
      if((y-y0)==size)
      {
        y=y0;
        x++;
        if(x>=LCD_WIDTH_MAX)return;	//超区域了
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
* 名称：LCD_ShowFont16
* 功能：16*16汉字字符显示（文件字库版）
* 参数：x，y：显示坐标 *data：字符串名Color：汉字颜色 BColor：背景颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_ShowFont16(u16 x,u16 y,u8 *data,u16 Color,u16 BColor)
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
          LCD.DrawPoint(x+j,y+i,Color);
        }
        else
        {
          LCD.DrawPoint(x+j,y+i,BColor);
        }
      }
      for(j=0; j<8; j++)
      {
        tmp_char=gbk_st16[index*32+i*2+1];
        if ( (tmp_char << j) & 0x80)
        {
          LCD.DrawPoint(x+j+8,y+i,Color);
        }
        else
        {
          LCD.DrawPoint(x+j+8,y+i,BColor);
        }
      }
    }
  }
}


/*********************************************************************************************
* 名称：LCD_ShowFontString16
* 功能：lcd显示16x16汉字（文件字库版）
* 参数：x1,y1:起始坐标值，color：字体颜色，Bcolor：背景颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_ShowFontString(u16 x,u16 y,char* text,u16 textSize,u16 showLen,u16 textColor,u16 backColor)
{
  u8* temp=(u8*)text;
  u16 x1=x;
  
  while(x1<(x+showLen))
  {
    if(*temp!='\0')
    {
      if (*temp & 0x80)
      {
        LCD_ShowFont16(x1,y,temp,textColor,backColor);
        temp+=2;
        x1+=textSize;
      }
      else
      {
        LCD_ShowChar(x1,y,*temp,16,0,textColor,backColor);
        temp ++;
        x1 += (textSize/2);
      }
    }
    else
    {
      LCD_ShowChar(x1,y,' ',16,0,textColor,backColor);
      x1 += 8;
    }
  }
}

#endif

