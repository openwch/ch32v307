/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/01/12
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
Filter and Multicast example, demonstrate filtering and multicast sending functions.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 */

#include "string.h"
#include "eth_driver.h"
#include "net_filter.h"

#define UDP_RECE_BUF_LEN                                    1472
u8 MACAddr[6];                                              //MAC address
u8 IPAddr[4] = {192, 168, 1, 10};                           //IP address
u8 GWIPAddr[4] = {192, 168, 1, 1};                          //Gateway IP address
u8 IPMask[4] = {255, 255, 255, 0};                          //subnet mask
u8 DESIP[4] = {192, 168, 1, 100};                           //destination IP address
u16 desport = 1000;                                         //destination port
u16 srcport = 1000;                                         //source port

u8 SocketId;
u8 ConFlag;
extern uint8_t SendFlag;
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][UDP_RECE_BUF_LEN];  //socket receive buffer
u8 MyBuf[UDP_RECE_BUF_LEN];

u8 MulticastData[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
u8 MulticastIPAddr[4] = {224, 1, 1, 10};                    //Multicast IP address

u8 UnicastMac0[6] = {0x38,0x3b,0x26,0x62,0x79,0x01};
u8 UnicastMac1[6] = {0x38,0x3b,0x26,0x62,0x79,0x02};
u8 MulticastMac0[6]  = {0x01,0x00,0x5e,0x01,0x01,0x0a};
u8 MulticastMac1[6]  = {0x01,0x00,0x5e,0x01,0x01,0x0b};

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

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
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
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("SocketId %d\r\n", SocketId);
    WCHNET_ModifyRecvBuf(SocketId, (u32) SocketRecvBuf[SocketId], UDP_RECE_BUF_LEN);
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
#if 1
    u8 i;
    u32 len;
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;          //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {       //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);            //send data
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);                                          //Clear sent data
    }
#else
    u32 len, totallen;
    u8 *p = MyBuf;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %02x\n", len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if(totallen)continue;                                            //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
#endif
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
        printf("Received data... ");
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
    u8 socketint;

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
        if (i & PHY_Linked_Status){
            ConFlag = 1;
            printf("PHY Link Success\r\n");
        }
        else{
            ConFlag = 0;
        }
    }
    if (intstat & GINT_STAT_SOCKET) {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {         //socket related interrupt
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_FilterCfg
 *
 * @brief   Configure filtering
 *
 * @param   none
 *
 * @return  none.
 */
void WCHNET_FilterCfg( void )
{
    uint8_t RegIndex;

    /* clear filter configuration in eth_driver.c */
    ETH->MACA0LR = 0xffffffff;

#if DES_ADDR_FILTER                 /* destination MAC address filtering */
    /* The following are the configuration
     * options for destination MAC address filtering */
#if UNICAST_PERFECT_FILTER          /* Unicast Perfect Filtering */
    RegIndex = ETH_PerfectFilter(MACAddr);
    printf("MAC address register index: %d\r\n", RegIndex);
#endif

#if UNICAST_HASH_FILTER             /* Unicast hash list filtering */
    ETH_HashFilter(UnicastMac1);
#endif

#if UNICAST_HASH_OR_PERFECT_FILTER  /* Unicast hash list or perfect filter*/
    RegIndex = ETH_PerfectFilter(UnicastMac0);
    printf("MAC address register index: %d\r\n", RegIndex);
    ETH_HashFilter(UnicastMac1);
    ETH_HashOrPerfectFilter(UNICAST_HashOrPerfectFilter);
#endif

#if MULTICAST_PERFECT_FILTER        /* multicast Perfect Filtering */
    RegIndex = ETH_PerfectFilter(MulticastMac0);
    printf("MAC address register index: %d\r\n", RegIndex);
#endif

#if MULTICAST_HASH_FILTER           /* multicast hash list filtering */
    ETH_HashFilter(MulticastMac1);
#endif

#if MULTICAST_HASH_OR_PERFECT_FILTER    /* multicast hash list or perfect filter*/
    RegIndex = ETH_PerfectFilter(MulticastMac0);
    printf("MAC address register index: %d\r\n", RegIndex);
    ETH_HashFilter(MulticastMac1);
    ETH_HashOrPerfectFilter(MULTICAST_HashOrPerfectFilter);
#endif
#endif

#if SRC_ADDR_PERFECT_FILTER         /* source MAC address filtering */
    /* Source address filtering takes
     * effect on the basis of destination address filtering*/
    RegIndex = ETH_PerfectFilter(UnicastMac0);
    printf("MAC address register index: %d\r\n", RegIndex);
    RegIndex = ETH_SrcAddrPerfectFilter(UnicastMac1);
    printf("MAC address register index: %d\r\n", RegIndex);
#endif

#if BROADCAST_FILTER                /* broadcast MAC address filtering*/
/* When enabling broadcast filtering, MACA0HR and MACA0LR should be initialized first.
 * Otherwise, the broadcast packet will be passed according to the default values of MACA0HR and MACA0LR.*/
    ETH->MACA0HR = (uint32_t)(((UnicastMac0[5]<<8)) | UnicastMac0[4]);
    ETH->MACA0LR = (uint32_t)(UnicastMac0[0] | (UnicastMac0[1]<<8) | (UnicastMac0[2]<<16) | (UnicastMac0[3]<<24));

    ETH_BroadcastFilter(ENABLE);
#endif
}

/*********************************************************************
 * @fn      MulticastSend
 *
 * @brief   send multicast packet
 *
 * @param   none
 *
 * @return  none.
 */
void MulticastSend(void)
{
    uint32_t DataLen = sizeof(MulticastData);

    WCHNET_SocketUdpSendTo(SocketId, MulticastData, &DataLen, MulticastIPAddr, 1000);
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

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);                                 //USART initialize
    printf("Filter and Multicast Test\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n", WCHNET_GetVer());
    if (WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);        //Ethernet library initialize
    mStopIfError(i);
    WCHNET_FilterCfg( );                                       //Configure filtering
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
    for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        WCHNET_CreateUdpSocket();                              //Create UDP Socket

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
        if (SendFlag) {
            SendFlag = 0;
            if(ConFlag) MulticastSend();
        }
    }
}

