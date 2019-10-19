#include "stm32f4xx.h"
#include <stdio.h>
#include "usart.h"

#define BUF_SIZE 256


static char _recv_buf[BUF_SIZE];
static int head=0, real = 0;
int mg2639_put_ch(char ch);

void mg2639_write(char* cmd, int len)
{
  int i;
  for (i=0; i<len; i++) {
    uart1_putc(cmd[i]);
  }
}

void mg2639_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);         //使能GPIOA时钟  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;            //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
  
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
  uart1_init(115200);
  uart1_set_input(mg2639_put_ch);
}

int mg2639_get_ch(void)
{
  char ch;
  if (head == real) return -1;
  ch = _recv_buf[real];
  real = (real + 1) % BUF_SIZE;
  return ch;
}

int mg2639_put_ch(char ch)
{
  int n = (head + 1) % BUF_SIZE;
  if (n != real) {
    _recv_buf[head] = ch;
    head = n;
  } else {
    printf(" error : mg2639 recv buffer over!\r\n");
  }
  return 0;
}