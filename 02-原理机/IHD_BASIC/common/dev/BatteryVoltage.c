#include "BatteryVoltage.h"

float BatteryVoltage=0;

extern void AdcInit(void);
extern unsigned short AdcGet(unsigned char ch);

/*********************************************************************************************
* 名称: BatteryVotage_Get
* 功能: 获取电池电压
* 参数: 无
* 返回: 电池电压值，单位伏特（V）
* 修改:
* 注释:
*********************************************************************************************/
float BatteryVotage_Get()
{
    return BatteryVoltage;
}

/*********************************************************************************************
申明进程名
*********************************************************************************************/
PROCESS(BatteryVoltageUpdate, "BatteryVoltageUpdate");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 板载传感器处理进程
* 参数: onboard_sensors, ev, data
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(BatteryVoltageUpdate, ev, data)
{
    static unsigned short adcValue=0;
    static struct etimer VBATT_timer;
    
    PROCESS_BEGIN();
    
    //Bsp_ADCInit();
    
    while(1)
    {
        adcValue = AdcGet(1);
        BatteryVoltage = (float)adcValue*(3.3/4095.0)*13.6/3.6;
        etimer_set(&VBATT_timer,100);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
    }
    
    PROCESS_END();
}