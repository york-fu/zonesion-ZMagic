/*********************************************************************************************
* 文件：oled.c
* 作者：zonesion
* 说明：
* 修改：Chenkm 2017.01.05 增加了注释
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "iic.h"
#include "oled.h"
#include "oledfont.h"

unsigned char OLED_GRAM[96][4];

unsigned char OLED_PEN_COLOR=WHITE;


/*********************************************************************************************
* 名称: OLED_Init()
* 功能: OLED初始化
* 参数: 无
* 返回: 无
* 注释：
* 修改:
*********************************************************************************************/
void  OLED_Init(void)
{
    iic_init();
    OLED_Write_command(0xAE); /*display off*/
    OLED_Write_command(0x00); /*set lower column address*/
    OLED_Write_command(0x10); /*set higher column address*/
    OLED_Write_command(0x40); /*set display start line*/
    OLED_Write_command(0xb0); /*set page address*/
    OLED_Write_command(0x81); /*contract control*/
    OLED_Write_command(0x45); /*128*/
    OLED_Write_command(0xA1); /*set segment remap*/
    OLED_Write_command(0xC0);/*Com scan direction 0XC0 */
    OLED_Write_command(0xA6); /*normal / reverse*/
    OLED_Write_command(0xA8); /*multiplex ratio*/
    OLED_Write_command(0x1F); /*duty = 1/32*/
    OLED_Write_command(0xD3); /*set display offset*/
    OLED_Write_command(0x00);
    OLED_Write_command(0xD5); /*set osc division*/
    OLED_Write_command(0x80);
    OLED_Write_command(0xD9); /*set pre-charge period*/
    OLED_Write_command(0x22);
    OLED_Write_command(0xDA); /*set COM pins*/
    OLED_Write_command(0x12);
    OLED_Write_command(0xdb); /*set vcomh*/
    OLED_Write_command(0x20);
    OLED_Write_command(0x8d); /*set charge pump enable*/
    OLED_Write_command(0x14);
    OLED_Write_command(0xAF); /*display ON*/

    OLED_Clear();
}


/*********************************************************************************************
* 名称: OLED_Write_command()
* 功能: IIC Write Command
* 参数: 命令
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Write_command(unsigned char IIC_Command)
{
    iic_start();						        //启动总线
    iic_write_byte(ADDR_W);		                        //地址设置
    iic_write_byte(0x00);			                        //命令输入
    iic_write_byte(IIC_Command);	                                //等待数据传输完成
    iic_stop();
}

/*********************************************************************************************
* 名称: OLED_IIC_write()
* 功能: IIC Write Data
* 参数: 数据
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void OLED_IIC_write(unsigned char IIC_Data)
{
    if(OLED_PEN_COLOR==BLACK)
    {
        IIC_Data = ~IIC_Data;
    }
    iic_start();                                                //启动总线
    iic_write_byte(ADDR_W);                                     //地址设置
    iic_write_byte(0x40);                                       //命令输入
    iic_write_byte(IIC_Data);	                                //等待数据传输完成
    iic_stop();
}

/*********************************************************************************************
* 名称: OLED_Display_On()
* 功能: 开启OLED显示
* 参数:
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Display_On(void)
{
    OLED_Write_command(0X8D);                                     //SET DCDC命令
    OLED_Write_command(0X14);                                     //DCDC ON
    OLED_Write_command(0XAF);                                     //DISPLAY ON
}

/*********************************************************************************************
* 名称: OLED_Display_Off()
* 功能: 关闭OLED显示
* 参数:
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Display_Off(void)
{
    OLED_Write_command(0X8D);                                     //SET DCDC命令
    OLED_Write_command(0X10);                                     //DCDC OFF
    OLED_Write_command(0XAE);                                     //DISPLAY OFF
}


/*********************************************************************************************
* 名称: OLED_Set_Pos()
* 功能: 坐标设置
* 参数: 坐标
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_Write_command(0xb0+y);
    OLED_Write_command(((x&0xf0)>>4)|0x10);
    OLED_Write_command((x&0x0f));
}


/*********************************************************************************************
* 名称: OLED_Clear()
* 功能: 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
* 参数:
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Clear(void)
{
    unsigned char i,n;

    for(i=0; i<OLED_HEIGHT; i++)
    {
        OLED_Write_command (0xb0+i);                                //设置页地址（0~7）
        OLED_Write_command (0x00);                                  //设置显示位置—列低地址
        OLED_Write_command (0x10);                                  //设置显示位置—列高地址
        for(n=0; n<OLED_WIDTH; n++)
            OLED_IIC_write(0);
    }
}


/*********************************************************************************************
* 名称: oled_areaClear()
* 功能: 区域清空
* 参数: 坐标
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void oled_areaClear(int Hstart,int Hend,int Lstart,int Lend)
{
    unsigned int x,y;

    for(y=Hstart; y<=Hend; y++)
    {
        OLED_Set_Pos(Lstart,y);
        for(x=Lstart; x<=Lend; x++)
        {
            OLED_IIC_write(0);
        }
    }
}

/*********************************************************************************************
* 名称: OLED_Fill
* 功能: 填充函数
* 参数: data：填充的数据
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Fill(unsigned char data)
{
    unsigned char x,y;
    for(y=0; y<OLED_HEIGHT; y++)
    {
        OLED_Set_Pos(x,y+1);
        for(x=0; x<OLED_WIDTH; x++)
            OLED_IIC_write(data);
    }
}


/*********************************************************************************************
* 名称: OLED_ShowChar()
* 功能: 在指定位置显示一个字符,包括部分字符
* 参数: 坐标（x:0~95；y:0~3），chr字符；Char_Size字符大小
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size)
{
    unsigned char c=0,i=0;

    c=chr-' ';//得到偏移后的值

    if(Char_Size ==16)
    {
        OLED_Set_Pos(x,y);
        for(i=0; i<8; i++)
            OLED_IIC_write(F8X16[c*16+i]);
        OLED_Set_Pos(x,y+1);
        for(i=0; i<8; i++)
            OLED_IIC_write(F8X16[c*16+i+8]);
    }
    else
    {
        OLED_Set_Pos(x,y);
        for(i=0; i<6; i++)
            OLED_IIC_write(F6x8[c][i]);
    }
}

/*********************************************************************************************
* 名称: OLED_ShowString()
* 功能: 显示一个字符串
* 参数: 起始坐标（x:0~95；y:0~3），chr字符串指针；Char_Size字体大小
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char Char_Size)
{
    unsigned char i=0;

    while(chr[i]!='\0')
    {
        OLED_ShowChar(x,y,chr[i],Char_Size);
        i++;
        if(Char_Size==16)
        {
            x+=8;
            if(x>OLED_WIDTH-1)
            {
                x=0;
                y+=2;
            }
        }
        else
        {
            x+=6;
            if(x>OLED_WIDTH-1)
            {
                x=0;
                y+=1;
            }
        }
    }
}

/*********************************************************************************************
* 名称: OLED_ShowCHinese()
* 功能: 显示一个汉字
* 参数: 起始坐标（x:0~127；y:0~63），num汉字在自定义字库中的编号（oledfont.h）编号
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char num)
{
    unsigned char t,adder=0;
    OLED_Set_Pos(x,y);
    for(t=0; t<12; t++)
    {
        OLED_IIC_write(Hzk[2*num][t]);
        adder+=1;
    }
    OLED_Set_Pos(x,y+1);
    for(t=0; t<12; t++)
    {
        OLED_IIC_write(Hzk[2*num+1][t]);
        adder+=1;
    }
}




//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)
{
    unsigned char pos,bx,temp=0;

    if(x>95||y>31)return;//超出范围了.
    pos=7-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)
        OLED_GRAM[x][pos]|=temp;
    else
        OLED_GRAM[x][pos]&=~temp;
}


//更新显存到LCD
void OLED_Refresh_Gram(void)
{
    unsigned char i,n;
    for(i=0; i<4; i++)
    {
        OLED_Write_command (0xb0+i);                                //设置页地址（0~3）
        OLED_Write_command (0x00);                                  //设置显示位置—列低地址
        OLED_Write_command (0x10);                                  //设置显示位置—列高地址
        for(n=0; n<96; n++)
            OLED_IIC_write(OLED_GRAM[n][i]);
    }
}

