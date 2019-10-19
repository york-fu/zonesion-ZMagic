#include "wifi-net.h"
#include "zxbee.h"

static unsigned char wifi_link=0;
static char wifi_keyType=-1;
static char wifi_mac[18]={0};
static char wifi_ssid[50]={0};
static char wifi_key[50]={0};
static char wifi_ip[20]={0};
static char wifi_sip[20]={0};

char wifiGat_keyType()
{
    return wifi_keyType;
}

char* wifiGat_mac()
{
    return wifi_mac;
}

char* wifiGat_ssid()
{
    return wifi_ssid;
}

char* wifiGat_key()
{
    return wifi_key;
}

char* wifiGat_ip()
{
    return wifi_ip;
}

char* wifiGat_sip()
{
    return wifi_sip;
}

unsigned char wifiGat_link(void)
{
    static unsigned short tick=0;
//    char pbuf[16]={0};
    tick++;
    if(tick>49)
    {
        tick=0;
//        strcpy(pbuf,"AT+LINK?\r\n");
//        rfUartSendString(2,pbuf,strlen(pbuf));    //周期性问询
    }
    return wifi_link;
}

uint8_t WiFi_sendReadCommand(uint8_t index)
{
    static const char Command_list[][30] = {
        "AT+MAC?\r\n",
        "AT+SSID?\r\n",
        "AT+KEYTYPE?\r\n",
        "AT+KEY?\r\n",
        "AT+IP?\r\n",
        "AT+SIP?\r\n",
//        "AT+LINK?\r\n"
    };
    if(index<(sizeof(Command_list)/sizeof(Command_list[0])))
    {
        rfUartSendString(2,(char*)Command_list[index], strlen(Command_list[index]));
        return 1;
    }
    return 0;
}

uint8_t WiFi_writeConfig()
{
#if RF_PLUS_ON   
    char pbuf[20]={0};
    static uint8_t index=0,configFlag=0;
    
    switch(index)
    {
        case 0:
            index++;
            if(memcmp(&wifi_ssid,WIFI_CONFIG_SSID,strlen(WIFI_CONFIG_SSID)))
            {
                configFlag=1;
                strcpy(pbuf,"AT+SSID=\"");
                strcat(pbuf,WIFI_CONFIG_SSID);
                strcat(pbuf,"\"\r\n");
                rfUartSendString(2,pbuf,strlen(pbuf));
                return 1;
            }
        case 1:
            index++;
            if(wifi_keyType != WIFI_CONFIG_KEYTYPE)
            {
                configFlag=1;
                uint8_t len=sprintf(pbuf,"AT+KEYTYPE=%u\r\n",WIFI_CONFIG_KEYTYPE);
                rfUartSendString(2,pbuf,len);
                return 1;
            }
        case 2:
            index++;
            if(memcmp(&wifi_key,WIFI_CONFIG_KEY,strlen(WIFI_CONFIG_KEY)))
            {
                configFlag=1;
                strcpy(pbuf,"AT+KEY=\"");
                strcat(pbuf,WIFI_CONFIG_KEY);
                strcat(pbuf,"\"\r\n");
                rfUartSendString(2,pbuf,strlen(pbuf));
                return 1;
            }
        case 3:
            index++;
            if(memcmp(wifi_sip,WIFI_CONFIG_SIP,strlen(WIFI_CONFIG_SIP)))
            {
                configFlag=1;
                strcpy(pbuf,"AT+SIP=");
                strcat(pbuf,WIFI_CONFIG_SIP);
                strcat(pbuf,"\r\n");
                rfUartSendString(2,pbuf,strlen(pbuf));
                return 1;
            }
        case 4:
            index++;
            if(configFlag)
            {
                strcpy(pbuf,"AT+ENVSAVE\r\n");
                rfUartSendString(2,pbuf,strlen(pbuf));
                return 1;
            }
        case 5:
            index++;
            if(configFlag)
            {
                configFlag=0;
                strcpy(pbuf,"AT+RESET\r\n");
                rfUartSendString(2,pbuf,strlen(pbuf));
                return 1;
            }
    }
    index=0;
#endif
    return 0;
}

void WiFi_UaerProgram()
{
    process_start(&sensor_process,NULL);
}


PROCESS(wifi_process, "wifi_process");

PROCESS_THREAD(wifi_process, ev, data)
{ 
    PROCESS_BEGIN();
    
    static process_event_t WiFiConfig_event;
    static char* temp=NULL;
    static unsigned char len=0,readIndex=0,writeIndex=0;
    static uint8_t configFlag=0;

    readIndex=0;
    configFlag=0x01;
    WiFiConfig_event=process_alloc_event();
    process_post(&wifi_process,WiFiConfig_event,NULL);
    
    while (1)
    {
        PROCESS_YIELD();
        if ((ev==WiFiConfig_event))
        {
            if((configFlag&0x80))
            {  
                configFlag=0;
                WiFi_UaerProgram();
            }
            else if(configFlag&0x01)
            {
                if(WiFi_sendReadCommand(readIndex)==0)
                {
                    configFlag=0x02; 
                    writeIndex=0;
                    process_post(&wifi_process,WiFiConfig_event,NULL);
                }
            }
            else if((configFlag&0x02))
            {
                if(WiFi_writeConfig()==0)
                {
                    if(writeIndex>0)
                    {
                        process_start(&getHwType_process,NULL);
                        PROCESS_EXIT();
                    }
                    else
                    {
                        configFlag=0x80;
                        readIndex=0;
                        process_post(&wifi_process,WiFiConfig_event,NULL);
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
                    _zxbee_onrecv_fun(2,pdata, dataLen);
                }
            }
            else if (memcmp(pdata, "OK", 2) == 0)
            {
                if(configFlag&0x01)
                {
                    readIndex++;
                    process_post(&wifi_process,WiFiConfig_event,NULL);
                }
                else if(configFlag&0x02)
                {
                    process_post(&wifi_process,WiFiConfig_event,NULL);
                }
            }
            else if(memcmp(pdata, "EER:", 4) == 0)
            {
                process_post(&wifi_process,WiFiConfig_event,NULL);
            }
            else if (memcmp(pdata, "+LINK:", 6) == 0)
            {
                wifi_link = atoi(&pdata[6]);
            }
            else if (memcmp(pdata, "+MAC:", 5) == 0)
            {
                memcpy(wifi_mac,&pdata[5],17);
                wifi_mac[17]=0;
            }
            else if (memcmp(pdata, "+SSID:", 6) == 0)
            {
                len=0;
                temp=&pdata[7];
                while(*temp!='"')
                {
                    temp++;
                    len++;
                }
                memcpy(wifi_ssid,&pdata[7],len);
                wifi_ssid[len+1]=0;
            }
            else if (memcmp(pdata, "+KEYTYPE:", 9) == 0)
            {
                wifi_keyType=atoi(&pdata[9]);
            }
            else if (memcmp(pdata, "+KEY:", 5) == 0)
            {
                len=0;
                temp=&pdata[6];
                while(*temp!='"')
                {
                    temp++;
                    len++;
                }
                memcpy(wifi_key,&pdata[6],len);
                wifi_key[len+1]=0;
            }
            else if (memcmp(pdata, "+IP:", 4) == 0)
            {
                len=0;
                temp=&pdata[4];
                while(*temp!='\r')
                {
                    temp++;
                    len++;
                }
                memcpy(wifi_ip,&pdata[4],len);
                wifi_ip[len+1]=0;
            }
            else if (memcmp(pdata, "+SIP:", 5) == 0)
            {
                len=0;
                temp=&pdata[5];
                while(*temp!='\r')
                {
                    temp++;
                    len++;
                }
                memcpy(wifi_sip,&pdata[5],len);
                wifi_sip[len+1]=0;
            }
        }
    }
    PROCESS_END();
}