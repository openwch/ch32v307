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
ETH_CFG example, create a UDP Server to communicate with the host computer, configure WCHNET features,
including parameters, and create a new communication.
This example uses the software for the 1_Tool_Doc folder under "WCH NET Network Parameter Configuration Tool".
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
*/
#include "string.h"
#include "eth_driver.h"
#include "ModuleConfig.h"

__attribute__((aligned(4))) u8 Default_cfg[MODULE_CFG_LEN]= {                   //Module default configuration
    'W','C','H','N','E','T','M','O','D','U','L','E',0,0,0,0,0,0,0,0,0,          //module name
    NET_MODULE_TYPE_NONE,                                                       //The module is in default mode (no mode is enabled by default)
    192,168,1,10,                                                               //The IP address of the module
    255,255,255,0,                                                              //The subnet mask of the module
    192,168,1,1,                                                                //The gateway address of the module
    1000%256,1000/256,                                                          //Module source port
    192,168,1,100,                                                              //destination IP address
    1000%256,1000/256 ,                                                         //destination port
    checkcode1,checkcode2                                                       //Verification code, used to verify configuration information
};

u8 sockFlag;
u8 Configbuf[MODULE_CFG_LEN];
pmodule_cfg CFG = (pmodule_cfg)Configbuf;

u8 MACAddr[6];                                                                  //MAC address
u8 IPAddr[4];                                                                   //IP address
u8 GWIPAddr[4];                                                                 //Gateway IP address
u8 IPMask[4];                                                                   //subnet mask
u8 DESIP[4];                                                                    //destination IP address

u8 SocketId;                                                                    //socket id
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];                          //socket data buff

/*Host computer communication parameters*/
u8  brocastIp[4] = {255, 255, 255, 255};                                        //Broadcast IP address, cannot be changed
u16 brocastPort = 60000;                                                        //Host computer communication port
u16 localPort = 50000;                                                          //local communication port
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
 * @fn      WCHNET_UdpServerRecv
 *
 * @brief   UDP Receive data function.
 *
 * @param   socinf - socket information.
 *          ipaddr - IP address
 *          port - port
 *          buf - data buff
 *          len - data length
 *
 * @return  none
 */
void WCHNET_UdpServerRecv(struct _SOCK_INF *socinf,u32 ipaddr,u16 port,u8 *buf,u32 len)
{
    u8 i;

    i = ParseConfigbuf(buf) ;                                                               //Analytical data
    if(i){
        NVIC_SystemReset();
    }
}

/*********************************************************************
 * @fn      WCHNET_UdpSRecv
 *
 * @brief   UDP data callback.
 *
 * @param   socinf - socket information.
 *          ipaddr - IP address
 *          port - port
 *          buf - data buff
 *          len - data length
 *
 * @return  none
 */
void WCHNET_UdpSRecv(struct _SOCK_INF *socinf,u32 ipaddr,u16 port,u8 *buf,u32 len)
{
    u8 ip_addr[4],i;

    printf("ip:");
    for(i=0;i<4;i++){
        ip_addr[i] = ipaddr&0xff;
        printf("%-4d",ip_addr[i]);
        ipaddr = ipaddr>>8;
    }

    printf("port=%-8d  len=%-8d  sockeid=%-4d\n",port,len,socinf->SockIndex);
    WCHNET_SocketUdpSendTo(socinf->SockIndex,buf,&len,ip_addr,port);
}

/*********************************************************************
 * @fn      WCHNET_Create_Communication_Socket
 *
 * @brief   create socket.
 *
 * @return  none
 */
void WCHNET_Create_Communication_Socket(void)
{
    u8 i;
    u8 *desip = CFG->dest_ip;
    SOCK_INF TmpSocketInf;

    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr,desip,4);
    TmpSocketInf.DesPort = ((u16)CFG->dest_port[0]+(u16)CFG->dest_port[1]*256);
    TmpSocketInf.SourPort = ((u16)CFG->src_port[0]+(u16)CFG->src_port[1]*256);
    switch(CFG->type){
        case NET_MODULE_TYPE_TCP_S:
            TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
            sockFlag= NET_MODULE_TYPE_TCP_S;
            break;
        case NET_MODULE_TYPE_TCP_C:
            TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
            sockFlag= NET_MODULE_TYPE_TCP_C;
            break;
        case NET_MODULE_TYPE_UDP_S:
            TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
            TmpSocketInf.AppCallBack = WCHNET_UdpSRecv;
            sockFlag= NET_MODULE_TYPE_UDP_S;
            break;
        case NET_MODULE_TYPE_UDP_C:
            TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
            sockFlag= NET_MODULE_TYPE_UDP_C;
            break;
        default:
            break;
    }
    TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf[1];
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    mStopIfError(i);
    switch(CFG->type){
        case NET_MODULE_TYPE_TCP_S:
            WCHNET_SocketListen(SocketId);
            printf("listening\n");
            break;
        case NET_MODULE_TYPE_TCP_C:
            WCHNET_SocketConnect(SocketId);
            printf("connecting\n");
            break;
        default:
            break;
    }
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

    memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
    TmpSocketInf.SourPort = localPort;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf[0];
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;
    TmpSocketInf.AppCallBack = WCHNET_UdpServerRecv;
    i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
    printf("WCHNET_SocketCreat %d\r\n",SocketId);
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
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;       //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {    //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);        //send data
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);                                      //Clear sent data
    }
#else
    u32 len, totallen;
    u8 *p = MyBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %d\r\n", len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
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
void WCHNET_HandleSockInt(u8 socketid,u8 initstat)
{
    if(initstat & SINT_STAT_RECV)                                              //receive data
    {
        WCHNET_DataLoopback(socketid);                                         //Data loopback
    }
    if(initstat & SINT_STAT_CONNECT)                                           //connect successfully
    {
        if(sockFlag == NET_MODULE_TYPE_TCP_S){
          WCHNET_ModifyRecvBuf(socketid,(u32)SocketRecvBuf[socketid],RECE_BUF_LEN);
        }
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                        //disconnect
    {
        printf("TCP Disconnect\r\n");
        WCHNET_Create_Communication_Socket();
    }
    if(initstat & SINT_STAT_TIM_OUT)                                           //timeout disconnect
    {
       printf("TCP Timeout\r\n");
       WCHNET_Create_Communication_Socket();
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
 * @fn      GPIOInit
 *
 * @brief   GPIO initialize
 *
 * @return  none
 */
void GPIOInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

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
    USART_Printf_Init(115200);                                            //USART initialize
    GPIOInit();
    printf("ETH CFG Test\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n", WCHNET_GetVer());
    if(WCHNET_LIB_VER != WCHNET_GetVer()){
        printf("version error.\n");
    }
    /*After the button(PB6) is pressed, initialize WCHNET
     *  and execute the default configuration*/
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {
       Delay_Ms(100);
       if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0){
           while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0);
           CFG_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
           CFG_WRITE( PAGE_WRITE_START_ADDR,Default_cfg,MODULE_CFG_LEN );
           NVIC_SystemReset();
       }
    }

    CFG_READ( PAGE_WRITE_START_ADDR,Configbuf,MODULE_CFG_LEN );           //Read the configuration information saved in the EEPROM
    /*If the information stored in the EEPROM is invalid,
     * or the WCHNET has not been configured by the host
     * computer, then initialize the WCHNET according to
     * the default configuration*/
    if((CFG->cfg_flag[0]!=checkcode1)||(CFG->cfg_flag[1]!=checkcode2)){   //Check whether the information in the EEPROM is valid
        CFG_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
        CFG_WRITE(PAGE_WRITE_START_ADDR,Default_cfg,MODULE_CFG_LEN );
        NVIC_SystemReset();
    }

    WCHNET_GetMacAddr(MACAddr);                                           //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ", MACAddr[i]);
    printf("\n");
    TIM2_Init();
    memcpy(IPAddr,CFG->src_ip,sizeof(CFG->src_ip));
    memcpy(GWIPAddr,CFG->getway,sizeof(CFG->getway));
    memcpy(IPMask,CFG->mask,sizeof(CFG->mask));
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                      //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    WCHNET_CreateUdpSocket();                                             //Create UDP Socket
    if(CFG->type != NET_MODULE_TYPE_NONE){                                //No mode is enabled by default
        WCHNET_Create_Communication_Socket();                             //Create a socket configured by the host computer
    }

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

