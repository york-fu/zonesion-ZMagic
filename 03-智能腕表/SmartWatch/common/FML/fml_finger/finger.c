#include "finger.h"

usart_TypeDef usartFinger;
finger_TypeDef fingerStruct;
param_TypeDef paramStruct;
handle_TypeDef handleStruct;

void (*finger_SendByte)(char byte)=NULL;

int finger_SetSendByteCall(void (*func)(char dat))
{
  finger_SendByte = func;
  return 0;
}

void finger_init(void)
{
  // ToDo 串口初始化 baud:57600
	finger_resetParam();
}

void finger_usartSend(unsigned char *buf, unsigned char len)
{
  for(unsigned char i=0; i<len; i++)
    finger_SendByte(buf[i]);
}

// 返回当前指纹模块模式
int finger_getCurrMode(void)
{
  return fingerStruct.fingerMode;
}

// 返回当前有效模板数量
unsigned char finger_getValidTempNum(void)
{
  return fingerStruct.validTempNum;
}

// 返回当前错误编号
unsigned char finger_getErrorNum(void)
{
  return fingerStruct.errorNum;
}

// 清空串口接收BUFF
void finger_delUsartBuff(void)
{
  memset(usartFinger.usartRecBuff, 0, usartFinger.usartRecLen);
  usartFinger.usartRecLen = 0;
}

// 校验和计算
unsigned int finger_checkOut(unsigned char *buf, unsigned char start, unsigned int len)
{
  unsigned int check = 0;
  for(unsigned char i=start; i<len; i++)
  {
    check += buf[i];
  }
  return check;
}

// 初始化参数
void finger_resetParam(void)
{
  paramStruct.led = true;                                    // 默认采集成功后LED熄灭
  paramStruct.pretreatment = false;                          // 默认采集成功后不预处理
  paramStruct.back = false;                                  // 默认在处理关键步骤返回当前状态
  paramStruct.cover = true;                                  // 默认覆盖新的指纹模板
  paramStruct.detection = false;                             // 默认不检测指纹模板是否存在
  paramStruct.leave = false;                                 // 默认要求手指离开
}

// 获取参数
unsigned int finger_getParam(void)
{
  unsigned int param = 0;
  if(paramStruct.led)
    param |= 0x01;
  else
    param &= ~0x01;
  if(paramStruct.pretreatment)
    param |= 0x02;
  else
    param &= ~0x02;
  if(paramStruct.back)
    param |= 0x04;
  else
    param &= ~0x04;
  if(paramStruct.cover)
    param |= 0x08;
  else
    param &= ~0x08;
  if(paramStruct.detection)
    param |= 0x10;
  else
    param &= ~0x10;
  if(paramStruct.leave)
    param |= 0x20;
  else
    param &= ~0x20;
  return param;
}

void finger_setParam(unsigned char cmd)
{
    paramStruct.led = cmd & 0x01;
    paramStruct.pretreatment = cmd & 0x02;
    paramStruct.back = cmd & 0x04;
    paramStruct.cover = cmd & 0x08;
    paramStruct.detection = cmd & 0x10;
    paramStruct.leave = cmd & 0x20;
}

// 获取图像
void finger_getImage(void)
{
  unsigned char getImageOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  //unsigned char *buf = getImageOrder;
  finger_usartSend(getImageOrder, 12);
}

// 注册用获取图像
void finger_getRnrollImage(void)
{
  unsigned char getRnrollImageOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x29, 0x00, 0x2D};
  finger_usartSend(getRnrollImageOrder, 12);
}

// 生成特征
// buffID-缓存区ID号
void finger_getChar(unsigned char buffID)
{
  unsigned int check = 0;
  unsigned char getCharOrder[13] = {HEADER, ADDR, 0x01, 0x00, 0x04, 0x02, buffID, 0x00, 0x00};
  check = finger_checkOut(getCharOrder, 6, 13);
  getCharOrder[11] = check / 256;
  getCharOrder[12] = check % 256;
  finger_usartSend(getCharOrder, 13);
}

// 合成模板
void finger_regMB(void)
{
  unsigned char regMBOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x05, 0x00, 0x09};
  //unsigned char *buf = regMBOrder;
  finger_usartSend(regMBOrder, 12);
}

// 储存模板
// buffID-模板号/pageID-指纹库位置号
void finger_storMB(char buffID, unsigned int pageID)
{
  unsigned int check = 0;
  unsigned char storMBOrder[15] = {HEADER, ADDR, 0x01, 0x00, 0x06, 0x06, buffID, pageID, 0x00, 0x00};
  //unsigned char *buf = storMBOrder;
  check = finger_checkOut(storMBOrder, 6, 15);
  storMBOrder[13] = (unsigned char)check / 256;
  storMBOrder[14] = (unsigned char)check % 256;
  finger_usartSend(storMBOrder, 15);
}

// 搜索模板
// buffID-模板号/pageID-开始位置/searchNum-搜索个数
void finger_searchMB(unsigned char buffID, unsigned int startPage, unsigned int searchNum)
{
  unsigned int check = 0;
  unsigned char searchMBOrder[17] = {HEADER, ADDR, 0x01, 0x00, 0x08, 0x04, buffID, startPage/256, startPage%256, searchNum/256, searchNum%256, 0x00, 0x00};
  //unsigned char *buf = searchMBOrder;
  check = finger_checkOut(searchMBOrder, 6, 17);
  searchMBOrder[15] = (unsigned char)check / 256;
  searchMBOrder[16] = (unsigned char)check % 256;
  finger_usartSend(searchMBOrder, 17);
}

// 读取索引表
// pageNum-页码(0-3)
void finger_readIndexTable(unsigned char pageNum)
{
  unsigned int check = 0;
  unsigned char readIndexTableOrder[13] = {HEADER, ADDR, 0x01, 0x00, 0x04, 0x1F, pageNum, 0x00, 0x00};
  check = finger_checkOut(readIndexTableOrder, 6, 13);
  readIndexTableOrder[11] = (unsigned char)check / 256;
  readIndexTableOrder[12] = (unsigned char)check % 256;
  finger_usartSend(readIndexTableOrder, 13);
  fingerStruct.fingerMode = 4;
}

// 三色灯控制
// function-执行功能/color-颜色/num-执行次数
void finger_controlBLN(unsigned char function, unsigned char color, unsigned char num)
{
  unsigned int check = 0;
  unsigned char controlBLNOrder[15] = {HEADER, ADDR, 0x01, 0x00, 0x06, 0x3C, function, color, num, 0x00, 0x00};
  //unsigned char *buf = controlBLNOrder;
  check = finger_checkOut(controlBLNOrder, 6, 15);
  controlBLNOrder[13] = check / 256;
  controlBLNOrder[14] = check % 256;
  finger_usartSend(controlBLNOrder, 15);
}

// 获取有效模板数量
void finger_validTempleteNum(void)
{
  unsigned char validTempleteNumOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x1D, 0x00, 0x21};
  finger_usartSend(validTempleteNumOrder, 12);
  fingerStruct.fingerMode = 3;
}

// 删除指纹模板
// id-开始删除的id号/num-删除的个数
void finger_deletChar(unsigned int id, unsigned int delNum)
{
  unsigned int check = 0;
  unsigned char deletCharOrder[16] = {HEADER, ADDR, 0x01, 0x00, 0x07, 0x0C, id/256, id%256, delNum/256, delNum%256, 0x00, 0x00};
  check = finger_checkOut(deletCharOrder, 6, 16);
  deletCharOrder[14] = check / 256;
  deletCharOrder[15] = check % 256;
  finger_usartSend(deletCharOrder, 16);
}

// 清空指纹库
void finger_empty(void)
{
  unsigned char empty[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x0d, 0x00, 0x11};
  finger_usartSend(empty, 12);
}

// 检测模组是否工作正常
void finger_getChipEcho(void)
{
  unsigned char getChipEchoOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x35, 0x00 ,0x39};
  finger_usartSend(getChipEchoOrder, 12);
}

// 下载图像数据包
void finger_upImage(void)
{
  unsigned char upImageOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x0A, 0x00, 0x0E};
  finger_usartSend(upImageOrder, 12);
}

// 取消
void finger_cancel(void)
{
  unsigned char cancelOrder[12] = {HEADER, ADDR, 0x01, 0x00, 0x03, 0x30, 0x00, 0x34};
  //unsigned char *buf = cancelOrder;
  finger_usartSend(cancelOrder, 12);
  fingerStruct.fingerMode = 0;
}

// 获取验证指纹ID号
// stepNum-当前步骤编号
void finger_getFingerID(unsigned char stepNum)
{
  switch(stepNum)
  {
  case 0: finger_getRnrollImage(); break;                            // 获取图像
  case 1: finger_getChar(1); break;                                  // 生成特征
  case 2: finger_searchMB(1, 0, 100); break;                         // 搜索模板
  default: finger_cancel(); break;
  }
}

// 自动注册模板
// id-ID号/inputNum-录入次数
void finger_autoEnroll(unsigned int id, unsigned char inputNum)
{
  unsigned int check = 0, param = 0;
  unsigned char autoEnrollOrder[17] = {HEADER, ADDR, 0x01, 0x00, 0x08, 0x31, id/256, id%256, inputNum, 0x00, 0x00, 0x00, 0x00};
  
  finger_cancel();
  param = finger_getParam();
  autoEnrollOrder[13] = param / 256;
  autoEnrollOrder[14] = param % 256;
  check = finger_checkOut(autoEnrollOrder, 6, 16);
  autoEnrollOrder[15] = (unsigned char)check / 256;
  autoEnrollOrder[16] = (unsigned char)check % 256;
  finger_usartSend(autoEnrollOrder, 17);
  handleStruct.enrollFlag = 0;
  handleStruct.enrollNum = 0;
  fingerStruct.fingerMode = 1;
}

// 自动验证指纹
// id-ID号/level-安全等级
void finger_autoIdentify( unsigned int id, unsigned char level)
{
  unsigned char check = 0;
  unsigned char param = 0;
  unsigned char autoIdentifyOrder[17] = {HEADER, ADDR, 0x01, 0x00, 0x08, 0x32, level, id/256, id%256, 0x00, 0x00, 0x00, 0x00};
  
  finger_cancel();
  param = finger_getParam();
  autoIdentifyOrder[13] = param / 256;
  autoIdentifyOrder[14] = param % 256;
  check = finger_checkOut(autoIdentifyOrder, 6, 17);
  autoIdentifyOrder[15] = (unsigned char)check / 256;
  autoIdentifyOrder[16] = (unsigned char)check % 256;
  finger_usartSend(autoIdentifyOrder, 17);
  handleStruct.identifyFlag = 0;
  fingerStruct.fingerMode = 2;
}

// 删除字符串
//buf-需要删除的字符串/len-截取的数据长度/bufLen-buf数据长度
void finger_delbuf(unsigned char *buf, unsigned char len, unsigned char bufLen)
{
  for(unsigned char i=0; i<bufLen; i++)
  {
    buf[i] = buf[len++];
  }
}

// 自动验证指纹解析
// 返回错误码
unsigned char finger_errorAnalysis(unsigned char ch1, unsigned char ch2)
{
  unsigned char num = 0;
  if(ch1 == 0x00)                                             // 成功
  {
    if(fingerStruct.fingerMode == 1)
    {
      if(ch2 == 0x01)                                         // 等待采图成功
        num = 20;
      else if(ch2 == 0x02)                                    // 合成特征成功
        num = 21;
      else if(ch2 == 0x03)                                    // 指纹录入成功，等待手指离开（参数设置）
        num = 22;
      else if(ch2 == 0x04)                                    // 合成模板成功
        num = 23;
      else if(ch2 == 0x05)                                    // 指纹重复检测
        num = 24;
      else if(ch2 == 0x06)                                    // 登记该模板数据
        num = 25;
    }
    else if(fingerStruct.fingerMode == 2)
    {
      if(ch2 == 0x01)                                         // 采集成功
        num = 26;                                           
      else if(ch2 == 0x05)                                    // 验证成功
        num = 27;
    }
  }
  else if(ch1 == 0x01)
    num = 1;                                                // 失败
  else if(ch1 == 0x07)
    num = 2;                                                // 生成特征失败
  else if(ch1 == 0x09)
    num = 3;                                                // 没搜索到指纹
  else if(ch1 == 0x0A)
    num = 4;                                                // 合并模板失败
  else if(ch1 == 0x0B)
    num = 5;                                                // ID号超出范围
  else if(ch1 == 0x17)
    num = 6;                                                // 残留指纹
  else if(ch1 == 0x1F)
    num = 7;                                                // 指纹库已满
  else if(ch1 == 0x22)
    num = 8;                                                // 指纹模板不为空
  else if(ch1 == 0x23)
    num = 9;                                                // 指纹模板为空
  else if(ch1 == 0x24)
    num = 10;                                                // 指纹库为空
  else if(ch1 == 0x25)
    num = 11;                                                // 录入次数设置错误
  else if(ch1 == 0x26)
    num = 11;                                                // 超时
  else if(ch1 == 0x27)
    num = 12;                                                // 表示指纹已存在
  return num;
}

void finger_fingerStructReset(void)
{
  fingerStruct.pageNum = 0;
  fingerStruct.errorNum = 0;
  fingerStruct.validTempNum = 0;
  fingerStruct.verifyScore = 0;
  handleStruct.enrollFlag=0;
}

int finger_usartInput(char ch)
{
  usartFinger.usartRecBuff[usartFinger.usartRecLen++] = ch;
  return 0;
}

// 串口解析
void finger_uartAnalysis(void)
{
  unsigned int orderLen = 0;
  unsigned int lastLen = 0;
  unsigned int check = 0;
  
  if(usartFinger.usartRecLen >= 10)
    for(unsigned int i=0; i<usartFinger.usartRecLen; i++)
    {
      if(usartFinger.usartRecBuff[i] == 0xEF && usartFinger.usartRecBuff[i+1] == 0x01)
      {
        lastLen = i;
        finger_fingerStructReset();
        orderLen = usartFinger.usartRecBuff[lastLen + 7] *256 + usartFinger.usartRecBuff[lastLen + 8];// 获取命令长度
        if(usartFinger.usartRecLen >= orderLen + lastLen + 9)   // 当前数据是否比指令长
        {
          check = finger_checkOut(usartFinger.usartRecBuff, lastLen + 6, orderLen + lastLen + 7);
          if((usartFinger.usartRecBuff[orderLen + lastLen + 7] == check / 256) 
             && (usartFinger.usartRecBuff[orderLen + lastLen + 8] == check % 256)) // 如果校验成功
          {
            fingerStruct.errorNum = finger_errorAnalysis(usartFinger.usartRecBuff[lastLen + 9], usartFinger.usartRecBuff[lastLen + 10]);
            if(fingerStruct.errorNum == 25)                     // 采集指纹成功
            {
              handleStruct.enrollFlag = 1;
              finger_cancel();
            }
            else if(fingerStruct.errorNum == 27)                // 验证指纹成功
            {
              handleStruct.identifyFlag = 1;
              finger_cancel();
            }
            if(fingerStruct.fingerMode == 1 && usartFinger.usartRecBuff[lastLen + 9] == 0x00)    // 获取当前录入的次数
            {
              handleStruct.enrollNum = usartFinger.usartRecBuff[lastLen + 11];
            }
            /*else if(fingerStruct.fingerMode == 5) // 手动验证指纹处理
            {
              if(usartFinger.usartRecBuff[lastLen + 8] == 0x03 && usartFinger.usartRecBuff[lastLen + 9] == 0x00)
              {
                fingerStruct.fingerMode = 6;
                getFingerID(1);
              }
              else
                getFingerID(0);
            }
            else if(fingerStruct.fingerMode == 6)
            {
              if(usartFinger.usartRecBuff[lastLen + 9] == 0x00)
              {
                fingerStruct.fingerMode = 7;
                getFingerID(2);
              }
              else
              {
                if(usartFinger.usartRecBuff[lastLen + 9] == 0x01)
                  fingerStruct.errorNum = 13;
                else if(usartFinger.usartRecBuff[lastLen + 9] == 0x06)
                  fingerStruct.errorNum = 14;
                else if(usartFinger.usartRecBuff[lastLen + 9] == 0x07)
                  fingerStruct.errorNum = 15;
                else if(usartFinger.usartRecBuff[lastLen + 9] == 0x15)
                  fingerStruct.errorNum = 16;
                cancel();
              }
            }
            else if(fingerStruct.fingerMode == 7)
            {
              if(usartFinger.usartRecBuff[lastLen + 9] == 0x00) // 验证成功
              {
                fingerStruct.fingerMode = 0;
                fingerStruct.pageNum = usartFinger.usartRecBuff[lastLen + 10] * 256 + usartFinger.usartRecBuff[lastLen + 11];
                fingerStruct.verifyScore = usartFinger.usartRecBuff[lastLen + 12] * 256 + usartFinger.usartRecBuff[lastLen + 13];
                fingerStruct.errorNum = 27;
              }
              else                                    // 验证失败
              {
                if(usartFinger.usartRecBuff[lastLen + 9] == 0x01)
                  fingerStruct.errorNum = 13;
                else if(usartFinger.usartRecBuff[lastLen + 9] == 0x09)
                  fingerStruct.errorNum = 17;
                cancel();
              }
            }*/
            finger_delbuf(usartFinger.usartRecBuff, orderLen + lastLen + 9, usartFinger.usartRecLen);
            usartFinger.usartRecLen -= orderLen + lastLen + 9;
            i = 0;
          }
        }
      }
    }
}

int finger_getIdentifyInfo(void)
{
  if(fingerStruct.errorNum == 27)
  {
    finger_fingerStructReset();
    return 1;
  }
  else if((fingerStruct.errorNum <= 12) && (fingerStruct.errorNum != 0))
  {
    finger_fingerStructReset();
    return -1;
  }
  return 0;
}

int finger_getEnrollInfo(void)
{
  if(handleStruct.enrollFlag == 0 && handleStruct.enrollNum == 0)
  {
    return -1;
  }
  else if(handleStruct.enrollFlag == 1)
  {
    finger_fingerStructReset();
    finger_cancel();
    return 0;
  }
  else
    return handleStruct.enrollNum;
}

/*
PROCESS(finger_process, "finger_process");

PROCESS_THREAD(finger_process, ev, data)
{
    static struct etimer finger_et;
    static struct etimer cancel_et;
    PROCESS_BEGIN();
    uart3_init(57600);
    etimer_set(&finger_et, 100);
    while(1)
    {
        PROCESS_WAIT_EVENT();
        static unsigned char fingerNum = 0;
        if(ev == key_event)
        {
            switch(*((unsigned char*)data))
            {
            case K1_PREESED: autoEnroll(fingerNum, 4, &paramStruct); break;
            case K2_PREESED: getFingerID(0); fingerStruct.fingerMode = 5; break;
            case K3_PREESED: fingerNum++; break;
            case K4_PREESED: cancel(); break;
            default: cancel(); break;
            }
        }
        if(ev == PROCESS_EVENT_TIMER)
        {
            if(etimer_expired(&finger_et))
            {
                etimer_set(&finger_et, 100);
                uartAnalysis(usartFinger.usartRecBuff, usartFinger.usartRecLen);
                if(fingerStruct.errorNum == 25)
                {
                    etimer_set(&cancel_et, 1000);
                    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&cancel_et));
                    cancel();                           // 登记该模板数据
                }
                else if(fingerStruct.errorNum == 27)
                {
                    
                }
                else
                {
                    switch(fingerStruct.errorNum)
                    {
                    case 1: break;                      // 失败
                    case 2: break;                      // 生成特征失败
                    case 3: break;                      // 没搜索到指纹
                    case 4: break;                      // 合并模块失败
                    case 5: break;                      // ID号超出范围
                    case 6: break;                      // 残留指纹
                    case 7: break;                      // 指纹库已满
                    case 8: break;                      // 指纹模板不为空
                    case 9: break;                      // 指纹模板为空
                    case 10: break;                      // 指纹库为空
                    case 11: break;                      // 超时
                    case 12: break;                      // 表示指纹已存在
                    case 13: break;                     // 收包错误
                    case 14: break;                      // 指纹太乱而生不成特征
                    case 15: break;                      // 特征点太少生不成特征
                    case 16: break;                      // 图像缓冲区没有有效原始图二生不成图像
                    case 17: break;                     // 没有收缩到，此时对应页码与得分
                    
                    case 20: break;                     // 等待采图成功
                    case 21: break;                     // 合成特征成功
                    case 22: break;                     // 指纹录入成功，等待手指离开（参数设置）
                    case 23: break;                     // 合成模板成功
                    case 24: break;                     // 指纹重复检测
                    case 26: break;                     // 采集成功
                    //case 27: break;                     // 验证成功
                    }
                }
            }
        }
    }
    
    PROCESS_END();
}
*/





