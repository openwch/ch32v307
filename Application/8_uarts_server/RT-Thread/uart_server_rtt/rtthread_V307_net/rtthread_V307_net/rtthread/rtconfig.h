/* RT-Thread config file */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

#if defined(__CC_ARM) || defined(__CLANG_ARM)
#include "RTE_Components.h"

#if defined(RTE_USING_FINSH)
#define RT_USING_FINSH
#endif //RTE_USING_FINSH

#endif //(__CC_ARM) || (__CLANG_ARM)


#define RT_THREAD_PRIORITY_MAX  16
#define RT_TICK_PER_SECOND  1000
#define RT_ALIGN_SIZE   4
#define RT_NAME_MAX    8
#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE     1024


#define RT_DEBUG_INIT 0
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_USING_TIMER_SOFT         0
#if RT_USING_TIMER_SOFT == 0
    #undef RT_USING_TIMER_SOFT
#endif
#define RT_TIMER_THREAD_PRIO        4
#define RT_TIMER_THREAD_STACK_SIZE  1024


#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE


//#define RT_USING_MEMPOOL
//#define RT_USING_MEMHEAP
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

//#define RT_USING_MODULE

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME  "uart1"


#define RT_USING_FINSH
#if defined(RT_USING_FINSH)
    #define FINSH_USING_MSH
    #define FINSH_USING_MSH_DEFAULT
    #define FINSH_USING_MSH_ONLY
    #define FINSH_THREAD_NAME "tshell"

    #define __FINSH_THREAD_PRIORITY     5
    #define FINSH_THREAD_PRIORITY       (RT_THREAD_PRIORITY_MAX / 8 * __FINSH_THREAD_PRIORITY + 1)
    // <o>the stack of finsh thread <1-4096>
    //  <i>the stack of finsh thread
    //  <i>Default: 4096  (4096Byte)
    #define FINSH_THREAD_STACK_SIZE     512

    #define FINSH_USING_HISTORY
    #define FINSH_HISTORY_LINES         5
    #define FINSH_USING_SYMTAB
    #define FINSH_USING_DESCRIPTION
    #define FINSH_CMD_SIZE 80


#endif

// <<< end of configuration section >>>

#define RT_USING_SERIAL
#define RT_USING_PIN
#define RT_USING_UART1

//#define RT_USING_SPI
//#define RT_USING_SFUD
//#define RT_SFUD_USING_SFDP
//#define RT_SFUD_USING_FLASH_INFO_TABLE
//#define RT_SFUD_SPI_MAX_HZ 50000000

#define RT_USING_DEVICE_IPC


/* bsp */
//#define BSP_USING_SPI
//#define BSP_USING_SPI1
//#define BSP_USING_SPI_FLASH
//
//#define BSP_USING_ADC1

//#define BSP_SPI1_RX_USING_DMA
//#define BSP_SPI1_TX_USING_DMA


/* Network interface device */
//#define RT_USING_NETDEV
////#define NETDEV_USING_IFCONFIG
//#define NETDEV_USING_PING
////#define NETDEV_USING_NETSTAT
//#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0

/* light weight TCP/IP stack */

#define RT_USING_LWIP
#define RT_USING_LWIP212
#define RT_LWIP_MEM_ALIGNMENT 4
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_DNS
//#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "192.168.1.30"
#define RT_LWIP_GWADDR "192.168.1.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
//#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 8
#define RT_LWIP_TCP_PCB_NUM 8
#define RT_LWIP_TCP_SEG_NUM 16
#define RT_LWIP_TCP_SND_BUF (TCP_MSS * 2)
#define RT_LWIP_TCP_WND TCP_MSS
#define RT_LWIP_TCPTHREAD_PRIORITY 4
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 32
#define RT_LWIP_TCPTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_PRIORITY 6
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 32
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_SO_LINGER 0
#define LWIP_NETIF_LOOPBACK 0
#define RT_LWIP_USING_PING



#define PKG_NETUTILS_IPERF

//#define RT_LWIP_DEBUG
#define RT_LWIP_SYS_DEBUG
//#define RT_LWIP_ETHARP_DEBUG
//#define RT_LWIP_NETIF_DEBUG
#define RT_LWIP_MEM_DEBUG
#define RT_LWIP_MEMP_DEBUG
//#define RT_LWIP_PBUF_DEBUG
//#define RT_LWIP_SOCKETS_DEBUG
#endif
