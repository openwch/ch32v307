/********************************** (C) COPYRIGHT *******************************
* File Name          : net_config.h
* Author             : WCH
* Version            : V1.30
* Date               : 2022/06/02
* Description        : This file contains the configurations of 
*                      Ethernet protocol stack library
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * socket configuration, IPRAW + UDP + TCP + TCP_LISTEN = number of sockets
 */
#define WCHNET_NUM_IPRAW              0  /* Number of IPRAW connections */

#define WCHNET_NUM_UDP                0  /* The number of UDP connections */

#define WCHNET_NUM_TCP                5  /* Number of TCP connections */

#define WCHNET_NUM_TCP_LISTEN         2  /* Number of TCP listening */

/* The number of sockets, the maximum is 31  */
#define WCHNET_MAX_SOCKET_NUM    (WCHNET_NUM_IPRAW+WCHNET_NUM_UDP+WCHNET_NUM_TCP+WCHNET_NUM_TCP_LISTEN)

#define WCHNET_TCP_MSS                800  /* Size of TCP MSS*/

#define WCHNET_NUM_POOL_BUF           (WCHNET_NUM_TCP*2+2)   /* The number of POOL BUFs, the number of receive queues */

/*********************************************************************
 * MAC queue configuration
 */
#define ETH_TXBUFNB                   2    /* The number of descriptors sent by the MAC  */

#define ETH_RXBUFNB                   4    /* Number of MAC received descriptors  */

#ifndef ETH_MAX_PACKET_SIZE
#define ETH_RX_BUF_SZE                1520  /* MAC receive buffer length, an integer multiple of 4 */
#define ETH_TX_BUF_SZE                1520  /* MAC send buffer length, an integer multiple of 4 */
#else
#define ETH_RX_BUF_SZE                ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SZE                ETH_MAX_PACKET_SIZE
#endif

/*********************************************************************
  *  Functional configuration
 */
#define WCHNET_PING_ENABLE            1    /* PING is enabled, PING is enabled by default */

#define TCP_RETRY_COUNT               20   /* The number of TCP retransmissions, the default value is 20 */

#define TCP_RETRY_PERIOD              10   /* TCP retransmission period, the default value is 10, the unit is 50ms */

#define SOCKET_SEND_RETRY             1    /* Send failed retry configuration, 1: enable, 0: disable */

#define CFG0_TCP_SEND_COPY            1    /* TCP send buffer copy, 1: copy, 0: not copy */

#define CFG0_TCP_RECV_COPY            1    /* TCP receive replication optimization, internal debugging use */

#define CFG0_TCP_OLD_DELETE           0    /* Delete oldest TCP connection, 1: enable, 0: disable */

/*********************************************************************
  *  Memory related configuration
 */
/*If a single socket cannot reach full speed,
 * try to increase RECE_BUF_LEN to (WCHNET_TCP_MSS*4)
 * and increase WCHNET_NUM_TCP_SEG to (WCHNET_NUM_TCP*4)*/
#define RECE_BUF_LEN                  (WCHNET_TCP_MSS*2)   /* socket receive buffer size */

#define WCHNET_NUM_PBUF               (WCHNET_MAX_SOCKET_NUM+WCHNET_NUM_TCP)   /* Number of PBUF structures */

#define WCHNET_NUM_TCP_SEG            (WCHNET_NUM_TCP*2)   /* The number of TCP segments used to send */

#define WCHNET_MEM_HEAP_SIZE          (((WCHNET_TCP_MSS+0x10+54)*WCHNET_NUM_TCP_SEG)+ETH_TX_BUF_SZE+64) /* memory heap size */

#define WCHNET_NUM_ARP_TABLE          50   /* Number of ARP lists */

#define WCHNET_NUM_IP_REASSDATA       4    /* Number of IP segments */

#define WCHNET_MEM_ALIGNMENT          4    /* 4 byte alignment */

#if (WCHNET_NUM_POOL_BUF * (((WCHNET_TCP_MSS + 58) + 3) & ~3) < ETH_RX_BUF_SZE)
    #error "WCHNET_NUM_POOL_BUF or WCHNET_TCP_MSS Error"
    #error "Please Increase WCHNET_NUM_POOL_BUF or WCHNET_TCP_MSS to make sure the receive buffer is sufficient"
#endif
/* Check the configuration of the SOCKET quantity */
#if( WCHNET_NUM_TCP_LISTEN && !WCHNET_NUM_TCP )
    #error "WCHNET_NUM_TCP Error"
#endif
/* Check byte alignment must be a multiple of 4 */
#if((WCHNET_MEM_ALIGNMENT % 4) || (WCHNET_MEM_ALIGNMENT == 0))
  #error "WCHNET_MEM_ALIGNMENT Error,Please Config WCHNET_MEM_ALIGNMENT = 4 * N, N >=1"
#endif
/* TCP maximum segment length */
#if((WCHNET_TCP_MSS > 1460) || (WCHNET_TCP_MSS < 60))
  #error "WCHNET_TCP_MSS Error,Please Config WCHNET_TCP_MSS >= 60 && WCHNET_TCP_MSS <= 1460"
#endif
/* Number of ARP cache tables */
#if((WCHNET_NUM_ARP_TABLE > 0X7F) || (WCHNET_NUM_ARP_TABLE < 1))
  #error "WCHNET_NUM_ARP_TABLE Error,Please Config WCHNET_NUM_ARP_TABLE >= 1 && WCHNET_NUM_ARP_TABLE <= 0X7F"
#endif
/* Check POOL BUF configuration */
#if(WCHNET_NUM_POOL_BUF < 1)
  #error "WCHNET_NUM_POOL_BUF Error,Please Config WCHNET_NUM_POOL_BUF >= 1"
#endif
/* Check PBUF structure configuration */
#if(WCHNET_NUM_PBUF < 1)
  #error "WCHNET_NUM_PBUF Error,Please Config WCHNET_NUM_PBUF >= 1"
#endif
/* Check IP Assignment Configuration */
#if((WCHNET_NUM_IP_REASSDATA > 10) || (WCHNET_NUM_IP_REASSDATA < 1))
  #error "WCHNET_NUM_IP_REASSDATA Error,Please Config WCHNET_NUM_IP_REASSDATA < 10 && WCHNET_NUM_IP_REASSDATA >= 1 "
#endif
/* Check the number of reassembled IP PBUFs  */
#if(WCHNET_IP_REASS_PBUFS > WCHNET_NUM_POOL_BUF)
  #error "WCHNET_IP_REASS_PBUFS Error,Please Config WCHNET_IP_REASS_PBUFS < WCHNET_NUM_POOL_BUF"
#endif

#if(WCHNETTIMERPERIOD > 50)
  #error "WCHNETTIMERPERIOD Error,Please Config WCHNETTIMERPERIOD < 50"
#endif

/* Configuration value 0 */
#define WCHNET_MISC_CONFIG0    (((CFG0_TCP_SEND_COPY) << 0) |\
                               ((CFG0_TCP_RECV_COPY)  << 1) |\
                               ((CFG0_TCP_OLD_DELETE) << 2) )
/* Configuration value 1 */
#define WCHNET_MISC_CONFIG1    (((WCHNET_MAX_SOCKET_NUM)<<0)|\
                               ((WCHNET_PING_ENABLE) << 13) |\
                               ((TCP_RETRY_COUNT)    << 14) |\
                               ((TCP_RETRY_PERIOD)   << 19) |\
                               ((SOCKET_SEND_RETRY)  << 25) )

#ifdef __cplusplus
}
#endif
#endif
