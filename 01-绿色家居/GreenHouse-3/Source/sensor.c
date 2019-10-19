/*********************************************************************************************
* 文件：sensor.c
* 作者：fuyou 2018.10.24
* 说明：GreenHouse节点C驱动驱动
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "sensor.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static uint8  D0 = 0xFF;                                        // 默认打开主动上报功能
static uint8  D1 = 0;                                           // 继电器初始状态为全关
static char  A0[16]={0};                                       // A0存储卡号
static uint16 V0 = 30;                                          // V0设置为上报时间间隔，默认为30s
uint8 V1 = 0;                                                   // 频道
uint8 V2 = 0;                                                   // 音量
uint8 oledDisplay = 0;
uint16 cardTick=0;
uint16 ElectronicLockTick=0;
uint16 BeepTick=0;
/*********************************************************************************************
* 名称：updateV0()
* 功能：更新V0的值
* 参数：*val -- 待更新的变量
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateV0(char *val)
{
    //将字符串变量val解析转换为整型变量赋值
    V0 = atoi(val);                                 // 获取数据上报时间更改值
}
/*********************************************************************************************
* 名称：sensorInit()
* 功能：传感器硬件初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorInit(void)
{
    led_init();                                                 // LED灯初始化
    beep_init();                                                // 蜂鸣器初始化
    ElectronicLock_init();                                      // 电子锁初始化
    OLED_Init();
    RFID7941Init();                                             // RFID初始化

    // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
    osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
}
/*********************************************************************************************
* 名称：sensorLinkOn()
* 功能：传感器节点入网成功调用函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorLinkOn(void)
{
    sensorUpdate();
}
/*********************************************************************************************
* 名称：sensorUpdate()
* 功能：处理主动上报的数据
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorUpdate(void)
{
    char pData[16];
    char *p = pData;

    ZXBeeBegin();

    sprintf(p, "%u", D1);                                  // 上报控制编码
    ZXBeeAdd("D1", p);

    p = ZXBeeEnd();                                               // 智云数据帧格式包尾
    if (p != NULL)
    {
        ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
    }
}

/*********************************************************************************************
* 名称：idCardHandle()
* 功能：id卡处理程序
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void idCardHandle()
{
    char idID[5]= {0};
    static char lastIdID[5]= {0};

    if (RFID7941CheckCard125kHzRsp(idID) > 0)                   //非同一张卡片
    {
        if (memcmp(lastIdID, idID, 5) != 0)
        {
            sprintf(A0, "%02X%02X%02X%02X%02X", idID[0],idID[1],idID[2],idID[3],idID[4]);
            ZXBeeAdd("A0", A0);
            memcpy(lastIdID, idID, 5);
            
            BeepTick=3;
        }
        cardTick = 5;
    }
    else
    {
        memcpy(lastIdID, 0, 5);
    }
    RFID7941CheckCard125kHzReq();
}

/*********************************************************************************************
* 名称：icCardHandle()
* 功能：ic卡处理程序
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void icCardHandle()
{
    char icID[16]= {0};
    static char lastIcID[16]= {0};

    if (RFID7941CheckCard1356MHzRsp(icID) > 0)
    {
        if (memcmp(lastIcID, icID, 4) != 0)                    //非同一张卡片
        {
            sprintf(A0,"%02X%02X%02X%02X",
                    icID[0],icID[1],icID[2],icID[3]);
            ZXBeeAdd("A0", A0);
            memcpy(lastIcID, icID, 4);
            
            BeepTick=3;
        }
        cardTick = 5;
    }
    else
    {
        memcpy(lastIcID, 0, 4);
    }
    RFID7941CheckCard1356MHzReq();
}

/*********************************************************************************************
* 名称：sensorCheck()
* 功能：传感器监测
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorCheck(void)
{
    char buf[32]={0};
    static uint8 cardIndex = 0;
    
    ZXBeeBegin();
    switch(cardIndex)
    {
        case 1:
            icCardHandle();
            break;
        default:
            idCardHandle();
    }
    char *p = ZXBeeEnd();
    if (p != NULL)
    {
        ZXBeeInfSend(p, strlen(p));
    }

    if(cardTick>0)
    {
        cardTick--;
    }
    else
    {
        cardTick=5;
        cardIndex++;
        if(cardIndex>1)
            cardIndex=0;
    }
    
    if(BeepTick>1)
    {
        BeepTick--;
        beep_on();
    }
    else if(BeepTick==1)
    {
        BeepTick--;
        beep_off();
    }
    
    if(oledDisplay)
    {
        sprintf(buf,"%3u",V1);
        OLED_ShowString(95-18,0,(uint8*)buf,8);
        sprintf(buf,"%3u",V2);
        OLED_ShowString(95-18,3,(uint8*)buf,8);
        OLED_ShowString(48-8,1,"TV",16);
    }
}
/*********************************************************************************************
* 名称：sensorControl()
* 功能：传感器控制
* 参数：cmd - 控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorControl(uint8 cmd)
{
    // 根据cmd参数处理对应的控制程序
    if(cmd&0x01)
        led_on(0x01);
    else
        led_off(0x01);
    
    if(cmd&0x02)
        led_on(0x02);
    else
        led_off(0x02);
    
    if(cmd&0x04)
        led_on(0x04);
    else
        led_off(0x04);
    
    if(cmd&0x08)
        led_on(0x08);
    else
        led_off(0x08);
    
    if(cmd&0x10)
        beep_on();
    else
        beep_off();
    
    if(cmd&0x20)
    {
        if(ElectronicLockTick==0)
        {
            ElectronicLockTick = 10*2;
            ElectronicLock_on();
        }
    }
    else
    {
        ElectronicLock_off();
    }
    
    if(cmd&0x40)
    {
        if(oledDisplay==0)
        {
            oledDisplay=1;
            OLED_PEN_COLOR=BLACK;
            OLED_Clear();
        }
    }
    else
    {
        oledDisplay=0;
        OLED_PEN_COLOR=WHITE;
        OLED_Clear();
    }
}
/*********************************************************************************************
* 名称：ZXBeeUserProcess()
* 功能：解析收到的控制命令
* 参数：*ptag -- 控制命令名称
*       *pval -- 控制命令参数
* 返回：ret -- p字符串长度
* 修改：
* 注释：
*********************************************************************************************/
int ZXBeeUserProcess(char *ptag, char *pval)
{
    int val;
    int ret = 0;
    char pData[16];
    char *p = pData;

    // 将字符串变量pval解析转换为整型变量赋值
    val = atoi(pval);
    // 控制命令解析
    if (0 == strcmp("CD0", ptag))                                 // 对D0的位进行操作，CD0表示位清零操作
    {
        D0 &= ~val;
    }
    if (0 == strcmp("OD0", ptag))                                 // 对D0的位进行操作，OD0表示位置一操作
    {
        D0 |= val;
    }
    if (0 == strcmp("D0", ptag))                                  // 查询上报使能编码
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", D0);
            ZXBeeAdd("D0", p);
        }
    }
    if (0 == strcmp("CD1", ptag))                                 // 对D1的位进行操作，CD1表示位清零操作
    {
        D1 &= ~val;
        sensorControl(D1);                                          // 处理执行命令
    }
    if (0 == strcmp("OD1", ptag))                                 // 对D1的位进行操作，OD1表示位置一操作
    {
        D1 |= val;
        sensorControl(D1);                                          // 处理执行命令
    }
    if (0 == strcmp("D1", ptag))                                  // 查询执行器命令编码
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", D1);
            ZXBeeAdd("D1", p);
        }
    }
    if (0 == strcmp("V0", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V0);                         	// 上报时间间隔
            ZXBeeAdd("V0", p);
        }
        else
        {
            updateV0(pval);
        }
    }
    if (0 == strcmp("V1", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V1);                         	// 上报时间间隔
            ZXBeeAdd("V1", p);
        }
        else
        {
            V1 = val;
        }
    }
    if (0 == strcmp("V2", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            ret = sprintf(p, "%u", V2);                         	// 上报时间间隔
            ZXBeeAdd("V2", p);
        }
        else
        {
            V2 = val;
            if(V2>100) V2=100;
        }
    }
    return ret;
}
/*********************************************************************************************
* 名称：MyEventProcess()
* 功能：自定义事件处理
* 参数：event -- 事件编号
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void MyEventProcess( uint16 event )
{
    if (event & MY_REPORT_EVT)
    {
        sensorUpdate();
        //启动定时器，触发事件：MY_REPORT_EVT
        osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, V0*1000);
    }
    if(event & MY_CHECK_EVT)
    {
        sensorCheck();
        osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
    }
}