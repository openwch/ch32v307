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
#include "MQTTPacket.h"
/*
 *@Note
MQTT例程，本程序用于演示基于TCP/IP的MQTT协议通讯，
                         单片机连接以太网、MQTT服务器后将会发布一个主题，
                         再订阅这个主题，并向这个主题发布消息，
                         最后接收到自己发送的消息。
*/

u8 MACAddr[6];                                               /* MAC地址*/
u8 IPAddr[4]   = {192,168,1,10};                             /* IP地址*/
u8 GWIPAddr[4] = {192,168,1,1};                              /* 网关*/
u8 IPMask[4]   = {255,255,255,0};                            /* 子网掩码*/
u8 DESIP[4]    = {0};                                        /* MQTT服务器IP地址 */

u8 SocketId;                                                 /* socket id号*/
u8 SocketRecvBuf[RECE_BUF_LEN];                              /* socket缓冲区*/
u8 MyBuf[RECE_BUF_LEN];
u16 desport = 1883;                                          /* MQTT服务器端口 */
u16 srcport = 4200;                                          /* 源端口号*/

char *username  = "user1";                                   /* 设备名，每个设备唯一，可用”/“做分级 */
char *password  = "user1";                                   /* 服务器登陆密码 */
char *sub_topic = "topic/1";                                 /* 订阅的会话名，为了自发自收，应与发布的会话名相同 */
char *pub_topic = "topic/1";                                 /* 发布的会话名 */
int pub_qos = 0;                                             /* 发布服务质量*/
int sub_qos = 0;                                             /* 订阅服务质量*/
char *payload = "WCHNET MQTT";                               /* 发布内容*/

u8 con_flag  = 0;                                            /* 已连接MQTT服务器标志位 */
u8 pub_flag  = 0;                                            /* 已发布会话消息标志位 */
u8 sub_flag  = 0;                                            /* 已订阅会话标志位 */
u8 tout_flag = 0;                                            /* 超时标志位 */
u16 packetid = 5;                                            /* 包ID */

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;               /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                 /* 显示错误 */
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

/*******************************************************************************
* Function Name : Transport_Open
* Description   : 创建TCP连接
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
u8 Transport_Open(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;                                  /* 创建临时socket变量 */

    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));       /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
    memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);            /* 设置目的IP地址 */
    TmpSocketInf.DesPort = desport;                         /* 设置目的端口 */
    TmpSocketInf.SourPort = srcport;                        /* 设置源端口 */
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                /* 设置socket类型 */
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;                 /* 设置接收缓冲区的接收长度 */
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);        /* 创建socket，将返回的socket索引保存在SocketId中 */
    mStopIfError(i);                                        /* 检查错误 */

    i = WCHNET_SocketConnect(SocketId);                     /* TCP连接 */
    mStopIfError(i);                                        /* 检查错误 */
    return SocketId;
}

/*******************************************************************************
* Function Name : Transport_Close
* Description   : 关闭TCP连接
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
u8 Transport_Close(void)
{
    u8 i;
    i = WCHNET_SocketClose(SocketId,TCP_CLOSE_NORMAL);
    mStopIfError(i);
    return i;
}

/*******************************************************************************
* Function Name : Transport_SendPacket
* Description   : 以太网发送数据
* Input         : *buf 发送数据的首字节地址
                  len  发送数据的长度
* Output        : None
* Return        : None
*******************************************************************************/
void Transport_SendPacket(u8 *buf, u32 len)
{
    u32 totallen;
    u8 *p = buf;

    totallen = len;
    while(1)
    {
        len = totallen;
        WCHNET_SocketSend(SocketId, p, &len);                   /* 将MyBuf中的数据发送 */
        totallen -= len;                                        /* 将总长度减去以及发送完毕的长度 */
        p += len;                                               /* 将缓冲区指针偏移*/
        if(totallen)continue;                                   /* 如果数据未发送完毕，则继续发送*/
        break;                                                  /* 发送完毕，退出 */
    }
}

/*******************************************************************************
* Function Name : MQTT_Connect
* Description   : 创建MQTT连接
* Input         : *username 设备名
                  *password 服务器连接密码
* Output        : None
* Return        : None
*******************************************************************************/
void MQTT_Connect(char *username, char *password)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    u32 len;
    u8 buf[200];

    data.clientID.cstring = "admin1";
    data.keepAliveInterval = 2000;
    data.cleansession = 1;
    data.username.cstring = username;
    data.password.cstring = password;

    len = MQTTSerialize_connect(buf,sizeof(buf),&data);
    Transport_SendPacket(buf,len);                              /*建立MQTT连接*/
}

/*******************************************************************************
* Function Name : MQTT_Subscribe
* Description   : MQTT订阅一个主题
* Input         : *topic 订阅的主题名
*                 req_qos 服务质量
* Output        : None
* Return        : None
*******************************************************************************/
void MQTT_Subscribe( char *topic,int req_qos)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_subscribe(buf,sizeof(buf),0,msgid,1,&topicString,&req_qos);
    Transport_SendPacket(buf,len);
}

/*******************************************************************************
* Function Name : MQTT_Unsubscribe
* Description   : MQTT取消订阅一个主题
* Input         : *topic 取消订阅的主题名
* Output        : None
* Return        : None
*******************************************************************************/
void MQTT_Unsubscribe(char *topic)
{
    MQTTString topicString = MQTTString_initializer;
    u32 len;
    u32 msgid = 1;
    u8 buf[200];

    topicString.cstring = topic;
    len = MQTTSerialize_unsubscribe(buf,sizeof(buf),0,msgid,1,&topicString);
    Transport_SendPacket(buf,len);
}

/*******************************************************************************
* Function Name : MQTT_Publish
* Description   : MQTT发布一个主题
* Input         : topic 发布的主题名
*                 qos 服务质量等级
*                 payload 有效载荷
* Output        : None
* Return        : None
*******************************************************************************/
void MQTT_Publish(char *topic, int qos, char *payload)
{
    MQTTString topicString = MQTTString_initializer;
    u32 payloadlen;
    u32 len;
    u8 buf[1024];

    topicString.cstring = topic;
    payloadlen = strlen(payload);
    len = MQTTSerialize_publish(buf,sizeof(buf),0,qos,0,packetid++,topicString,payload,payloadlen);
    Transport_SendPacket(buf,len);
}

/*******************************************************************************
* Function Name : MQTT_Pingreq
* Description   : MQTT发送心跳包
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void MQTT_Pingreq(void)
{
    u32 len;
    u8 buf[200];

    len = MQTTSerialize_pingreq(buf,sizeof(buf));
    Transport_SendPacket(buf,len);
}

/*******************************************************************************
* Function Name : MQTT_Disconnect
* Description   : 断开MQTT连接
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
void MQTT_Disconnect(void)
{
    u32 len;
    u8 buf[50];
    len = MQTTSerialize_disconnect(buf,sizeof(buf));
    Transport_SendPacket(buf,len);
}

/*******************************************************************************
* Function Name : msgDeal
* Description   : 处理订阅信息
* Input         : msg 订阅信息
*                 len 接收数据长度
* Output        : None
* Return        : None
*******************************************************************************/
void msgDeal(unsigned char *msg, int len)
{
    unsigned char *ptr = msg;
    printf("payload len = %d\r\n",len);
    printf("payload: ");
    for(u8 i = 0; i < len; i++)
    {
        printf("%c ",(u16)*ptr);
        ptr++;
    }
    printf("\r\n");
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
    int qos, payloadlen;
    MQTTString topicName;
    unsigned short packetid;
    unsigned char retained, dup;
    unsigned char *payload;

    if(initstat & SINT_STAT_RECV)                                                /* socket接收中断*/
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);                               /* 查询长度 */
        WCHNET_SocketRecv(socketid,MyBuf,&len);                                  /* 将接收缓冲区的数据读到MyBuf中*/
        switch(MyBuf[0]>>4)
        {
            case CONNACK:
                printf("CONNACK\r\n");
                con_flag = 1;
                MQTT_Subscribe(sub_topic, sub_qos);
                break;

            case PUBLISH:
                MQTTDeserialize_publish(&dup,&qos,&retained,&packetid,&topicName,
                                        &payload,&payloadlen,MyBuf,len);
                msgDeal(payload, payloadlen);
                break;

            case SUBACK:
                sub_flag = 1;
                printf("SUBACK\r\n");
                break;

            default:

                break;
        }
        memset(MyBuf, 0 ,sizeof(MyBuf));
    }
    if(initstat & SINT_STAT_CONNECT)                                             /* socket连接成功中断*/
    {
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf, RECE_BUF_LEN);
        MQTT_Connect(username, password);
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                          /* socket连接断开中断*/
    {
        con_flag = 0;
        printf("TCP Disconnect\r\n");
    }
    if(initstat & SINT_STAT_TIM_OUT)                                             /* socket连接超时中断*/
    {
       con_flag = 0;
       printf("TCP Timeout\r\n");
       Transport_Open();
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
	USART_Printf_Init(115200);                                               /*串口打印初始化*/
	printf("MQTT\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                              /*获取芯片MAC地址*/
    printf("mac addr:");
    for(int i=0;i<6;i++) printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                         /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    Transport_Open();                                                        /*创建TCP socket*/

	while(1)
	{
	  WCHNET_MainTask();                                                     /*以太网库主任务函数，需要循环调用*/
	  if(WCHNET_QueryGlobalInt())                                            /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
	  {
         WCHNET_HandleGlobalInt();
	  }
      if (publishValid == 1) {
          publishValid = 0;
            if(con_flag) MQTT_Publish(pub_topic,pub_qos,payload);
//            if(con_flag) MQTT_Pingreq();                                   /*心跳包*/
        }
    }
}
