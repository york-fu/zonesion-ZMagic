#ifndef _ftdlg_h_
#define _ftdlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"

#define FTBufferSize 700

typedef enum
{
  FT_RECV=0,
  FT_SEND,
}ftMode_t;

typedef enum
{
  FT_IDLE=0,
  FT_READY,
  FT_CONNECT,
  FT_CONNECTED,
  FT_TRANSMIT,
}ftStatus_t;
  
typedef enum
{
  FT_NONE = 0,
  FT_FINISH,
  FT_FAIL,
}ftResult_t;

typedef struct
{
  ftMode_t mode;
  ftResult_t result;
  ftStatus_t status;
}FT_Status_t;


typedef struct
{
  unsigned long time;
  unsigned long Count;
}FTTimeout_t;


typedef struct
{
	FIL ftFile;
  char* ftName;
  UINT ftReadDataCnt;
  FTTimeout_t ftTimeout;
  uint32_t ftPackMax; 
  uint32_t ftPackIndex;
  uint32_t ftSentCnt;
  uint16_t flags;
  char* ftDataBuffer;
}FileTransmit_t;

#endif
