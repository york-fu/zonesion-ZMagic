#include "zxbee.h"
#include "sensor.h"

extern int z_process_command_call(char* ptag, char* pval, char* obuf);

static char wbuf[256];

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
  uint8_t radio_type = 0;
  uint8_t dev_type = 0;
  
  switch(index)
  {
  case 1:
    radio_type = BLE_CONFIG_RADIO_TYPE;
    dev_type = BLE_CONFIG_DEVICE_TYPE;
    ret = sprintf(obuf, "TYPE=%u%u%s", radio_type, dev_type, sensor_type());
    break;
  case 2:
    switch(RF2_hwTypeGet())
    {
    case 1:
      radio_type = LTE_CONFIG_RADIO_TYPE;
      dev_type = LTE_CONFIG_DEVICE_TYPE;
      break;
    case 2:
      radio_type = NB_CONFIG_RADIO_TYPE;
      dev_type = NB_CONFIG_DEVICE_TYPE;
      break;
    }
    ret = sprintf(obuf, "TYPE=%u%u%s", radio_type, dev_type, sensor_type());
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
    }
  }
}