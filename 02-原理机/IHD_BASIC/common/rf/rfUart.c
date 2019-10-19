/*********************************************************************************************
* 文件：rfUart.c
* 作者：fuyou 2018.12.18
* 说明：原理机与无线的串口通信驱动
* 修改：
* 注释：
*********************************************************************************************/
#include "rfUart.h"

PROCESS_NAME(getHwType_process);
PROCESS_NAME(zigbee_process);
PROCESS_NAME(ble_process);
PROCESS_NAME(wifi_process);
PROCESS_NAME(lora_process);

process_event_t uart_command_event;
process_event_t RF1_commandEvent;
process_event_t RF2_commandEvent;
process_event_t RF3_commandEvent;
process_event_t RF4_commandEvent;

uint8_t RF1_SendDataACK = 0;
uint8_t RF2_SendDataACK = 0;
uint8_t RF3_SendDataACK = 0;


/*无线发送字节回调函数*/
void (*rfSendByte_call[4])(char)={NULL};

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
  case 3:RF_PRINT("RF3 <-- %s\r\n",buf);
    break;
  case 4:RF_PRINT("RF4 <-- %s\r\n",buf);
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
  uint8_t t=0, len=0;
  char pbuf[20]={0};
  
  *DataACK = 0;
  len = sprintf(pbuf, "AT+SEND=%u\r\n", strlen(dat));
  rfUartSendString(index, pbuf, len);
  
  while(!(*DataACK)) //等待收到'>'
  {
    delay_ms(1);
    t++;
    if(t>45) break; //超时
  }
  
  if(*DataACK)
  {
    rfUartSendString(index, dat, strlen(dat));
    return 0;
  }
  return 1;
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
  if(rfUartSendData(2, &RF2_SendDataACK, dat))
    return 1;
  return 0;
}

/* RF3 */
uint8_t RF3_SendData(char* dat)
{
  if(rfUartSendData(3, &RF3_SendDataACK, dat))
    return 1;
  return 0;
}

/* RF4 */
uint8_t RF4_SendData(char* dat)
{
  RF_PRINT("RF4 <-- %s\r\n",dat);
  zhiyun_send(dat);
  return 0;
}

/*下面使用单独的接收函数可提高效率*/
/*RF1 recv byte*/
static void RF1_RecvByte(char ch)
{
  static unsigned char RF1_dataLen=0;
  static unsigned char RF1_bufOffset=0;
  static char RF1_BUF_RECV[2][RF1_BUF_SIZE]={0};
  static char* RF1_pbuf=RF1_BUF_RECV[0];
  
  if(RF1_bufOffset<RF1_BUF_SIZE)
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
        process_post(&rfUart_process, RF1_commandEvent, RF1_pbuf);
        
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
          process_post(&rfUart_process, RF1_commandEvent, RF1_pbuf);
          
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
}

/*RF2 recv byte*/
static void RF2_RecvByte(char ch)
{
  static unsigned char RF2_dataLen=0;
  static unsigned char RF2_bufOffset=0;
  static char RF2_BUF_RECV[2][RF2_BUF_SIZE]={0};
  static char* RF2_pbuf=RF2_BUF_RECV[0];
  
  if(RF2_bufOffset<RF2_BUF_SIZE)
  {
    if(ch=='>')
    {
      RF_PRINT("RF2 --> >\r\n");
      RF2_SendDataACK = 1;
    }
    else if(RF2_dataLen>0)
    {
      RF2_pbuf[RF2_bufOffset++]=ch;
      RF2_dataLen--;
      if(RF2_dataLen==0)
      {
        RF2_pbuf[RF2_bufOffset]='\0';
        process_post(&rfUart_process, RF2_commandEvent, RF2_pbuf);
        
        if(RF2_pbuf==RF2_BUF_RECV[0])
          RF2_pbuf=RF2_BUF_RECV[1];
        else
          RF2_pbuf=RF2_BUF_RECV[0];
        RF2_bufOffset = 0;
      }
    }
    else
    {
      RF2_pbuf[RF2_bufOffset++]=ch;
      if(memcmp(&RF2_pbuf[RF2_bufOffset-2],"\r\n",2)==0)
      {
        if((RF2_bufOffset>6)&&(memcmp(RF2_pbuf,"+RECV:",6)==0))
        {
          RF2_dataLen=atoi(&RF2_pbuf[6]);
        }
        else if(RF2_bufOffset>2)
        {
          RF2_pbuf[RF2_bufOffset]='\0';
          process_post(&rfUart_process, RF2_commandEvent, RF2_pbuf);
          
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
  }
  else
  {
    RF2_bufOffset = 0;
  }
}

/*RF3 recv byte*/
static void RF3_RecvByte(char ch)
{
  static unsigned char RF3_dataLen=0;
  static unsigned char RF3_bufOffset=0;
  static char RF3_BUF_RECV[2][RF3_BUF_SIZE]={0};
  static char* RF3_pbuf=RF3_BUF_RECV[0];
  
  if(RF3_bufOffset<RF3_BUF_SIZE)
  {
    if(ch=='>')
    {
      RF_PRINT("RF3 --> >\r\n");
      RF3_SendDataACK = 1;
    }
    else if(RF3_dataLen>0)
    {
      RF3_pbuf[RF3_bufOffset++]=ch;
      RF3_dataLen--;
      if(RF3_dataLen==0)
      {
        RF3_pbuf[RF3_bufOffset]='\0';
        process_post(&rfUart_process, RF3_commandEvent, RF3_pbuf);
        
        if(RF3_pbuf==RF3_BUF_RECV[0])
          RF3_pbuf=RF3_BUF_RECV[1];
        else
          RF3_pbuf=RF3_BUF_RECV[0];
        RF3_bufOffset = 0;
      }
    }
    else
    {
      RF3_pbuf[RF3_bufOffset++]=ch;
      if(memcmp(&RF3_pbuf[RF3_bufOffset-2],"\r\n",2)==0)
      {
        if((RF3_bufOffset>6)&&(memcmp(RF3_pbuf,"+RECV:",6)==0))
        {
          RF3_dataLen=atoi(&RF3_pbuf[6]);
        }
        else if(RF3_bufOffset>2)
        {
          RF3_pbuf[RF3_bufOffset]='\0';
          process_post(&rfUart_process, RF3_commandEvent, RF3_pbuf);
          
          if(RF3_pbuf==RF3_BUF_RECV[0])
            RF3_pbuf=RF3_BUF_RECV[1];
          else
            RF3_pbuf=RF3_BUF_RECV[0];
          RF3_bufOffset = 0;
        }
        else
        {
          RF3_bufOffset = 0;
        }
      }
    }
  }
  else
  {
    RF3_bufOffset = 0;
  }
}

/*RF4 recv byte*/
static void RF4_RecvByte(char ch)
{
  static unsigned char RF4_bufOffset=0;
  static char RF4_BUF_RECV[2][RF4_BUF_SIZE]={0};
  static char* RF4_pbuf=RF4_BUF_RECV[0];
  
  if(RF4_hwTypeGet() != 0)
  {
    
#if 1   //打印调试信息
    static char buf[2];
    static uint8_t debugInfo=0;
    if(!debugInfo)
    {
      debugInfo = 1;
      RF_PRINT("RF4 --> ");
    }
    RF_PRINT("%c",ch);
    buf[0] = buf[1];
    buf[1] = ch;
    if(strcmp(buf,"\r\n")==0) //"\r\n"一条消息结束
      debugInfo = 1;
#endif
    
    switch(RF4_hwTypeGet())
    {
    case 1:gsm_recv_ch(ch); //ec20接收字节处理
      break;
    case 2: //bc95接收字节处理
      break;
    }
  }
  else
  {
    if(RF4_bufOffset<RF4_BUF_SIZE)
    {
      RF4_pbuf[RF4_bufOffset++]=ch;
      if(memcmp(&RF4_pbuf[RF4_bufOffset-2],"\r\n",2)==0)
      {
        if(RF4_bufOffset>2)
        {
          RF4_pbuf[RF4_bufOffset]='\0';
          process_post(&rfUart_process, RF4_commandEvent, RF4_pbuf);
          
          if(RF4_pbuf==RF4_BUF_RECV[0])
            RF4_pbuf=RF4_BUF_RECV[1];
          else
            RF4_pbuf=RF4_BUF_RECV[0];
          RF4_bufOffset = 0;
        }
        else
        {
          RF4_bufOffset = 0;
        }
      }
    }
    else
    {
      RF4_bufOffset = 0;
    }
  }
}


void WK2114_init()
{
  uart2_init(115200);//wk2114主串口
  uart2_set_input(Wk2114_UartIrqCallback);
  
  exti9_5_init();
	EXTI9_5_CALL_Set(3,Wk2114_ExIrqCallback);
  
	Wk2114_config();
  
  Wk2114_SlaveRecv_Set(1,RF1_RecvByte);
  Wk2114_SlaveRecv_Set(2,RF3_RecvByte);
  Wk2114_SlaveRecv_Set(3,RF4_RecvByte);
  Wk2114_SlaveRecv_Set(4,RF2_RecvByte);
}


PROCESS(rfUart_process, "rfUart_process");

PROCESS_THREAD(rfUart_process, ev, data)
{
  PROCESS_BEGIN();
  
  WK2114_init();
  
  rfSendByteCallSet(1,Wk2114_Uart1SendByte);//RF1发送回调设置
  rfSendByteCallSet(2,Wk2114_Uart4SendByte);
  rfSendByteCallSet(3,Wk2114_Uart2SendByte);
  rfSendByteCallSet(4,Wk2114_Uart3SendByte);
  
  uart_command_event = process_alloc_event();
  RF1_commandEvent = process_alloc_event();
  RF2_commandEvent = process_alloc_event();
  RF3_commandEvent = process_alloc_event();
  RF3_commandEvent = process_alloc_event();
  
  process_start(&getHwType_process,NULL);//启动自动识别无线进程
  
  while(1)
  {
    PROCESS_WAIT_EVENT();
    
    if(ev==RF1_commandEvent)      //uart command handle
    {
      RF_PRINT("RF1 --> %s\r\n", (char*)data);
      
      if(RF1_hwTypeGet() != 0)
      {
        switch(RF1_hwTypeGet())
        {
        case 1:process_post(&zigbee_process, uart_command_event, data);
          break;
        case 2:process_post(&ble_process, uart_command_event, data);
          break;
        }
      }
      else
      {
        process_post(&RF1_GetHwTypeProcess, uart_command_event, data);
      }
    }
    
    if(ev==RF2_commandEvent)      //uart command handle
    {
      RF_PRINT("RF2 --> %s\r\n", (char*)data);
      
      if(RF2_hwTypeGet() != 0)
      {
        switch(RF2_hwTypeGet())
        {
        case 1:process_post(&wifi_process, uart_command_event, data);
          break;
        }
      }
      else
      {
        process_post(&RF2_GetHwTypeProcess, uart_command_event, data);
      }
    }
    
    if(ev==RF3_commandEvent)      //uart command handle
    {
      RF_PRINT("RF3 --> %s\r\n", (char*)data);
      
      if(RF3_hwTypeGet() != 0)
      {
        switch(RF3_hwTypeGet())
        {
        case 1:process_post(&lora_process, uart_command_event, data);
          break;
        }
      }
      else
      {
        process_post(&RF3_GetHwTypeProcess, uart_command_event, data);
      }
    }
    if(ev==RF4_commandEvent)      //uart command handle
    {
      RF_PRINT("RF4 --> %s\r\n", (char*)data);
      
      process_post(&RF4_GetHwTypeProcess, uart_command_event, data);
    }
  }
  
  PROCESS_END();
}

