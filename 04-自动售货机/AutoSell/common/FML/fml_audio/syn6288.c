/*********************************************************************************************
* 文件：syn6288.c
* 作者：
* 说明：syn6288驱动程序
* 修改：
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "syn6288.h"
/*********************************************************************************************
* 名称：uart_send_char()
* 功能：串口发送字节函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void uart_send_char(unsigned char ch)
{
  uart6_putc(ch);
}

void uart_sendString(unsigned char *data, unsigned char strLen)
{
  for(unsigned char i=0; i<strLen; i++)
  {
    uart_send_char(data[i]);
  }
}

/*********************************************************************************************
* 名称：syn6288_init()
* 功能：syn6288初始化
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void syn6288_init()
{
//  GPIO_InitTypeDef  GPIO_InitStructure;
 	
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	 //使能PA端口时钟
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  uart6_init(9600); 
}

int syn6288_busy(void)
{ 
//  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))                                                      //忙检测引脚
    return 0;                                                   //没有检测到信号返回 0
//  else
//    return 1;                                                   //检测到信号返回 1
}

/*********************************************************************************************
* 名称：syn6288_play()
* 功能：
* 参数：s -- 数组名
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void syn6288_play(char *s)
{
  int i;
  int len = strlen(s);
  unsigned char c = 0;  
  unsigned char head[] = {0xFD,0x00,0x00,0x01,0x00};            //数据包头  
  head[1] = (len + 3) >> 8;
  head[2] = (len + 3) & 0xff;
  for (i=0; i<5; i++){
    uart_send_char(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++){
    uart_send_char(s[i]);
    c ^= s[i];
  }
  uart_send_char(c);
}

/*******************************************************************************
* 名称: hex2unicode()
* 功能: 将16进制unicode字符串转换成bin格式
* 参数: 
* 返回: 
* 修改:
* 注释: 
*******************************************************************************/
char *hex2unicode(char *str)
{       
  static char uni[64];
  int n = strlen(str)/2;
  if (n > 64) n = 64;
  
  for (int i=0; i<n; i++) {
    unsigned int x = 0;
    for (int j=0; j<2; j++) {
      char c = str[2*i+j];
      char o;
      if (c>='0' && c<='9') o = c - '0';
      else if (c>='A' && c<='F') o = 10+(c-'A');
      else if (c>='a' && c<='f') o = 10+(c-'a');
      else o = 0;
      x = (x<<4) | (o&0x0f);
    }
    uni[i] = x;
  }
  uni[n] = 0;
  return uni;
}

/*******************************************************************************
* syn6288_play_unicode()
* 功能：
* 参数：s -- 数组名
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_play_unicode(uint16_t *s, int len)
{
  int i;
  char c = 0;  
  unsigned char head[] = {0xFD,0x00,0x00,0x01,0x03};            //数据包头 
  
  head[1] = (len*2 + 3) >> 8;
  head[2] = (len*2 + 3) & 0xff;
  for (i=0; i<5; i++)
  {
    uart_send_char(head[i]);
    c ^= head[i];
  }
  for (i=0; i<len; i++)
  {
    uart_send_char(s[i]>>8);
    c ^= s[i]>>8;
    uart_send_char(s[i]&0xff);
    c ^= s[i]&0xff;
  }
  uart_send_char(c);
}

/*******************************************************************************
* 名称：syn6288_stop()
* 功能：停止合成
* 参数：
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_stop(void)
{
  unsigned char orderBuf[5] = {0xFD, 0x00, 0x02, 0x02, 0xFD};
  uart_sendString(orderBuf, 5);
}

/*******************************************************************************
* 名称：syn6288_suspend()
* 功能：暂停合成
* 参数：
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_suspend(void)
{
  unsigned char orderBuf[5] = {0xFD, 0x00, 0x02, 0x03, 0xFC};
  uart_sendString(orderBuf, 5);
}

/*******************************************************************************
* 名称：syn6288_suspend()
* 功能：恢复合成
* 参数：
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_continue(void)
{
  unsigned char orderBuf[5] = {0xFD, 0x00, 0x02, 0x04, 0xFD};
  uart_sendString(orderBuf, 5);
}

/*******************************************************************************
* 名称：syn6288_playMusic()
* 功能：合成语音含背景音乐
* 参数：musicNum-背景音乐(0-15) 合成字符串-str
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_playMusic(unsigned char musicNum, char *str)
{
  unsigned char xorChar = 0;
  int len = strlen(str);
  if(musicNum >= 15)
    musicNum = 15;
  musicNum <<= 3;
  char orderBuf[] = {0xFD, 0x00, 0x00, 0x01, musicNum};
  orderBuf[1] = (len + 3) >> 8;
  orderBuf[2] = (len + 3) & 0xFF;
  for(unsigned char i=0; i<5; i++){
    uart_send_char(orderBuf[i]);
    xorChar ^= orderBuf[i];
  }
  for(unsigned char i=0; i<len; i++)
  {
    uart_send_char(str[i]);
    xorChar ^= str[i];
  }
  uart_send_char(xorChar);
}

/*******************************************************************************
* 名称：syn6288_playHint()
* 功能：播放提示音
* 参数：hintNum-提示音编号(0-25)
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_playHint(unsigned char hintNum)
{
  unsigned char xorChar = 0;
  unsigned char orderBuf[5] = {0xFD, 0x00, 0x0D, 0x01, 0x01};
  if(hintNum >= 25)
    hintNum = 25; 
  unsigned char strBuf[10] = {0x5B, 0x78, 0x31, 0x5D, 0x73, 0x6F, 0x75, 0x6e, 0x64, hintNum+0x60};
  for(unsigned char i=0; i<5; i++)
  {
    uart_send_char(orderBuf[i]);
    xorChar ^= orderBuf[i];
  }
  for(unsigned char i=0; i<10; i++)
  {
    uart_send_char(strBuf[i]);
    xorChar ^= strBuf[i];
  }
  uart_send_char(xorChar);
}

/*******************************************************************************
* 名称：syn6288_setVolume()
* 功能：音量调节
* 参数：vValue-合成音量 mValue-背景音乐音量
* 返回：
* 修改：
* 注释：
*******************************************************************************/
void syn6288_setVolume(unsigned char vValue, unsigned char mValue)
{
  unsigned char xorChar = 0;
  unsigned char orderBuf[5] = {0xFD, 0x00, 0x0D, 0x01, 0x01};
  if(vValue >= 16)
    vValue = 16;
  if(mValue >= 16)
    mValue = 16;
  unsigned char configBuf[10] = {0x5B, 0x76, vValue/10+0x30, vValue%10+0x30, 
                                  0x5D, 0x5B, 0x6D, mValue/10+0x30, mValue%10+0x30, 0x5D};
  for(unsigned char i=0; i<5; i++)
  {
    uart_send_char(orderBuf[i]);
    xorChar ^= orderBuf[i];
  }
  for(unsigned char i=0; i<10; i++)
  {
    uart_send_char(configBuf[i]);
    xorChar ^= configBuf[i];
  }
  uart_send_char(xorChar);
}















