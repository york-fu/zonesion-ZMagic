#include "lora-net.h"

static unsigned short lora_nodeID=0;//地址:0x01-0xFE
static unsigned short lora_netID=0; //网络ID
static unsigned short lora_fp=0;    //基频
static unsigned char lora_pv=0;     //发射功率:1-20
static unsigned char lora_sf=0;     //扩频因子:6-12
static unsigned char lora_cr=0;     //编码率:1-4
static unsigned char lora_ps=0;     //前导码长度
static unsigned char lora_bw=0;     //带宽:0-9
static unsigned char lora_hop=0;    //跳频开关:0-1
static unsigned char lora_hopTab[40]={0};//跳频表

unsigned short loraGet_nodeID()
{
    return lora_nodeID;
}

unsigned short loraGet_netID()
{
    return lora_netID;
}

unsigned short loraGet_fp()
{
    return lora_fp;
}

unsigned char loraGet_pv()
{
    return lora_pv;
}

unsigned char loraGet_sf()
{
    return lora_sf;
}

unsigned char loraGet_cr()
{
    return lora_cr;
}

unsigned char loraGet_ps()
{
    return lora_ps;
}

unsigned char loraGet_bw()
{
    return lora_bw;
}

unsigned char loraGet_hop()
{
    return lora_hop;
}

unsigned char* loraGet_hopTab()
{
    return lora_hopTab;
}


uint8_t lora_sendReadCommand(uint8_t index)
{
    static const char command_list[][30] = {
        "AT+FP?\r\n",       //基频
        "AT+PV?\r\n",       //发射功率
        "AT+SF?\r\n",       //扩频因子
        "AT+CR?\r\n",       //编码率
        "AT+PS?\r\n",       //前导码长度
        "AT+BW?\r\n",       //带宽
        "AT+HOP?\r\n",      //跳频开关
        "AT+HOPTAB?\r\n",   //调频表
        "AT+NETID?\r\n",    //网络ID
        "AT+NODEID?\r\n"    //地址
    };
    if(index<(sizeof(command_list)/sizeof(command_list[0])))
    {
        rfUartSendString(3,(char*)command_list[index], strlen(command_list[index]));
        return 1;
    }
    return 0;
}

uint8_t lora_writeConfig()
{
#if !XLABTOOLS_ON   
    char pbuf[20]={0};
    static uint8_t index=0,configFlag=0;
    uint8_t len=0;
    
    switch(index)
    {
        case 0:
            index++;
            if(lora_netID != LORA_CONFIG_NETID)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+NETID=%u\r\n",LORA_CONFIG_NETID);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 1:
            index++;
            if(lora_fp != LORA_CONFIG_FP)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+FP=%u\r\n",LORA_CONFIG_FP);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 2:
            index++;
            if(lora_pv != LORA_CONFIG_PV)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+PV=%u\r\n",LORA_CONFIG_PV);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 3:
            index++;
            if(lora_sf != LORA_CONFIG_SF)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+SF=%u\r\n",LORA_CONFIG_SF);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 4:
            index++;
            if(lora_cr != LORA_CONFIG_CR)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+CR=%u\r\n",LORA_CONFIG_CR);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 5:
            index++;
            if(lora_ps != LORA_CONFIG_PS)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+PS=%u\r\n",LORA_CONFIG_PS);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 6:
            index++;
            if(lora_bw != LORA_CONFIG_BW)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+BW=%u\r\n",LORA_CONFIG_BW);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 7:
            index++;
            if(lora_hop != LORA_CONFIG_HOP)
            {
                configFlag=1;
                len=sprintf(pbuf,"AT+HOP==%u\r\n",LORA_CONFIG_HOP);
                rfUartSendString(3,pbuf,len);
                return 1;
            }
        case 8:
            index++;
            if(memcmp(lora_hopTab,LORA_CONFIG_HOPTAB,strlen(LORA_CONFIG_HOPTAB)))
            {
                configFlag=1;
                strcpy(pbuf,"AT+HOPTAB=");
                strcat(pbuf,LORA_CONFIG_HOPTAB);
                strcat(pbuf,"\r\n");
                rfUartSendString(3,pbuf,strlen(pbuf));
                return 1;
            }
        case 9:
            index++;
            if(configFlag)
            {
                configFlag=1;
                strcpy(pbuf,"at+envsave\r\n");
                rfUartSendString(3,pbuf,strlen(pbuf));
                return 1;
            }
        case 10:
            index++;
            if(configFlag)
            {
                configFlag=0;
                strcpy(pbuf,"at+reset\r\n");
                rfUartSendString(3,pbuf,strlen(pbuf));
                return 1;
            }
    }
    index=0;
#endif
    return 0;
}

void lora_UaerProgram()
{
    process_start(&sensor_process,NULL);
}

PROCESS(lora_process, "lora_process");

PROCESS_THREAD(lora_process, ev, data)
{
    static process_event_t loraConfig_event;
    static unsigned char readIndex=0,writeIndex=0;
    static unsigned char configFlag=0;

    PROCESS_BEGIN();
    
    readIndex=0;
    writeIndex=0;
    configFlag=0x01;
    loraConfig_event=process_alloc_event();
    process_post(&lora_process,loraConfig_event,NULL);
    
    while (1)
    {
        PROCESS_YIELD();
        if (ev==loraConfig_event)
        {
            if((configFlag&0x80))
            {
                configFlag=0;
                lora_UaerProgram();
            }
            else if(configFlag&0x01)
            {
                if(lora_sendReadCommand(readIndex)==0)
                {
                    configFlag=0x02;
                    writeIndex=0;
                    process_post(&lora_process,loraConfig_event,NULL);
                }
            }
            else if(configFlag&0x02)
            {
                if(lora_writeConfig()==0)
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
                        process_post(&lora_process,loraConfig_event,NULL);
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
                    _zxbee_onrecv_fun(3,pdata, dataLen);
                }
            }
            else if (memcmp(pdata, "OK", 2) == 0)
            {
                if(configFlag&0x01)
                {
                    readIndex++;
                    process_post(&lora_process,loraConfig_event,NULL);
                }
                else if(configFlag&0x02)
                {
                    process_post(&lora_process,loraConfig_event,NULL);
                }
            }
            else if(memcmp(pdata, "EER:", 4) == 0)
            {
                process_post(&lora_process,loraConfig_event,NULL);
            }
            else if (memcmp(pdata, "+FP:", 4) == 0)
            {
                lora_fp=atoi(&pdata[4]);
            }
            else if (memcmp(pdata, "+PV:", 4) == 0)
            {
                lora_pv=atoi(&pdata[4]);
            }
            else if (memcmp(pdata, "+SF:", 4) == 0)
            {
                lora_sf=atoi(&pdata[4]);
            }
            else if (memcmp(pdata, "+CR:", 4) == 0)
            {
                lora_cr=atoi(&pdata[4]);
            }
            else if (memcmp(pdata, "+PS:", 4) == 0)
            {
                lora_ps=atoi(&pdata[4]);
            }
            else if (memcmp(pdata, "+BW:", 4) == 0)
            {
                lora_bw=atoi(&pdata[4]);
            }
            else if (memcmp(pdata, "+HOP:", 5) == 0)
            {
                lora_hop=atoi(&pdata[5]);
            }
            else if (memcmp(pdata, "+HOPTAB:", 8) == 0)
            {
                memcpy(lora_hopTab,&pdata[8],39);
                lora_hopTab[39]=0;
            }
            else if (memcmp(pdata, "+NETID:", 7) == 0)
            {
                lora_netID=atoi(&pdata[7]);
            }
            else if (memcmp(pdata, "+NODEID:", 8) == 0)
            {
                lora_nodeID=atoi(&pdata[8]);
            }
        }
    }
    PROCESS_END();
}