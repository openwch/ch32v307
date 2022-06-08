/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/20
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
/*
 *@Note
  DHCP例程，演示DHCP自动获取IP功能
*/
#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"

u8 MACAddr[6];                                          /*MAC地址*/
u8 IPAddr[4]   = {192, 168, 1, 10};                     /*IP地址*/
u8 GWIPAddr[4] = {192, 168, 1, 1};                      /*网关*/
u8 IPMask[4]   = {255, 255, 255, 0};                    /*子网掩码*/
u8 DESIP[4]    = {255, 255, 255, 255};                  /*目的IP地址*/

u8  SocketId;                                           /*socket id号*/
u8  SocketRecvBuf[RECE_BUF_LEN];                        /*socket缓冲区*/
u8  MyBuf[RECE_BUF_LEN];
u16 desport = 1000;                                     /*目的端口号*/
u16 srcport = 1000;                                     /*源端口号*/

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if(iError == WCHNET_ERR_SUCCESS) return;            /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);             /* 显示错误 */
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_SetPriority(TIM2_IRQn, 0x80);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 sockeid, u8 initstat)
{
    if(initstat & SINT_STAT_RECV)                               /* socket接收中断*/
    {
    }
    if(initstat & SINT_STAT_CONNECT)                            /* socket连接成功中断*/
    {
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                         /* socket连接断开中断*/
    {
        printf("TCP Disconnect\r\n");
    }
    if(initstat & SINT_STAT_TIM_OUT)                            /* socket连接超时中断*/
    {
        printf("TCP Timeout\r\n");
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
    u8  initstat;
    u16 i;
    u8  socketinit;

    initstat = WCHNET_GetGlobalInt();                           /* 获取全局中断标志*/
    if(initstat & GINT_STAT_UNREACH)                            /* 不可达中断 */
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if(initstat & GINT_STAT_IP_CONFLI)                          /* IP冲突中断 */
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if(initstat & GINT_STAT_PHY_CHANGE)                         /* PHY状态变化中断 */
    {
        i = WCHNET_GetPHYStatus();                              /* 获取PHY连接状态*/
        if(i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if(initstat & GINT_STAT_SOCKET)
    {
        for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        {
            socketinit = WCHNET_GetSocketInt(i);
            if(socketinit)
                WCHNET_HandleSockInt(i, socketinit);
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_DHCPCallBack
 *
 * @brief   DHCPCallBack
 *
 * @return  DHCP status
 */
u8 WCHNET_DHCPCallBack(u8 status, void *arg)
{
    u8 *p;

    if(!status)
    {
        p = arg;
        printf("DHCP Success\r\n");
        memcpy(IPAddr, p, 4);
        memcpy(GWIPAddr, &p[4], 4);
        memcpy(IPMask, &p[8], 4);
        printf("IPAddr = %d.%d.%d.%d \r\n", (u16)IPAddr[0], (u16)IPAddr[1],
               (u16)IPAddr[2], (u16)IPAddr[3]);
        printf("GWIPAddr = %d.%d.%d.%d \r\n", (u16)GWIPAddr[0], (u16)GWIPAddr[1],
               (u16)GWIPAddr[2], (u16)GWIPAddr[3]);
        printf("IPAddr = %d.%d.%d.%d \r\n", (u16)IPMask[0], (u16)IPMask[1],
               (u16)IPMask[2], (u16)IPMask[3]);
        printf("DNS1: %d.%d.%d.%d \r\n", p[12], p[13], p[14], p[15]);
        printf("DNS2: %d.%d.%d.%d \r\n", p[16], p[17], p[18], p[19]);
        return 0;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        return 1;
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
    USART_Printf_Init(115200);                                            /*串口打印初始化*/
    printf("DHCP Test\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                           /*获取芯片MAC地址*/
    printf("mac addr:");
    for(int i=0;i<6;i++) printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    WCHNET_DHCPSetHostname("ch32v307");                                   /*设置DHCP主机名称*/
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                      /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    WCHNET_DHCPStart(WCHNET_DHCPCallBack);                                /*开始DHCP,返回结果在WCHNET_DHCPCallBack函数中*/

    while(1)
    {
        WCHNET_MainTask();                                                /*以太网库主任务函数，需要循环调用*/
        if(WCHNET_QueryGlobalInt())                                       /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
        {
            WCHNET_HandleGlobalInt();
        }
    }
}
