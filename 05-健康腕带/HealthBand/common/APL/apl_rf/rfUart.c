/*********************************************************************************************
* 文件：rfUart.c
* 作者：fuyou 2018.12.18
* 说明：原理机与无线的串口通信驱动
* 修改：
* 注释：
*********************************************************************************************/
#include "rfUart.h"
#include "delay.h"
#include "hw.h"
#include "usart.h"
#include "lte-inf.h"
#include "lte-ec20.h"
#include "lte_zhiyun.h"
#include "apl_rf/ble/ble-net.h"

PROCESS_NAME(getHwType_process);
PROCESS_NAME(ble_process);

process_event_t uart_command_event;
process_event_t RF1_commandEvent;
process_event_t RF2_commandEvent;

uint8_t RF1_SendDataACK = 0;


/*无线发送字节回调函数*/
void (*rfSendByte_call[2])(char)={NULL};

void rfSendByteCallSet(uint8_t index,void (*func)(char))
{
  rfSendByte_call[index-1]=func;
}

void rfUartSendByte(uint8_t index,char ch)
{
  if (rfSendByte_call[index-1] != NULL) 
  {
    rfSendByte_call[index-1](ch);
  }
}

//向无线发送字符串
void rfUartSendString(uint8_t index,char *buf, int len)
{
  switch(index)   //DEBUG info
  {
  case 1:RF_PRINT("RF1 <-- %s\r\n",buf);
    break;
  case 2:RF_PRINT("RF2 <-- %s\r\n",buf);
    break;
  }
  
  while(len--)
  {
    rfUartSendByte(index,*buf++);
  }
}

//向无线发送透传数据
uint8_t rfUartSendData(uint8_t index, uint8_t* DataACK, char *dat)
{
  uint16_t t=0, len=0;
  char pbuf[20]={0};
  
  *DataACK = 0;
  len = sprintf(pbuf, "AT+SEND=%d\r\n", strlen(dat));
  rfUartSendString(index, pbuf, len);
  
  while(!(*DataACK)) //等待收到'>'
  {
    delay_ms(1);
    t++;
    if(t>49)
      return 1; //超时
  }
  
  if(*DataACK)
  {
    rfUartSendString(index, dat, strlen(dat));
  }
  return 0;
}

/* RF1 */
uint8_t RF1_SendData(char* dat)
{
  if(rfUartSendData(1, &RF1_SendDataACK, dat))
    return 1;
  return 0;
}

/* RF2 */
uint8_t RF2_SendData(char* dat)
{
  RF_PRINT("RF2 <-- %s\r\n",dat);
  zhiyun_send(dat);
  return 0;
}

/*使用单独的接收回调函数可提高效率*/
/*RF1 recv byte*/
static int RF1_RecvByte(char ch)
{
  static unsigned char RF1_dataLen=0;
  static unsigned char RF1_bufOffset=0;
  static char RF1_BUF_RECV[2][RF1_BUF_SIZE]={0};
  static char* RF1_pbuf=RF1_BUF_RECV[0];
  
  if(RF1_bufOffset < RF1_BUF_SIZE)
  {
    if(ch=='>')
    {
      RF_PRINT("RF1 --> >\r\n");
      RF1_SendDataACK = 1;
    }
    else if(RF1_dataLen>0)
    {
      RF1_pbuf[RF1_bufOffset++]=ch;
      RF1_dataLen--;
      if(RF1_dataLen==0)
      {
        RF1_pbuf[RF1_bufOffset]='\0';
        process_post(&rfUartProcess, RF1_commandEvent, RF1_pbuf);
        
        if(RF1_pbuf==RF1_BUF_RECV[0])
          RF1_pbuf=RF1_BUF_RECV[1];
        else
          RF1_pbuf=RF1_BUF_RECV[0];
        RF1_bufOffset = 0;
      }
    }
    else
    {
      RF1_pbuf[RF1_bufOffset++]=ch;
      if(memcmp(&RF1_pbuf[RF1_bufOffset-2],"\r\n",2)==0)
      {
        if((RF1_bufOffset>6)&&(memcmp(RF1_pbuf,"+RECV:",6)==0))
        {
          RF1_dataLen=atoi(&RF1_pbuf[6]);
        }
        else if(RF1_bufOffset>2)
        {
          RF1_pbuf[RF1_bufOffset]='\0';
          process_post(&rfUartProcess, RF1_commandEvent, RF1_pbuf);
          
          if(RF1_pbuf==RF1_BUF_RECV[0])
            RF1_pbuf=RF1_BUF_RECV[1];
          else
            RF1_pbuf=RF1_BUF_RECV[0];
          RF1_bufOffset = 0;
        }
        else
        {
          RF1_bufOffset = 0;
        }
      }
    }
  }
  else
  {
    RF1_bufOffset = 0;
  }
  return 0;
}


/*RF2 recv byte*/
static int RF2_RecvByte(char ch)
{
  static unsigned char RF2_bufOffset=0;
  static char RF2_BUF_RECV[2][RF2_BUF_SIZE]={0};
  static char* RF2_pbuf=RF2_BUF_RECV[0];
  
  if(RF2_hwTypeGet() != 0)
  {
    switch(RF2_hwTypeGet())
    {
    case 1:gsm_recv_ch(ch); //ec20接收字节处理
      break;
    case 2: //bc95接收字节处理
      break;
    }
  }
  else
  {
    if(RF2_bufOffset<RF2_BUF_SIZE)
    {
      RF2_pbuf[RF2_bufOffset++]=ch;
      if(memcmp(&RF2_pbuf[RF2_bufOffset-2],"\r\n",2)==0)
      {
        if(RF2_bufOffset>2)
        {
          RF2_pbuf[RF2_bufOffset]='\0';
          process_post(&rfUartProcess, RF2_commandEvent, RF2_pbuf);
          
          if(RF2_pbuf==RF2_BUF_RECV[0])
            RF2_pbuf=RF2_BUF_RECV[1];
          else
            RF2_pbuf=RF2_BUF_RECV[0];
          RF2_bufOffset = 0;
        }
        else
        {
          RF2_bufOffset = 0;
        }
      }
    }
    else
    {
      RF2_bufOffset = 0;
    }
  }
  return 0;
}


void RF1_InfInit()
{
  uart4_init(38400);//BLE
  rfSendByteCallSet(1,uart4_putc);
  uart4_set_input(RF1_RecvByte);//RF1接收回调设置
}

void RF2_InfInit()
{
  uart6_init(115200);//LTE
  rfSendByteCallSet(2,uart6_putc);
  uart6_set_input(RF2_RecvByte);
}


PROCESS(rfUartProcess, "rfUartProcess");

PROCESS_THREAD(rfUartProcess, ev, data)
{
  PROCESS_BEGIN();
  
  RF1_InfInit();
  RF2_InfInit();
  
  uart_command_event = process_alloc_event();
  RF1_commandEvent = process_alloc_event();
  RF2_commandEvent = process_alloc_event();
  
  process_start(&getHwType_process,NULL);//启动自动识别无线进程
  
  while(1)
  {
    PROCESS_WAIT_EVENT();
    
    if(ev==RF1_commandEvent)      //uart command handle
    {
      RF_PRINT("RF1 --> %s\r\n", (char*)data);
      
      if(RF1_hwTypeGet() != 0)
      {
        process_post(&ble_process, uart_command_event, data);
      }
      else
      {
        process_post(&RF1_GetHwTypeProcess, uart_command_event, data);
      }
    }
    if(ev==RF2_commandEvent)      //uart command handle
    {
      RF_PRINT("RF2 --> %s\r\n", (char*)data);
      
      process_post(&RF2_GetHwTypeProcess, uart_command_event, data);
    }
  }
  
  PROCESS_END();
}



/*********************************************************************************************
* 名称：RFSendData
* 功能：通过无线发送数据
* 参数：dat：要发送的数据指针
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void RFSendData(char* dat)
{
  if(RF1_hwTypeGet() != 0)
  {
    RF1_SendData(dat);
  }
  if((Lte_GetTcpConnect()))
  {
    RF2_SendData(dat);
  }
}

