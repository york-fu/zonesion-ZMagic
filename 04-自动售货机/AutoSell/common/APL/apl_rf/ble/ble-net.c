#include "ble-net.h"
#include "zxbee.h"
#include "rfUart.h"

static char ble_mac[18]={0};
static unsigned char ble_link=0;

char* bleGat_mac()
{
  return ble_mac;
}

unsigned char bleGat_link()
{
  static unsigned short tick=0;
  tick++;
  if(tick>49)
  {
    tick=0;
    rfUartSendString(1,"AT+LINK?\r\n",strlen("AT+LINK?\r\n"));    //周期性问询
  }
  return ble_link;
}

uint8_t ble_sendReadCommand(uint8_t index)
{
  static const char command_list[][30] = {
    "AT+MAC?\r\n",
    "AT+LINK?\r\n",
  };
  if(index < (sizeof(command_list)/sizeof(command_list[0])))
  {
    rfUartSendString(1,(char*)command_list[index], strlen(command_list[index]));
    return 1;
  }
  return 0;
}

void ble_UaerProgram()
{
}

PROCESS(ble_process, "ble_process");

PROCESS_THREAD(ble_process, ev, data)
{
  static process_event_t bleConfig_event;
  static unsigned char readIndex=0;
  static uint8_t configFlag=0;
  
  PROCESS_BEGIN();
  
  readIndex=0;
  configFlag=1;
  process_post(&ble_process,bleConfig_event,NULL);
  
  while (1)
  {
    PROCESS_YIELD();
    if (ev == bleConfig_event)
    {
      if(ble_sendReadCommand(readIndex) == 0)
      {
        readIndex = 0;
        configFlag = 0;
        ble_UaerProgram();
      }
    }
    if (ev == uart_command_event)
    {
      char* pdata = (char *)data;
      if (memcmp(pdata, "OK", 2) == 0)
      {
        if(configFlag)
        {
          readIndex++;
          process_post(&ble_process,bleConfig_event,NULL);
        }
      }
      else if(memcmp(pdata, "EER:", 4) == 0)
      {
        process_post(&ble_process,bleConfig_event,NULL);
      }
      else if (memcmp(pdata, "+RECV:", 6) == 0)
      {
        short dataLen = atoi(&pdata[6]);
        while((*(pdata++))!='\n');
        if (dataLen > 0)
        {
          _zxbee_onrecv_fun(1,pdata, dataLen);
        }
      }
      else if (memcmp(pdata, "+MAC:", 5) == 0)
      {
        memcpy(ble_mac, &pdata[5], 17);
        ble_mac[17] = 0;
      }
      else if (memcmp(pdata, "+LINK:", 6) == 0)
      {
        ble_link = atoi(&pdata[6]);
      }
    }
  }
  PROCESS_END();
}