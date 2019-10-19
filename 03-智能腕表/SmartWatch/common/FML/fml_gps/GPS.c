/*********************************************************************************************
* 文件：GPS.c
* 作者：zonesion
* 说明：驱动程序
* 修改：
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <string.h>
#include "wk2xxx.h"
#include <stdlib.h>
#include "fml_gps/GPS.h"
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
static char _lat[16] = {0}, _lng[16] = {0};//纬度，经度
static char _gps_status = '0';

/*********************************************************************************************
* 名称：gps_get()
* 功能：获取gps经纬度
* 参数：lat，lng 存放经纬度的缓存
* 返回：0：定位无效，1，定位有效
* 修改：
* 注释：
*********************************************************************************************/
int gps_get(char *lat, char *lng)
{
  if (lat != NULL) strcpy(lat, _lat);
  if (lng != NULL) strcpy(lng, _lng);
  if (_gps_status == '1' || _gps_status == '2') {
    return 1;
  }
  return 0;
}

static char* next(char *ip, char **ot)
{
  char *e = strchr(ip, ',');
  *ot = ip;
  if (e != NULL) {
    *e = '\0';
    return e+1;
  } 
  return NULL;
}

/*********************************************************************************************
* 名称：gps_recv_ch()
* 功能：
* 参数：ch
* 返回：0
* 修改：
* 注释：
*********************************************************************************************/
static int gps_recv_ch(char ch)
{
  static char f_idx = 0;
  static char tag[128];
  if (f_idx == 0) {
    tag[0] = tag[1];
    tag[1] = tag[2];
    tag[2] = tag[3];
    tag[3] = tag[4];
    tag[4] = tag[5];
    tag[5] = ch;
    if (memcmp(tag, "$GNGGA", 6) == 0) {
      f_idx = 6;
      return 0;
    }
  }
  if (f_idx) {
    tag[f_idx++] = ch;
    if (tag[f_idx-2]=='\r' && tag[f_idx-1]=='\n') {
      tag[f_idx] = '\0';
      char *p = tag, *pt;
      p = next(p, &pt);                                         //got $GPGGA
      p = next(p, &pt);                                         //utc time
      p = next(p, &pt);                                         //lat  ddmm.mmmm
      sprintf(_lat, pt);
      p = next(p, &pt);                                         //N/S
      if (*pt=='S') {
         char buf[16];
         sprintf(buf, "-%s", _lat);
         sprintf(_lat, buf);
      }
      p = next(p, &pt);                                         //lng  dddmm.mmmm
      sprintf(_lng, pt);
      p = next(p, &pt);                                         //E/W
      if (*pt == 'W') {
         char buf[16];
         sprintf(buf, "-%s", _lng);
         sprintf(_lng, buf);
      }
      p = next(p, &pt);                                         //st
      _gps_status = *pt;      
      f_idx = 0;
    }
  }
  return 0;
}

void gps_init(void)
{
  Wk2114PortInit(1); 
  Wk2114SetBaud(1,9600);
  Wk2114_SlaveRecv_Set(1,gps_recv_ch);
}