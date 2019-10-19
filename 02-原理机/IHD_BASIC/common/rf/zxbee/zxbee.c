#include "zxbee.h"

extern int z_process_command_call(char* ptag, char* pval, char* obuf);

static char wbuf[128];

void zxbeeBegin(void)
{
  wbuf[0] = '{';
  wbuf[1] = '\0';
}
int zxbeeAdd(char* tag, char* val)
{
  int offset = strlen(wbuf);
  sprintf(&wbuf[offset],"%s=%s,", tag, val);
  return 0;
}
char* zxbeeEnd(void)
{
  int offset = strlen(wbuf);
  wbuf[offset-1] = '}';
  wbuf[offset] = '\0';
  if (offset > 2) return wbuf;
  return NULL;
}

uint8_t ResponseType_Set(uint8_t index,char* obuf)
{
  uint16_t ret = -1;
  signed char temp=0;
  
  switch(index)
  {
  case 1:
    {
      switch(RF1_hwTypeGet())
      {
      case 1:
        temp = zigbeeGat_type();
        if (temp < 0) temp = 2;
        ret = sprintf(obuf, "TYPE=1%d%s", temp, sensor_type());     //zigbee
        break;
      case 2:
        ret = sprintf(obuf, "TYPE=42%s", sensor_type());            //ble
        break;
      }
    }
    break;
  case 2:
    {
      switch(RF2_hwTypeGet())
      {
      case 1:ret = sprintf(obuf, "TYPE=32%s", sensor_type());     //wifi
      break;
      }
    }
    break;
  case 3:
    {
      switch(RF3_hwTypeGet())
      {
      case 1:ret = sprintf(obuf, "TYPE=82%s", sensor_type());     //lora
      break;
      }
    }
    break;
  case 4:
    {
      uint8_t radio_type = CONFIG_RADIO_TYPE;
      uint8_t dev_type = CONFIG_DEV_TYPE;
      switch(RF4_hwTypeGet())
      {
      case 1:ret = sprintf(obuf, "TYPE=%u%u%s", radio_type, dev_type, sensor_type());//lte
      break;
      }
    }
    break;
  }
  return ret;
}

int SysProcess_command_call(uint8_t index,char* ptag, char* pval, char* obuf)
{
  int ret = -1;
  if (memcmp(ptag, "TYPE", 4) == 0)
  {
    if (pval[0] == '?')
    {
      ret = ResponseType_Set(index,obuf);
    }
  }
  return ret;
}

void _zxbee_onrecv_fun(uint8_t index,char *pkg, int len)
{
  char *p;
  char *ptag = NULL;
  char *pval = NULL;
  //int len = strlen(pkg);
  char *pwbuf = wbuf+1;
  
  
  if (pkg[0] != '{' || pkg[len-1] != '}') return;
  
  pkg[len-1] = 0;
  p = pkg+1;
  do
  {
    ptag = p;
    p = strchr(p, '=');
    if (p != NULL)
    {
      *p++ = 0;
      pval = p;
      p = strchr(p, ',');
      if (p != NULL) *p++ = 0;
      /*if (process_command_call != NULL) */
      {
        int ret;
        ret = SysProcess_command_call(index,ptag, pval, pwbuf);
        if (ret < 0)
        {
          ret = z_process_command_call(ptag, pval, pwbuf);
        }
        if (ret > 0)
        {
          pwbuf += ret;
          *pwbuf++ = ',';
        }
      }
    }
  }
  while (p != NULL);
  if (pwbuf - wbuf > 1)
  {
    wbuf[0] = '{';
    pwbuf[0] = 0;
    pwbuf[-1] = '}';
    switch(index)
    {
    case 1:RF1_SendData(wbuf);//Ó¦´ðÊý¾Ý
    break;
    case 2:RF2_SendData(wbuf);
    break;
    case 3:RF3_SendData(wbuf);
    break;
    case 4:RF4_SendData(wbuf);
    break;
    }
  }
}