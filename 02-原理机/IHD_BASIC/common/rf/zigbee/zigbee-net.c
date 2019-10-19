#include "zigbee-net.h"
#include "zxbee.h"

static char zigbee_mac[24];
static char zigbee_type = -1;
static unsigned short zigbee_panid=0;
static unsigned short zigbee_channel=0;
static unsigned char zigbee_link=0;

char* zigbeeGat_mac()
{
    return zigbee_mac;
}
unsigned short zigbeeGat_panid()
{
    return zigbee_panid;
}
unsigned short zigbeeGat_channel()
{
    return zigbee_channel;
}
char zigbeeGat_type()
{
    return zigbee_type;
}

unsigned char zigbeeGat_link()
{
    static unsigned short tick=0;
    tick++;
    if(tick>49)
    {
        tick=0;
        rfUartSendString(1,"AT+LINK?\r\n",strlen("AT+LINK?\r\n"));    //周期性问询
    }
    return zigbee_link;
}

void zigbee_readConfigHandle(char* pdata)
{
    if (memcmp(pdata, "+MAC:", 5) == 0)
    {
        memcpy(zigbee_mac, &pdata[5], 23);
        zigbee_mac[23] = 0;
    }
    else if (memcmp(pdata, "+LOGICALTYPE:", 13) == 0)
    {
        zigbee_type = pdata[13] - '0';
    }
    else if (memcmp(pdata, "+PANID:", 7) == 0)
    {
        zigbee_panid = atoi(&pdata[7]);
    }
    else if (memcmp(pdata, "+CHANNEL:", 9) == 0)
    {
        zigbee_channel = atoi(&pdata[9]);
    }
    else if (memcmp(pdata, "+LINK:", 6) == 0)
    {
        zigbee_link = atoi(&pdata[6]);
    }
}

uint8_t zigbee_sendReadCommand(uint8_t index)
{
    static const char command_list[][30] = {
        "AT+LOGICALTYPE?\r\n",
        "AT+PANID?\r\n",
        "AT+CHANNEL?\r\n",
        "AT+MAC?\r\n",
        "AT+LINK?\r\n"
    };
    if(index<(sizeof(command_list)/sizeof(command_list[0])))
    {
        rfUartSendString(1,(char*)command_list[index], strlen(command_list[index]));
        return 1;
    }
    return 0;
}

uint8_t zigbee_writeConfig()
{
#if RF_PLUS_ON    
    char pbuf[20]={0};
    static uint8_t index=0,configFlag=0;;
    uint8_t len=0;
    switch(index)
    {
        case 0:
            index++;
            if(zigbee_panid != ZIGBEE_CONFIG_PANID)
            {
                configFlag=1;
                len = sprintf(pbuf,"at+panid=%u\r\n",ZIGBEE_CONFIG_PANID);
                rfUartSendString(1,pbuf,len);
                return 1;
            }
        case 1:
            index++;
            if(zigbee_channel != ZIGBEE_CONFIG_CHANNEL)
            {
                configFlag=1;
                len = sprintf(pbuf,"at+channel=%u\r\n",ZIGBEE_CONFIG_CHANNEL);
                rfUartSendString(1,pbuf,len);
                return 1;
            }
        case 2:
            index++;
            if(configFlag)
            {
                configFlag=0;
                strcpy(pbuf,"at+reset\r\n");
                rfUartSendString(1,pbuf,strlen(pbuf));
                return 1;
            }
    }
    index=0;
#endif
    return 0;
}

void zigbee_UaerProgram()
{
    process_start(&sensor_process,NULL);
}

PROCESS(zigbee_process, "zigbee_process");

PROCESS_THREAD(zigbee_process, ev, data)
{
    PROCESS_BEGIN();
    
    static process_event_t zigbeeConfig_event;
    static unsigned char readIndex=0,writeIndex=0;
    static uint8_t configFlag=0;
    
    readIndex=0;
    configFlag=0x01;
    zigbeeConfig_event=process_alloc_event();
    process_post(&zigbee_process,zigbeeConfig_event,NULL);

    while (1)
    {
        PROCESS_YIELD();
        if (ev==zigbeeConfig_event)
        {
            if((configFlag&0x80))
            {  
                configFlag=0;
                zigbee_UaerProgram();
            }
            else if(configFlag&0x01)
            {
                if(zigbee_sendReadCommand(readIndex)==0)
                {
                    configFlag=0x02; 
                    writeIndex=0;
                    process_post(&zigbee_process,zigbeeConfig_event,NULL);
                }
            }
            else if(configFlag&0x02)
            {
                if(zigbee_writeConfig()==0)
                {
                    if(writeIndex>0)
                    {
                        process_start(&getHwType_process,NULL);
                        PROCESS_EXIT();
                    }
                    else
                    {
                        configFlag=(0x80);
                        readIndex=0;
                        process_post(&zigbee_process,zigbeeConfig_event,NULL);
                    }
                }
                else
                    writeIndex++;
            }
        }
        if (ev == uart_command_event)
        {
            char* pdata = (char *)data;
            if (memcmp(pdata, "+RECV:", 6) == 0)
            {
                short dataLen = atoi(&pdata[6]);
                while((*(pdata++))!='\n');
                if (dataLen > 0)
                {
                    _zxbee_onrecv_fun(1,pdata, dataLen);
                }
            }
            else if(memcmp(pdata, "OK", 2) == 0)
            {
                if(configFlag&0x01)
                {
                    readIndex++;
                    process_post(&zigbee_process,zigbeeConfig_event,NULL);
                }
                else if(configFlag&0x02)
                {
                    process_post(&zigbee_process,zigbeeConfig_event,NULL);
                }
            }
            else if(memcmp(pdata, "EER:", 4) == 0)
            {
                process_post(&zigbee_process,zigbeeConfig_event,NULL);
            }
            else
            {
                zigbee_readConfigHandle(pdata);
            }
        }
    }
    PROCESS_END();
}


