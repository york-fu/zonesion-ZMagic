#ifndef __FINGER_H_
#define __FINGER_H_

#include "stm32f4xx.h"
#include "string.h"
#include "usart.h"

#define MAXRECBYTE      128

#define HEADER          0xEF, 0x01
#define ADDR            0xFF, 0xFF, 0xFF, 0xFF

#define LED_Breathe     0x01                                    // LED功能-呼吸
#define LED_Glint       0x02                                    // LED功能-闪烁
#define LED_LongON      0x03                                    // LED功能-长亮
#define LED_LongOFF     0x04                                    // LED功能-长闭
#define LED_RED         0x04                                    // LED颜色-红色
#define LED_Green       0x02                                    // LED颜色-绿色
#define LED_Blue        0x01                                    // LED颜色-蓝色
#define LED_RG          0x06                                    // LED颜色-红绿灯
#define LED_RB          0x05                                    // LED颜色-红蓝灯
#define LED_GB          0x03                                    // LED颜色-绿蓝灯
#define LED_RGB         0x07                                    // LED颜色-红绿蓝灯
#define LED_OFF         0x00                                    // LED颜色-全灭

#define false           0
#define true            1

typedef struct{
  unsigned char validTempNum;
  unsigned char errorNum;
  unsigned int pageNum;               // 指纹验证页码
  unsigned int verifyScore;          // 得分
  int fingerMode;
}finger_TypeDef;

typedef struct{
  unsigned char usartRecBuff[MAXRECBYTE];
  unsigned int usartRecLen;
}usart_TypeDef;

typedef struct{
  unsigned char led : 1;                                                  // 0-LED 长亮，1-LED 获取图像成功后灭     
  unsigned char pretreatment : 1;                                         // 0-采图图像不预处理，1-采图图像预处理
  unsigned char back : 1;                                                 // 录入过程中是否要求模组在关键步骤返回当前状态：0-要求返回，1-不要求返回
  unsigned char cover : 1;                                                // 新的指纹模板是否覆盖：1-覆盖，0-不覆盖
  unsigned char detection : 1;                                            // 检测新的指纹模板是否已经存在： 1-检测，0-不检测
  unsigned char leave : 1;                                                // 录入过程中,是否要求手指离开才进入下一次指纹图像采集:0-要求离开,1-不要求离开
}param_TypeDef;

typedef struct{
  unsigned char enrollNum : 6;
  unsigned char enrollFlag : 1;
  unsigned char identifyFlag : 1;
}handle_TypeDef;

void finger_init(void);
int finger_SetSendByteCall(void (*func)(char dat));
int finger_usartInput(char ch);//串口接收回调
void finger_uartAnalysis(void);//轮询处理
void finger_cancel(void);//取消当前操作
void finger_autoEnroll(unsigned int id, unsigned char inputNum);//注册指纹，id，采集次数
void finger_autoIdentify(unsigned int id, unsigned char level);//进行一次验证
void finger_resetParam(void);
unsigned int finger_getParam(void);
void finger_setParam(unsigned char cmd);
void finger_deletChar(unsigned int id, unsigned int delNum);//删除指纹，开始id，删除个数
int finger_getEnrollInfo(void);//获取注册结果
int finger_getIdentifyInfo(void);//获取验证结果

#endif
