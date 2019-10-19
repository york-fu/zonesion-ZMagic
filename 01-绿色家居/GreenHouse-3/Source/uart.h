#ifndef __UART_H__
#define __UART_H__

void uart_init(double baud);
void uart_write(char ch);
void uart_write_buf(char *buf, uint8 len);
void uart_set_input_call(void (*c)(char ch));
#endif
