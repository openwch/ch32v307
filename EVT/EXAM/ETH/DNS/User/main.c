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
DNS example ,demonstrate that DHCP automatically obtains
an IP address and then requests domain name resolution.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
*/
#include "string.h"
#include "debug.h"
#include "wchnet.h"
#include "eth_driver.h"

u8 MACAddr[6];                                      //MAC address
u8 IPAddr[4]   = {0, 0, 0, 0};                      //IP address
u8 GWIPAddr[4] = {0, 0, 0, 0};                      //Gateway IP address
u8 IPMask[4]   = {0, 0, 0, 0};                      //subnet mask
u8 DESIP[4]    = {255, 255, 255, 255};              //destination IP address
u16 DnsPort = 53;
__attribute__((__aligned__(4))) u8  RemoteIp[4];

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
    NVIC_EnableIRQ(TIM2_IRQn);
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
    if (intstat & SINT_STAT_RECV)                              //receive data
    {
    }
    if (intstat & SINT_STAT_CONNECT)                           //connect successfully
    {
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                        //disconnect
    {
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                           //timeout disconnect
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
 * @fn      WCHNET_DNSCallBack
 *
 * @brief   DNSCallBack
 *
 * @return  none
 */
void WCHNET_DNSCallBack(const char *name, u8 *ipaddr, void *callback_arg)
{
    if(ipaddr == NULL)
    {
        printf("DNS Fail\r\n");
        return;
    }
    printf("Host Name = %s\r\n", name);
    printf("IP= %d.%d.%d.%d\r\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    if(callback_arg != NULL)
    {
        printf("callback_arg = %02x\r\n", (*(u8 *)callback_arg));
    }
    WCHNET_DNSStop();                                                          //stop DNS,and release socket
}

/*********************************************************************
 * @fn      WCHNET_DHCPCallBack
 *
 * @brief   DHCPCallBack
 *
 * @param   status - status returned by DHCP
 *          arg - Data returned by DHCP
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
        printf("DNS1: %d.%d.%d.%d \r\n", p[12], p[13], p[14], p[15]);            //DNS server address provided by the router
        printf("DNS2: %d.%d.%d.%d \r\n", p[16], p[17], p[18], p[19]);

        WCHNET_InitDNS(&p[12], DnsPort);                                         //Set DNS server IP address, and DNS server port is 53
        WCHNET_HostNameGetIp("www.wch.cn", RemoteIp, WCHNET_DNSCallBack, NULL);  //Start DNS
        return READY;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        return NoREADY;
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
    USART_Printf_Init(115200);                                          //USART initialize
    printf("DNS Test\r\n");	
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                         //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    WCHNET_DHCPSetHostname("WCHNET");                                   //Configure DHCP host name
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                    //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
    WCHNET_DHCPStart(WCHNET_DHCPCallBack);                              //Start DHCP

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
