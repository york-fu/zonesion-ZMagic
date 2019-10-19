
#include "StatusBarDLG.h"
#include "BatteryVoltage.h"
#include "lte/lte-ec20.h"
#include "fml_audio/audioAmplifier.h"
#include "MusicDLG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0   (GUI_ID_USER + 0x07)
#define ID_TEXT_0   (GUI_ID_USER + 0x08)


// USER START (Optionally insert additional defines)

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bmEarphone_16X16;

static GUI_CONST_STORAGE unsigned long _acEarphone_16X16[] = {
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xEAFFFFFF, 0xC1FFFFFF, 0xC2FFFFFF, 0xEAFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xD1FFFEFF, 0x46FFFFFF, 0x24FEFEFE, 0x55FFFFFF, 0x56FFFFFF, 0x24FEFEFE, 0x46FFFFFF, 0xD1FFFEFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xB3FEFEFE, 0x22FFFEFF, 0xB2FEFEFE, 0xFEFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFEFFFFFF, 0xB3FEFEFE, 0x23FFFEFF, 0xB4FEFEFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xDEFFFFFF, 0x20FEFEFE, 0xDDFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xDEFFFEFF, 0x21FEFEFE, 0xDEFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0x64FFFFFF, 0x9CFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x9CFFFFFF, 0x65FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0x1BFEFEFE, 0xF5FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xF5FFFFFF, 0x1BFEFEFE, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xDCFEFEFE, 0x10FFFFFF, 0x88FEFEFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x88FEFEFE, 0x10FFFFFF, 0xDDFEFEFE, 0xFFFFFFFF,
  0xECFFFFFF, 0x19FFFFFF, 0x00FFFFFF, 0x05FEFEFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x05FEFEFE, 0x00FFFFFF, 0x19FFFFFF, 0xECFFFFFF,
  0xA4FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x05FEFEFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x05FEFEFE, 0x00FFFFFF, 0x00FFFFFF, 0xA4FFFFFF,
  0x9DFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x05FEFEFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x05FEFEFE, 0x00FFFFFF, 0x00FFFFFF, 0x9DFFFFFF,
  0xC7FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x05FEFEFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x05FEFEFE, 0x00FFFFFF, 0x00FFFFFF, 0xC7FFFFFF,
  0xFFFFFFFF, 0x8BFFFEFF, 0x15FEFEFE, 0x17FEFDFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x17FEFDFE, 0x01FFFFFF, 0x88FEFEFE, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xF0FFFFFF, 0x1CFEFEFF, 0xFDFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xF9FFFFFF, 0x59FEFFFF, 0x83FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xF3FEFFFF, 0x5DFFFFFF, 0x22FEFEFF, 0x71FEFEFF, 0xFCFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFDFFFFFF, 0xC2FFFFFF, 0xE9FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

GUI_CONST_STORAGE GUI_BITMAP bmEarphone_16X16 = {
  16, // xSize
  16, // ySize
  64, // BytesPerLine
  32, // BitsPerPixel
  (unsigned char *)_acEarphone_16X16,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_BMP8888
};


typedef struct
{
  uint16_t x;
  uint16_t y;
  uint8_t wide;
  uint8_t high;
  uint32_t color;
  uint32_t BkColor;
}BatIcon_t;

static uint8_t SBarStyle=0;
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
void StatusBarSet(char style)
{
  SBarStyle=style;
}

void SetBatIcon(BatIcon_t* bat,uint8_t wide,uint8_t high,uint32_t color,uint32_t BkColor)
{
  bat->wide=wide;
  bat->high=high;
  bat->color=color;
  bat->BkColor=BkColor;
}

void DrawBatteryIcon(uint16_t x,uint16_t y,BatIcon_t* bat,uint8_t percent)
{
  if(bat->wide<12) return;
  if(bat->high<6) return;
  
  uint32_t LastColor=GUI_GetColor();
  
  bat->x=x;
  bat->y=y;
  GUI_SetColor(bat->color);
  GUI_DrawRect(bat->x,bat->y,bat->x+bat->wide-3,bat->y+bat->high-1);
  GUI_FillRect(bat->x+bat->wide-2,bat->y+(bat->high/3)-1,bat->x+bat->wide-1,bat->y+(bat->high/3*2));
  
  if(percent > 100)
    percent = 100;
  percent = (bat->wide-6)*percent/100;
  
  GUI_FillRect(bat->x+2,bat->y+2,bat->x+1+percent,bat->y+bat->high-3);
  GUI_SetColor(bat->BkColor);
  GUI_FillRect(bat->x+2+percent,bat->y+2,bat->x+bat->wide-5,bat->y+bat->high-3);
  
  GUI_SetColor(LastColor);
}

//算术平均滤波处理数据
#define AVERAGE_NUM 12

uint8_t AverageHandle(float value,float* result)
{
    static uint8_t count = 0;
    static float V_Max = 0,V_Min = 0xfff;
    static float dataBuffer[AVERAGE_NUM] = {0};
    
    dataBuffer[count] = value;
    count++;
    
    if(dataBuffer[count]<V_Min)
        V_Min = dataBuffer[count];
    if(dataBuffer[count]>V_Max)
        V_Max = dataBuffer[count];
    
    if(count>=AVERAGE_NUM)
    {
        count = 0;
        *result = 0;
        
        for(uint8_t i=0;i<AVERAGE_NUM;i++)
        {
            *result += dataBuffer[i];
        }
        *result -= (V_Min+V_Max);
        *result /= (AVERAGE_NUM-2); 
        
        V_Max = 0,V_Min = 0xfff;
        
        return 1;
    }
    return 0;
}

void ShowBattery(uint16_t x,uint16_t y,uint32_t color,uint32_t BkColor)
{
  static short percent = -1;
  char buf[32] = {0};
  float voltage = 0;
  BatIcon_t bat;
  
  voltage = BatteryVotage_Get() - 8.6;
  //if(percent==-1)
  {
    percent = (uint8_t)(voltage*100.0/4.0);
  }
//  if(AverageHandle(voltage,&voltage))
//  {
//    percent = (uint8_t)(voltage*100.0/4.0);
//  }
  
  GUI_SetColor(color);
  GUI_SetFont(FontList16[System.font]);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  
  sprintf(buf,"%3u",percent);
  GUI_DispStringAt(buf,x,y);
  SetBatIcon(&bat,24,12,color,BkColor);
  DrawBatteryIcon(x+26,y+2,&bat,percent);
}


short GetNetwork()
{
  gsm_info_t NetInfo;
  NetInfo = lte_GetInof();
  if(NetInfo.network!=NULL)
  {
    if (memcmp(gsm_info.network, "46000", 5) == 0) 
    {
      return 1;
    }
    if (memcmp(gsm_info.network, "46001", 5) == 0) 
    {
      return 2;
    }
    if (memcmp(gsm_info.network, "46011", 5) == 0) 
    {
      return 3;
    }
  }
  return -1;
}

short GetSignalStrength()
{
  gsm_info_t NetInfo;
  NetInfo = lte_GetInof();
  if((NetInfo.SignalStrength>0) && (NetInfo.SignalStrength<8))
    return 1;
  else if((NetInfo.SignalStrength>=8) && (NetInfo.SignalStrength<16))
    return 2;
  else if((NetInfo.SignalStrength>=16) && (NetInfo.SignalStrength<24))
    return 3;
  else if((NetInfo.SignalStrength>=24) && (NetInfo.SignalStrength<32))
    return 4;
  return 0;
}


void ShowNetworkIcon(uint16_t x,uint16_t y,uint32_t color,uint32_t BkColor)
{
  uint8_t wide=27;
  uint8_t high=16;
  short i=0,v1=0,v2=0;
  
  char operatorList[][4][20]={
    {"NoNetwork","Mobile","Unicom","Telecom"},
    {"无网络","中国移动","中国联通","中国电信"}
  };
  GUI_POINT pPoints[]={
    {0,0},{7,0},{4,3},{3,3},
  };
  
  v1=GetSignalStrength();//转换信号强度，范围：1-4
  v2=GetNetwork();
  
  GUI_SetColor(color);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_SetFont(FontList16[System.font]);
  GUI_FillRect(x+3-1,y,x+4-1,y+high-1);
  GUI_FillPolygon(pPoints,4,x-1,y);
  if(v1!=0)
  { 
    for(i=1;i<=v1;i++)
      GUI_FillRect(x+(wide/9*(i*2))-1,y+(high/4*(4-i))-1,x+(wide/9*((i*2)+1))-1,y+high-1);
    for(;i<=4;i++)
      GUI_DrawRect(x+(wide/9*(i*2))-1,y+(high/4*(4-i))-1,x+(wide/9*((i*2)+1))-1,y+high-1);
  }
  else
  {
#if 0
    GUI_AA_SetFactor(5);
    GUI_AA_DrawLine(x+(wide/9*3)-1,y+high/4*2-1,x+(wide/9*7)-1,y+high-1);
    GUI_AA_DrawLine(x+(wide/9*7)-1,y+high/4*2-1,x+(wide/9*3)-1,y+high-1);
#else
    GUI_DrawLine(x+(wide/9*3)-1,y+high/4*2-1,x+(wide/9*7)-1,y+high-1);
    GUI_DrawLine(x+(wide/9*7)-1,y+high/4*2-1,x+(wide/9*3)-1,y+high-1);
#endif
  }
  if(v2!=-1)
  {
    GUI_DispStringAt(operatorList[System.language][v2],x+wide+3,y);
  }
  else
  {
    GUI_DispStringAt(operatorList[System.language][0],x+wide+3,y);
  }
}

void ShowCPUUsage(short x,short y,uint32_t color,uint32_t BkColor)
{
  char buf[32]={0};
  GUI_SetColor(color);
  GUI_SetFont(FontList16[System.font]);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  
  sprintf(buf,"CPU:%3u",GetCPUUsage());
  GUI_DispStringAt(buf,x,y);
}

void ShowMusicIcon(short x,short y,uint32_t color,uint32_t BkColor)
{
  GUI_SetColor(color);
  GUI_FillRect(x+4,y,x+15,y+1);
  GUI_FillRect(x+4,y+2,x+5,y+11);
  GUI_FillRect(x+14,y+2,x+15,y+11);
  GUI_FillCircle(x+2,y+13,2);
  GUI_FillCircle(x+12,y+13,2);
}

void ShowEarphoneIcon(short x,short y,uint32_t color,uint32_t BkColor)
{
  GUI_DrawBitmap(&bmEarphone_16X16,x,y);
}
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "StatusBar", ID_WINDOW_0, 0, 0, 320, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "time", ID_TEXT_0, 130, 0, 60, 20, 0, 0x64, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  static short tick = 0;
  Calendar_t calendar;
  char buf[16] = {0};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'StatusBar'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_DARKGRAY);
    //
    // Initialization of 'time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, FontList16[System.font]);
    TEXT_SetText(hItem, "00:00");
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
  case WM_PAINT:
    ShowNetworkIcon(3,2,GUI_WHITE,GUI_DARKGRAY);
    if(ProcessIsRun(&MusicProcess))
    {
      if(MusicStatus == STA_PLAYING)
        ShowMusicIcon(100,2,GUI_ORANGE,GUI_DARKGRAY);
      else
        ShowMusicIcon(100,2,GUI_WHITE,GUI_DARKGRAY);
    }
    if(EarphoneGet() == 0)
    {
      ShowEarphoneIcon(118,2,GUI_WHITE,GUI_DARKGRAY);
    }
    ShowCPUUsage(205,2,GUI_LIGHTGREEN,GUI_DARKGRAY);
    ShowBattery(268,2,GUI_WHITE,GUI_DARKGRAY);
    break;
  case WM_TIMER:
    WM_RestartTimer(pMsg->Data.v, 50);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    if(SBarStyle)
    {
      WM_ShowWindow(hItem);
      calendar = Calendar_Get();
      if(tick > 9)
        sprintf(buf,"%02u %02u",calendar.hour,calendar.minute);
      else
        sprintf(buf,"%02u:%02u",calendar.hour,calendar.minute);
      TEXT_SetText(hItem, buf);
      tick++;
      if(tick >= 20) tick = 0;
    }
    else
      WM_HideWindow(hItem);
    WM_InvalidateWindow(pMsg->hWin);
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateStatusBar
*/
WM_HWIN CreateStatusBarDLG(void);
WM_HWIN CreateStatusBarDLG(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(hWin, 0x00, 50, 0); //创建一个软件定时器
  return hWin;
}


WM_HWIN WMStatusBar;

void CreateStatusBar()
{
  WMStatusBar = CreateStatusBarDLG();
}

void DeleteStatusBar()
{
  if(WMStatusBar != 0)
  {
    WM_DeleteWindow(WMStatusBar);
    WMStatusBar = 0;
  }
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
