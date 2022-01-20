/********************************** (C) COPYRIGHT *******************************
* File Name          : WCHNET.H
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/13
* Description        : 以太网协议栈库头文件
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
*******************************************************************************/
#ifndef __WCHNET_H__
#define __WCHNET_H__
#include "ch32v30x_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
  #define TRUE     1
  #define FALSE    0
#endif
#ifndef NULL
  #define NULL    0
#endif

#define WCHNET_LIB_VER    0x10

#ifndef WCHNET_MAX_SOCKET_NUM
  #define WCHNET_MAX_SOCKET_NUM    8  /* Socket的个数，用户可以配置，默认为4个Socket,最大为32 */
#endif

#define ETH_RXBUFNB    10                          /* MAC接收描述符个数 */
#define ETH_TXBUFNB    2   /* MAC发送描述符个数 */ /* #ifndef RX_QUEUE_ENTRIES */

#ifndef RX_BUF_SIZE
  #define RX_BUF_SIZE    1520  /* MAC接收每个缓冲区长度，为4的整数倍 */
#endif

#ifndef WCHNET_PING_ENABLE
  #define WCHNET_PING_ENABLE    TRUE  /* 默认PING开启 */
#endif                                /* PING使能 */

#ifndef TCP_RETRY_COUNT
  #define TCP_RETRY_COUNT    20  /* TCP重试次数，位宽为5位*/
#endif

#ifndef TCP_RETRY_PERIOD
  #define TCP_RETRY_PERIOD    10  /* TCP重试周期，单位为MS， */
#endif

#ifndef WCHNETTIMERPERIOD
  #define WCHNETTIMERPERIOD    10  /* 定时器周期，单位Ms,不得大于500 */
#endif

#ifndef SOCKET_SEND_RETRY
  #define SOCKET_SEND_RETRY    1  /* 默认发送重试 */
#endif

#define LIB_CFG_VALUE    ((SOCKET_SEND_RETRY << 25) | \
                       (1 << 24) |                    \
                       (TCP_RETRY_PERIOD << 19) |     \
                       (TCP_RETRY_COUNT << 14) |      \
                       (WCHNET_PING_ENABLE << 13) |   \
                       (ETH_TXBUFNB << 9) |           \
                       (ETH_RXBUFNB << 5) |           \
                       (WCHNET_MAX_SOCKET_NUM))
#ifndef MISCE_CFG0_TCP_SEND_COPY
  #define MISCE_CFG0_TCP_SEND_COPY    1  /* TCP发送缓冲区复制 */
#endif

#ifndef CFG0_TCP_RECV_COPY
  #define CFG0_TCP_RECV_COPY    1  /* TCP接收复制优化，内部调试使用 */
#endif

#ifndef MISCE_CFG0_TCP_OLD_DELETE
  #define MISCE_CFG0_TCP_OLD_DELETE    0  /* 删除最早的TCP连接 */
#endif

/*关于内存分配 */
#ifndef WCHNET_NUM_IPRAW
  #define WCHNET_NUM_IPRAW    4  /* IPRAW连接的个数 */
#endif

#ifndef WCHNET_NUM_UDP
  #define WCHNET_NUM_UDP    8  /* UDP连接的个数 */
#endif

#ifndef WCHNET_NUM_TCP
  #define WCHNET_NUM_TCP    8  /* TCP连接的个数 */
#endif

#ifndef WCHNET_NUM_TCP_LISTEN
  #define WCHNET_NUM_TCP_LISTEN    8  /* TCP监听的个数 */
#endif

#ifndef WCHNET_NUM_PBUF
  #define WCHNET_NUM_PBUF    5  /* PBUF结构的个数 */
#endif

#ifndef WCHNET_NUM_POOL_BUF
  #define WCHNET_NUM_POOL_BUF    6  /* POOL BUF的个数 */
#endif

#ifndef WCHNET_NUM_TCP_SEG
  #define WCHNET_NUM_TCP_SEG    10  /* tcp段的个数*/
#endif

#ifndef WCHNET_NUM_IP_REASSDATA
  #define WCHNET_NUM_IP_REASSDATA    5  /* IP分段的长度 */
#endif

#ifndef WCHNET_TCP_MSS
  #define WCHNET_TCP_MSS    800  /* tcp MSS的大小*/
#endif

#ifndef WCH_MEM_HEAP_SIZE
  #define WCH_MEM_HEAP_SIZE    4600  /* 内存堆大小 */
#endif

#ifndef WCHNET_NUM_ARP_TABLE
  #define WCHNET_NUM_ARP_TABLE    10  /* ARP列表个数 */
#endif

#ifndef WCHNET_MEM_ALIGNMENT
  #define WCHNET_MEM_ALIGNMENT    4  /* 4字节对齐 */
#endif

#ifndef WCHNET_IP_REASS_PBUFS
  #if(WCHNET_NUM_POOL_BUF < 32)
    #define WCHNET_IP_REASS_PBUFS    (WCHNET_NUM_POOL_BUF - 1) /* IP分片的PBUF个数，最大为31 */
  #else
    #define WCHNET_IP_REASS_PBUFS    31
  #endif
#endif

#define WCHNET_MISC_CONFIG0    ((MISCE_CFG0_TCP_SEND_COPY << 0) | \
                             (CFG0_TCP_RECV_COPY << 1) |          \
                             (MISCE_CFG0_TCP_OLD_DELETE << 2) |   \
                             (WCHNET_IP_REASS_PBUFS) << 3)
/* PHY 状态 */
#define PHY_LINK_SUCCESS              (1 << 2) /*PHY建立连接*/
#define PHY_AUTO_SUCCESS              (1 << 5) /* PHY自动协商完成*/

/* Socket 工作模式定义,协议类型 */
#define PROTO_TYPE_IP_RAW             0     /* IP层原始数据 */
#define PROTO_TYPE_UDP                2     /* UDP协议类型 */
#define PROTO_TYPE_TCP                3     /* TCP协议类型 */

/* 中断状态 */
/* 以下为GLOB_INT会产生的状态 */
#define GINT_STAT_UNREACH             (1 << 0) /* 不可达中断*/
#define GINT_STAT_IP_CONFLI           (1 << 1) /* IP冲突*/
#define GINT_STAT_PHY_CHANGE          (1 << 2) /* PHY状态改变 */
#define GINT_STAT_SOCKET              (1 << 4) /* scoket 产生中断 */

/*以下为Sn_INT会产生的状态*/
#define SINT_STAT_RECV                (1 << 2) /* socket端口接收到数据或者接收缓冲区不为空 */
#define SINT_STAT_CONNECT             (1 << 3) /* 连接成功,TCP模式下产生此中断 */
#define SINT_STAT_DISCONNECT          (1 << 4) /* 连接断开,TCP模式下产生此中断 */
#define SINT_STAT_TIM_OUT             (1 << 6) /* ARP和TCP模式下会发生此中断 */

/* 错误码 */
#define WCHNET_ERR_SUCCESS            0x00  /* 命令操作成功 */
#define WCHNET_RET_ABORT              0x5F  /* 命令操作失败 */
#define WCHNET_ERR_BUSY               0x10  /* 忙状态，表示当前正在执行命令 */
#define WCHNET_ERR_MEM                0x11  /* 内存错误 */
#define WCHNET_ERR_BUF                0x12  /* 缓冲区错误 */
#define WCHNET_ERR_TIMEOUT            0x13  /* 超时 */
#define WCHNET_ERR_RTE                0x14  /* 路由错误*/
#define WCHNET_ERR_ABRT               0x15  /* 连接停止*/
#define WCHNET_ERR_RST                0x16  /* 连接复位 */
#define WCHNET_ERR_CLSD               0x17  /* 连接关闭/socket 在关闭状态*/
#define WCHNET_ERR_CONN               0x18  /* 无连接 */
#define WCHNET_ERR_VAL                0x19  /* 错误的值 */
#define WCHNET_ERR_ARG                0x1a  /* 错误的参数 */
#define WCHNET_ERR_USE                0x1b  /* 已经被使用 */
#define WCHNET_ERR_IF                 0x1c  /* MAC错误  */
#define WCHNET_ERR_ISCONN             0x1d  /* 已连接 */
#define WCHNET_ERR_SOCKET_MEM         0X20  /* Socket信息列表已满或者错误 */
#define WCHNET_ERR_UNSUPPORT_PROTO    0X21  /* 不支持的协议类型 */
#define WCHNET_ERR_UNKNOW             0xFA  /* 未知错误 */

/* 不可达代码 */
#define UNREACH_CODE_HOST             0     /* 主机不可达 */
#define UNREACH_CODE_NET              1     /* 网络不可达 */
#define UNREACH_CODE_PROTOCOL         2     /* 协议不可达 */
#define UNREACH_CODE_PROT             3     /* 端口不可达 */
/*其他值请参考RFC792文档*/

/* TCP关闭参数 */
#define TCP_CLOSE_NORMAL              0     /* 正常关闭，进行4此握手 */
#define TCP_CLOSE_RST                 1     /* 复位连接，并关闭  */
#define TCP_CLOSE_ABANDON             2     /* 内部丢弃连接，不会发送任何终止报文 */

/* socket状态 */
#define SOCK_STAT_CLOSED              0X00  /* socket关闭 */
#define SOCK_STAT_OPEN                0X05  /* socket打开 */

/* TCP状态 */
#define TCP_CLOSED                    0     /* TCP连接 */
#define TCP_LISTEN                    1     /* TCP关闭 */
#define TCP_SYN_SENT                  2     /* SYN发送，连接请求 */
#define TCP_SYN_RCVD                  3     /* SYN接收，接收到连接请求 */
#define TCP_ESTABLISHED               4     /* TCP连接建立 */
#define TCP_FIN_WAIT_1                5     /* WAIT_1状态 */
#define TCP_FIN_WAIT_2                6     /* WAIT_2状态 */
#define TCP_CLOSE_WAIT                7     /* 等待关闭 */
#define TCP_CLOSING                   8     /* 正在关闭 */
#define TCP_LAST_ACK                  9     /* LAST_ACK*/
#define TCP_TIME_WAIT                 10    /* 2MSL等待 */

/* sokcet信息表 */

typedef struct _SCOK_INF
{
    u32   IntStatus;                                              /* 中断状态 */
    u32   SockIndex;                                              /* Socket索引值 */
    u32   RecvStartPoint;                                         /* 接收缓冲区的开始指针 */
    u32   RecvBufLen;                                             /* 接收缓冲区长度 */
    u32   RecvCurPoint;                                           /* 接收缓冲区的当前指针 */
    u32   RecvReadPoint;                                          /* 接收缓冲区的读指针 */
    u32   RecvRemLen;                                             /* 接收缓冲区的剩余长度 */
    u32   ProtoType;                                              /* 协议类型 */
    u32   ScokStatus;                                             /* 低字节Socket状态，次低字节为TCP状态，仅TCP模式下有意义 */
    u32   DesPort;                                                /* 目的端口 */
    u32   SourPort;                                               /* 源端口在IPRAW模式下为协议类型 */
    u8    IPAddr[4];                                              /* Socket目标IP地址 32bit*/
    void *Resv1;                                                  /* 保留，内部使用，用于保存各个PCB */
    void *Resv2;                                                  /* 保留，内部使用，TCP Server使用 */
    void (*AppCallBack)(struct _SCOK_INF *, u32, u16, u8 *, u32); /* 接收回调函数*/

} SOCK_INF;

struct _WCH_CFG
{
    u32 RxBufSize;   /* MAC接收缓冲区大小 */
    u32 TCPMss;      /* TCP MSS大小 */
    u32 HeapSize;    /* 堆分配内存大小 */
    u32 ARPTableNum; /* ARP列表个数 */
    u32 MiscConfig0; /* 杂项配置 */
};

struct _NET_SYS
{
    u8  IPAddr[4];        /* IP地址 32bit */
    u8  GWIPAddr[4];      /* 网关地址 32bit */
    u8  MASKAddr[4];      /* 子网掩码 32bit */
    u8  MacAddr[8];       /* MAC地址 48bit */
    u8  UnreachIPAddr[4]; /* 不可到达IP */
    u32 RetranCount;      /* 重试次数 默认为10次 */
    u32 RetranPeriod;     /* 重试周期,单位MS,默认200MS */
    u32 PHYStat;          /* PHY状态码 8bit */
    u32 NetStat;          /* 以太网的状态 ，包含是否打开等 */
    u32 MackFilt;         /*  MAC过滤，默认为接收广播，接收本机MAC 8bit */
    u32 GlobIntStatus;    /* 全局中断 */
    u32 UnreachCode;      /* 不可达 */
    u32 UnreachProto;     /* 不可达协议 */
    u32 UnreachPort;      /* 不可到达端口 */
    u32 SendFlag;
    u32 Flags;
};

/* KEEP LIVE配置结构体 */
#ifndef ST_KEEP_CFG
  #define ST_KEEP_CFG
struct _KEEP_CFG
{
    u32 KLIdle;  /* KEEPLIVE空闲时间 */
    u32 KLIntvl; /* KEEPLIVE周期 */
    u32 KLCount; /* KEEPLIVE次数 */
};
#endif

/* 以下值为固定值不可以更改 */
#define WCHNET_MEM_ALIGN_SIZE(size)    (((size) + WCHNET_MEM_ALIGNMENT - 1) & ~(WCHNET_MEM_ALIGNMENT - 1))
#define WCHNET_SIZE_IPRAW_PCB          0xFF                   /* IPRAW PCB大小 */
#define WCHNET_SIZE_UDP_PCB            0x30                   /* UDP PCB大小 */
#define WCHNET_SIZE_TCP_PCB            0xFF                   /* TCP PCB大小 */
#define WCHNET_SIZE_TCP_PCB_LISTEN     0x40                   /* TCP LISTEN PCB大小 */
#define WCHNET_SIZE_IP_REASSDATA       0x40                   /* IP分片管理  */
#define WCHNET_SIZE_PBUF               0x40                   /* Packet Buf */
#define WCHNET_SIZE_TCP_SEG            0x60                   /* TCP SEG结构 */
#define WCHNET_SIZE_MEM                0x0c                   /* sizeof(struct mem) */
#define WCHNET_SIZE_ARP_TABLE          0x20                   /* sizeof arp table */

#define WCHNET_SIZE_POOL_BUF           WCHNET_MEM_ALIGN_SIZE(WCHNET_TCP_MSS + 40 + 14) /* pbuf池大小 */
#define WCHNET_MEMP_SIZE               ((WCHNET_MEM_ALIGNMENT - 1) +                                                 \
                          (WCHNET_NUM_IPRAW * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IPRAW_PCB)) +                        \
                          (WCHNET_NUM_UDP * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_UDP_PCB)) +                            \
                          (WCHNET_NUM_TCP * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB)) +                            \
                          (WCHNET_NUM_TCP_LISTEN * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB_LISTEN)) +              \
                          (WCHNET_NUM_TCP_SEG * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_SEG)) +                        \
                          (WCHNET_NUM_IP_REASSDATA * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IP_REASSDATA)) +              \
                          (WCHNET_NUM_PBUF * (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(0))) + \
                          (WCHNET_NUM_POOL_BUF * (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_POOL_BUF))))

#define HEAP_MEM_ALIGN_SIZE          (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_MEM))
#define WCHNET_RAM_HEAP_SIZE         (WCH_MEM_HEAP_SIZE + (2 * HEAP_MEM_ALIGN_SIZE) + WCHNET_MEM_ALIGNMENT)
#define WCHNET_RAM_ARP_TABLE_SIZE    (WCHNET_SIZE_ARP_TABLE * WCHNET_NUM_ARP_TABLE)
/* DNS结构体回调 */
typedef void (*dns_callback)(const char *name, u8 *ipaddr, void *callback_arg);

/* 检查配置 */
/* 检查WCHNET_NUM_IPRAW是否不小于1 */
#if(WCHNET_NUM_IPRAW < 1)
  #error "WCHNET_NUM_IPRAW Error,Please Config WCHNET_NUM_IPRAW >= 1"
#endif
/* 检查WCHNET_SIZE_UDP_PCB是否不小于1 */
#if(WCHNET_SIZE_UDP_PCB < 1)
  #error "WCHNET_SIZE_UDP_PCB Error,Please Config WCHNET_SIZE_UDP_PCB >= 1"
#endif
/* 检查WCHNET_NUM_TCP是否不小于1 */
#if(WCHNET_NUM_TCP < 1)
  #error "WCHNET_NUM_TCP Error,Please Config WCHNET_NUM_TCP >= 1"
#endif
/* 检查WCHNET_NUM_TCP_LISTEN是否不小于1 */
#if(WCHNET_NUM_TCP_LISTEN < 1)
  #error "WCHNET_NUM_TCP_LISTEN Error,Please Config WCHNET_NUM_TCP_LISTEN >= 1"
#endif
/* 检查字节对齐必须为4的整数倍 */
#if((WCHNET_MEM_ALIGNMENT % 4) || (WCHNET_MEM_ALIGNMENT == 0))
  #error "WCHNET_MEM_ALIGNMENT Error,Please Config WCHNET_MEM_ALIGNMENT = 4 * N, N >=1"
#endif
/* TCP最大报文段长度 */
#if((WCHNET_TCP_MSS > 1460) || (WCHNET_TCP_MSS < 60))
  #error "WCHNET_TCP_MSS Error,Please Config WCHNET_TCP_MSS >= 60 && WCHNET_TCP_MSS <= 1460"
#endif
/* ARP缓存表个数 */
#if((WCHNET_NUM_ARP_TABLE > 0X7F) || (WCHNET_NUM_ARP_TABLE < 1))
  #error "WCHNET_NUM_ARP_TABLE Error,Please Config WCHNET_NUM_ARP_TABLE >= 1 && WCHNET_NUM_ARP_TABLE <= 0X7F"
#endif
/* 检查POOL BUF配置 */
#if(WCHNET_NUM_POOL_BUF < 1)
  #error "WCHNET_NUM_POOL_BUF Error,Please Config WCHNET_NUM_POOL_BUF >= 1"
#endif
/* 检查PBUF结构配置 */
#if(WCHNET_NUM_PBUF < 1)
  #error "WCHNET_NUM_PBUF Error,Please Config WCHNET_NUM_PBUF >= 1"
#endif
/* 检查IP分配配置 */
#if((WCHNET_NUM_IP_REASSDATA > 10) || (WCHNET_NUM_IP_REASSDATA < 1))
  #error "WCHNET_NUM_IP_REASSDATA Error,Please Config WCHNET_NUM_IP_REASSDATA < 10 && WCHNET_NUM_IP_REASSDATA >= 1 "
#endif
/* 检查IP分片大小 */
#if(WCHNET_IP_REASS_PBUFS > WCHNET_NUM_POOL_BUF)
  #error "WCHNET_IP_REASS_PBUFS Error,Please Config WCHNET_IP_REASS_PBUFS < WCHNET_NUM_POOL_BUF"
#endif

extern ETH_DMADESCTypeDef /* 发送描述符表 */ DMARxDscrTab[ETH_RXBUFNB];
extern ETH_DMADESCTypeDef /* 接收描述符表 */ DMATxDscrTab[ETH_TXBUFNB];
extern u8                                    MACRxBuf[ETH_RXBUFNB * ETH_MAX_PACKET_SIZE];
extern u8                                    MACTxBuf[ETH_TXBUFNB * ETH_MAX_PACKET_SIZE];

/* 库内部函数声明 */

u8 WCHNET_Init(const u8 *ip, const u8 *gwip, const u8 *mask, const u8 *macaddr); /* 库初始化 */

u8 WCHNET_GetVer(void);

u8 WCH_GetMac(u8 *macaddr);

u8 WCHNET_ConfigLIB(struct _WCH_CFG *cfg); /* 配置库*/

void WCHNET_MainTask(void); /* 库主任务函数，需要一直不断调用 */

void WCHNET_TimeIsr(u16 timperiod); /* 时钟中断服务函数，调用前请配置时钟周期 */

void WCHNET_ETHIsr(void); /* ETH中断服务函数 */

u8 WCHNET_GetPHYStatus(void); /* 获取PHY状态 */

u8 WCHNET_QueryGlobalInt(void); /* 查询全局中断 */

u8 WCHNET_GetGlobalInt(void); /* 读全局中断并将全局中断清零 */

void WCHNET_OpenMac(void); /* 打开MAC */

void WCHNET_CloseMac(void); /* 关闭MAC */

u8 WCHNET_SocketCreat(u8 *socketid, SOCK_INF *socinf); /* 创建socket */

u8 WCHNET_SocketSend(u8 socketid, u8 *buf, u32 *len); /* Socket发送数据 */

u8 WCHNET_SocketRecv(u8 socketid, u8 *buf, u32 *len); /* Socket接收数据 */

u8 WCHNET_GetSocketInt(u8 sockedid); /* 获取socket中断并清零 */

u32 WCHNET_SocketRecvLen(u8 socketid, u32 *bufaddr); /* 获取socket接收长度 */

u8 WCHNET_SocketConnect(u8 socketid); /* TCP连接*/

u8 WCHNET_SocketListen(u8 socindex); /* TCP监听 */

u8 WCHNET_SocketClose(u8 socindex, u8 flag); /* 关闭连接 */

void WCHNET_ModifyRecvBuf(u8 sockeid, u32 bufaddr, u32 bufsize); /* 修改接收缓冲区 */

u8 WCHNET_SocketUdpSendTo(u8 socketid, u8 *buf, u32 *slen, u8 *sip, u16 port); /* 向指定的目的IP，端口发送UDP包 */

u8 WCHNET_Aton(const u8 *cp, u8 *addr); /* ASCII码地址转网络地址 */

u8 *WCHNET_Ntoa(u8 *ipaddr); /* 网络地址转ASCII地址 */

u8 WCHNET_SetSocketTTL(u8 socketid, u8 ttl); /* 设置socket的TTL */

void WCHNET_RetrySendUnack(u8 socketid); /* TCP重传 */

u8 WCHNET_QueryUnack(SOCK_INF *sockinf, u32 *addrlist, u16 lislen) /* 查询未发送成功的数据包 */;

u8 WCHNET_DHCPStart(u8 (*usercall)(u8 status, void *)); /* DHCP启动 */

u8 WCHNET_DHCPStop(void); /* DHCP停止 */

void WCHNET_InitDNS(u8 *dnsip, u16 port); /* DNS初始化 */

u8 WCHNET_GetHostName(const char *hostname, u8 *addr, dns_callback found, void *arg); /* DNS获取主机名 */

void WCHNET_ConfigKeepLive(struct _KEEP_CFG *cfg); /* 配置库KEEP LIVE参数 */

u8 WCHNET_SocketSetKeepLive(u8 socindex, u8 cfg); /* 配置socket KEEP LIVE*/

void WCHNET_SetHostname(char *name); /* 配置DHCP主机名*/

void Ethernet_LED_Configuration(void);

void Ethernet_LED_LINKSET(u8 setbit);

void Ethernet_LED_DATASET(u8 setbit);

#ifdef __cplusplus
}
#endif
#endif
