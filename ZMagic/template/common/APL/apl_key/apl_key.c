/*********************************************************************************************
* 文件：fml_Key.c
* 作者：fuyou
* 说明：按键的应用文件
* 功能：提供按键检测功能,广播按键事件
* 修改：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "apl_key.h"

process_event_t key_event;//按键按下
process_event_t keyUp_event;//按键释放

PROCESS(KeyProcess, "KeyProcess");

/*********************************************************************************************
* 名称：PROCESS_THREAD(KeyProcess, ev, data)
* 功能：按键处理进程
* 参数：KeyProcess, ev, data
* 返回：无
* 修改：无
*********************************************************************************************/
PROCESS_THREAD(KeyProcess, ev, data)
{
  PROCESS_BEGIN();
  
  static struct etimer Key_timer;
  static char KeyStatus=0,KeyFlag=1;
  
  key_GpioInit();
  key_event = process_alloc_event();
  keyUp_event = process_alloc_event();
  
  while (1)
  {
    KeyStatus = key_GetStatus();
    if(KeyStatus==0)
    {
      KeyFlag&=~0x7f;
      if(KeyFlag&0x80)
      {
        KeyFlag&=~0x80;
        process_post(PROCESS_BROADCAST, keyUp_event, &KeyStatus);
      }
    }
    else
    {
      KeyFlag|=0x80;
      if((KeyFlag&0x7f)%4==0)
      {
        KeyFlag&=~0x7f;
        process_post(PROCESS_BROADCAST, key_event, &KeyStatus);
      }
      KeyFlag++;
      if((KeyFlag&0x7f)>39) KeyFlag&=~0x7f;
    }
    etimer_set(&Key_timer,50);
    PROCESS_WAIT_EVENT_UNTIL(ev==PROCESS_EVENT_TIMER);  
  }
  
  PROCESS_END();
}

