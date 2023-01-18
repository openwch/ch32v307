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
MQTT example, this program is used to demonstrate TCP/IP-based MQTT protocol communication,
After the single-chip microcomputer connects to Ethernet and MQTT server, a topic will be released.
Subscribe to this topic and publish messages to this topic.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
*/
#include "string.h"
#include "debug.h"
#include "wchnet.h"
#include "eth_driver.h"
#include "MQTTPacket.h"

u8 MACAddr[6];                                     //MAC address
u8 IPAddr[4]   = {192,168,1,10};                   //IP address
u8 GWIPAddr[4] = {192,168,1,1};                    //Gateway IP address
u8 IPMask[4]   = {255,255,255,0};                  //subnet mask
u8 DESIP[4]    = {0};                              //MQTT server IP address,!!need to be modified manually

u8 SocketId;                                       //socket id
u8 SocketRecvBuf[RECE_BUF_LEN];                    //socket receive buffer
u8 MyBuf[RECE_BUF_LEN];
u16 desport = 1883;                                //MQTT server port
u16 srcport = 4200;                                //source port

char *username  = "user1";                         //Device name, unique for each device, available "/" for classification
char *password  = "user1";                         //Server login password
char *sub_topic = "topic/1";                       //subscribed session name
char *pub_topic = "topic/1";                       //Published session name
int pub_qos = 0;                                   //Publish quality of service
int sub_qos = 0;                                   //Subscription quality of service
char *payload = "WCHNET MQTT";                     //Publish content

u8 con_flag  = 0;                                  //Connect MQTT server flag
u8 pub_flag  = 0;                                  //Publish session message flag/
u8 sub_flag  = 0;                                  //Subscription session flag
u8 tout_flag = 0;                                  //time-out flag
u16 packetid = 5;                                  //package id

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

/*********************************************************************
 * @fn      Transport_Open
 *
 * @brief   open the TCP connection.
 *
 * @return  socket id
 */
u8 Transport_Open(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    mStopIfError(i);

    i = WCHNET_SocketConnect(SocketId);
    mStopIfError(i);
    return SocketId;
}

/*********************************************************************
 * @fn      Transport_Close
 *
 * @brief   close the TCP connection.
 *
 * @return  @ERR_T
 */
u8 Transport_Close(void)
{
    u8 i;
    i = WCHNET_SocketClose(SocketId,TCP_CLOSE_NORMAL);
    mStopIfError(i);
    return i;
}

/*********************************************************************
 * @fn      Transport_SendPacket
 *
 * @brief   send data.
 *
 * @param   buf - data buff.
 *          len - data length
 *
 * @return  none
 */
void Transport_SendPacket(u8 *buf, u32 len)
{
    WCHNET_SocketSend(SocketId, buf, &len);
    printf("%d bytes uploaded!",len);
}

/*********************************************************************
 * @fn      MQTT_Connect
 *
 * @brief   Establish MQTT connection.
 *
 * @param   username - user name.
 *          password - password
 *
 * @return  none
 */
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
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Subscribe
 *
 * @brief   MQTT subscribes to a topic.
 *
 * @param   topic - Topic name to subscribe to.
 *          req_qos - quality of service
 *
 * @return  none
 */
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

/*********************************************************************
 * @fn      MQTT_Unsubscribe
 *
 * @brief   MQTT unsubscribe from a topic.
 *
 * @param   topic - Topic name to unsubscribe to.
 *
 * @return  none
 */
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

/*********************************************************************
 * @fn      MQTT_Publish
 *
 * @brief   MQTT publishes a topic.
 *
 * @param   topic - Published topic name.
 *          qos - quality of service
 *          payload - data buff
 *
 * @return  none
 */
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

/*********************************************************************
 * @fn      MQTT_Pingreq
 *
 * @brief   MQTT sends heartbeat packet
 *
 * @return  none
 */
void MQTT_Pingreq(void)
{
    u32 len;
    u8 buf[200];

    len = MQTTSerialize_pingreq(buf,sizeof(buf));
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      MQTT_Disconnect
 *
 * @brief   Disconnect the MQTT connection
 *
 * @return  none
 */
void MQTT_Disconnect(void)
{
    u32 len;
    u8 buf[50];
    len = MQTTSerialize_disconnect(buf,sizeof(buf));
    Transport_SendPacket(buf,len);
}

/*********************************************************************
 * @fn      msgDeal
 *
 * @brief   Dealing with subscription information.
 *
 * @param   msg - data buff
 *          len - data length
 *
 * @return  none
 */
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
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u32 len;
    int qos, payloadlen;
    MQTTString topicName;
    unsigned short packetid;
    unsigned char retained, dup;
    unsigned char *payload;

    if(intstat & SINT_STAT_RECV)                                                //receive data
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);
        WCHNET_SocketRecv(socketid,MyBuf,&len);
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
    if(intstat & SINT_STAT_CONNECT)                                             //connect successfully
    {
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf, RECE_BUF_LEN);
        MQTT_Connect(username, password);
        printf("TCP Connect Success\r\n");
    }
    if(intstat & SINT_STAT_DISCONNECT)                                          //disconnect
    {
        con_flag = 0;
        printf("TCP Disconnect\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT)                                             //timeout disconnect
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
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();                                    //get global interrupt flag
    if(intstat & GINT_STAT_UNREACH)                                     //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if(intstat & GINT_STAT_IP_CONFLI)                                   //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if(intstat & GINT_STAT_PHY_CHANGE)                                  //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if(i&PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if(intstat & GINT_STAT_SOCKET)                                      //socket related interrupt
    {
        for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        {
            socketint = WCHNET_GetSocketInt(i);
            if(socketint) WCHNET_HandleSockInt(i, socketint);
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
    USART_Printf_Init(115200);                                               //USART initialize
    printf("MQTT\r\n");   	
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                              //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                         //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    Transport_Open();                                                        //open the TCP connection.

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
        if (publishValid == 1) {
            publishValid = 0;
            if(con_flag) MQTT_Publish(pub_topic, pub_qos, payload);
//            if(con_flag) MQTT_Pingreq();                                   //heartbeat packet
        }
    }
}
