/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/01/18
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
Web Server example, this program is used to demonstrate the function
of configuring the WCHNET chip through a web browser, the WCHNET chip has a built-in web server,
The web page can implement WCHNET's network parameter configuration and password management.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 */
#include "string.h"
#include "eth_driver.h"
#include "HTTPS.h"

u8 MACAddr[6];                                                  //MAC address
u8 IPAddr[4];                                                   //IP address
u8 GWIPAddr[4];                                                 //Gateway IP address
u8 IPMask[4];                                                   //subnet mask

u8 SocketId, SocketIdForListen;
u8 RecvBuffer[RECE_BUF_LEN], HTTPDataBuffer[RECE_BUF_LEN];
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];          //socket receive buffer
u16 DESPORT, SRCPORT;                                           //port
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
 * @fn      WCHNET_CreateTcpSocketListen
 *
 * @brief   Create TCP Socket for Listening
 *
 * @return  none
 */
void WCHNET_CreateTcpSocketListen(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = HTTP_SERVER_PORT;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    i = WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);
    printf("SocketIdForListen %d\r\n", SocketIdForListen);
    mStopIfError(i);
    i = WCHNET_SocketListen(SocketIdForListen);
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_CreateCfgSocket
 *
 * @brief   According to the configuration information of the webpage,
 *          WCHNET establishes the corresponding socket.
 *
 *@param    mode - connection mode.
 *          Desip - destination IP
 *          Desport - destination port
 *          Srcport - source port
 * @return  none
 */
void WCHNET_CreateCfgSocket(u8 mode, u8 *Desip, u16 Desport, u16 Srcport)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    printf("desport: %d, srcport: %d\n", Desport, Srcport);
    printf("desip:%d.%d.%d.%d\n", Desip[0], Desip[1], Desip[2], Desip[3]);
    printf("mode %d\n", mode);
    switch (mode) {
        case MODE_TCPSERVER:
            TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
            TmpSocketInf.SourPort = Srcport;
            i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
            mStopIfError(i);
            i = WCHNET_SocketListen(SocketId);
            mStopIfError(i);
            break;

        case MODE_TCPCLIENT:
            TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
            memcpy((void *) &TmpSocketInf.IPAddr, Desip, 4);
            TmpSocketInf.SourPort = Srcport;
            TmpSocketInf.DesPort = Desport;
            TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
            i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
            mStopIfError(i);
            i = WCHNET_SocketConnect(SocketId);
            mStopIfError(i);
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      WCHNET_RestoreDefaults
 *
 * @brief   Parameter restore default value
 *
 * @return  none
 */
void WCHNET_RestoreDefaults(void)                                       /*WCHNET restore default settings*/
{
    WCHNET_GetMacAddr(&Basic_Default[2]);
    WEB_ERASE( PAGE_WRITE_START_ADDR, FLASH_PAGE_SIZE * 3);
    WEB_WRITE( BASIC_CFG_ADDR, Basic_Default, BASIC_CFG_LEN);
    WEB_WRITE( PORT_CFG_ADDR, Port_Default, PORT_CFG_LEN);
    WEB_WRITE( LOGIN_CFG_ADDR, Login_Default, LOGIN_CFG_LEN);
    NVIC_SystemReset();
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
    u32 len;

    if (intstat & SINT_STAT_RECV)                                   //receive data
    {
        len = WCHNET_SocketRecvLen(socketid, NULL);
        if (SocketInf[socketid].SourPort == HTTP_SERVER_PORT) {     // receive HTTP data
            socket = socketid;
            WCHNET_SocketRecv(socketid, HTTPDataBuffer, &len);
            Web_Server();
        }
        else                                                        //receive the data of the configured socket
            WCHNET_SocketRecv(socketid, RecvBuffer, &len);
        printf("socketid:%d Received data length:%d\r\n",socketid, len);
    }
    if (intstat & SINT_STAT_CONNECT)                                //connect successfully
    {
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                             //disconnect
    {
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                                //timeout disconnect
    {
        printf("TCP Timeout\r\n");
        WCHNET_CreateCfgSocket(Port_CfgBuf.mode, Port_CfgBuf.des_ip, DESPORT, SRCPORT);
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

    intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET) {                             //socket related interrupt
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}

/*********************************************************************
 * @fn      GPIOInit
 *
 * @brief   GPIO initialization
 *
 * @return  none
 */
void GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
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
    USART_Printf_Init(115200);                                                  //USART initialize
    GPIOInit();
    printf("Web Server\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n", WCHNET_GetVer());
    if (WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    /*After the button(PB6) is pressed, initialize
     * WCHNET and execute the default configuration*/
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {
        Delay_Ms(100);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0);
            WCHNET_RestoreDefaults();
        }
    }
    WEB_READ( BASIC_CFG_ADDR, (u8 *)&Basic_CfgBuf, BASIC_CFG_LEN );                //Read configuration information
    WEB_READ( PORT_CFG_ADDR, (u8 *)&Port_CfgBuf, PORT_CFG_LEN );
    WEB_READ( LOGIN_CFG_ADDR, (u8 *)&Login_CfgBuf, LOGIN_CFG_LEN );
    /*Determine configuration information*/
    if(((Basic_CfgBuf.flag[0] != 0x57) || (Basic_CfgBuf.flag[1] != 0xAB)) ||\
       ((Port_CfgBuf.flag[0] != 0x57) || (Port_CfgBuf.flag[1] != 0xAB)) ||\
       ((Login_CfgBuf.flag[0] != 0x57) || (Login_CfgBuf.flag[1] != 0xAB)))
    {
        WCHNET_RestoreDefaults();
    }
    memcpy(MACAddr, Basic_CfgBuf.mac, 6);
    memcpy(IPAddr, Basic_CfgBuf.ip, 4);
    memcpy(IPMask, Basic_CfgBuf.mask, 4);
    memcpy(GWIPAddr, Basic_CfgBuf.gateway, 4);
    printf("ip: ");
    for (i = 0; i < 4; i++)
        printf("%d.", IPAddr[i]);
    printf("\n");

    printf("mac addr: ");
    for(i = 0; i < 6; i++) 
        printf("%x ", MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);                         //Ethernet library initialize
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
    WCHNET_CreateTcpSocketListen();                                             //Create  TCP Socket

    DESPORT = Port_CfgBuf.des_port[0] * 256 + Port_CfgBuf.des_port[1];
    SRCPORT = Port_CfgBuf.src_port[0] * 256 + Port_CfgBuf.src_port[1];
    WCHNET_CreateCfgSocket(Port_CfgBuf.mode, Port_CfgBuf.des_ip, DESPORT, SRCPORT);
    Init_Para_Tab();

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

