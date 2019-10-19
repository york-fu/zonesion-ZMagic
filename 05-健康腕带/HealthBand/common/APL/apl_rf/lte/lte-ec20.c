/*********************************************************************************************
* 文件：lte-ec20.c
* 作者：xuzhy 2018.1.11
* 说明：lte gsm 模块驱动
*       
*       
*       
* 修改：
* 注释：
*********************************************************************************************/
#include "lte/lte-ec20.h"

#define DEBUG 0

#if DEBUG
#define Debug      printf
#define DebugAT    printf
#else
#define Debug(...)
#define DebugAT(...)
#endif

/* 上层tcp回调函数*/
void on_gsm_tcp_open(int id, int err);
void on_gsm_tcp_recv(int id, int len, char *buf);
void on_gsm_tcp_close(int id);

/* gsm_info 保存gsm当前状态信息*/
gsm_info_t gsm_info;

static int gsm_response_mode = 0;
static char  _response_buf[_RESPONSE_BUF_SIZE];
static int _response_offset = 0;


/* at指令请求结构 */
struct _at_request{
  void *req;
  void (*fun)(char * response);
  unsigned int timeout_tm;
  struct _at_request *next;
};

struct _at_request *current_at = NULL; //保存当前正在执行的at指令
struct _at_request * list_at = NULL;  //保存at指令请求列表



static void check_sim_card(void);

/*********************************************************************************************
* 名称：_request_at_1
* 功能：发送at指令，不处理结果，仅仅发送而已
* 参数：at ：需要执行的at指令
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void _request_at_1(char *at)
{
  _request_at_3(at, 1000, NULL);
}

/*********************************************************************************************
* 名称：_request_at_1
* 功能：发送at指令，带超时，和结果处理回调函数
* 参数：at：需要执行的at指令
*       to：指令执行超时时间，单位 ms
*       fun：处理结果回调函数
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int _request_at_3(char *at, unsigned int to, void (*fun)(char*))
{
  int len = strlen(at);
  char *p = malloc(len+1);
  if (p) strcpy(p, at);
  else return -1;
  struct _at_request *pa = malloc(sizeof (struct _at_request));
  if (pa) {
    pa->req = p;
    pa->next = NULL;
    pa->fun = fun;
    pa->timeout_tm = to; //记录超时时间
    if (list_at == NULL) list_at = pa;
    else {
      struct _at_request *pl = list_at;
      while (pl->next != NULL) pl = pl->next;
      pl->next = pa; 
    }
  } else {
    free(p);
    return -1;
  }
  return 0;
}

/*********************************************************************************************
* 名称：end_process_at
* 功能：at指令处理结束释放内存
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
static void end_process_at(void)
{
  if (current_at != NULL) {
    free(current_at->req);
    free(current_at);
    current_at = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////

static void _rp_check_gsm(char *rsp)
{
  if (rsp == NULL) {
   //_request_at_3("at\r\n", 3000, _rp_check_gsm);
  } else if (memcmp(rsp, "OK", 2) == 0) {
    gsm_info.gsm_status = 1;
  }
}
/*********************************************************************************************
* 名称：check_gsm
* 功能：检查gsm模块链接是否正常
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
static void check_gsm(void)
{
   _request_at_3("at\r\n", 500, _rp_check_gsm);
}


static void _rs_check_sim_card(char *rsp)
{
  if (rsp == NULL) {
    return;
  } 
  else if (memcmp(rsp, "+CPIN: READY", 12)==0) {
     gsm_info.gsm_status = 2;
     gsm_info.simcard_status = 1;
  } 
  
}
/*********************************************************************************************
* 名称：check_sim_card
* 功能：检测是否有插入sim卡
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void check_sim_card(void)
{
   _request_at_3("at+cpin?\r\n", 1000, _rs_check_sim_card);
  //_request_at_3("at*tsimins?\r\n", 3000, _rs_check_sim_card);
}


static void _rs_register_network(char *rsp)
{
  if (rsp != NULL && memcmp(rsp, "+COPS:", 6) == 0) {
    //+COPS: 0,2,"46000"
    if (strlen(rsp) > 12) {
      char *p = &rsp[12];
      char *e = strchr(p, '\"');
      if (e) {
        *e = '\0';
        strcpy(gsm_info.network, p);
      }
    }
  }
}

/*********************************************************************************************
* 名称：gsm_register_network
* 功能：检测当前注册到的网络
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_register_network(void)
{
  _request_at_3("at+cops=0,2\r\n", 1000, NULL);
  _request_at_3("at+cops?\r\n", 1000, _rs_register_network);
}



void _rs_gsm_call_accept(char *res)
{
  if (res == NULL) {
    Debug(" error : gsm mast not timeout whic rua ata\r\n");
    return;
  }
  if (memcmp(res, "OK", 2) == 0) {
    gsm_info.phone_status = PHONE_ST_TALK;
  } 
  if (memcmp(res, "ERROR", 5) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
  }/*
  if (memcmp(res, "NO CARRIER", 10) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
  }*/
}
/*********************************************************************************************
* 名称：gsm_call_accept
* 功能：接听电话
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_call_accept(void)
{
  if (gsm_info.phone_status == PHONE_ST_RING) {
    _request_at_3("ata\r\n", 1000, _rs_gsm_call_accept);
  }
}


void _rs_gsm_call_hangup(char *rsp){
  gsm_info.phone_status = PHONE_ST_IDLE;
}
/*********************************************************************************************
* 名称：gsm_call_hangup
* 功能：挂断电话
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_call_hangup(void)
{
  _request_at_3("ath\r\n", 1000, _rs_gsm_call_hangup);
}


void _rs_gsm_request_call(char *rsp)
{
  if (rsp == NULL) {
    Debug("error : gsm must not timeout when run atd\r\n");
    return;
  }
  if (memcmp(rsp, "OK", 2) == 0) {
    gsm_info.phone_status = PHONE_ST_COUT;
    
  }
  if (memcmp(rsp, "ERROR", 5) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
  } /*
  if (memcmp(rsp, "NO CARRIER", 10) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
  }
  if (memcmp(rsp, "BUSY", 4) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
  }*/
}

/*********************************************************************************************
* 名称：gsm_request_call
* 功能：拨号
* 参数：pnum:手机号码
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int gsm_request_call(char *pnum)
{
  if (gsm_info.phone_status == PHONE_ST_IDLE) {
    char buf[32];
    sprintf(buf, "atd%s;\r\n", pnum);
    strcpy(gsm_info.phone_number, pnum);
    _request_at_3(buf, 2000, _rs_gsm_request_call);
    return 1;
  }
  else return 0;
}

void _rs_gsm_get_current_call(char *rsp)
{
  if (memcmp(rsp, "OK", 2) == 0) {
  }
  if (memcmp(rsp, "+CLCC", 5) == 0) {
    //+CLCC: 1,0,0,0,0,"10010",129
    if (strlen(rsp)>12) {
      if (rsp[11] == '0') {
        //gsm_info.phone_status = PHONE_ST_TALK;
      }
    }
  }
}

/*********************************************************************************************
* 名称：gsm_get_current_call
* 功能：获取当前通话状态
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_get_current_call(void)
{
  _request_at_3("at+clcc\r\n", 1000, _rs_gsm_get_current_call);
}

/*********************************************************************************************
* 名称：gsm_ppp_init
* 功能：数据链接初始化
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_ppp_init(void)
{
  void gsm_ppp_stats();
  char *apn = "cmiot";
  char buf[32];
  if (memcmp(gsm_info.network, "46000", 5) == 0) {
    apn = "cmiot";
  }
  if (memcmp(gsm_info.network, "46001", 5) == 0) {
    apn = "3gnet";
  }
  sprintf(buf, "at+qicsgp=1,1,\"%s\"\r\n", apn);
  _request_at_3(buf, 1000, NULL);
  
  gsm_info.ppp_status = 1;
  //_request_at_3("at+zpppstatus\r\n", 3000, NULL);
  //_request_at_3("at+qiact?\r\n", 3000, NULL);
  gsm_ppp_stats();
}

/*********************************************************************************************
* 名称：gsm_ppp_stats
* 功能：检测ppp链接状态
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_ppp_stats()
{
  _request_at_3("at+qiact=1\r\n", 1000, NULL);
  _request_at_3("at+qiact?\r\n", 1000, NULL);
}

void _rs_gsm_get_imei(char *rsp)
{
  if (rsp == NULL) {
    DebugAT("error: get imei not timeout\r\n");
    return;
  }
  int len = strlen(rsp);
  if (len == 15) {
    strcpy(gsm_info.imei, rsp);
  }
}

/*********************************************************************************************
* 名称：gsm_get_imei
* 功能：读取模块imei
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_get_imei(void)
{
  _request_at_3("at+gsn\r\n", 500, _rs_gsm_get_imei);
}

static char short_message[72];
static void _rs_gsm_send_message(char *rsp)
{
  if (rsp == NULL) {
     DebugAT("error: send message timeout\r\n");
    return;
  }
  if (rsp[0] == '>')  {
    gsm_uart_write(short_message, strlen(short_message));
    char ch = 0x1A;
    gsm_uart_write(&ch, 1);
    DebugAT("\r\nGSM <<< %s\r\n", short_message);
  }
}

/*********************************************************************************************
* 名称：trim
* 功能：去掉字符串前后空格
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void trim ( char *s )
{
   char *p;
   int i;
   
    p = s;
    while ( *p == ' ' || *p == '\t' ) {*p++;}
    strcpy ( s,p );
    
    i = strlen ( s )-1;
    while ( ( s[i] == ' ' || s[i] == '\t' ) && i >= 0 ) {i--;};
    s[i+1] = '\0';
}

/*********************************************************************************************
* 名称：gsm_send_message
* 功能：发送短信息
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_send_message(char *num, char *msg)
{
  char buf[32];
  sprintf(buf, "at+cmgs=\"%s\"\r", num);
  sprintf(short_message, msg);
  _request_at_3(buf, 5000, _rs_gsm_send_message);
}

void _rs_gsm_query_creg(char *rsp)
{
  /* +CREG: 2, 1, "712C", "DF6A", 0 */
  if (rsp == NULL) return;
  if (memcmp(rsp, "+CREG:", 6) == 0) {
    if (rsp[7] != '2') {
      _request_at_3("at+creg=2\r\n", 1000, NULL);
      return;
    } else {
      char *p = &rsp[6];
      p = strchr(p, ',')+1;  //2;
      char *st = p;
      p = strchr(p, ',');
      if (p != NULL){
        *p = '\0'; p += 1;
      }
      
      
      trim(st);
      if (atoi(st) == 1 || atoi(st) == 5) {
        gsm_info.ppp_status = 2;
      } else {
        gsm_info.ppp_status = 0;
      }

      /*
      if (p == NULL) return; 
      char *lac = p;
      p = strchr(p, ',');
      if (p == NULL) return;
      *p = '\0'; p += 1;
      char *ci = p;
      p = strchr(p, ',');
      if (p == NULL) return;
      *p = '\0'; p += 1;
      char *act = p;
      */
      //gsm_info.ppp_status   
        /*
        
        char *lac = &rsp[14];
        char *p = strchr(lac, '\"');
        *p = '\0';
        char *ci = p+4;
        p = strchr(ci, '\"');
        *p = '\0';
        char *act = p + 3;
        //printf(" got lac: %s, ci: %s, act:%s\r\n", lac, ci, act);
        strcpy(gsm_info.lac, lac);
        strcpy(gsm_info.ci, ci);
        strcpy(gsm_info.act, act);
        */
     // }
    }
  }
}


/*********************************************************************************************
* 名称：gsm_query_creg
* 功能：查询当前网络注册状态
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void gsm_query_creg(void)
{
  _request_at_3("at+creg?\r\n", 2000, _rs_gsm_query_creg);
}

static void __on_ec20_at_port(char *rsp)
{
  //+QURCCFG: "urcport","usbat"
  if (memcmp(rsp, "+QURCCFG:", 9) == 0) {
    char *p = strchr(&rsp[9], ',');
    if (p != NULL) {
      if (memcmp(p+1, "\"uart1\"", 5) != 0) {
        /* 设置默认at指令上报为串口 */
        _request_at_3("at+qurccfg=\"urcport\",\"uart1\"\r\n", 1000, NULL);
      }
    }
  }
}
/*********************************************************************************************
* 名称：ec20_at_port_init
* 功能：ec20 设置at上报状态为串口
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void ec20_at_port_init(void)
{
  _request_at_3("at+qurccfg=\"urcport\",\"uart1\"\r\n", 1000, NULL);
  _request_at_3("at+qurccfg?\r\n", 2000, __on_ec20_at_port);
}

void GetSignalStrengthCallback(char * rsp)
{
  if (memcmp(rsp, "+CSQ:", 5) == 0)
  {
    gsm_info.SignalStrength=atoi(&rsp[5]);
    
    char *p = &_response_buf[6];
    char *e = strchr(p, ',');
    *e = '\0';
    int v = atoi(p);
    if (v == 99) v = 0;
    gsm_info.signal = v/6;
  }
}

void EC20_AT_GetSignalStrength(void)
{
  _request_at_3("AT+CSQ\r\n",1000,GetSignalStrengthCallback);
}

void SetBaudCallback(char * rsp)
{
  if (memcmp(rsp, "OK:", 2) == 0)
  {
  }
}
void EC20_AT_SetBaud(uint32_t baud)
{
  char buf[32]={0};
  sprintf(buf,"AT+IPR=%d\r\n",baud);
  _request_at_3(buf,300,SetBaudCallback);
}

static struct ctimer gsm_timer;

/*********************************************************************************************
* 名称：e_gsm_timer_call
* 功能：定时器回调函数，处理gsm模块状态
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
static void _gsm_timer_call(void *ptr)
{
  static int tick = 0;
  tick ++;
  if (gsm_info.gsm_status == 0) {
    if(tick % 2 ==0)
    {
      check_gsm();
    }
  }
  if (gsm_info.gsm_status == 1) {
    static int init = 0;
#if 0
    if (init == 0) {
      //at+qurccfg="urcport","uart1"
      _request_at_3("ate0\r\n", 1000, NULL);
      /* 配置ec20 at指令默认输出为 uart 而不是 usb */
      //_request_at_3("at+qurccfg=\"urcport\",\"uart1\"\r\n", 1000, NULL);
      ec20_at_port_init();
      
      //_request_at_3("at+clip=1\r\n", 1000, NULL);
       //_request_at_3("at+speaker=0\r\n", 1000, NULL);
      
      //_request_at_3("at+cops=0,2\r\n", 1000, NULL);
      
      //gsm_get_imei();
      _request_at_3("at+creg=2\r\n", 1000, NULL);
      //_request_at_3("at+cced=1,3\r\n", 1000, NULL);
      
      // _request_at_3("at+cmgf=1\r\n", 1000, NULL);
      
    }
  
#else
    if (init == 0) {
      _request_at_3("ATE0\r\n", 1000, NULL);
      _request_at_3("AT+CLIP=1\r\n", 1000, NULL);
      gsm_get_imei();
      init = 1;
    }
    else if (init == 1 && tick % 2 == 0) {
      check_sim_card();
    }
#endif
  }
  if (gsm_info.gsm_status == 2) {
#if 1
    if (tick % 4 == 0 && strlen(gsm_info.network) == 0) {
      gsm_register_network();
    }
    if (strlen(gsm_info.network) > 0) {
      if (tick % 4 == 0 && gsm_info.ppp_status == 0) {
        gsm_ppp_init();
      } else if (tick%4==2 &&gsm_info.ppp_status == 1) {
        gsm_ppp_stats();
      }
      /*if (gsm_info.ppp_status != 0 && tick % 60 == 30) {
        _request_at_3("at+zpppstatus\r\n", 1000, NULL);
      }*/
    }
    /*if (tick % 60 == 15) {
      gsm_query_creg();
    }*/
#else
    if (tick % 4 == 0) {
      gsm_query_creg();
    }
#endif
    if (gsm_info.phone_status == PHONE_ST_COUT) {
      gsm_get_current_call();
    }
    if(tick % 4 == 1)
    {
      EC20_AT_GetSignalStrength();
    }
  }
  if (gsm_info.gsm_status == 3) {
    //无sim卡
    if (tick % 5 == 0) check_sim_card();
  }
  ctimer_reset(&gsm_timer);
}

/*********************************************************************************************
* 名称：_poll_request
* 功能：at请求轮询
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void _poll_request(void)
{
  if (current_at == NULL) {
    current_at = list_at;
    if (current_at != NULL) {
      DebugAT(" gsm <<< %s", (char*)current_at->req);
      if (strstr(current_at->req, "zipsend")) {
        DebugAT("\r\n");
      }
      
      current_at->timeout_tm = clock_time()+ current_at->timeout_tm;
      gsm_uart_write(current_at->req, strlen(current_at->req));
      list_at = list_at->next;
    }
  } else {
    unsigned int tm = clock_time();
    if (((int)(tm - current_at->timeout_tm)) > 0) {
      DebugAT(" gsm : timeout\r\n");
      if (current_at->fun != NULL) {
        current_at->fun(NULL);
      }
      end_process_at();
    } 
  }
}

//at+clcc响应处理
void AT_ClccHandle(char * buf)
{
  if (strlen(buf)>18) 
  {
    if(buf[18] != '"')
    {
      if(buf[9] == '0')
      {
        switch(buf[11])
        {
        case '0':
          gsm_info.phone_status = PHONE_ST_TALK;
          break;
        }
      }
    }
  }
}

gsm_info_t lte_GetInof()
{
    return gsm_info;
}


/*********************************************************************************************
* 名称：_poll_response
* 功能：处理at响应
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void _poll_response(void)
{
  int ch;
  ch = gsm_uart_read_ch();
  while (ch >= 0) {
    if (gsm_response_mode == 0) {
      if (_response_offset == 0 && ch == '>') {
        if (current_at != NULL && current_at->fun != NULL) {    
          current_at->fun(">");
        }
      } else
      if (ch != '\n') {
        _response_buf[_response_offset++] = ch;
      } else {
        if (_response_offset>0 && _response_buf[_response_offset-1] == '\r') {
          _response_buf[_response_offset-1] = '\0';
          _response_offset -= 1;
          if (_response_offset > 0) {
            DebugAT(" gsm >>> %s\r\n", _response_buf);
            if (current_at != NULL && current_at->fun != NULL) {
                current_at->fun(_response_buf);
                //end_process_at();
            }
            if ((memcmp(_response_buf, "OK", 2) == 0) 
                || (memcmp(_response_buf, "ERROR", 5) == 0)
                || (memcmp(_response_buf, "+CME ERROR", 10) == 0)
                || (memcmp(_response_buf, "SEND OK", 7) == 0))
            {
                end_process_at();
            }
              /* unsolite message */
              if (memcmp(_response_buf, "+CLCC: ", 7) == 0) {
                AT_ClccHandle(_response_buf);
              }
              else if (memcmp(_response_buf, "+CLIP: ", 7) == 0) {
                //+CLIP: "13135308483",161,"","",0
                gsm_info.phone_status = PHONE_ST_RING;
                char *p = &_response_buf[8];
                char *e = strchr(p, '\"');
                *e = '\0';
                strcpy(gsm_info.phone_number, p);
              }
              else if (memcmp(_response_buf, "NO CARRIER", 10) == 0) {
                gsm_info.phone_status = PHONE_ST_IDLE;
              }
              else if (memcmp(_response_buf, "BUSY", 4) == 0) {
                gsm_info.phone_status = PHONE_ST_IDLE;
              }
              else if (memcmp(_response_buf, "+QIACT: 1,0", 11) == 0) {
                gsm_info.ppp_status = 0;
              }
              else if (memcmp(_response_buf, "+QIACT: 1,1", 11) == 0) {
                gsm_info.ppp_status = 2;
              }
              else if (memcmp(_response_buf, "+QIOPEN:", 8) == 0) {
                char *p = &_response_buf[8];
                char *pr = strchr(p, ',');
                int tid = atoi(p);
                int err = -1;
                if (pr != NULL) err = atoi(pr+1);
                
                on_gsm_tcp_open(tid, err);
              }
              else if (memcmp(_response_buf, "+QIURC:", 7) == 0) {
                char *p = &_response_buf[7+1]; //1 byte space
                if (memcmp(p, "\"closed\"", 8) == 0) {
                  p = strchr(p, ',');
                  if (p != NULL) {
                    int tid = atoi(p+1);
                    on_gsm_tcp_close(tid);
                  }
                }
                else if (memcmp(p, "\"recv\"", 6) == 0) {
                  p = strchr(p, ',');
                  if (p != NULL) {
                    int tid = atoi(p+1);
                    p = strchr(p+1, ',');
                    if (p != NULL) {
                      int dlen = atoi(p+1);
                      if (dlen > 0) {
                       /* 读取数据 */
                        /*
                        do {       
                          _response_offset=0;
                          int min = dlen < _RESPONSE_BUF_SIZE ? dlen : _RESPONSE_BUF_SIZE;
                          do {
                            ch = gsm_uart_read_ch();
                            if (ch >= 0) {
                              _response_buf[_response_offset++] = ch;
                            }
                          } while (_response_offset < min);
                          //_response_buf[_response_offset] = 0;
                        //} while (dlen > 0);
                        */
                        char *p = malloc(dlen);
                        if (p != NULL) {
                          _response_offset = 0;
                          do {
                            ch = gsm_uart_read_ch();
                            if (ch >= 0) {
                              p[_response_offset++] = ch;
                            }
                          } while (_response_offset < dlen);
                         
                          on_gsm_tcp_recv(tid, _response_offset, p);
                        } else {
                          DebugAT("Error: malloc faile for %d byte.\r\n", dlen);
                        }
                        
                      }
                    }
                    
                  }
                }
              }
             
              /*if (memcmp(_response_buf, "+CPIN: READY", 12)==0) {
                gsm_info.gsm_status = 2;
                gsm_info.simcard_status = 1;
              }*/    
            _response_offset = 0;
          }
        }
      }
    }
    
    /**/
    ch = gsm_uart_read_ch();
  }
}
//////////////////////////////////////////////////////

#include "shell.h"
PROCESS(shell_gsm_process, "gsm shell process");
SHELL_COMMAND(gsm_command, "gsm", "gsm: gsm operator",
	      &shell_gsm_process);
PROCESS_THREAD(shell_gsm_process, ev, data)
{
  
  PROCESS_BEGIN();
  
  //printf("gsm data %s\r\n", data);
  char buf[64];
  if (strlen(data) > 0) {
    sprintf(buf, "%u\r\n", data);
    _request_at_3(buf, 1000, NULL);
  } else 
    gsm_send_message("13135308483", "abc");
  PROCESS_EXIT();
  
  PROCESS_END();
}


PROCESS(gsm_process, "gsm_process");

PROCESS_THREAD(gsm_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  gsm_inf_init();
  etimer_set(&et, CLOCK_SECOND/10); // 100 ms
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); 
  
  ctimer_set(&gsm_timer, CLOCK_SECOND/2, _gsm_timer_call, NULL);
  shell_register_command(&gsm_command);
  while (1) 
  {
    _poll_request();
    _poll_response();
    etimer_set(&et, 5);
    //process_post(&gsm_process,PROCESS_EVENT_TIMER,NULL);
    PROCESS_YIELD();
  }
  PROCESS_END();
}
