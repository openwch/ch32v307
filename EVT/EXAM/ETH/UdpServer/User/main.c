/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/18
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

#include "debug.h"
#include "WCHNET.h"
#include "string.h"

/*
 *@Note
Udp Server例程，演示Udp Server接收数据并回传.
*/


__attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];                        /* MAC接收描述符 ，4字节对齐*/
__attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];                        /* MAC发送描述符，4字节对齐 */

__attribute__((__aligned__(4))) u8  MACRxBuf[ETH_RXBUFNB*ETH_MAX_PACKET_SIZE];                       /* MAC接收缓冲区，4字节对齐 */
__attribute__((__aligned__(4))) u8  MACTxBuf[ETH_TXBUFNB*ETH_MAX_PACKET_SIZE];                       /* MAC发送缓冲区，4字节对齐 */


__attribute__((__aligned__(4))) SOCK_INF SocketInf[WCHNET_MAX_SOCKET_NUM];                           /* Socket信息表，4字节对齐 */
const u16 MemNum[8] = {WCHNET_NUM_IPRAW,
                         WCHNET_NUM_UDP,
                         WCHNET_NUM_TCP,
                         WCHNET_NUM_TCP_LISTEN,
                         WCHNET_NUM_TCP_SEG,
                         WCHNET_NUM_IP_REASSDATA,
                         WCHNET_NUM_PBUF,
                         WCHNET_NUM_POOL_BUF
                         };
const u16 MemSize[8] = {WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IPRAW_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_UDP_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB_LISTEN),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_SEG),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IP_REASSDATA),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(0),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_POOL_BUF)
                         };
 __attribute__((__aligned__(4)))u8 Memp_Memory[WCHNET_MEMP_SIZE];
 __attribute__((__aligned__(4)))u8 Mem_Heap_Memory[WCHNET_RAM_HEAP_SIZE];
 __attribute__((__aligned__(4)))u8 Mem_ArpTable[WCHNET_RAM_ARP_TABLE_SIZE];


#define RECE_BUF_LEN                          WCHNET_TCP_MSS*2                                   /*socket接收缓冲区的长度,最小为TCP MSS*/

u8 MACAddr[6];                                                                                   /*Mac地址*/
u8 IPAddr[4] = {192,168,1,10};                                                                   /*IP地址*/
u8 GWIPAddr[4] = {192,168,1,1};                                                                  /*网关*/
u8 IPMask[4] = {255,255,255,0};                                                                  /*子网掩码*/
u8 DESIP[4] = {255,255,255,255};                                                                   /*目的IP地址*/

u8 SocketId;                                                                                     /*socket id号*/
u8 SocketRecvBuf[RECE_BUF_LEN];                                                                  /*socket缓冲区*/
u8 MyBuf[RECE_BUF_LEN];
u16 desport=1000;                                                                                /*目的端口号*/
u16 srcport=1000;                                                                                /*源端口号*/


/*********************************************************************
 * @fn      Ethernet_LED_Configuration
 *
 * @brief   set eth data and link led pin
 *
 * @return  none
 */
void Ethernet_LED_Configuration(void)
{
    GPIO_InitTypeDef  GPIO={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO);
    Ethernet_LED_LINKSET(1);
    Ethernet_LED_DATASET(1);
}


/*********************************************************************
 * @fn      Ethernet_LED_LINKSET
 *
 * @brief   set eth link led,setbit 0 or 1,the link led turn on or turn off
 *
 * @return  none
 */
void Ethernet_LED_LINKSET(u8 setbit)
{
     if(setbit){
         GPIO_SetBits(GPIOC, GPIO_Pin_0);
     }
     else {
         GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    }
}


/*********************************************************************
 * @fn      Ethernet_LED_DATASET
 *
 * @brief   set eth data led,setbit 0 or 1,the data led turn on or turn off
 *
 * @return  none
 */
void Ethernet_LED_DATASET(u8 setbit)
{
     if(setbit){
         GPIO_SetBits(GPIOC, GPIO_Pin_1);
     }
     else {
         GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    }
}

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;                                   /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                                    /* 显示错误 */
}


/*********************************************************************
 * @fn      WCHNET_LibInit
 *
 * @brief   Initializes NET.
 *
 * @return  command status
 */
u8 WCHNET_LibInit(const u8 *ip,const u8 *gwip,const u8 *mask,const u8 *macaddr)
{
    u8 i;
    struct _WCH_CFG  cfg;

    cfg.RxBufSize = RX_BUF_SIZE;
    cfg.TCPMss   = WCHNET_TCP_MSS;
    cfg.HeapSize = WCH_MEM_HEAP_SIZE;
    cfg.ARPTableNum = WCHNET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = WCHNET_MISC_CONFIG0;
    WCHNET_ConfigLIB(&cfg);
    i = WCHNET_Init(ip,gwip,mask,macaddr);
    return (i);
}

/*********************************************************************
 * @fn      SET_MCO
 *
 * @brief   Set ETH Clock.
 *
 * @return  none
 */
void SET_MCO(void)
{

    RCC_PLL3Cmd(DISABLE);
    RCC_PREDIV2Config(RCC_PREDIV2_Div2);
    RCC_PLL3Config(RCC_PLL3Mul_15);
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
    RCC_PLL3Cmd(ENABLE);
    Delay_Ms(100);
    while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLL3RDY))
    {
        Delay_Ms(500);
    }
    RCC_AHBPeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 200-1;
    TIM_TimeBaseStructure.TIM_Prescaler =7200-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update ,ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
    NVIC_SetPriority(TIM2_IRQn, 0x80);
    NVIC_EnableIRQ(TIM2_IRQn);
}


/*********************************************************************
 * @fn      WCHNET_UdpServerRecv
 *
 * @brief   Udp Recv function
 *
 * @return  none
 */
void WCHNET_UdpServerRecv(struct _SCOK_INF *socinf,u32 ipaddr,u16 port,u8 *buf,u32 len)
{
    u8 ip_addr[4],i;


    printf("Remote IP:");
    for(i=0;i<4;i++){
        ip_addr[i] = ipaddr&0xff;
        printf("%d ",ip_addr[i]);
        ipaddr = ipaddr>>8;
    }

    printf("srcport=%d len=%d socketid=%d\r\n",port,len,socinf->SockIndex);

 WCHNET_SocketUdpSendTo(socinf->SockIndex, buf, &len, ip_addr, port);
}

/*********************************************************************
 * @fn      WCHNET_CreatUdpSocket
 *
 * @brief   Creat Udp Socket
 *
 * @return  none
 */
void WCHNET_CreatUdpSocket(void)
{
   u8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                                 /* 设置目的IP地址 */
   TmpSocketInf.DesPort  = desport;
   TmpSocketInf.SourPort = srcport;                                             /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_UDP;                                     /* 设置socekt类型 */
   TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf;                            /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   TmpSocketInf.AppCallBack = WCHNET_UdpServerRecv;                             /* Udp接收回调函数 ,Udp的接收在回调函数中处理*/
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 */
   printf("WCHNET_SocketCreat %d\r\n",SocketId);
   mStopIfError(i);                                                             /* 检查错误 */

}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 sockeid,u8 initstat)
{


    if(initstat & SINT_STAT_RECV)                                               /* socket接收中断*/
    {
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* socket连接成功中断*/
    {
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* socket连接断开中断*/
    {
        printf("TCP Disconnect\r\n");
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* socket连接超时中断*/
    {
       printf("TCP Timout\r\n");                                                /* 延时200ms，重连*/
    }
}


/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 initstat;
    u16 i;
    u8 socketinit;

    initstat = WCHNET_GetGlobalInt();                                           /* 获取全局中断标志*/
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
       printf("GINT_STAT_UNREACH\r\n");
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
       printf("GINT_STAT_IP_CONFLI\r\n");
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY状态变化中断 */
   {
       i = WCHNET_GetPHYStatus();                                               /* 获取PHY连接状态*/
       if(i&PHY_Linked_Status)
       printf("PHY Link Success\r\n");
   }
   if(initstat & GINT_STAT_SOCKET)                                              /* SocketÖÐ¶Ï */
   {
       for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i ++)
       {
           socketinit = WCHNET_GetSocketInt(i);                               /* ¶ÁsocketÖÐ¶Ï²¢ÇåÁã */
           if(socketinit)WCHNET_HandleSockInt(i,socketinit);                  /* Èç¹ûÓÐÖÐ¶ÏÔòÇåÁã */
       }
   }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    u8 i;

	Delay_Init();
	USART_Printf_Init(115200);                                              /*串口打印初始化*/
	printf("UdpServer Test\r\n");
    SET_MCO();
    TIM2_Init();
    WCH_GetMac(MACAddr);                                                     /*获取芯片Mac地址*/
    i=WCHNET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                        /*以太网库初始化*/
    mStopIfError(i);
    if(i==WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    while(!(WCHNET_GetPHYStatus()&PHY_LINK_SUCCESS))                         /*等待PHY连接成功*/
    {
        Delay_Ms(100);
    }
    WCHNET_CreatUdpSocket();                                                 /*创建Udp socket*/

	while(1)
	{
	  WCHNET_MainTask();                                                     /*以太网库主任务函数，需要循环调用*/
	  if(WCHNET_QueryGlobalInt())                                            /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
	  {
         WCHNET_HandleGlobalInt();
	  }
    }
}

