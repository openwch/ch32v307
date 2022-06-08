/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/05/31
 * Description        : Main program body.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
/*
 *@Note
 UDP Client例程，演示UDP Client接收数据并回传.
 */

#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"

u8 MACAddr[6];                                            //MAC address
u8 IPAddr[4] = { 192, 168, 1, 10 };                       //IP address
u8 GWIPAddr[4] = { 192, 168, 1, 1 };                      //Gateway IP address
u8 IPMask[4] = { 255, 255, 255, 0 };                      //subnet mask
u8 DESIP[4] = { 192, 168, 1, 100 };                       //destination IP address
u16 desport = 1000;                                       //destination port
u16 srcport = 1000;                                       //source port

u8 SocketId;
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];   //socket receive buffer

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
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
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreateUdpSocket
 *
 * @brief   Create UDP Socket
 *
 * @return  none
 */
void WCHNET_CreateUdpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[SocketId];
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("WCHNET_SocketCreat %d srcport %d\r\n", SocketId, srcport);
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_DataLoopback(u8 id)
{
    u8 i;
    u32 len;
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    if (intstat & SINT_STAT_RECV)                             //receive data
    {
        WCHNET_DataLoopback(socketid);                        //Data loopback
    }
    if (intstat & SINT_STAT_CONNECT)                          //connect successfully
    {
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                       //disconnect
    {
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                          //timeout disconnect
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
    u8 intstat;
    u16 i;
    u8 socketinit;

    intstat = WCHNET_GetGlobalInt();                          //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                          //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                        //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                       //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET) {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {         //socket related interrupt
            socketinit = WCHNET_GetSocketInt(i);
            if (socketinit)
                WCHNET_HandleSockInt(i, socketinit);
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
    USART_Printf_Init(115200);                                 //USART initialize
    printf("UdpClient Test\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("net version:%x\n", WCHNET_GetVer());
    if ( WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                //get the chip MAC address
    printf("mac addr:");
    for (int i = 0; i < 6; i++)
        printf("%x ", MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);        //Ethernet library initialize
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
    for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        WCHNET_CreateUdpSocket();                                  //Create UDP Socket

    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }
    }
}

