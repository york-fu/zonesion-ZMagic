/*********************************************************************************************
* 文件：hw.c
* 作者：fuyou 2018.12.20
* 说明：原理机自动识别无线类型驱动
* 修改：
* 注释：
*********************************************************************************************/
#include "hw.h"
#include "lte_zhiyun.h"
#include "lte-ec20.h"
#include "apl_rf/ble/ble-net.h"
#include "apl_rf/at/at.h"

//无线类型标识
static uint8_t RF1_hwType=0;
static uint8_t RF2_hwType=0;

//获取无线类型
uint8_t RF1_hwTypeGet(void)
{
  return RF1_hwType;
}


uint8_t RF2_hwTypeGet(void)
{
  return RF2_hwType;
}

//打印调试信息
#define INFO_PREFIX       "[ "
#define INFO_POSTFIX      " ]"

void RF1_StartHandleProcess(char* name)
{
  char nameList[][8]={"CC2540","New type"};
  
  if(memcmp(name, nameList[0], strlen(nameList[0])) == 0)
  {
    RF_PRINT(INFO_PREFIX "RF1 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[0]);
    RF1_hwType=1;
    process_start(&ble_process,NULL);
  }
  else
  {
    RF_PRINT(INFO_PREFIX "RF1 new wireless type" INFO_POSTFIX "\r\n\r\n");
    RF1_hwType=0xfe;
  }
}


void RF2_StartHandleProcess(char* name)
{
  char nameList[][5]={"EC20", "BC95"};
  
  if(memcmp(name, nameList[0], strlen(nameList[0])) == 0)
  {
    RF_PRINT(INFO_PREFIX "RF2 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[0]);
    RF2_hwType=1;
    process_start(&gsm_process,NULL);//启动EC20处理进程
    process_start(&u_zhiyun,NULL);
#if XLABTOOLS_ON
    process_start(&AtProcess,NULL);
#endif
  }
  else if(memcmp(name, nameList[1], strlen(nameList[1])) == 0)
  {
    RF_PRINT(INFO_PREFIX "RF2 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[1]);
    RF2_hwType=2;
    //启动BC95处理进程
  }
  else
  {
    RF_PRINT(INFO_PREFIX "RF2 new wireless type" INFO_POSTFIX "\r\n\r\n");
    RF2_hwType=0xfe;
  }
}

PROCESS(RF1_GetHwTypeProcess, "get name driver");

PROCESS_THREAD(RF1_GetHwTypeProcess, ev, data)
{
  static struct etimer RF1_GetName_etimer;
  static uint8_t RF1_commandIndex;
  static char* RF1_pbuf;
  
  PROCESS_BEGIN();
  
  RF1_hwType = 0;
  RF1_commandIndex = 0;
  process_post(&RF1_GetHwTypeProcess,PROCESS_EVENT_TIMER,NULL);
  
  while(1)
  {
    PROCESS_WAIT_EVENT();
    
    if(ev == uart_command_event)
    {
      RF1_pbuf = (char*)data;
      if(RF1_commandIndex > 0)
      {
        if (memcmp(RF1_pbuf, "OK", strlen("OK")) == 0)
        {
          RF1_commandIndex = 2;
        }
        if (memcmp(RF1_pbuf, "+HW:", strlen("+HW:")) == 0)
        {
          if(RF1_commandIndex > 0)
          {
            etimer_stop(&RF1_GetName_etimer);
            RF1_StartHandleProcess(&RF1_pbuf[4]);
            PROCESS_EXIT();
          }
        }
      }
    }
    
    if(ev==PROCESS_EVENT_TIMER)
    {
      etimer_set(&RF1_GetName_etimer, 500);
      
      if(RF1_commandIndex == 2)
      {
        rfUartSendString(1,"AT+HW?\r\n",strlen("AT+HW?\r\n"));
      }
      else
      {
        rfUartSendString(1,"ATE0\r\n",strlen("ATE0\r\n"));
        RF1_commandIndex = 1;
      }
    }
  }
  PROCESS_END();
}


uint8_t RF2_SendReadCommand(uint8_t index)
{
  const char command_list[][30] = {
    "AT\r\n",
    "ATE0\r\n",
    "AT+CGMM\r\n",
  };
  if(index<(sizeof(command_list)/sizeof(command_list[0])))
  {
    rfUartSendString(2,(char*)command_list[index], strlen(command_list[index]));
    return 1;
  }
  return 0;
}

PROCESS(RF2_GetHwTypeProcess, "get name driver");

PROCESS_THREAD(RF2_GetHwTypeProcess, ev, data)
{
  static struct etimer RF2_GetName_etimer;
  static uint8_t count=0,commandIndex=0;
  static char* RF2_pbuf;
  
  PROCESS_BEGIN();
  
  RF2_hwType = 0;
  commandIndex = 0;
  etimer_set(&RF2_GetName_etimer, 100);//等待100ms再发
  
  while(1)
  {
    PROCESS_WAIT_EVENT();
    if(ev==PROCESS_EVENT_TIMER)
    {
      etimer_set(&RF2_GetName_etimer, 1000);
      
      if(commandIndex==0)
      {
        switch(count)
        {
        case 0:uart6_init(115200);//LTE
          break;
        case 1:uart6_init(9600);//NB
          break;
        }
        count++;
        if(count>1) count=0;
      }
      RF2_SendReadCommand(commandIndex);
    }
    if(ev == uart_command_event)
    {
      RF2_pbuf = (char*)data;
      if (memcmp(RF2_pbuf, "OK", strlen("OK")) == 0)
      {
        commandIndex++;
      }
      if((memcmp(RF2_pbuf, "EC20", strlen("EC20")) == 0)||
         (memcmp(RF2_pbuf, "BC95", strlen("BC95")) == 0))
      {
        etimer_stop(&RF2_GetName_etimer);
        RF2_StartHandleProcess(RF2_pbuf);
        PROCESS_EXIT();
      }
    }
  }
  PROCESS_END();
}


PROCESS(getHwType_process, "get name driver");

PROCESS_THREAD(getHwType_process, ev, data)
{
  PROCESS_BEGIN();
  
  process_start(&RF1_GetHwTypeProcess, NULL);
  process_start(&RF2_GetHwTypeProcess, NULL);
  
  PROCESS_EXIT();
  PROCESS_END();
}
