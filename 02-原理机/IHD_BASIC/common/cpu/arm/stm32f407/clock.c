
#include <sys/clock.h>
#include <sys/cc.h>
#include <sys/etimer.h>
//#include <debug-uart.h>
#include <misc.h>

static unsigned int COUNT_1US;
static volatile clock_time_t current_clock = 0;
static volatile unsigned long current_seconds = 0;
static unsigned int second_countdown = CLOCK_SECOND;

void SysTick_Handler(void);

void SysTick_Handler(void)
{
    clock_time_t nt = etimer_next_expiration_time();

    current_clock++;
    if( nt &&  ((int)current_clock-(int)nt)>0 )
    {
        etimer_request_poll();
        /* printf("%d,%d\n", clock_time(),etimer_next_expiration_time  	()); */

    }
    if (--second_countdown == 0)
    {
        current_seconds++;
        second_countdown = CLOCK_SECOND;
    }
}


void clock_init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

    RCC_ClocksTypeDef RCC_Clocks;    //时钟结构体
    RCC_GetClocksFreq(&RCC_Clocks);  //获取系统时钟的主时钟频率
    SysTick_Config(RCC_Clocks.HCLK_Frequency/CLOCK_SECOND);    //配置操作系统时钟分频
    COUNT_1US = RCC_Clocks.HCLK_Frequency/ 1000 / 1000;    //设置微秒的时间节拍
}

clock_time_t clock_time(void)
{
    return current_clock;
}

void clock_delay_us(unsigned int us)
{

    unsigned int c_x_us = us * COUNT_1US;
    unsigned int v2, v = SysTick->VAL;
    unsigned int e;
    while (c_x_us != 0)
    {
        while (v == SysTick->VAL);
        v2 = SysTick->VAL;
        if (v2 < v)
        {
            e = v - v2;
        }
        else
        {
            e = v + (COUNT_1US - v2);
        }
        if (c_x_us >= e) c_x_us -= e;
        else c_x_us  = 0;
        v = v2;
    }
}

void clock_delay_ms(unsigned int ms)
{
    while(ms--)
    {
        clock_delay_us(1000);
    }
}

#if 0
/* The inner loop takes 4 cycles. The outer 5+SPIN_COUNT*4. */

#define SPIN_TIME 2 /* us */
#define SPIN_COUNT (((MCK*SPIN_TIME/1000000)-5)/4)

#ifndef __MAKING_DEPS__

void
clock_delay(unsigned int t)
{
#ifdef __THUMBEL__
    asm volatile("1: mov r1,%2\n2:\tsub r1,#1\n\tbne 2b\n\tsub %0,#1\n\tbne 1b\n":"=l"(t):"0"(t),"l"(SPIN_COUNT));
#else
#error Must be compiled in thumb mode
#endif
}
#endif /* __MAKING_DEPS__ */
#endif

unsigned long
clock_seconds(void)
{
    return current_seconds;
}
