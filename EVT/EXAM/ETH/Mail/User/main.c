/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/07
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "mail.h"
/*
 *@Note
mail例程，演示SMTP以及POP3的邮件收发
*/

u8 MACAddr[6];                                                                 /*MAC地址*/
u8 IPAddr[4]   = {192,168,1,10};                                               /*IP地址*/
u8 GWIPAddr[4] = {192,168,1,1};                                                /*网关*/
u8 IPMask[4]   = {255,255,255,0};                                              /*子网掩码*/
u8 DESIP[4]    = {192,168,1,100};                                              /*目的IP地址*/
u8 SocketId;                                                                   /*socket id号*/
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];                         /*socket缓冲区*/

/* 处理流程相关变量定义 */
u8     ReceDatFlag = 0;
u8     CheckType;
u8     OrderType;
u16    ReceLen;

/* socket SMTP 相关定义,SMTP发送邮件    */
const u16 SmtpSourPrt     = 5300;
const u8  SocketsmtpIP[4] = {183,47,101,192};                                  /* Socket SMTP目的IP地址 */

/*服务器IP可能会更改，建议使用之前手动ping获取最新IP*/
/*
 *   {220,181,15,73}    "smtp.126.com"
 *   {220,181,15,161}   "smtp.163.com"
 *   {183,47,101,192}   "smtp.qq.com"
 *   {64,233,189,108}   "smtp.gmail.com"
*/

/* socket POP3 相关定义, POP收邮件 */
const u16 Pop3SourPrt     = 4567;
const u8  Socketpop3IP[4] = {183,47,101,192};                                  /* Socket pop3目的IP地址 */

/*服务器IP可能会更改，建议使用之前手动ping获取最新IP*/
/*
 *   {220,181,15,128}   "pop.126.com",
 *   {220,181,12,110}   "pop.163.com",
 *   {183,47,101,192}   "pop.qq.com",
 *   {108,177,125,108}  "pop.gmail.com"
*/

/* smtp发送邮件相关参数  */
const char  *m_Server     =  "smtp.qq.com";                                      /* 服务器名称 */
const char  *m_UserName   =  "XXX";                                              /* 用户名 */
const char  *m_PassWord   =  "******";                                           /* 密码 */
const char  *m_SendFrom   =  "XXX@qq.com";                                       /* 发件人地址 */
const char  *m_SendName   =  "one";                                              /* 发送人名字 */
const char  *m_SendTo     =  "XXX@qq.com";                                       /* 收件人地址 */
const char  *m_Subject    =  "text";                                             /* 主题 */
const char  *m_FileName   =  "m_file.txt";                                       /* 附件名字(如果不发送附件,则置为"\0") */

/* pop接收邮件相关 */
const char  *p_Server     =  "pop.qq.com";                                       /* POP服务器 */
const char  *p_UserName   =  "XXX";                                              /* POP登陆用户名 */
const char  *p_PassWord   =  "******";                                           /* POP登陆密码 */


/*邮件相关内容*/
char     AttachmentData[attach_max_len] = "0123456789abcdefghijklmnopqrstuvwxyz";              /* 附件内容，用于演示（回复时用于暂存解码后的附件内容）*/
char     MailBodyData[128]              = "Demonstration test WCHNET mail function wch.cn";    /* 邮件正文内容，用于演示（回复时用于暂存解码的正文内容）*/

#ifdef   receive_over_reply
const    char *g_mailbody               = "welcome.";                                          /* 回复邮件正文内容，可修改 */
const    char *g_autograph1             = "\r\n\r\nBest Regards!\r\n----- Original Message \
                                           ----\r\n";                                          /* 回复邮件签名栏部分内容，可修改 */
const    char *g_autograph2             = "\r\nweb:http://www.wch.cn\r\n";                     /* 回复邮件签名栏部分内容，可修改 */
char     MacAddr[6];
char     MacAddrC[18];
#endif


/*******************************************************************************
* Function Name  : mStopIfError
* Description    : check if error.
* Input          : iError  error constants
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;                                     /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                                       /* 显示错误 */
}

/*******************************************************************************
* Function Name  : TIM2_Init
* Description    : Initializes TIM2.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_CreatTcpSmtp
* Description    : create TCP Client  socket for SMTP
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_CreatTcpSmtp( void )
{
   u8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)SocketRecvBuf[0],'\0',sizeof(SocketRecvBuf[0]));
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,SocketsmtpIP,4);                          /* 设置目的IP地址 */
   TmpSocketInf.DesPort = SMTP_SERVER_PORT;                                     /* 设置目的端口 */
   TmpSocketInf.SourPort = SmtpSourPrt;                                         /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socket类型 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 */

   printf("SocketId TCP SMTP = %02x\n",(u16)SocketId);

   mStopIfError(i);                                                             /* 检查错误 */
   p_smtp->Socket = SocketId;
   CheckType = SMTP_CHECK_CNNT;
   i = WCHNET_SocketConnect(SocketId);                                          /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */
}

/*******************************************************************************
* Function Name  : WCHNET_CreatTcpPop3
* Description    : create TCP Client socket for POP3
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_CreatTcpPop3( void )
{
   u8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)SocketRecvBuf[1],'\0',sizeof(SocketRecvBuf[1]));
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,Socketpop3IP,4);                          /* 设置目的IP地址 */
   TmpSocketInf.DesPort = POP3_SERVER_PORT;                                     /* 设置目的端口 */
   TmpSocketInf.SourPort = Pop3SourPrt;                                         /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socket类型 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 */

   printf("SocketId TCP pop3 = %02x\n",(u16)SocketId);

   mStopIfError(i);                                                             /* 检查错误 */
   p_pop3->Socket = SocketId;
   CheckType = POP_CHECK_CNNT;
   i = WCHNET_SocketConnect(SocketId);                                          /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */
   memset((void *)SocketRecvBuf[SocketId],'\0',sizeof(SocketRecvBuf[SocketId]));
}

/*******************************************************************************
* Function Name  : WCHNET_SendData
* Description    : 发送数据
* Input          : PSend  发送数据buff
*                  Len    发送数据长度
*                  type   控制命令码
*                  index  socket
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SendData( char *PSend, u32 Len,u8 type,u8 index  )
{
    u32 length;
    u8 i;
    u8 *p;
    p = (u8 *)PSend;
    length = Len;
    CheckType = type;
    while(1)
    {
        Len = length;
        i = WCHNET_SocketSend(index,p,&Len);                                    /* 数据发送 */
        mStopIfError(i);                                                        /* 检查错误 */
        length -= Len;                                                          /* 将总长度减去以及发送完毕的长度 */
        p += Len;                                                               /* 将缓冲区指针偏移*/
        if(length)continue;                                                     /* 如果数据未发送完毕，则继续发送*/
        break;                                                                  /* 发送完毕，退出 */
    }
}

/*******************************************************************************
* Function Name  : WCHNET_HandleSockInt
* Description    : Socket Interrupt Handle
* Input          : socketid  socket id
*                  initstat  中断状态
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_HandleSockInt(u8 socketid,u8 initstat)
{
    u32 len;
    u8 i;
    if(initstat & SINT_STAT_RECV)                                                 /* socket接收中断*/
    {
       ReceDatFlag = 1;
       len = WCHNET_SocketRecvLen(socketid,NULL);                                 /* 获取socket缓冲区数据长度  */
       ReceLen = len;
       WCHNET_SocketRecv(socketid,SocketRecvBuf[socketid],&len);                  /* 将接收缓冲区的数据读到缓存区中*/
       if( CheckType != uncheck ){
           i = WCHNET_CheckResponse((char *)SocketRecvBuf[socketid],CheckType);
           if(i != CHECK_SUCCESS){
               if(socketid == p_smtp->Socket) OrderType = SMTP_ERR_CHECK;
               if(socketid == p_pop3->Socket) OrderType = POP_ERR_CHECK;
               printf("ERROR: %02x\n",(u16)i);
           }
       }
       memset((void *)SocketRecvBuf[socketid],'\0',sizeof(SocketRecvBuf[socketid]));
    }
    if(initstat & SINT_STAT_CONNECT)                                              /* socket连接成功中断*/
    {
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                           /* socket连接断开中断*/
    {
        printf("TCP Disconnect\r\n");
        if(socketid == p_pop3->Socket) p_pop3->DiscnntFlag = 1;
    }
    if(initstat & SINT_STAT_TIM_OUT)                                              /* socket连接超时中断*/
    {
       printf("TCP Timeout\r\n");
       if(socketid == p_pop3->Socket) p_pop3->DiscnntFlag = 1;
    }
}

/*******************************************************************************
* Function Name  : WCHNET_HandleGlobalInt
* Description    : Global Interrupt Handle
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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
       i = WCHNET_GetPHYStatus();                                                 /* 获取PHY连接状态 */
       if(i&PHY_Linked_Status)
       printf("PHY Link Success\r\n");
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

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
    u8 i;

	Delay_Init();
	USART_Printf_Init(115200);                                                    /*串口打印初始化*/
	printf("Mail\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                                   /*获取芯片MAC地址*/
    printf("mac addr:");
    for(int i=0;i<6;i++) printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                              /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");

#ifdef send_mail
    if(send_mail) WCHNET_CreatTcpSmtp( );
    WCHNET_SMTPInit( );
#endif
#ifdef receive_mail
    if(receive_mail) WCHNET_CreatTcpPop3( );
    WCHNET_POP3Init( );
#endif

	while(1)
	{
        WCHNET_MainTask();                                                        /*以太网库主任务函数，需要循环调用*/
        if(WCHNET_QueryGlobalInt())                                               /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
        {
            WCHNET_HandleGlobalInt();
        }
        WCHNET_MailQuery( );
        if(ReceDatFlag){
            ReceDatFlag = 0;
            WCHNET_MailCmd( OrderType );
        }
    }
}
