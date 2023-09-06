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
IPRaw_PING example, demonstrate the PING function.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
*/
#include "string.h"
#include "eth_driver.h"
#include "PING.h"

u8 MACAddr[6];                                  //MAC address
u8 IPAddr[4]   = {192, 168, 1, 10};             //IP address
u8 GWIPAddr[4] = {192, 168, 1, 1};              //Gateway IP address
u8 IPMask[4]   = {255, 255, 255, 0};            //subnet mask
u8 DESIP[4]    = {192, 168, 1, 100};            //destination IP address
u8 IPRawProto  = 1;

u8 SocketId;                                    //socket id
u8 SocketRecvBuf[RECE_BUF_LEN];                 //socket receive buffer
u8 MyBuf[RECE_BUF_LEN];

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
    if (iError == WCHNET_ERR_SUCCESS) return;
    printf("Error: %02X\r\n", (u16)iError);
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

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
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
    SOCK_INF TmpSocketInf;

    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);
    TmpSocketInf.SourPort = IPRawProto;                               //In IPRAW mode, SourPort is the protocol type
    TmpSocketInf.ProtoType = PROTO_TYPE_IP_RAW;
    TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    mStopIfError(i);
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
void WCHNET_HandleSockInt(u8 socketid,u8 intstat)
{
    u32 len;

    if(intstat & SINT_STAT_RECV)                           //receive data
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);         //get socket buffer data length
        WCHNET_SocketRecv(socketid,MyBuf,&len);            //Read the data of the receive buffer into MyBuf
        WCHNET_ICMPRecvData( len,MyBuf );
    }
    if(intstat & SINT_STAT_CONNECT)                        //connect successfully
    {
        /***/
    }
    if(intstat & SINT_STAT_DISCONNECT)                     //disconnect
    {
        /***/
    }
    if(intstat & SINT_STAT_TIM_OUT)                        //timeout disconnect
    {
        /***/
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
        if(i&PHY_Linked_Status){
            ICMPSuc = ICMP_SOKE_CON;
            printf("PHY Link Success\r\n");
        }
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
    USART_Printf_Init(115200);                                              //USART initialize
    printf("IPRaw_PING Test\r\n");   	
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n",WCHNET_GetVer());
    if(WCHNET_LIB_VER != WCHNET_GetVer()){
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                             //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ", MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                        //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    WCHNET_CreateIPRawSocket();                                             //create IPRAW socket
    InitParameter();
    InitPING();
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
        WCHNET_PINGCmd();
    }
}
