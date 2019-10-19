/*********************************************************************************************
* 文件：sensor.h
* 作者：
* 说明：通用传感器控制接口程序头文件
* 修改：
* 注释：
*********************************************************************************************/
#ifndef _sensor_h_
#define _sensor_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "zxbee.h"

#define SENSOR_TYPE  "710"                                      // 传感器编码

char *sensor_type(void);
void sensor_init(void);
void sensor_poll(unsigned int t);
unsigned short sensor_check(void);
void sensorLinkOn(void);

#endif