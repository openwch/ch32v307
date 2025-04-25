/*
 * app_net.c
 *
 *  Created on: Sep 9, 2022
 *      Author: TECH66
 */

#include "string.h"
#if defined(CH32V20X) || defined(CH32V30X)	
#include "debug.h"
#endif
#include "app_net.h"
#include "wchiochub.h"

#define KEEPLIVE_ENABLE
#define TCP_SINGLE_CLIENT

u8 MACAddr[6]; 
u8 IPAddr[4] = { 192, 168, 1, 10 };
u8 GWIPAddr[4] = { 192, 168, 1, 1 };
u8 IPMask[4] = { 255, 255, 255, 0 };
u8 DESIP[4] = { 192, 168, 1, 100 };

u8 dhcpflag = 0;
u8 socket[WCHNET_MAX_SOCKET_NUM];
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];
u8 MyBuf[RECE_BUF_LEN];


void TCP_Send (uint8_t socketid, uint8_t *buf, uint32_t len)
{
    uint32_t sendlen = len;
    WCHNET_SocketSend (socketid, buf, &sendlen);
}

void UDP_Send (uint8_t socketid, uint8_t *buf, uint32_t slen, uint8_t *dstIP, uint16_t port)
{
    uint32_t sendlen = slen;
    uint16_t dstport = port;
    WCHNET_SocketUdpSendTo (socketid, buf, &sendlen, dstIP, dstport);
}

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError (u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)


    printf ("Error: %02X\r\n", (u16) iError); /*œ‘ æ¥ÌŒÛ */
}

/*******************************************************************************
 * Function Name  : appCallBack
 * Description    : UDP Recv
 * Input          :
 * Output         : None
 * Return         : None
 ********************************************************************************/
void udpCallBack (SOCK_INF *socinf, uint32_t ipaddr, uint16_t port, uint8_t *buf, uint32_t len)
{
    memcpy (udpAddr.addr, &ipaddr, 4);
    udpAddr.port = port;
    WCHIOCHUB_NetDataToUsrData(buf, len);
}

/*********************************************************************
 * @fn      WCHNET_CreatTcpSocket
 *
 * @brief   Creat Tcp Socket
 *
 * @return  none
 */
void WCHNET_CreatTcpSocket (uint32_t srcport, uint32_t desport, uint8_t *DESIP, uint8_t *SocketId)
{
    uint8_t i;
    SOCK_INF TmpSocketInf;

    memset ((void *) &TmpSocketInf, 0, sizeof( SOCK_INF)); 
    memcpy ((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat (SocketId, &TmpSocketInf);
    mStopIfError (i);
    i = WCHNET_SocketConnect (*SocketId);
    mStopIfError (i);
}

/*********************************************************************
 * @fn      WCHNET_CreatUdpSocket
 *
 * @brief   Creat Udp Socket
 *
 * @return  none
 */
void WCHNET_CreatUdpSocket (uint32_t srcport, uint8_t *SocketId)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset ((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    TmpSocketInf.AppCallBack = udpCallBack;
    i = WCHNET_SocketCreat (SocketId, &TmpSocketInf);
    WCHNET_ModifyRecvBuf(*SocketId, (u32) SocketRecvBuf[*SocketId], RECE_BUF_LEN);
    mStopIfError (i);

}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleSockInt (u8 socketid, u8 initstat)
{
    uint32_t len;
    if (initstat & SINT_STAT_RECV)
    {
        len = WCHNET_SocketRecvLen (socketid, NULL);
        WCHNET_SocketRecv (socketid, MyBuf, &len);
        WCHIOCHUB_NetDataToUsrData( MyBuf, len);
    }

    if (initstat & SINT_STAT_CONNECT)
    {
        #ifdef KEEPLIVE_ENABLE
        WCHNET_SocketSetKeepLive (socketid, ENABLE);
        #endif
        printf ("TCP CONNECT SUCCESS, Socketid: %d    TCP_SOUR_Port:%u\r\n",
                socketid, iochubScrPort);
        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid], RECE_BUF_LEN);
        if (iochubScrPort == SocketInf[socketid].SourPort)
        {
        	SocketIdForSer = socketid;
            socketIsSucc |= 0x01;
        }

        printf ("socketIsSucc: %d\r\n", socketIsSucc);
    }

    if (initstat & SINT_STAT_DISCONNECT)
    {
        printf ("TCP Disconnect\r\n");
        socketIsSucc &= ~0x01;
    }

    if (initstat & SINT_STAT_TIM_OUT)
    {
        printf ("TCP Timeout\r\n");
        socketIsSucc &= ~0x01;
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt (void)
{
    u8 initstat;
    u16 i;
    u8 socketinit;

    initstat = WCHNET_GetGlobalInt ();

    if (initstat & GINT_STAT_UNREACH)
    {
        printf ("GINT_STAT_UNREACH\r\n");
    }

    if (initstat & GINT_STAT_IP_CONFLI)
    {
        printf ("GINT_STAT_IP_CONFLI\r\n");
    }

    if (initstat & GINT_STAT_PHY_CHANGE)
    {
        i = WCHNET_GetPHYStatus ();

        if (i & PHY_Linked_Status)
            printf ("PHY Link Success\r\n");
    }

    if (initstat & GINT_STAT_SOCKET)
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        {
            socketinit = WCHNET_GetSocketInt (i);

            if (socketinit)
                WCHNET_HandleSockInt (i, socketinit);
        }
    }
}

/*******************************************************************************
 * Function Name  : WCHNET_DHCPCallBack
 * Description    : 
 * Input          : None
 * Output         : None
 * Return         : result
 *******************************************************************************/
 extern void WCHIOCHUB_StartEn (void);
uint8_t WCHNET_DHCPCallBack (uint8_t status, void *arg)
{
    uint8_t *p;
    dhcpflag = 1;
    if (!status)
    {
        p = arg;
        printf ("DHCP Success\r\n");
        memcpy (IPAddr, p, 4);
        memcpy (GWIPAddr, &p[4], 4);
        memcpy (IPMask, &p[8], 4);
        printf ("IPAddr   = %d.%d.%d.%d \r\n", (u16) IPAddr[0], (u16) IPAddr[1],
                (u16) IPAddr[2], (u16) IPAddr[3]);
        printf ("GWIPAddr = %d.%d.%d.%d \r\n", (u16) GWIPAddr[0],
                (u16) GWIPAddr[1], (u16) GWIPAddr[2], (u16) GWIPAddr[3]);
        printf ("IPAddr   = %d.%d.%d.%d \r\n", (u16) IPMask[0], (u16) IPMask[1],
                (u16) IPMask[2], (u16) IPMask[3]);
        printf ("DNS1     = %d.%d.%d.%d \r\n", p[12], p[13], p[14], p[15]);
        printf ("DNS2     = %d.%d.%d.%d \r\n", p[16], p[17], p[18], p[19]);
        WCHNET_DHCPStop ();
        WCHNET_SocketClose (0,0);
        memcpy (localIP, IPAddr, 4);
        WCHIOCHUB_StartEn ();
        return 0;
    }
    else
    {
        printf ("DHCP Fail %02x \r\n", status);
        printf ("IPAddr   = %d.%d.%d.%d \r\n", (u16) IPAddr[0], (u16) IPAddr[1],
                (u16) IPAddr[2], (u16) IPAddr[3]);
        printf ("GWIPAddr = %d.%d.%d.%d \r\n", (u16) GWIPAddr[0],
                (u16) GWIPAddr[1], (u16) GWIPAddr[2], (u16) GWIPAddr[3]);
        printf ("IPAddr   = %d.%d.%d.%d \r\n", (u16) IPMask[0], (u16) IPMask[1],
                (u16) IPMask[2], (u16) IPMask[3]);
        WCHNET_DHCPStop ();
        memcpy (localIP, IPAddr, 4);

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
void NET_Init (void)
{
    u8 i;
    printf("net version:%x\n", WCHNET_GetVer ());

    if ( WCHNET_LIB_VER != WCHNET_GetVer ())
    {
    	printf("version error.\n");
    }
    WCHNET_GetMacAddr (MACAddr); 
    printf("mac addr:");
    for ( i = 0; i < 6; i++)
    	printf("%02X ", MACAddr[i]);
    printf("\n");

    i = ETH_LibInit (IPAddr, GWIPAddr, IPMask, MACAddr);
    mStopIfError (i);

    #ifdef KEEPLIVE_ENABLE
    {
        struct _KEEP_CFG cfg;
        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive (&cfg);
    }
    #endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    WCHNET_DHCPStart (WCHNET_DHCPCallBack);

    if (i == WCHNET_ERR_SUCCESS)
    	printf("WCHNET_LibInit Success\r\n");

}
