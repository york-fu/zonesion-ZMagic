/*********************************************************************************************
* 文件：sensor.c
* 作者：fuyou 2018.10.24
* 说明：GreenHouse节点B驱动驱动
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
static uint8  A0 = 0;                                           // 人体红外检测
static uint8  A1 = 0;                                           // 震动检测
static uint8  A2 = 0;                                           // 霍尔检测
static uint8  A3 = 0;                                           // 火焰检测
static uint8  A4 = 0;                                           // 可燃气体检测
static uint8  A5 = 0;                                           // 红外光栅检测
static uint16 V0 = 30;                                          // V0设置为上报时间间隔，默认为30s
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
    V0 = atoi(val);                                               // 获取数据上报时间更改值
}
/*********************************************************************************************
* 名称：updateA0()
* 功能：更新A0的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA0(void)
{
    static uint32 ct = 0;
    
    A0 = get_infrared_status();
    if (A0 != 0)
    {
        ct = osal_GetSystemClock();
    }
    else if (osal_GetSystemClock() > ct+1000)
    {
        ct = 0;
        A0 = 0;
    }
    else
    {
        A0 = 1;
    }
}
/*********************************************************************************************
* 名称：updateA1()
* 功能：更新A1的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA1(void)
{
    static uint32 ct = 0;
    
    A1 = get_vibration_status();
    if (A1 != 0)
    {
        ct = osal_GetSystemClock();
    }
    else if (osal_GetSystemClock() > ct+2000)
    {
        ct = 0;
        A1 = 0;
    }
    else
    {
        A1 = 1;
    }
}
/*********************************************************************************************
* 名称：updateA2()
* 功能：更新A2的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA2(void)
{
    A2 = get_hall_status();                                     // 更新霍尔传感器的值
}
/*********************************************************************************************
* 名称：updateA3()
* 功能：更新A3的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA3(void)
{
    static uint32 ct = 0;
    
    A3 = get_flame_status();                                    // 更新火焰传感器的值
    if (A3 != 0)
    {
        ct = osal_GetSystemClock();
    }
    else if (osal_GetSystemClock() > ct+2000)
    {
        ct = 0;
        A3 = 0;
    }
    else
    {
        A3 = 1;
    }
}
/*********************************************************************************************
* 名称：updateA4()
* 功能：更新A4的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA4(void)
{
    A4 = get_combustiblegas_data();                               // 更新燃气传感器的值
}
/*********************************************************************************************
* 名称：updateA5()
* 功能：更新A5的值
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void updateA5(void)                                             // 更新光栅传感器的值
{
    A5 = get_grating_status();
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
    // 初始化传感器代码
    infrared_init();                                            // 人体红外传感器初始化
    hall_init();                                                // 霍尔传感器初始化
    vibration_init();                                           // 振动传感器初始化
    flame_init();                                               // 火焰传感器初始化
    combustiblegas_init();                                      // 燃气传感器初始化
    grating_init();                                             // 光栅传感器初始化
    rgb_init();                                                 // RGB灯初始化
    
    // 启动定时器，触发传感器上报数据事件：MY_REPORT_EVT
    osal_start_timerEx(sapi_TaskID, MY_REPORT_EVT, (uint16)((osal_rand()%10) * 1000));
    // 启动定时器，触发传感器监测事件：MY_CHECK_EVT
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
    
    ZXBeeBegin();                                                 // 智云数据帧格式包头
    
    // 根据D0的位状态判定需要主动上报的数值
    if ((D0 & 0x01) == 0x01)                                      // 若温度上报允许，则pData的数据包中添加温度数据
    {
        updateA0();
        sprintf(p, "%u", A0);
        ZXBeeAdd("A0", p);
    }
    if ((D0 & 0x02) == 0x02)                                      // 若湿度上报允许，则pData的数据包中添加湿度数据
    {
        updateA1();
        sprintf(p, "%u", A1);
        ZXBeeAdd("A1", p);
    }
    if ((D0 & 0x04) == 0x04)                                      // 若光照上报允许，则pData的数据包中添加光照数据
    {
        updateA2();
        sprintf(p, "%u", A2);
        ZXBeeAdd("A2", p);
    }
    if ((D0 & 0x08) == 0x08)                                      // 若空气质量上报允许，则pData的数据包中添加空气质量数据
    {
        updateA3();
        sprintf(p, "%u", A3);
        ZXBeeAdd("A3", p);
    }
    if ((D0 & 0x10) == 0x10)                                      // 若大气压力上报允许，则pData的数据包中添加大气压力数据
    {
        updateA4();
        sprintf(p, "%u", A4);
        ZXBeeAdd("A4", p);
    }
    if ((D0 & 0x10) == 0x10)                                      // 若跌倒状态上报允许，则pData的数据包中添加跌倒状态数据
    {
        updateA5();
        sprintf(p, "%u", A5);
        ZXBeeAdd("A5", p);
    }
    
    sprintf(p, "%u", D1);                                  // 上报控制编码
    ZXBeeAdd("D1", p);
    
    p = ZXBeeEnd();                                               // 智云数据帧格式包尾
    if (p != NULL)
    {
        ZXBeeInfSend(p, strlen(p));	                                // 将需要上传的数据进行打包操作，并通过zb_SendDataRequest()发送到协调器
    }
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
    static char lastA0 = 0,lastA1=0,lastA2=0,lastA3=0,lastA4=0,lastA5=0;
    static uint32 ct0=0, ct1=0, ct2=0, ct3=0, ct4=0, ct5=0;
    char pData[16];
    char *p = pData;
    
    updateA0();
    updateA1();
    updateA2();
    updateA3();
    updateA4();
    updateA5();
    
    ZXBeeBegin();
    
    if (lastA0 != A0 || ( ct0 != 0 && osal_GetSystemClock() > (ct0+3000)))
    {
        if (D0 & 0x01)
        {
            sprintf(p, "%u", A0);
            ZXBeeAdd("A0", p);
            ct0 = osal_GetSystemClock();
        }
        if (A0 == 0)
        {
            ct0 = 0;
        }
        lastA0 = A0;
    }
    if (lastA1 != A1 || (ct1 != 0 && osal_GetSystemClock() > (ct1+3000)))    // 振动传感器监测
    {
        if (D0 & 0x02)
        {
            sprintf(p, "%u", A1);
            ZXBeeAdd("A1", p);
            ct1 = osal_GetSystemClock();
        }
        if (A1 == 0)
        {
            ct1 = 0;
        }
        lastA1 = A1;
    }
    if (lastA2 != A2 || (ct2 != 0 && osal_GetSystemClock() > (ct2+3000)))    // 霍尔传感器监测
    {
        if (D0 & 0x04)
        {
            sprintf(p, "%u", A2);
            ZXBeeAdd("A2", p);
            ct2 = osal_GetSystemClock();
        }
        if (A2 == 0)
        {
            ct2 = 0;
        }
        lastA2 = A2;
    }
    
    if (lastA3 != A3 || (ct3 != 0 && osal_GetSystemClock() > (ct3+3000)))    // 火焰传感器监测
    {
        if (D0 & 0x08)
        {
            sprintf(p, "%u", A3);
            ZXBeeAdd("A3", p);
            ct3 = osal_GetSystemClock();
        }
        if (A3 == 0)
        {
            ct3 = 0;
        }
        lastA3 = A3;
    }
    if (lastA4 != A4 || (ct4 != 0 && osal_GetSystemClock() > (ct4+3000)))    // 燃气传感器监测
    {
        if (D0 & 0x10)
        {
            sprintf(p, "%u", A4);
            ZXBeeAdd("A4", p);
            ct4 = osal_GetSystemClock();
        }
        if (A4 == 0)
        {
            ct4 = 0;
        }
        lastA4 = A4;
    }
    if (lastA5 != A5 || (ct5 != 0 && osal_GetSystemClock() > (ct5+3000)))    // 光栅传感器监测
    {
        if (D0 & 0x01)
        {
            sprintf(p, "%u", A5);
            ZXBeeAdd("A5", p);
            ct5 = osal_GetSystemClock();
        }
        if (A5 == 0)
        {
            ct5 = 0;
        }
        lastA5 = A5;
    }
    
    p = ZXBeeEnd();
    if (p != NULL)
    {
        int len = strlen(p);
        ZXBeeInfSend(p, len);
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
    if((D1&0x03)==0x01)     //01
    {
        rgb1_on(0x01);
    }
    else
    {
        rgb1_off(0x01);
    }
    if((D1&0x03)==0x02)     //10
    {
        rgb1_on(0x02);
    }
    else
    {
        rgb1_off(0x02);
    }
    if((D1&0x03)==0x03)     //11
    {
        rgb1_on(0x04);
    }
    else
    {
        rgb1_off(0x04);
    }
    if((D1&0x0c)==0x04)     //0100
    {
        rgb2_on(0x01);
    }
    else
    {
        rgb2_off(0x01);
    }
    if((D1&0x0c)==0x08)     //1000
    {
        rgb2_on(0x02);
    }
    else
    {
        rgb2_off(0x02);
    }
    if((D1&0x0c)==0x0c)     //1100
    {
        rgb2_on(0x04);
    }
    else
    {
        rgb2_off(0x04);
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
    if (0 == strcmp("A0", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            updateA0();
            ret = sprintf(p, "%u", A0);
            ZXBeeAdd("A0", p);
        }
    }
    if (0 == strcmp("A1", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            updateA1();
            ret = sprintf(p, "%u", A1);
            ZXBeeAdd("A1", p);
        }
    }
    if (0 == strcmp("A2", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            updateA2();
            ret = sprintf(p, "%u", A2);
            ZXBeeAdd("A2", p);
        }
    }
    if (0 == strcmp("A3", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            updateA3();
            ret = sprintf(p, "%u", A3);
            ZXBeeAdd("A3", p);
        }
    }
    if (0 == strcmp("A4", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            updateA4();
            ret = sprintf(p, "%u", A4);
            ZXBeeAdd("A4", p);
        }
    }
    if (0 == strcmp("A5", ptag))
    {
        if (0 == strcmp("?", pval))
        {
            updateA5();
            ret = sprintf(p, "%u", A5);
            ZXBeeAdd("A5", p);
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
    if (event & MY_CHECK_EVT)
    {
        sensorCheck();
        // 启动定时器，触发事件：MY_CHECK_EVT
        osal_start_timerEx(sapi_TaskID, MY_CHECK_EVT, 100);
    }
}