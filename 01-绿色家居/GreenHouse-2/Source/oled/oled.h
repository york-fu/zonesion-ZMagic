#ifndef OLED_H
#define OLED_H
#include <ioCC2530.h>

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
