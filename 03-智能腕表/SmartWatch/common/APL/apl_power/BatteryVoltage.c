#include "BatteryVoltage.h"


#define ARR_LEN 10 /*数组长度*/

double BatteryVoltage = 0;
double CpuTemperature = 0;

/*********************************************************************************************
* 名称: BatteryVotage_Get
* 功能: 获取电池电压
* 参数: 无
* 返回: 电池电压值，单位伏特（V）
* 修改:
* 注释:
*********************************************************************************************/
double BatteryVotage_Get()
{
  return BatteryVoltage;
}

double CpuTemperature_Get()
{
  return CpuTemperature;
}

#define elemType uint32_t /*元素类型*/
 
/* 冒泡排序 */
/* 1. 从当前元素起，向后依次比较每一对相邻元素，若逆序则交换 */
/* 2. 对所有元素均重复以上步骤，直至最后一个元素 */
/* elemType arr[]: 排序目标数组; int len: 元素个数 */
void bubbleSort (elemType arr[], int len) 
{
  elemType temp;
  int i, j;
  for (i=0; i<len-1; i++) /* 外循环为排序趟数，len个数进行len-1趟 */
  {
    for (j=0; j<len-1-i; j++)  /* 内循环为每趟比较的次数，第i趟比较len-i次 */
    {
      if (arr[j] > arr[j+1])  /* 相邻元素比较，若逆序则交换（升序为左大于右，逆序反之） */
      {
        temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
}

/*********************************************************************************************
申明进程名
*********************************************************************************************/
PROCESS(PowerProcess, "PowerProcess");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 板载传感器处理进程
* 参数: onboard_sensors, ev, data
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(PowerProcess, ev, data)
{
  static struct etimer VBATT_timer;
  static short tick=0;
  static uint32_t AdcValue=0;
  static uint32_t AdcDataBuffer[ARR_LEN]={0};
  
  PROCESS_BEGIN();
  
  //BspADC_Init();
  
  CpuTemperature = Get_CpuTemprateX(10);
  AdcValue = AdcGetX(1,10);
  BatteryVoltage = (double)AdcValue*(3.3/4095.0)*13.6/3.6;
  AdcValue=0;
  
  while(1)
  {
    etimer_set(&VBATT_timer,50);
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
    
    CpuTemperature = Get_CpuTemprateX(10);
    AdcDataBuffer[tick] = AdcGetX(1,10);
    tick++;
    if(tick>=ARR_LEN)
    {
      bubbleSort(AdcDataBuffer,ARR_LEN);
      for(short i=1;i<ARR_LEN-1;i++)
      {
        AdcValue+=AdcDataBuffer[i];
      }
      AdcValue=(uint16_t)(AdcValue/(ARR_LEN-2));
      BatteryVoltage = (double)AdcValue*(3.3/4095.0)*13.6/3.6;
      tick=0;
      AdcValue=0;
    }
  }
  
  PROCESS_END();
}