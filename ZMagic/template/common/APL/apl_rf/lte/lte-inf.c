#include "lte-inf.h"

/*gsm底层接口函数*/

#define RECV_BUF_SIZE   1024  //contiki ringbuf 原版最大不能超过256，现在修改为最大0xffff
static struct ringbuf _recv_ring;

int gsm_recv_ch(char ch)
{
  return ringbuf_put(&_recv_ring, ch);
}

void gsm_inf_init(void)
{
  static uint8_t _recv[RECV_BUF_SIZE];
  
  //uart4_init(115200);
  //uart4_set_input(gsm_recv_ch);
  ringbuf_init(&_recv_ring, _recv, (uint8_t)RECV_BUF_SIZE);
}

void gsm_uart_write(char *buf, int len)
{
  for (int i=0; i<len; i++) {
    uart2_putc(buf[i]);
  }
}

int gsm_uart_read_ch(void)
{
  return ringbuf_get(&_recv_ring);
}