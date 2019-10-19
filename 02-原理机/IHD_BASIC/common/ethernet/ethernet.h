#ifndef _ethernet_h_
#define _ethernet_h_

#include <contiki.h>
#include "uip_arp.h"
#include "uip.h"
#include "uipopt.h"
#include "tcpip.h"
#include "dm9051_stm.h"
#include "key.h"

#define BUF             ((struct uip_eth_hdr *)&uip_buf[0])
#define UIP_IP_BUF      ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_ICMP_BUF    ((struct uip_icmp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])

#define PING_DATALEN 16
#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO       8

#define DHCP_ON 0

PROCESS_NAME(ethernet_process);
void sendPing(uip_ipaddr_t *dest_addr);

#endif