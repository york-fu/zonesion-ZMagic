#ifndef _selldlg_h_
#define _selldlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"

#define GOODS_NUM   3

#define WM_USERMSG_PAYMENT    (WM_USER+1)

typedef struct
{
  uint8_t id;
  uint8_t num;
  float price;
}GoodsLast_t;


typedef struct
{
  short index;
  short request;
  uint8_t payment;
  WM_HWIN hWin;
}Sell_t;

extern Sell_t Sell;
extern GoodsLast_t GoodsList[GOODS_NUM];
PROCESS_NAME(SellProcess);

void SellStatusSet(short status);
uint8_t SellRequestGet();
double Sell_MoneyTotalGet();
void SellPayment_ZxbeeGet(char* buf);
void SellPayment_ZxbeeSet(char* str);
void SellNum_ZxbeeGet(char* buf);
void SellNum_ZxbeeSet(char* str);
void SellPrice_ZxbeeGet(char* buf);
void SellPrice_ZxbeeSet(char* str);

#endif