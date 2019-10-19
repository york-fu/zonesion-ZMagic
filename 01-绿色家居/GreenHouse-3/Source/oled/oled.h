#ifndef OLED_H
#define OLED_H
#include <ioCC2530.h>

#define ADDR_W  0X78                                            //主机写地址
#define ADDR_R  0X79                                            //主机读地址

#define OLED_HEIGHT (32/8)
#define OLED_WIDTH  96

#define WHITE 0
#define BLACK 1

extern unsigned char OLED_PEN_COLOR; 

void OLED_Init(void);
void OLED_Write_command(unsigned char IIC_Command);
void OLED_IIC_write(unsigned char IIC_Data);
void OLED_Display_Off(void);
void OLED_Display_On(void);
void OLED_Clear(void);
void oled_areaClear(int Hstart,int Hend,int Lstart,int Lend);
void OLED_Fill(unsigned char data); 
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size);
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char Char_Size);
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);

#endif
