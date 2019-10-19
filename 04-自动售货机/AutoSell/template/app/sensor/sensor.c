/*********************************************************************************************
* 文件：sensor.c
* 作者：
* 说明：通用传感器控制接口程序
* 修改：
* 注释：
*********************************************************************************************/
#include "sensor.h"
#include "Relay.h"
#include "BatteryVoltage.h"
#include "lte_zhiyun.h"
#include "fml_stepmotor/fml_stepmotor.h"
#include "SellDLG.h"

uint8_t D0 = 0xff;                                              // 主动上报使能，默认只允许A0主动上报
uint8_t D1 = 0;                                                 // 默认关闭控制类传感器
uint16_t V0 = 30;                                               // 主动上报时间间隔，单位秒,0不主动上报
double A0 = 0;                                                  // 售卖总金额
float A7=0;                                                     // 板载电压

/*********************************************************************************************
* 名称：sensor_type()
* 功能：
* 参数：
* 返回：返回传感器类型，3字节字符串表示
* 修改：
* 注释：
*********************************************************************************************/
char *sensor_type(void)
{
  return SENSOR_TYPE;                                           //返回传感器类型
}

/*********************************************************************************************
* 名称：updateA0
* 功能：更新A0的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA0(void)
{
  A0 = Sell_MoneyTotalGet();
}

/*********************************************************************************************
* 名称：updateA7
* 功能：更新A7的值
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void updateA7(void)
{
  A7 = BatteryVotage_Get();                                   // 更新A7 电池电压
}


/*********************************************************************************************
* 名称：sensor_init()
* 功能：
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void sensor_init(void)
{
  relay_init();                                                 //初始化控制类传感器（继电器）
}


/*********************************************************************************************
* 名称：sensor_control()
* 功能：传感器控制
* 参数：cmd - 控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensor_control(unsigned char cmd)
{
  if((cmd & (3<<0)) == (3<<0))
  {
    Stepmotor_Run(1,1,1000);
  }
  else if(cmd & (1<<0))
  {
    Stepmotor_Run(1,0,1000);
  }
  
  if((cmd & (3<<2)) == (3<<2))
  {
    Stepmotor_Run(2,1,1000);
  }
  else if(cmd & (1<<2))
  {
    Stepmotor_Run(2,0,1000);
  }
  
  if((cmd & (3<<4)) == (3<<4))
  {
    Stepmotor_Run(3,1,1000);
  }
  else if(cmd & (1<<4))
  {
    Stepmotor_Run(3,0,1000);
  }
  
  D1 &= ~0x3f;
    
  
  if(cmd & 0x40)
  {
    relay_on(1);
  }
  else
  {
    relay_off(1);
  }
  if(cmd & 0x80)
  {
    relay_on(2);
  }
  else
  {
    relay_off(2);
  }
}

/*********************************************************************************************
* 名称：sensorLinkOn
* 功能：
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void sensorLinkOn()
{
  char buf[64]={0};
  updateA0();
  updateA7();
  
  zxbeeBegin();
  if (D0 & 0x01)
  {
    sprintf(buf, "%.2f", A0);
    zxbeeAdd("A0", buf);
  }
  if (D0 & 0x80)
  {
    sprintf(buf, "%.1f", A7);
    zxbeeAdd("A7", buf);
  }
  char *p = zxbeeEnd();
  if (p != NULL)
  {
    RFSendData(p);                             //发送数据
  }
}

/*********************************************************************************************
* 名称：sensor_poll()
* 功能：轮询传感器，并主动上报传感器数据
* 参数：t: 调用次数
* 返回：
* 修改：
* 注释：此函数每秒钟调用1次，t为调用次数
*********************************************************************************************/
void sensor_poll(unsigned int t)
{
  char buf[64]={0};
  updateA0();
  updateA7();
  
  if (V0 != 0)
  {
    if (t % V0 == 0)
    {
      zxbeeBegin();
      if (D0 & 0x01)
      {
        sprintf(buf, "%.2f", A0);
        zxbeeAdd("A0", buf);
      }
      if (D0 & 0x80)
      {
        sprintf(buf, "%.1f", A7);
        zxbeeAdd("A7", buf);
      }
      char *p = zxbeeEnd();
      if (p != NULL)
      {
        RFSendData(p);                             //发送数据
      }
    }
  }
}

/*********************************************************************************************
* 名称：sensor_check()
* 功能：周期性检查函数，可设定轮询时间
* 参数：无
* 返回：设置轮询的时间，单位ms,范围:1-65535
* 修改：
* 注释：此函数用于需要快速做出响应的传感器
*********************************************************************************************/
unsigned short sensor_check()
{
  return 100;                                                 //返回值决定下次调用时间，此处为100ms
}

/*********************************************************************************************
* 名称：z_process_command_call()
* 功能：处理上层应用发过来的指令
* 参数：ptag: 指令标识 D0，D1， A0 ...
*       pval: 指令值， “？”表示读取，
*       obuf: 指令处理结果存放地址
* 返回：>0指令处理结果返回数据长度，0：没有返回数据，<0：不支持指令。
* 修改：
* 注释：
*********************************************************************************************/
int z_process_command_call(char* ptag, char* pval, char* obuf)
{
  int ret = -1;
  
  if (memcmp(ptag, "D0", 2) == 0)
  {
    if (pval[0] == '?')
    {
      ret = sprintf(obuf, "D0=%d", D0);
    }
  }
  if (memcmp(ptag, "CD0", 3) == 0)
  {
    int v = atoi(pval);
    if (v > 0)
    {
      D0 &= ~v;
    }
  }
  if (memcmp(ptag, "OD0", 3) == 0)
  {
    int v = atoi(pval);
    if (v > 0)
    {
      D0 |= v;
    }
  }
  if (memcmp(ptag, "D1", 2) == 0)
  {
    if (pval[0] == '?')
    {
      ret = sprintf(obuf, "D1=%d", D1);
    }
  }
  if (memcmp(ptag, "CD1", 3) == 0)  				                    //若检测到CD1指令
  {
    int v = atoi(pval);                                         //获取CD1数据
    D1 &= ~v;                                                   //更新D1数据
    sensor_control(D1);                                         //更新电磁阀直流电机状态
  }
  if (memcmp(ptag, "OD1", 3) == 0)  				                    //若检测到OD1指令
  {
    int v = atoi(pval);                                         //获取OD1数据
    D1 |= v;                                                    //更新D1数据
    sensor_control(D1);                                         //更新电磁阀直流电机状态
  }
  if (memcmp(ptag, "V0", 2) == 0)
  {
    if (pval[0] == '?')
    {
      ret = sprintf(obuf, "V0=%d", V0);
    }
    else
    {
      V0 = atoi(pval);
    }
  }
  if (memcmp(ptag, "V1", 2) == 0)
  {
    if (pval[0] == '?')
    {
      char V1[128] = {0};
      Calender_ZxbeeGet(V1);
      ret = sprintf(obuf, "V1=%s", V1);
    }
    else
    {
      Calender_ZxbeeSet(pval);
    }
  }
  if (memcmp(ptag, "V2", 2) == 0)
  {
    if (pval[0] == '?')
    {
      char V2[128] = {0};
      Alarm_ZxbeeGet(V2);
      ret = sprintf(obuf, "V2=%s", V2);
    }
    else
    {
      Alarm_ZxbeeSet(pval);
    }
  }
  if (memcmp(ptag, "V3", 2) == 0)
  {
    if (pval[0] == '?')
    {
      char V3[32] = {0};
      SellNum_ZxbeeGet(V3);
      ret = sprintf(obuf, "V3=%s", V3);
    }
    else
    {
      SellNum_ZxbeeSet(pval);
    }
  }
  if (memcmp(ptag, "V4", 2) == 0)
  {
    if (pval[0] == '?')
    {
      char V4[32] = {0};
      SellPrice_ZxbeeGet(V4);
      ret = sprintf(obuf, "V4=%s", V4);
    }
    else
    {
      SellPrice_ZxbeeSet(pval);
    }
  }
  if (memcmp(ptag, "V5", 2) == 0)
  {
    if (pval[0] == '?')
    {
      char V5[32] = {0};
      SellPayment_ZxbeeGet(V5);
      ret = sprintf(obuf, "V5=%s", V5);
    }
    else
    {
      SellPayment_ZxbeeSet(pval);
    }
  }
  if (memcmp(ptag, "A0", 2) == 0)
  {
    if (pval[0] == '?')
    {
      ret = sprintf(obuf, "A0=%.2f", A0);
    }
  }
  if (memcmp(ptag, "A7", 2) == 0)
  {
    if (pval[0] == '?')
    {
      ret = sprintf(obuf, "A7=%.1f", A7);
    }
  }
  return ret;
}

