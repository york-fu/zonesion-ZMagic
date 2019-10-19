#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
//#include "stm32f4xx_conf.h"

void uart1_init(unsigned int bpr);
void uart1_putc( char x);
void uart1_set_input(int (*fun)( char ) );

void uart2_init(unsigned int bpr);
void uart2_putc( char x);
void uart2_set_input(int (*fun)( char ) );

void uart3_init(unsigned int bpr);
void uart3_putc( char x);
void  uart3_set_input(int (*fun)( char ) );

void uart4_init(unsigned int bpr);
void uart4_putc( char x);
void  uart4_set_input(int (*fun)( char ) );

void uart5_init(unsigned int bpr);
void uart5_putc( char x);
void uart5_set_input(int(*fun)( char ));

void uart6_init(unsigned int bound);
void uart6_set_input(int(*fun)( char));
void uart6_putc( char x);

#endif


