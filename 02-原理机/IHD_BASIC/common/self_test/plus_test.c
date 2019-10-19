#include "plus_test.h"

uint8_t testStatus=0;

uint8_t testFlag_key1 = 0;
uint8_t testFlag_key2 = 0;
uint8_t testFlag_key3 = 0;
uint8_t testFlag_key4 = 0;
uint8_t testFlag_uart1 = 0;
uint8_t testFlag_flash = 0;
uint8_t testFlag_ethernet = 0;

//flash
uint16_t flash_startAddr = 4096;                                //起始位置
uint8_t flash_write_buff[] = {"Hello World!"};
uint8_t flash_read_buff[16];

void StartProcesslist(void);

uint8_t test_getStatus(void)
{
    return testStatus;
}

//ethernet
void test_ethPing()
{
    uip_ipaddr_t ipaddr;
    uip_ipaddr(&ipaddr, 192,168,0,1);
    sendPing(&ipaddr);
    
    uip_ipaddr(&ipaddr, 192,168,1,1);
    sendPing(&ipaddr);
}

//test led process
PROCESS(test_led, "test_led");

PROCESS_THREAD(test_led, ev, data)
{
    PROCESS_BEGIN();
    
    static struct etimer etimer_testLed;
    static unsigned char i = 0x08;
    
    led_init();
    etimer_set(&etimer_testLed, 1000);
    
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&etimer_testLed, 1000);
            i <<= 1;
            if(i == 0x80)
                i = 0x10;
              turn_off(0x70);
              turn_on(i);
              LED1 = !LED1;
              LED2 = !LED2;
              LED3 = !LED3;
              LED4 = !LED4;
        }
    }
    
    PROCESS_END();
}

PROCESS(test_plus, "test_plus");

PROCESS_THREAD(test_plus, ev, data)
{
    PROCESS_BEGIN();
    static struct etimer etime_test;
    static uint8_t test_relay=0;
    static uint16_t test_tick=0;
    
    process_start(&test_led,NULL);
    process_start(&test_ui,NULL);
    process_start(&rfUart_process,NULL);
    process_start(&ethernet_process,NULL);
    
    W25QXX_Init();
    W25QXX_Write(flash_write_buff,flash_startAddr,strlen((char const *)flash_write_buff));// 写入数据
    etimer_set(&etime_test, 1000);
    
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if(ev == key_event)
        {
            if(*((uint8_t*)data) == 0x01)
            {
                if(testFlag_key1)
                {
                }
                else
                    testFlag_key1=1;
            }
            else if(*((uint8_t*)data) == 0x02)
            {
                if(testFlag_key2)
                {
                    testFlag_key1=0;
                    testFlag_key3=0;
                    testFlag_key4=0;
                    testFlag_flash=0;
                    testFlag_ethernet=0;
                    test_tick=0;
                    test_ethPing();
                }
                else
                    testFlag_key2=1;
            }
            else if(*((uint8_t*)data) == 0x04)
            {
                if(testFlag_key3)
                {
                    if(test_relay&0x01)
                    {
                        test_relay &= ~(0x01);
                        relay_off(1);
                    }
                    else
                    {
                        test_relay |= 0x01;
                        relay_on(1);
                    }
                }
                else
                    testFlag_key3 = 1;
            }
            else if(*((uint8_t*)data) == 0x08)
            {
                if(testFlag_key4)
                {
                    if(test_relay&0x02)
                    {
                        test_relay &= ~(0x02);
                        relay_off(2);
                    }
                    else
                    {
                        test_relay |= 0x02;
                        relay_on(2);
                    }
                }
                else
                    testFlag_key4 = 1;
            }
        }
        if (ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&etime_test, 1000);
            if(test_tick<1000) test_tick++;
                
            if(!(testFlag_flash&0x03))
            {
                if(memcmp(flash_read_buff, "Hello World!", 12) == 0)
                    testFlag_flash = 0x01;
                else
                    W25QXX_Read(flash_read_buff,flash_startAddr,strlen((char const *)flash_write_buff));
                if(test_tick==5)
                    testFlag_flash = 0x02;
            }
            if(!(testFlag_uart1&0x03))
            {
                if(RF1_hwTypeGet()!=0)
                    testFlag_uart1 = 0x01;
                if(test_tick==5)
                    testFlag_uart1 = 0x02;
            }
            if(!(testFlag_ethernet&0x03))
            {
                if(!(testFlag_ethernet&0x01))
                    test_ethPing();
                if(test_tick==10)
                    testFlag_ethernet = 0x02;
            }
        }
    }
    PROCESS_END();
}


//test init process
PROCESS(test_init, "test_init");

PROCESS_THREAD(test_init, ev, data)
{
    PROCESS_BEGIN();
    
    static struct etimer etimer_testInit;
    
    process_start(&key,NULL);
    etimer_set(&etimer_testInit, 100);
        
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if(ev==key_event)
        {
            if(*((uint8_t*)data)==0x01)     //K1按键按下进入测试模式
            {
                testStatus=1;
                process_start(&test_plus,NULL);
                PROCESS_EXIT();
            }
        }
        if (ev==PROCESS_EVENT_TIMER)
        {
            if(!testStatus)                 //K1没有按下进入正常模式
            {
                StartProcesslist();
                PROCESS_EXIT();
            }
        }
    }
    
    PROCESS_END();
}

