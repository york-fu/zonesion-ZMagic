#include "ethernet.h"

void tcp_client_connect();

uint8_t tapdev_init()
{
    return etherdev_init();
}

uint16_t tapdev_read()
{
    return etherdev_read();
}

void tapdev_send(void)
{
    etherdev_send();
}

void localHostAddrInit(void)
{
    uip_lladdr_t ethAddr;
    ethAddr.addr[0] = emacETHADDR0;
    ethAddr.addr[1] = emacETHADDR1;
    ethAddr.addr[2] = emacETHADDR2;
    ethAddr.addr[3] = emacETHADDR3;
    ethAddr.addr[4] = emacETHADDR4;
    ethAddr.addr[5] = emacETHADDR5;
    uip_setethaddr(ethAddr);
    
#if !DHCP_ON
    uip_ipaddr_t ipaddr;
    uip_ipaddr(&ipaddr, 192,168,1,101);		//Host IP address
    uip_sethostaddr(&ipaddr);
    uip_ipaddr(&ipaddr, 192,168,1,1);		//Default Gateway
    uip_setdraddr(&ipaddr);
    uip_ipaddr(&ipaddr, 255,255,255,0);     //Network Mask
    uip_setnetmask(&ipaddr);
#endif
}

uint8_t my_tcpip_output()
{
    uip_arp_out();
    tapdev_send();
    return 0;
}

void tcpipFuncInit(void)
{
    tcpip_set_outputfunc(my_tcpip_output);
}

void sendPing(uip_ipaddr_t *dest_addr)
{
    static uint16_t ipid = 0;
    static uint16_t seqno = 0;

    UIP_IP_BUF->vhl = 0x45;
    UIP_IP_BUF->tos = 0;
    UIP_IP_BUF->ipoffset[0] = UIP_IP_BUF->ipoffset[1] = 0;
    ++ipid;
    UIP_IP_BUF->ipid[0] = ipid >> 8;
    UIP_IP_BUF->ipid[1] = ipid & 0xff;
    UIP_IP_BUF->proto = UIP_PROTO_ICMP;
    UIP_IP_BUF->ttl = UIP_TTL;

    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest_addr);
    uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &uip_hostaddr);

    UIP_ICMP_BUF->type = ICMP_ECHO;
    UIP_ICMP_BUF->icode = 0;
    UIP_ICMP_BUF->id = 0xadad;
    UIP_ICMP_BUF->seqno = uip_htons(seqno++);

    uip_len = UIP_ICMPH_LEN + UIP_IPH_LEN + PING_DATALEN;
    UIP_IP_BUF->len[0] = (uint8_t)((uip_len) >> 8);
    UIP_IP_BUF->len[1] = (uint8_t)((uip_len) & 0x00ff);

    UIP_ICMP_BUF->icmpchksum = 0;
    UIP_ICMP_BUF->icmpchksum = ~uip_chksum((uint16_t *)&(UIP_ICMP_BUF->type),
                                           UIP_ICMPH_LEN + PING_DATALEN);

    /* Calculate IP checksum. */
    UIP_IP_BUF->ipchksum = 0;
    UIP_IP_BUF->ipchksum = ~(uip_ipchksum());

    tcpip_output();
}


extern uint8_t testFlag_ethernet;
PROCESS(uip_poll, "uip_poll");

PROCESS_THREAD(uip_poll, ev, data)
{
    PROCESS_BEGIN();
    
    static struct etimer arp_timer,pollTimer;
    
    etimer_set(&pollTimer, 5000);
    while(tapdev_init())
    {   
        if(etimer_expired(&pollTimer)) 
        {
            testFlag_ethernet=0x02;
            PROCESS_EXIT();
        }
    }
    
    uip_arp_init();
    localHostAddrInit();
    tcpipFuncInit();
    
    etimer_set(&pollTimer, 1);
    etimer_set(&arp_timer, 1000*10);
    
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&pollTimer))
        {
            etimer_reset(&pollTimer);
            uip_len = tapdev_read();
            if(uip_len > 0)
            {
                testFlag_ethernet=0x01;
                
                if(BUF->type == uip_htons(UIP_ETHTYPE_IP))
                {
                    uip_arp_ipin();
                    tcpip_input();
                    /* If the above function invocation resulted in data that
                    should be sent out on the network, the global variable
                    uip_len is set to a value > 0. */
                    if(uip_len > 0)
                    {
                        uip_arp_out();
                        tapdev_send();
                    }
                }
                else if(BUF->type == uip_htons(UIP_ETHTYPE_ARP))
                {
                    uip_arp_arpin();
                    /* If the above function invocation resulted in data that
                    should be sent out on the network, the global variable
                    uip_len is set to a value > 0. */
                    if(uip_len > 0)
                    {
                        tapdev_send();
                    }
                }
                
            }
            /* Call the ARP timer function every 10 seconds. */
            if(etimer_expired(&arp_timer))
            {
                etimer_reset(&arp_timer);
                uip_arp_timer();
            }
        }
    }
    PROCESS_END();
}



PROCESS(ethernet_process, "ethernet_process");

PROCESS_THREAD(ethernet_process, ev, data)
{
    PROCESS_BEGIN();
    
    process_start(&tcpip_process,NULL);
    process_start(&uip_poll,NULL);
    
    uip_ipaddr_t ipaddr;
    uip_ipaddr(&ipaddr, 192,168,1,1);
    sendPing(&ipaddr);
    
    PROCESS_END();
}
