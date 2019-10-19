#include <stdio.h>
#include <contiki.h>
#include <contiki-net.h>
#include <dev/leds.h>
#include "usart.h"
#include "adc.h"
#include "plus_test.h"

unsigned int idle_count = 0;

void debug_init(void (*f)(char));
int debug_input(char c);


 void main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  clock_init();
  Bsp_ADCInit();
  uart1_init(115200);
  debug_init(uart1_putc);
  uart1_set_input(debug_input);
  
  printf("\r\nStarting ");
  printf(CONTIKI_VERSION_STRING);
  printf(" on STM32F40x\r\n");
  
  process_init();
  ctimer_init();
  process_start(&etimer_process, NULL);
  //serial_shell_init();
  process_start(&test_init, NULL);
  
  while(1)
  {
    do
    {
    }
    while(process_run() > 0);
    idle_count++;
    /* Idle! */
    /* Stop processor clock */
    /* asm("wfi"::); */
  }
}