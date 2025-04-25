/********************************** (C) COPYRIGHT *******************************
 * File Name          : wchnet.h
 * Author             : WCH
 * Version            : V1.90
 * Date               : 2023/05/12
 * Description        : This file contains the headers of 
*                    the Ethernet protocol stack library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __WCHNET_H__
#define __WCHNET_H__

#include "stdint.h"
#ifndef NET_LIB
#include "net_config.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

#define WCHNET_LIB_VER                  0x1B              //the library version number
#define WCHNET_CFG_VALID                0x12345678        //Configuration value valid flag

/* LED state @LED_STAT */
#define  LED_ON                         0
#define  LED_OFF                        1

/* PHY state @PHY_STAT */
#define PHY_LINK_SUCCESS                (1 << 2)          //PHY connection success
#define PHY_AUTO_SUCCESS                (1 << 5)          //PHY auto negotiation completed

/* Library initialization state @CFG_INIT_STAT */
#define  INIT_OK                        0x00
#define  INIT_ERR_RX_BUF_SIZE           0x01
#define  INIT_ERR_TCP_MSS               0x02
#define  INIT_ERR_HEAP_SIZE             0x03
#define  INIT_ERR_ARP_TABLE_NEM         0x04
#define  INIT_ERR_MISC_CONFIG0          0x05
#define  INIT_ERR_MISC_CONFIG1          0x06
#define  INIT_ERR_FUNC_SEND             0x09
#define  INIT_ERR_CHECK_VALID           0xFF

/* Socket protocol type */
#define PROTO_TYPE_IP_RAW               0                 //IP layer raw data
#define PROTO_TYPE_UDP                  2                 //UDP protocol
#define PROTO_TYPE_TCP                  3                 //TCP protocol

/* interrupt status */
/* The following are the states 
 * that GLOB_INT will generate */
#define GINT_STAT_UNREACH               (1 << 0)          //unreachable interrupt
#define GINT_STAT_IP_CONFLI             (1 << 1)          //IP conflict interrupt
#define GINT_STAT_PHY_CHANGE            (1 << 2)          //PHY state change interrupt
#define GINT_STAT_SOCKET                (1 << 4)          //socket related interrupt

/* The following are the states 
 * that Sn_INT will generate*/
#define SINT_STAT_RECV                  (1 << 2)          //the socket receives data or the receive buffer is not empty
#define SINT_STAT_CONNECT               (1 << 3)          //connect successfully,generated in TCP mode
#define SINT_STAT_DISCONNECT            (1 << 4)          //disconnect,generated in TCP mode
#define SINT_STAT_TIM_OUT               (1 << 6)          //timeout disconnect,generated in TCP mode


/* Definitions for error constants. @ERR_T */
#define ERR_T
#define WCHNET_ERR_SUCCESS              0x00              //No error, everything OK
#define WCHNET_ERR_BUSY                 0x10              //busy
#define WCHNET_ERR_MEM                  0x11              //Out of memory error
#define WCHNET_ERR_BUF                  0x12              //Buffer error
#define WCHNET_ERR_TIMEOUT              0x13              //Timeout
#define WCHNET_ERR_RTE                  0x14              //Routing problem
#define WCHNET_ERR_ABRT                 0x15              //Connection aborted
#define WCHNET_ERR_RST                  0x16              //Connection reset
#define WCHNET_ERR_CLSD                 0x17              //Connection closed
#define WCHNET_ERR_CONN                 0x18              //Not connected
#define WCHNET_ERR_VAL                  0x19              //Illegal value
#define WCHNET_ERR_ARG                  0x1a              //Illegal argument
#define WCHNET_ERR_USE                  0x1b              //Address in use
#define WCHNET_ERR_IF                   0x1c              //Low-level netif error 
#define WCHNET_ERR_ISCONN               0x1d              //Already connected
#define WCHNET_ERR_INPROGRESS           0x1e              //Operation in progress
#define WCHNET_ERR_SOCKET_MEM           0X20              //Socket information error
#define WCHNET_ERR_UNSUPPORT_PROTO      0X21              //unsupported protocol type
#define WCHNET_RET_ABORT                0x5F              //command process fail
#define WCHNET_ERR_UNKNOW               0xFA              //unknow

/* unreachable condition related codes */
#define UNREACH_CODE_HOST               0                 //host unreachable
#define UNREACH_CODE_NET                1                 //network unreachable
#define UNREACH_CODE_PROTOCOL           2                 //protocol unreachable
#define UNREACH_CODE_PROT               3                 //port unreachable
/*For other values, please refer to the RFC792 document*/

/* TCP disconnect related codes */
#define TCP_CLOSE_NORMAL                0                 //normal disconnect,a four-way handshake
#define TCP_CLOSE_RST                   1                 //reset the connection and close
#define TCP_CLOSE_ABANDON               2                 //drop connection, and no termination message is sent

/* socket state code */
#define SOCK_STAT_CLOSED                0X00              //socket close
#define SOCK_STAT_OPEN                  0X05              //socket open 

/* TCP state code */
#define TCP_CLOSED                      0                 //TCP close
#define TCP_LISTEN                      1                 //TCP listening
#define TCP_SYN_SENT                    2                 //SYN send, connect request
#define TCP_SYN_RCVD                    3                 //SYN received, connection request received
#define TCP_ESTABLISHED                 4                 //TCP connection establishment
#define TCP_FIN_WAIT_1                  5                 //WAIT_1 state
#define TCP_FIN_WAIT_2                  6                 //WAIT_2 state
#define TCP_CLOSE_WAIT                  7                 //wait to close
#define TCP_CLOSING                     8                 //closing
#define TCP_LAST_ACK                    9                 //LAST_ACK
#define TCP_TIME_WAIT                   10                //2MSL wait

/* The following values are fixed and cannot be changed */
#define WCHNET_MEM_ALIGN_SIZE(size)    (((size) + WCHNET_MEM_ALIGNMENT - 1) & ~(WCHNET_MEM_ALIGNMENT - 1))
#define WCHNET_SIZE_IPRAW_PCB          0x1C               //IPRAW PCB size
#define WCHNET_SIZE_UDP_PCB            0x20               //UDP PCB size
#define WCHNET_SIZE_TCP_PCB            0xB4               //TCP PCB size
#define WCHNET_SIZE_TCP_PCB_LISTEN     0x24               //TCP LISTEN PCB size
#define WCHNET_SIZE_IP_REASSDATA       0x20               //IP reassembled Management
#define WCHNET_SIZE_PBUF               0x10               //Packet Buf
#define WCHNET_SIZE_TCP_SEG            0x14               //TCP SEG structure
#define WCHNET_SIZE_MEM                0x08               //sizeof(struct mem)
#define WCHNET_SIZE_ARP_TABLE          0x18               //sizeof ARP table

#define WCHNET_MEMP_SIZE               ((WCHNET_MEM_ALIGNMENT - 1) +                                    \
                          (WCHNET_NUM_IPRAW * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IPRAW_PCB)) +           \
                          (WCHNET_NUM_UDP * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_UDP_PCB)) +               \
                          (WCHNET_NUM_TCP * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB)) +               \
                          (WCHNET_NUM_TCP_LISTEN * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB_LISTEN)) + \
                          (WCHNET_NUM_TCP_SEG * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_SEG)) +           \
                          (WCHNET_NUM_IP_REASSDATA * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IP_REASSDATA)) + \
                          (WCHNET_NUM_PBUF * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF)) +                 \
                          (WCHNET_NUM_POOL_BUF * (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_POOL_BUF))))

#define HEAP_MEM_ALIGN_SIZE          (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_MEM))
#define WCHNET_RAM_HEAP_SIZE         (WCHNET_MEM_ALIGN_SIZE(WCHNET_MEM_HEAP_SIZE) + HEAP_MEM_ALIGN_SIZE )
#define WCHNET_RAM_ARP_TABLE_SIZE    (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_ARP_TABLE) * WCHNET_NUM_ARP_TABLE)

typedef struct
{
    uint32_t length;
    uint32_t buffer;
}ETHFrameType;

/* LED callback type */
typedef void (*led_callback)( uint8_t setbit );

/* net send callback type */
typedef uint32_t (*eth_tx_set )( uint16_t len, uint32_t *pBuff );

/* net receive callback type */
typedef uint32_t (*eth_rx_set )( ETHFrameType *pkt );

/* DNS callback type */
typedef void (*dns_callback)( const char *name, uint8_t *ipaddr, void *callback_arg );

/* DHCP callback type */
typedef uint8_t (*dhcp_callback)( uint8_t status, void * );

/* socket receive callback type */
struct _SOCK_INF;
typedef void (*pSockRecv)( struct _SOCK_INF *, uint32_t, uint16_t, uint8_t *, uint32_t);

typedef uint8_t (*pTcpVerifyConnection)(uint32_t src_ip, uint16_t src_port, uint16_t dest_port);

/* Socket information struct */
typedef struct _SOCK_INF
{
    uint32_t IntStatus;                       //interrupt state
    uint32_t SockIndex;                       //Socket index value
    uint32_t RecvStartPoint;                  //Start pointer of the receive buffer
    uint32_t RecvBufLen;                      //Receive buffer length
    uint32_t RecvCurPoint;                    //current pointer to receive buffer
    uint32_t RecvReadPoint;                   //The read pointer of the receive buffer
    uint32_t RecvRemLen;                      //The length of the remaining data in the receive buffer
    uint32_t ProtoType;                       //protocol type
    uint32_t SockStatus;                      //Low byte Socket state, the next low byte is TCP state, only meaningful in TCP mode
    uint32_t DesPort;                         //destination port
    uint32_t SourPort;                        //Source port, protocol type in IPRAW mode
    uint8_t  IPAddr[4];                       //Socket destination IP address
    void *Resv1;                              //Reserved, for internal use, for saving individual PCBs
    void *Resv2;                              //Reserved, used internally, used by TCP Server
    pSockRecv AppCallBack;                    //receive callback function
} SOCK_INF;

struct _WCH_CFG
{
  uint32_t TxBufSize;                         //MAC send buffer size, reserved for use
  uint32_t TCPMss;                            //TCP MSS size
  uint32_t HeapSize;                          //heap memory size
  uint32_t ARPTableNum;                       //Number of ARP lists
  uint32_t MiscConfig0;                       //Miscellaneous Configuration 0
  /* Bit 0 TCP send buffer copy 1: copy, 0: not copy */
  /* Bit 1 TCP receive replication optimization, used for internal debugging */
  /* bit 2 delete oldest TCP connection 1: enable, 0: disable */
  /* Bits 3-7 Number of PBUFs of IP segments  */
  /* Bit 8 TCP Delay ACK disable */
  uint32_t MiscConfig1;                       //Miscellaneous Configuration 1
  /* Bits 0-7 Number of Sockets*/
  /* Bits 8-12 Reserved */
  /* Bit  13 PING enable, 1: On 0: Off  */
  /* Bits 14-18 TCP retransmission times  */
  /* Bits 19-23 TCP retransmission period, in 50 milliseconds  */
  /* bit  25 send failed retry, 1: enable, 0: disable */
  /* bit  26 Select whether to perform IPv4 checksum check on
   *         the TCP/UDP/ICMP header of the received frame payload by hardware,
   *         and calculate and insert the checksum of the IP header and payload of the sent frame by hardware.*/
  /* Bits 27-31 period (in 250 milliseconds) of Fine DHCP periodic process */
  led_callback led_link;                      //PHY Link Status Indicator
  led_callback led_data;                      //Ethernet communication indicator
  eth_tx_set net_send;                        //Ethernet send
  eth_rx_set net_recv;                        //Ethernet receive
  uint32_t   CheckValid;                      //Configuration value valid flag, fixed value @WCHNET_CFG_VALID
};

struct _NET_SYS
{
  uint8_t  IPAddr[4];                         //IP address 
  uint8_t  GWIPAddr[4];                       //Gateway IP address
  uint8_t  MASKAddr[4];                       //subnet mask
  uint8_t  MacAddr[8];                        //MAC address
  uint8_t  UnreachIPAddr[4];                  //Unreachable IP address
  uint32_t RetranCount;                       //number of retries,default is 10 times
  uint32_t RetranPeriod;                      //Retry period, unit MS, default 500MS
  uint32_t PHYStat;                           //PHY state code
  uint32_t NetStat;                           //The status of the Ethernet, including whether it is open, etc.
  uint32_t MackFilt;                          //MAC filtering, the default is to receive broadcasts, receive local MAC
  uint32_t GlobIntStatus;                     //global interrupt
  uint32_t UnreachCode;                       //unreachable code
  uint32_t UnreachProto;                      //unreachable protocol
  uint32_t UnreachPort;                       //unreachable port
  uint32_t SendFlag;
  uint32_t Flags;
};

/* KEEP LIVE configuration structure */
struct _KEEP_CFG
{
  uint32_t KLIdle;                            //KEEPLIVE idle time, in ms
  uint32_t KLIntvl;                           //KEEPLIVE period, in ms
  uint32_t KLCount;                           //KEEPLIVE times
};

/**
 * @brief   Library initialization .
 *
 * @param   ip - IP address pointer 
 * @param   gwip - Gateway address pointer 
 * @param   mask - Subnet mask pointer 
 * @param   macaddr - MAC address pointer
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_Init(const uint8_t *ip, const uint8_t *gwip, const uint8_t *mask, const uint8_t *macaddr);

/**
 * @brief   get library version
 *
 * @param   None
 *
 * @return  library version
 */
uint8_t WCHNET_GetVer(void);

/**
 * @brief   Get MAC address. 
 *
 * @param(in)   macaddr - MAC address
 *
 * @param(out)  MAC address
 *
 * @return  None
 */
void WCHNET_GetMacAddr(uint8_t *macaddr);

/**
 * @brief   Library parameter configuration.
 *
 * @param   cfg - Configuration parameter  @_WCH_CFG
 *
 * @return  Library configuration initialization state @CFG_INIT_STAT
 */
uint8_t WCHNET_ConfigLIB(struct _WCH_CFG *cfg);

/**
 * @brief   Handle periodic tasks in the protocol stack
 *
 * @param   None
 *
 * @return  None
 */
void WCHNET_PeriodicHandle(void);

/**
 * @brief   Ethernet data input. Always called in the main program, 
 *          or called after the reception interrupt is detected.
 *
 * @param
 *
 * @return  None
 */
void WCHNET_NetInput( void );

/**
 * @brief   Ethernet interrupt service function. Called after 
 *          Ethernet interrupt is generated. 
 *
 * @param   None
 *
 * @return  None
 */
void WCHNET_ETHIsr(void);

/**
 * @brief   Get PHY status
 *
 * @param   None
 *
 * @return  PHY status @PHY_STAT
 */
uint8_t WCHNET_GetPHYStatus(void);

/**
 * @brief   Query global interrupt status.
 *
 * @param   None
 *
 * @return  GLOB_INT
 */
uint8_t WCHNET_QueryGlobalInt(void);

/**
 * @brief   Read global interrupt and clear it.
 *
 * @param   None
 *
 * @return  GLOB_INT
 */
uint8_t WCHNET_GetGlobalInt(void);

/**
 * @brief   create socket
 *
 * @param(in)   *socketid - socket variable pointer
 * @param       socinf - Configuration parameters for creating sockets @SOCK_INF
 *
 * @param(out)  *socketid - socket value
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketCreat( uint8_t *socketid, SOCK_INF *socinf);

/**
 * @brief   Socket sends data.
 *
 * @param       socketid - socket id value
 * @param       *buf - the first address of send buffer
 * @param(in)   *len - pointer to the length of the data expected to be sent
 *
 * @param(out)  *len - pointer to the length of the data sent actually
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketSend( uint8_t socketid, uint8_t *buf, uint32_t *len);

/**
 * @brief   Socket receives data.
 *
 * @param       socketid - socket id value
 * @param       *buf - the first address of receive buffer
 * @param(in)   *len - pointer to the length of the data expected to be read
 *
 * @param(out)  *buf - the first address of data buffer
 * @param(out)  *len - pointer to the length of the data read actually
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketRecv( uint8_t socketid, uint8_t *buf, uint32_t *len);

/**
 * @brief   Get socket interrupt, and clear socket interrupt. 
 *
 * @param   socketid - socket id value
 *
 * @return  Sn_INT
 */
uint8_t WCHNET_GetSocketInt( uint8_t socketid );

/**
 * @brief   Get the length of the data received by socket. 
 *
 * @param       socketid - socket id value
 * @param(in)   *bufaddr - the first address of receive buffer
 *
 * @param(out)  *bufaddr - the first address of data buffer
 *
 * @return  the length of the data
 */
uint32_t WCHNET_SocketRecvLen( uint8_t socketid, uint32_t *bufaddr);

/**
 * @brief   TCP connect. Used in TCP Client mode. 
 *
 * @param   socketid - socket id value
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketConnect( uint8_t socketid);

/**
 * @brief   TCP listen. Used in TCP SERVER mode. 
 *
 * @param   socketid - socket id value
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketListen( uint8_t socketid);

/**
 * @brief   Close socket. 
 *
 * @param   socketid - socket id value
 * @param   mode - the way of disconnection.Used in TCP connection.
 *                 @TCP disconnect related codes
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketClose( uint8_t socketid, uint8_t mode );

/**
 * @brief   Modify socket receive buffer. 
 *
 * @param   socketid - socket id value
 * @param   bufaddr - Address of the receive buffer 
 * @param   bufsize - Size of the receive buffer 
 *
 * @return  None
 */
void WCHNET_ModifyRecvBuf( uint8_t socketid, uint32_t bufaddr, uint32_t bufsize);

/**
 * @brief   UDP send, specify the target IP and target port 
 *
 * @param       socketid - socket id value
 * @param       *buf - Address of the sent data
 * @param(in)   *slen - Address of the sent length 
 * @param       *sip - destination IP address
 * @param       port - destination port
 *
 * @param(out)  *slen - actual length sent
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketUdpSendTo( uint8_t socketid, uint8_t *buf, uint32_t *slen, uint8_t *sip, uint16_t port);

/**
 * @brief   Convert ASCII address to network address. 
 *
 * @param       *cp - ASCII address to be converted, such as "192.168.1.2"
 * @param(in)   *addr - First address of the memory stored in the converted network address 
 * @param(out)  *addr -  Converted network address, such as 0xC0A80102 
 * @return  0 - Success.   Others - Failure. 
 */
uint8_t WCHNET_Aton(const char *cp, uint8_t *addr);

/**
 * @brief   Convert network address to ASCII address. 
 *
 * @param   *ipaddr - socket id value
 *
 * @return  Converted ASCII address 
 */
uint8_t *WCHNET_Ntoa( uint8_t *ipaddr);

/**
 * @brief   Set socket TTL. 
 *
 * @param   socketid - socket id value
 * @param   ttl - TTL value 
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SetSocketTTL( uint8_t socketid, uint8_t ttl);

/**
 * @brief   Start TCP retry sending immediately. 
 *
 * @param   socketid - TTL value
 *
 * @return  None
 */
void WCHNET_RetrySendUnack( uint8_t socketid);

/**
 * @brief   Query the packets that are not sent successfully. 
 *
 * @param       socketid - TTL value
 * @param(in)   *addrlist - pointer to the address of the address list
 * @param       lislen - Length of the list
 *
 * @param(out)  *addrlist - Address list of the data packets that are not sent successfully 
 *
 * @return  Number of unsent and unacknowledged segments 
 */
uint8_t WCHNET_QueryUnack( uint8_t socketid, uint32_t *addrlist, uint16_t lislen );

/**
 * @brief   Start DHCP. 
 *
 * @param   dhcp - Application layer callback function 
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_DHCPStart( dhcp_callback dhcp );

/**
 * @brief   Stop DHCP.
 *
 * @param   None
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_DHCPStop( void );

/**
 * @brief   Configure DHCP host name. 
 *
 * @param   *name - First address of DHCP host name 
 *
 * @return  0 - Success.    Others - Failure. 
 */
uint8_t WCHNET_DHCPSetHostname(char *name);

/**
 * @brief   Initialize the resolver: set up the UDP pcb and configure the default server
 *
 * @param   *dnsip - the IP address of dns server 
 * @param   port - the port number of dns server 
 *
 * @return  None
 */
void WCHNET_InitDNS( uint8_t *dnsip, uint16_t port);

/**
 * @brief   Stop DNS. 
 *
 * @param   None
 *
 * @return  None
 */
void WCHNET_DNSStop(void);

/**
 * Resolve a hostname (string) into an IP address.
 *
 * @param   hostname - the hostname that is to be queried
 * @param   addr     - pointer to a struct ip_addr where to store the address if it is already
 *                     cached in the dns_table (only valid if ERR_OK is returned!)
 * @param   found    - a callback function to be called on success, failure or timeout (only if
 *                     ERR_INPROGRESS is returned!)
 * @param   arg      - argument to pass to the callback function
 *
 * @return  @ERR_T
 *   WCHNET_ERR_SUCCESS if hostname is a valid IP address string or the host name is already in the local names table.
 *   ERR_INPROGRESS     enqueue a request to be sent to the DNS server for resolution if no errors are present.
 */
uint8_t WCHNET_HostNameGetIp( const char *hostname, uint8_t *addr, dns_callback found, void *arg );

/**
 * @brief   Configure KEEP LIVE parameter. 
 *
 * @param   *cfg - KEEPLIVE configuration parameter 
 *
 * @return  None
 */
void WCHNET_ConfigKeepLive( struct _KEEP_CFG *cfg );

/**
 * @brief   Configure ARP retransmission parameters.
 *
 * @param   retry_period - ARP retransmission period, the default value is 10, the unit is 100ms
 *          retry_cnt - The number of ARP retransmissions, the default value is 3.
 *                      When the value of the retry_cnt is 0xff, it will be retried
 *                      until the MAC address of the device is obtained.
 *          Arp_Entry_timeout_s - the time an ARP entry stays valid after its last update.
 *                                the default value is 300, the unit is 1s
 *
 * @return  None
 */
void WCHNET_ARPRetryCfg(uint8_t retry_period, uint8_t retry_cnt, uint16_t Arp_Entry_timeout_s);

/**
 * @brief   Configure socket KEEP LIVE enable. 
 *
 * @param   socketid - socket id value
 * @param   enable - 1: Enabled.   0: Disabled. 
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketSetKeepLive( uint8_t socketid, uint8_t enable );

/**
 * @brief   Configure PHY state
 *
 * @param   phy_stat - PHY state
 *
 * @return  None
 */
void WCHNET_PhyStatus( uint32_t phy_stat );

/**
 * @brief   Configure socket Nagle Func.
 *
 * @param   socketid - socket id value
 * @param   enable - 1: Enabled.   0: Disabled.
 *
 * @return  @ERR_T
 */
uint8_t WCHNET_SocketSetNagle( uint8_t socketid, uint8_t enable );

#ifdef __cplusplus
}
#endif
#endif
