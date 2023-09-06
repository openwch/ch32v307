/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/07
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
Mail example, demo SMTP and POP3 mail sending and receiving.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
*/
#include "string.h"
#include "eth_driver.h"
#include "mail.h"

u8 MACAddr[6];                                                      //MAC address
u8 IPAddr[4]   = {192, 168, 1, 10};                                 //IP address
u8 GWIPAddr[4] = {192, 168, 1, 1};                                  //Gateway IP address
u8 IPMask[4]   = {255, 255, 255, 0};                                //subnet mask
u8 DESIP[4]    = {192, 168, 1, 100};                                //destination IP address
u8 SocketId;                                                        //socket id
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];              //socket data buff

u8  ReceDatFlag = 0;
u8  CheckType;
u8  OrderType;
u16 ReceLen;

/*SMTP related definitions, SMTP sending mail*/
const u16 SmtpSourPrt     = 5300;
const u8  SocketSMTPIP[4] = {183, 47, 101, 192};                    //SMTP destination IP address

/*It is recommended to manually PING to
 * obtain the latest IP before using*/
/*
 *   {220, 181, 15, 73}    "smtp.126.com"
 *   {220, 181, 15, 161}   "smtp.163.com"
 *   {183, 47, 101, 192}   "smtp.qq.com"
 *   {64, 233, 189, 108}   "smtp.gmail.com"
*/

/*POP3 related definitions, POP receiving mail*/
const u16 Pop3SourPrt     = 4567;
const u8  SocketPOP3IP[4] = {183, 47, 101, 192};                    //POP3 destination IP address

/*It is recommended to manually PING to
 * obtain the latest IP before using*/
/*
 *   {220, 181, 15, 128}   "pop.126.com",
 *   {220, 181, 12, 110}   "pop.163.com",
 *   {183, 47, 101, 192}   "pop.qq.com",
 *   {108, 177, 125, 108}  "pop.gmail.com"
*/

/* SMTP send email related parameters  */
const char  *m_Server     =  "smtp.qq.com";                        //server name
const char  *m_UserName   =  "XXX";                                //user name
const char  *m_PassWord   =  "******";                             //password
const char  *m_SendFrom   =  "XXX@qq.com";                         //sender address
const char  *m_SendName   =  "one";                                //sender name
const char  *m_SendTo     =  "XXX@qq.com";                         //receiver's address
const char  *m_Subject    =  "text";                               //subject
const char  *m_FileName   =  "m_file.txt";                         //Attachment name (set to "\0" if no attachment is sent)

/* POP receive mail related parameters */
const char  *p_Server     =  "pop.qq.com";                         //POP server
const char  *p_UserName   =  "XXX";                                //POP login user name
const char  *p_PassWord   =  "******";                             //POP login password


/*Email related content*/
char     AttachmentData[attach_max_len] = "0123456789abcdefghijklmnopqrstuvwxyz";              //Attachment content
char     MailBodyData[128]              = "Demonstration test WCHNET mail function wch.cn";    //Email body content

#ifdef   receive_over_reply
const    char *g_mailbody               = "welcome.";                                          //Email body content for reply
const    char *g_autograph1             = "\r\n\r\nBest Regards!\r\n----- Original Message \
                                           ----\r\n";                                          //Email autograph field content for reply
const    char *g_autograph2             = "\r\nweb:http://www.wch.cn\r\n";                     //Email autograph field content for reply
char     MacAddr[6];
char     MacAddrC[18];
#endif

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
 * @fn      WCHNET_CreateTcpSmtp
 *
 * @brief   create TCP Client  socket for SMTP
 *
 * @return  none
 */
void WCHNET_CreateTcpSmtp( void )
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *)SocketRecvBuf[0],'\0',sizeof(SocketRecvBuf[0]));
    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr,SocketSMTPIP,4);
    TmpSocketInf.DesPort = SMTP_SERVER_PORT;
    TmpSocketInf.SourPort = SmtpSourPrt;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    printf("SocketId TCP SMTP = %02x\n",(u16)SocketId);
    mStopIfError(i);
    p_smtp->Socket = SocketId;
    CheckType = SMTP_CHECK_CNNT;
    i = WCHNET_SocketConnect(SocketId);
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpPop3
 *
 * @brief   create TCP Client socket for POP3
 *
 * @return  none
 */
void WCHNET_CreateTcpPop3( void )
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *)SocketRecvBuf[1],'\0',sizeof(SocketRecvBuf[1]));
    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr,SocketPOP3IP,4);
    TmpSocketInf.DesPort = POP3_SERVER_PORT;
    TmpSocketInf.SourPort = Pop3SourPrt;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    printf("SocketId TCP pop3 = %02x\n",(u16)SocketId);
    mStopIfError(i);
    p_pop3->Socket = SocketId;
    CheckType = POP_CHECK_CNNT;
    i = WCHNET_SocketConnect(SocketId);
    mStopIfError(i);
    memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));
}

/*********************************************************************
 * @fn      WCHNET_SendData
 *
 * @brief   send data.
 *
 * @param   PSend  data buff
*           Len    data length
*           type   control command code
*           id     socket id
 * @return  none
 */
void WCHNET_SendData( char *PSend, u32 Len,u8 type,u8 id  )
{
    u32 length;
    u8 TimoutCnt = 0xff;
    u8 i;
    u8 *p;

    p = (u8 *)PSend;
    length = Len;
    CheckType = type;
    while(1)
    {
        Len = length;
        i = WCHNET_SocketSend(id,p,&Len);
        mStopIfError(i);
        length -= Len;
        p += Len;
        if(TimoutCnt-- == 0)
            break;
        if(length)continue;
        break;
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
void WCHNET_HandleSockInt(u8 socketid,u8 intstat)
{
    u32 len;
    u8 i;
    if(intstat & SINT_STAT_RECV)                                                 //receive data
    {
       ReceDatFlag = 1;
       len = WCHNET_SocketRecvLen(socketid,NULL);
       ReceLen = len;
       WCHNET_SocketRecv(socketid,SocketRecvBuf[socketid],&len);
       if( CheckType != UNCHECK ){
           i = WCHNET_CheckResponse((char *)SocketRecvBuf[socketid],CheckType);
           if(i != CHECK_SUCCESS){
               if(socketid == p_smtp->Socket) OrderType = SMTP_ERR_CHECK;
               if(socketid == p_pop3->Socket) OrderType = POP_ERR_CHECK;
               printf("ERROR: %02x\n",(u16)i);
           }
       }
       memset((void *)SocketRecvBuf[socketid],'\0',sizeof(SocketRecvBuf[socketid]));
    }
    if(intstat & SINT_STAT_CONNECT)                                              //connect successfully
    {
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if(intstat & SINT_STAT_DISCONNECT)                                           //disconnect
    {
        printf("TCP Disconnect\r\n");
        if(socketid == p_pop3->Socket) p_pop3->DiscnntFlag = 1;
    }
    if(intstat & SINT_STAT_TIM_OUT)                                              //timeout disconnect
    {
       printf("TCP Timeout\r\n");
       if(socketid == p_pop3->Socket) p_pop3->DiscnntFlag = 1;
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
    u8 socketint;
    u16 i;

    intstat = WCHNET_GetGlobalInt();                                           //get global interrupt flag
    if(intstat & GINT_STAT_UNREACH)                                            //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if(intstat & GINT_STAT_IP_CONFLI)                                          //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if(intstat & GINT_STAT_PHY_CHANGE)                                         //PHY status change
    {
        i = WCHNET_GetPHYStatus();                                             //socket related interrupt
        if(i&PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if(intstat & GINT_STAT_SOCKET)
    {
        for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i ++)
        {
            socketint = WCHNET_GetSocketInt(i);
            if(socketint)WCHNET_HandleSockInt(i,socketint);
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
	USART_Printf_Init(115200);                                                    //USART initialize
	printf("Mail Test\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer()){
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                                   //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ", MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                              //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");

#ifdef send_mail
    if(send_mail) WCHNET_CreateTcpSmtp( );
    WCHNET_SMTPInit( );
#endif
#ifdef receive_mail
    if(receive_mail) WCHNET_CreateTcpPop3( );
    WCHNET_POP3Init( );
#endif

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
        WCHNET_MailQuery( );
        /*Process the received data*/
        if(ReceDatFlag){
            ReceDatFlag = 0;
            WCHNET_MailCmd( OrderType );
        }
    }
}
