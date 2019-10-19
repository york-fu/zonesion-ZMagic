/*********************************************************************************************
* 文件：hw.c
* 作者：fuyou 2018.12.20
* 说明：原理机自动识别无线类型驱动
* 修改：
* 注释：
*********************************************************************************************/
#include "hw.h"

//无线类型标识
static uint8_t RF1_hwType=0;
static uint8_t RF2_hwType=0;
static uint8_t RF3_hwType=0;
static uint8_t RF4_hwType=0;
//无线类型名称
static char RF1_hwName[10]={0};
static char RF2_hwName[10]={0};
static char RF3_hwName[10]={0};
static char RF4_hwName[10]={0};

//获取无线类型
uint8_t RF1_hwTypeGet(void)
{
    return RF1_hwType;
}

uint8_t RF2_hwTypeGet(void)
{
    return RF2_hwType;
}

uint8_t RF3_hwTypeGet(void)
{
    return RF3_hwType;
}

uint8_t RF4_hwTypeGet(void)
{
    return RF4_hwType;
}

//打印调试信息
#define INFO_PREFIX       "[ "
#define INFO_POSTFIX      " ]"

void RF1_StartHandleProcess(void)
{
    char nameList[][8]={"CC2530", "CC2540"};
    
    if(memcmp(RF1_hwName, nameList[0], strlen(nameList[0])) == 0)
    {
        RF_PRINT(INFO_PREFIX "RF1 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[0]);
        RF1_hwType=1;
        process_start(&zigbee_process,NULL);
    }
    else if(memcmp(RF1_hwName, nameList[1], strlen(nameList[1])) == 0)
    {
        RF_PRINT(INFO_PREFIX "RF1 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[1]);
        RF1_hwType=2;
        process_start(&ble_process,NULL);
    }
    else
    {
        RF_PRINT(INFO_PREFIX "RF1 new wireless type" INFO_POSTFIX "\r\n\r\n");
        RF1_hwType=0xfe;
    }
}

void RF2_StartHandleProcess(void)
{
    char nameList[][8]={"CC3200", " "};
    
    if(memcmp(RF2_hwName, nameList[0], strlen(nameList[0])) == 0)
    {
        RF_PRINT(INFO_PREFIX "RF2 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[0]);
        RF2_hwType=1;
        process_start(&wifi_process,NULL);
    }
    else
    {
        RF_PRINT(INFO_PREFIX "RF2 new wireless type" INFO_POSTFIX "\r\n\r\n");
        RF2_hwType=0xfe;
    }
}

void RF3_StartHandleProcess(void)
{
    char nameList[][5]={"LoRa", " "};
    
    if(memcmp(RF3_hwName, nameList[0], strlen(nameList[0])) == 0)
    {
        RF_PRINT(INFO_PREFIX "RF3 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[0]);
        RF3_hwType=1;
        process_start(&lora_process,NULL);
    }
    else
    {
        RF_PRINT(INFO_PREFIX "RF3 new wireless type" INFO_POSTFIX "\r\n\r\n");
        RF3_hwType=0xfe;
    }
}

void RF4_StartHandleProcess(void)
{
    char nameList[][5]={"EC20", "BC95"};
    
    if(memcmp(RF4_hwName, nameList[0], strlen(nameList[0])) == 0)
    {
        RF_PRINT(INFO_PREFIX "RF4 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[0]);
        RF4_hwType=1;
        process_start(&gsm_process,NULL);//启动EC20处理进程
        process_start(&u_zhiyun,NULL);
    }
    else if(memcmp(RF4_hwName, nameList[1], strlen(nameList[1])) == 0)
    {
        RF_PRINT(INFO_PREFIX "RF4 wireless type: %s" INFO_POSTFIX "\r\n\r\n",nameList[1]);
        RF4_hwType=2;
        //在这里启动BC95处理进程
    }
    else
    {
        RF_PRINT(INFO_PREFIX "RF4 new wireless type" INFO_POSTFIX "\r\n\r\n");
        RF4_hwType=0xfe;
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
            if (memcmp(RF1_pbuf, "OK", strlen("OK")) == 0)
            {
                RF1_commandIndex=1;
            }
            if (memcmp(RF1_pbuf, "+HW:", strlen("+HW:")) == 0)
            {
                etimer_stop(&RF1_GetName_etimer);
                memcpy(RF1_hwName, &RF1_pbuf[4], 6);
                RF1_StartHandleProcess();
                PROCESS_EXIT();
            }
        }
        
        if(ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&RF1_GetName_etimer, 500);
            
            if(RF1_commandIndex==1)
            {
                rfUartSendString(1,"AT+HW?\r\n",strlen("AT+HW?\r\n"));
            }
            else
            {
                rfUartSendString(1,"ATE0\r\n",strlen("ATE0\r\n"));
            }
        }
    }
    PROCESS_END();
}

PROCESS(RF2_GetHwTypeProcess, "get name driver");

PROCESS_THREAD(RF2_GetHwTypeProcess, ev, data)
{
    static struct etimer RF2_GetName_etimer;
    static uint8_t RF2_commandIndex;
    static char* RF2_pbuf;
    
    PROCESS_BEGIN();

    RF2_hwType = 0;
    RF2_commandIndex = 0;
    process_post(&RF2_GetHwTypeProcess,PROCESS_EVENT_TIMER,NULL);
    
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if(ev == uart_command_event)
        {
            RF2_pbuf = (char*)data;
            if (memcmp(RF2_pbuf, "OK", strlen("OK")) == 0)
            {
                RF2_commandIndex=1;
            }
            if (memcmp(RF2_pbuf, "+HW:", strlen("+HW:")) == 0)
            {
                etimer_stop(&RF2_GetName_etimer);
                memcpy(RF2_hwName, &RF2_pbuf[4], 6);
                RF2_StartHandleProcess();
                PROCESS_EXIT();
            }
        }
        
        if(ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&RF2_GetName_etimer, 500);
            
            if(RF2_commandIndex==1)
            {
                rfUartSendString(2,"AT+HW?\r\n",strlen("AT+HW?\r\n"));
            }
            else
            {
                rfUartSendString(2,"ATE0\r\n",strlen("ATE0\r\n"));
            }
        }
    }
    PROCESS_END();
}

PROCESS(RF3_GetHwTypeProcess, "get name driver");

PROCESS_THREAD(RF3_GetHwTypeProcess, ev, data)
{
    static struct etimer RF3_GetName_etimer;
    static uint8_t RF3_commandIndex;
    static char* RF3_pbuf;
    
    PROCESS_BEGIN();

    RF3_hwType = 0;
    RF3_commandIndex = 0;
    process_post(&RF3_GetHwTypeProcess,PROCESS_EVENT_TIMER,NULL);
    
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if(ev == uart_command_event)
        {
            RF3_pbuf = (char*)data;
            if (memcmp(RF3_pbuf, "OK", strlen("OK")) == 0)
            {
                RF3_commandIndex=1;
            }
            if (memcmp(RF3_pbuf, "+HW:", strlen("+HW:")) == 0)
            {
                etimer_stop(&RF3_GetName_etimer);
                memcpy(RF3_hwName, &RF3_pbuf[4], 6);
                RF3_StartHandleProcess();
                PROCESS_EXIT();
            }
        }
        
        if(ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&RF3_GetName_etimer, 500);
            
            if(RF3_commandIndex==1)
            {
                rfUartSendString(3,"AT+HW?\r\n",strlen("AT+HW?\r\n"));
            }
            else
            {
                rfUartSendString(3,"ATE0\r\n",strlen("ATE0\r\n"));
            }
        }
    }
    PROCESS_END();
}

PROCESS(RF4_GetHwTypeProcess, "get name driver");

PROCESS_THREAD(RF4_GetHwTypeProcess, ev, data)
{
    static struct etimer RF4_GetName_etimer;
    static uint8_t RF4_commandIndex;
    static uint8_t RF4_Status;      //5-4bit:command status  3-0bit:command count
    static char* RF4_pbuf;
    static char CommandList[][20]={
        "AT\r\n",
        "ATE0\r\n",
        "AT+CGMM\r\n"
    };
    
    PROCESS_BEGIN();

    RF4_hwType = 0;
    RF4_Status = 0;
    RF4_commandIndex = 0;
    process_post(&RF4_GetHwTypeProcess,PROCESS_EVENT_TIMER,NULL);
    
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if(ev == uart_command_event)
        {
            RF4_pbuf = (char*)data;
            if(memcmp(RF4_pbuf, "OK", strlen("OK")) == 0)
            {
                if(RF4_Status & 0x10)
                    RF4_commandIndex = 1;
                else if(RF4_Status & 0x20)
                    RF4_commandIndex = 2;
            }
            if( (memcmp(RF4_pbuf, "EC20", strlen("EC20")) == 0) ||
                (memcmp(RF4_pbuf, "BC95", strlen("BC95")) == 0) )
            {
                etimer_stop(&RF4_GetName_etimer);
                memcpy(RF4_hwName, RF4_pbuf, 4);
                RF4_StartHandleProcess();
                PROCESS_EXIT();
            }
        }
        
        if(ev==PROCESS_EVENT_TIMER)
        {
            etimer_set(&RF4_GetName_etimer, 1000);
            
            switch(RF4_commandIndex)
            {
                case 0:
                    RF4_Status |= 0x10;
                    RF4_Status++;
                    break;
                    
                case 1:
                    RF4_Status &= ~0x10;
                    RF4_Status |= 0x20;
                    RF4_Status++;
                    break;
            }
            
            if((RF4_Status&0x0f)>1)
            {
                RF4_Status &= 0xf0;
                if(RF4_Status&0x80)
                {
                    RF4_Status &= ~0x80;
                    Wk2114SetBaud(3,9600);      //iot bc95 module
                    delay_ms(10);
                }
                else
                {
                    RF4_Status |= 0x80;
                    Wk2114SetBaud(3,115200);    //lte ec20 module
                    delay_ms(10);
                }
            }
            
            rfUartSendString(4,CommandList[RF4_commandIndex],strlen(CommandList[RF4_commandIndex]));
        }
    }
    PROCESS_END();
}

PROCESS(getHwType_process, "get name driver");

PROCESS_THREAD(getHwType_process, ev, data)
{
    PROCESS_BEGIN();

    process_start(&RF1_GetHwTypeProcess, NULL);
//    process_start(&RF2_GetHwTypeProcess, NULL);
//    process_start(&RF3_GetHwTypeProcess, NULL);
//    process_start(&RF4_GetHwTypeProcess, NULL);
    
    PROCESS_EXIT();
    PROCESS_END();
}
