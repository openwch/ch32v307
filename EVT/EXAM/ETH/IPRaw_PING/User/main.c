/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/18
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "PING.h"
/*
 *@Note
IP_RAW例程，演示TCP client连接服务器后接收数据再回传
*/
#define TCP_SINGLE_CLIENT                                                                 /*创建一个客户端，当创建多个客户端时，注意更改 WCHNET_NUM_TCP 的值*/

u8 MACAddr[6];                                                                            /*MAC地址*/
u8 IPAddr[4]   = {192,168,1,10};                                                            /*IP地址*/
u8 GWIPAddr[4] = {192,168,1,1};                                                           /*网关*/
u8 IPMask[4]   = {255,255,255,0};                                                         /*子网掩码*/
u8 DESIP[4]    = {192,168,1,100};                                                         /*目的IP地址*/

u8 SocketId;                                                                              /*socket id号*/
u8 SocketRecvBuf[RECE_BUF_LEN];                                                             /*socket缓冲区*/
u8 MyBuf[RECE_BUF_LEN];
u16 desport = 1000;                                                                         /*目的端口号*/
u16 srcport = 1000;                                                                         /*源端口号*/

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;                                             /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                                               /* 显示错误 */
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

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update ,ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreateIPRawSocket
 *
 * @brief   Create raw IP Socket
 *
 * @return  none
 */
void WCHNET_CreateIPRawSocket(void)
{
   u8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);
   TmpSocketInf.SourPort = 1;                                                 /* 在IPRAW模式下，SourPort为协议类型*/
   TmpSocketInf.ProtoType = PROTO_TYPE_IP_RAW;                                  /* 设置socket类型 */
   TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf;                            /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                                    /* 检查错误 */
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid,u8 initstat)
{
    u32 len;

    if(initstat & SINT_STAT_RECV)                                                /* socket接收中断*/
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);                                 /* 获取socket缓冲区数据长度  */
        WCHNET_SocketRecv(socketid,MyBuf,&len);                                   /* 将接收缓冲区的数据读到MyBuf中*/
        WCHNET_ICMPRecvData( len,MyBuf );
    }
    if(initstat & SINT_STAT_CONNECT)                                             /* socket连接成功中断*/
    {
        /***/
    }
    if(initstat & SINT_STAT_DISCONNECT)                                          /* socket连接断开中断*/
    {
        /***/
    }
    if(initstat & SINT_STAT_TIM_OUT)                                             /* socket连接超时中断*/
    {
        printf("Failed to send ping packet!\r\n");
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

    initstat = WCHNET_GetGlobalInt();                                             /* 获取全局中断标志*/
    if(initstat & GINT_STAT_UNREACH)                                              /* 不可达中断 */
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if(initstat & GINT_STAT_IP_CONFLI)                                             /* IP冲突中断 */
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if(initstat & GINT_STAT_PHY_CHANGE)                                            /* PHY状态变化中断 */
    {
        i = WCHNET_GetPHYStatus();                                                 /* 获取PHY连接状态*/
        if(i&PHY_Linked_Status){
            ICMPSuc = ICMP_SOKE_CON;
            printf("PHY Link Success\r\n");
        }
    }
    if(initstat & GINT_STAT_SOCKET)
    {
        for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        {
            socketinit = WCHNET_GetSocketInt(i);
            if(socketinit)WCHNET_HandleSockInt(i,socketinit);
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
	printf("TcpClient Test\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                             /*获取芯片MAC地址*/
    printf("mac addr:");
    for(int i=0;i<6;i++) printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                        /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    WCHNET_CreateIPRawSocket();                                                /*创建TCP socket*/                                              /* 创建IPRAW */
    InitParameter( );
    InitPING( );
	while(1)
	{
        WCHNET_MainTask();                                                     /*以太网库主任务函数，需要循环调用*/
        if(WCHNET_QueryGlobalInt())                                            /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
        {
            WCHNET_HandleGlobalInt();
        }
        WCHNET_PINGCmd();
    }
}
