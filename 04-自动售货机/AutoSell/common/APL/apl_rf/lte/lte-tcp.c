/*********************************************************************************************
* 文件：lte-tcp.c
* 作者：xuzhy 2018.1.11
* 说明：lte tcp管理模块
*       
*       
*       
* 修改：
* 注释：
*********************************************************************************************/

#include "lte-tcp.h"

void gsm_uart_write(char *buf, int len);
int gsm_uart_read_ch(void);

process_event_t evt_tcp = 0xff;


#define DEBUG 0

#if DEBUG
#define Debug(x...) printf(x)
#else
#define Debug(x...)
#endif

#define CONNECT_NUMBER 12

#define TCP_BUF_SIZE  1500

/* ec20 一个上下文支持0-11共12tcp链接*/
static tcp_t tcps[CONNECT_NUMBER];
static char send_buf[TCP_BUF_SIZE];
static int send_len = 0;
static void __gsm_tcp_close(int tid);

/*********************************************************************************************
* 名称：on_gsm_tcp_open
* 功能：调用gsm_tcp_open后，被系统调用，用来通知tcp打开结果，并发送evt_tcp事件到用户程序
* 参数：
* 返回：
* 修改：
* 注释：用户不应该主动调用此函数
*********************************************************************************************/
void on_gsm_tcp_open(int id, int err)
{
  if (tcps[id].p != NULL) {
    if (err == 0) {
      tcps[id].status = TCP_STATUS_CONNECTED;
    } else /*if (err == 563) */{ //已经打开,需要关闭 
      /*强制关闭，重新打开*/
      __gsm_tcp_close(id);
      tcps[id].status = TCP_STATUS_CLOSED;
    } /*else {
      tcps[id].status = TCP_STATUS_CLOSED;
    }*/
    process_post(tcps[id].p, evt_tcp, (process_data_t)&tcps[id]);
  }
}

/*********************************************************************************************
* 名称：on_gsm_tcp_close
* 功能：在tcp链接被关闭时，发送evt_tcp事件通知用户应用程序
* 参数：
* 返回：
* 修改：
* 注释：用户不应该主动调用此函数
*********************************************************************************************/
void on_gsm_tcp_close(int id)
{
  if (tcps[id].p != NULL) {
    tcps[id].status = TCP_STATUS_CLOSED;
    process_post(tcps[id].p, evt_tcp, (process_data_t)&tcps[id]);
  }
}

/*********************************************************************************************
* 名称：on_gsm_tcp_recv
* 功能：在tcp链接收到数据后，发送evt_tcp事件通知用户应用程序
* 参数：
* 返回：
* 修改：
* 注释：用户不应该主动调用此函数
*********************************************************************************************/
void on_gsm_tcp_recv(int id, int len, char *buf)
{
  if (tcps[id].p == NULL) {
    return;
  }
  /*
  char *p = malloc(len);
  if (p != NULL) {
    memcpy(p, buf, len);
    tcps[id].pdat = p;
    tcps[id].datlen = len;
    if (PROCESS_ERR_OK != process_post(tcps[id].p, evt_tcp, (process_data_t)&tcps[id])) {
      free(p);
      tcps[id].pdat = NULL;
    }
  }*/
  Debug("tcp >>> ");
  for (int i=0; i<len; i++) {
    Debug("%c", buf[i]);
  }
  Debug("\r\n");
  
  tcps[id].pdat = buf;
  tcps[id].datlen = len;
   if (PROCESS_ERR_OK != process_post(tcps[id].p, evt_tcp, (process_data_t)&tcps[id])) {
      free(buf);
      tcps[id].pdat = NULL;
    }
}

/*********************************************************************************************
* 名称：__gsm_tcp_close
* 功能：主动关闭tcp链接底层接口
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
static void __gsm_tcp_close(int tid)
{
  char buf[32];
  sprintf(buf, "at+qiclose=%d\r\n", tid);
  gsm_request_at(buf, 3000, NULL);
}

/*********************************************************************************************
* 名称：gsm_tcp_close
* 功能：关闭tcp链接
* 参数：ptcp 本模块分配的tcp链接控制块
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void  gsm_tcp_close(tcp_t *ptcp)
{
  __gsm_tcp_close(ptcp->id);
  ptcp->status = TCP_STATUS_CLOSED;
  process_post(ptcp->p, evt_tcp, (process_data_t)ptcp);
 
}


/*********************************************************************************************
* 名称：on_gsm_tcp_open
* 功能：打开tcp链接
* 参数：ptcp：调用tcp_alloc分配的tcp控制块
*       sip：服务器ip地址
*       sport：服务器端口
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
tcp_t* gsm_tcp_open(tcp_t *ptcp, char *sip, int sport)
{

  char buf[64];
  
  /*初始化tcp事件*/
  if (evt_tcp == 0xff) {
    evt_tcp = process_alloc_event();
  }
  
  //ptcp->p = PROCESS_CURRENT();
  //ptcp->id = i;
  ptcp->status = TCP_STATUS_OPEN;
  sprintf(buf, "at+qiopen=1,%d,\"TCP\",\"%s\", %u,0,1\r\n", ptcp->id, sip, sport);
  gsm_request_at(buf, 10000, NULL);
  return ptcp;
}



/*********************************************************************************************
* 名称：__on_send_data
* 功能：
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
static tcp_t *current_tcp = NULL;

static void __on_send_data(char *rsp)
{
  extern void trim(char *);
  if (rsp == NULL) { //timeout
    send_len = 0;
    return;
  }
  else if (rsp[0] == '>') {
    /*  write data */
    Debug("tcp <<< ");
    for (int i=0; i<send_len; i++) {
      Debug("%c", send_buf[i]);
    }
    Debug("\r\n");
    
    gsm_uart_write(send_buf, send_len);
    send_len = 0;
  }
  else {
    //error ?
    send_len = 0;
    trim(rsp);
    Debug("__on_send_data(),rsp: %s\r\n", rsp);
    if((strlen(rsp)>0) && (strcmp(rsp, "SEND OK") == 0) )
    {
    }
    if((strlen(rsp)>0) && ((strcmp(rsp, "SEND FAIL") == 0) || (strcmp(rsp, "ERROR") == 0)))
    {
      printf("__on_send_data(),rsp: %s\r\n", rsp);
      //gsm_tcp_close(current_tcp);
    }
  }
}

/*********************************************************************************************
* 名称：gsm_tcp_send
* 功能：tcp链接发送数据
* 参数：ptcp：tcp链接控制块
*       dat：待发送数据缓存地址
*       len：待发送数据长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int gsm_tcp_send(tcp_t *ptcp,int len)
{
  char buf[32];
  if (len > TCP_BUF_SIZE) return -1;
  if (send_len != 0) return -2; //busy
  if (ptcp->status == TCP_STATUS_CONNECTED) {
    send_len = len;
    sprintf(buf, "at+qisend=%d,%d\r\n",ptcp->id, len);
    current_tcp = ptcp;
    gsm_request_at(buf, 1000, __on_send_data);
  }
  return -3;
}

/*********************************************************************************************
* 名称：gsm_tcp_send
* 功能：tcp链接发送数据
* 参数：ptcp：tcp链接控制块
*       dat：待发送数据缓存地址
*       len：待发送数据长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
char *gsm_tcp_buf(void)
{
  if (send_len != 0) return NULL; //busy
  return send_buf;
}

/*********************************************************************************************
* 名称：tcp_alloc
* 功能：分配tcp链接控制块
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
tcp_t* tcp_alloc()
{
  if (PROCESS_CURRENT() == NULL) return NULL;
  
  for (int i=0; i<CONNECT_NUMBER; i++) {
    if (tcps[i].p == NULL) {
      tcps[i].p = PROCESS_CURRENT();
      tcps[i].id = i;
      tcps[i].status = TCP_STATUS_CLOSED;
      return &tcps[i];
    }
  }
  return NULL;
}

/*********************************************************************************************
* 名称：tcp_free
* 功能：释放tcp链接控制块
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void tcp_free(tcp_t *ptcp)
{
  if (ptcp->status != TCP_STATUS_CLOSED) {
    __gsm_tcp_close(ptcp->id);
  }
  ptcp->p = NULL;
}
/*
void gsm_tcp_init(void)
{
  evt_tcp = process_alloc_event();
}
*/
