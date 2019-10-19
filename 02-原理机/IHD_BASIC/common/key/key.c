/*********************************************************************************************
* 文件：key.c
* 作者：fuyou
* 说明：
* 功能：按键处理进程
* 修改：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "key.h"
#include "drive_key.h"

PROCESS_NAME(lcd);

process_event_t key_event;

PROCESS(key, "key");

/*********************************************************************************************
* 名称：PROCESS_THREAD(key, ev, data)
* 功能：按键处理进程
* 参数：key, ev, data
* 返回：无
* 修改：无
*********************************************************************************************/
PROCESS_THREAD(key, ev, data)
{
    PROCESS_BEGIN();
    
    static struct etimer key_timer;
    static char keyStatus=0,keyFlag=1;
    
    key_init();
    key_event = process_alloc_event();
    
    while (1)
    {
        keyStatus = get_key_status();
        if(keyStatus==0)
        {
            keyFlag=1;
        }
        else if(keyFlag)
        {
            keyFlag=0;
            process_post(NULL, key_event, &keyStatus);
        }
        etimer_set(&key_timer,50);
        PROCESS_WAIT_EVENT_UNTIL(ev==PROCESS_EVENT_TIMER);  
    }
    
    PROCESS_END();
}