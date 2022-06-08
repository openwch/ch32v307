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
#include "HTTPS.h"
/*
 *@Note
 Web Server例程，本程序用于演示通过Web浏览器配置WCHNET芯片的功能，WCHNET芯片内置web服务器 ，
                                                通过网页可以实现WCHNET的网络参数配置，以及密码管理
 */
u8 MACAddr[6];                                                  /* MAC地址 */
u8 IPAddr[4];                                                   /* IP地址 */
u8 GWIPAddr[4];                                                 /* 网关 */
u8 IPMask[4];                                                   /* 子网掩码 */

/* 常用变量定义 */
u8 SocketId, SocketIdForListen, RecvBuffer[RECE_BUF_LEN];
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];          /* socket接收缓冲区 */
u8 flag = 0;
u16 DESPORT, SRCPORT;
/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError) {
    if (iError == WCHNET_ERR_SUCCESS)
        return;                                                 /* 操作成功 */
    printf("Error: %02X\r\n", (u16) iError);                    /* 显示错误 */
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

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
 * @fn      WCHNET_CreatTcpSocketListen
 *
 * @brief   Create TCP Socket for Listening
 *
 * @return  none
 */
void WCHNET_CreatTcpSocketListen(void) {
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
    TmpSocketInf.SourPort = HTTP_SERVER_PORT;                       /* 设置源端口 */
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                        /* 设置socket类型 */
    i = WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);      /* 配置socket，将返回的socket索引保存在SocketIdForListen中*/
    printf("SocketIdForListen %d\r\n", SocketIdForListen);
    mStopIfError(i);                                                /* 检查错误 */
    i = WCHNET_SocketListen(SocketIdForListen);                     /* TCP监听 */
    mStopIfError(i);                                                /* 检查错误 */
}

void WCHNET_CreatCfgSocket(u8 mode, u8 *Desip, u16 Desport, u16 Srcport) /*根据网页的配置信息，WCHNET建立相应的socket*/
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

void WCHNET_RestoreDefaults()                                       /*WCHNET恢复全部出厂设置*/
{
    EEPROM_ERASE( PAGE_WRITE_START_ADDR, FLASH_PAGE_SIZE * 3);
    EEPROM_WRITE( BASIC_CFG_ADDR, Basic_Default, BASIC_CFG_LEN);
    EEPROM_WRITE( PORT_CFG_ADDR, Port_Default, PORT_CFG_LEN);
    EEPROM_WRITE( LOGIN_CFG_ADDR, Login_Default, LOGIN_CFG_LEN);
    NVIC_SystemReset();
}
/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 initstat) {
    u32 len;

    if (initstat & SINT_STAT_RECV)                                  /* socket接收中断*/
    {
        len = WCHNET_SocketRecvLen(socketid, NULL);                 /* 获取socket缓冲区数据长度  */
        printf("WCHNET_SocketRecvLen %d  socket id %d\r\n", len, socketid);
        if (len) {
            WCHNET_SocketRecv(socketid, RecvBuffer, &len);          /* 将接收到的浏览器请求保存在RecvBuffer[]里*/
            flag = 1;
            socket = socketid;
        }
    }
    if (initstat & SINT_STAT_CONNECT)                               /* socket连接成功中断*/
    {
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if (initstat & SINT_STAT_DISCONNECT)                            /* socket连接断开中断*/
    {
        printf("TCP Disconnect\r\n");
    }
    if (initstat & SINT_STAT_TIM_OUT)                               /* socket连接超时中断*/
    {
        printf("TCP Timeout\r\n");
        WCHNET_CreatCfgSocket(Port_CfgBuf->mode, Port_CfgBuf->des_ip, DESPORT, SRCPORT);
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void) {
    u8 initstat;
    u16 i;
    u8 socketinit;

    initstat = WCHNET_GetGlobalInt();                               /* 获取全局中断标志*/
    if (initstat & GINT_STAT_UNREACH)                               /* 不可达中断 */
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (initstat & GINT_STAT_IP_CONFLI)                             /* IP冲突中断 */
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (initstat & GINT_STAT_PHY_CHANGE)                            /* PHY状态变化中断 */
    {
        i = WCHNET_GetPHYStatus();                                  /* 获取PHY连接状态*/
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (initstat & GINT_STAT_SOCKET) {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketinit = WCHNET_GetSocketInt(i);
            if (socketinit)
                WCHNET_HandleSockInt(i, socketinit);
        }
    }
}

/*********************************************************************
 * @fn      gpioInit
 *
 * @brief   GPIO initialization
 *
 * @return  none
 */
void gpioInit(void) {
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
int main(void) {
    u8 i;
    Delay_Init();
    USART_Printf_Init(115200);                                                  /*串口打印初始化*/
    gpioInit();
    printf("WEB SERVER\r\n");
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("net version:%x\n", WCHNET_GetVer());
    if ( WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {                        /*按键按下后，初始化WCHNET，执行默认配置*/
        Delay_Ms(100);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0);
            WCHNET_RestoreDefaults();
        }
    }
    EEPROM_READ( BASIC_CFG_ADDR, (u8 *)Basic_CfgBuf, BASIC_CFG_LEN );           /*从EEPROM里读取配置信息*/
    EEPROM_READ( PORT_CFG_ADDR, (u8 *)Port_CfgBuf, PORT_CFG_LEN );
    EEPROM_READ( LOGIN_CFG_ADDR, (u8 *)Login_CfgBuf, LOGIN_CFG_LEN );
    if (Basic_CfgBuf->flag[0] != 0x57 || Basic_CfgBuf->flag[1] != 0xAB)         /*判断网络配置信息标志位，若判断为错误，则说明，EEPROM里保存的配置信息错误，恢复出厂配置*/
        WCHNET_RestoreDefaults();
    else {
        if (Port_CfgBuf->flag[0] != 0x57 || Port_CfgBuf->flag[1] != 0xAB)     /*判断密码配置信息，同上*/
            WCHNET_RestoreDefaults();
        else {
            if (Login_CfgBuf->flag[0] != 0x57 || Login_CfgBuf->flag[1] != 0xAB) /*判断密码配置信息，同上*/
                WCHNET_RestoreDefaults();
        }
    }
    memcpy(MACAddr, Basic_CfgBuf->mac, 6);
    memcpy(IPAddr, Basic_CfgBuf->ip, 4);
    memcpy(IPMask, Basic_CfgBuf->mask, 4);
    memcpy(GWIPAddr, Basic_CfgBuf->gateway, 4);
    printf("ip:\n");
    for (i = 0; i < 4; i++)
        printf("%d.", IPAddr[i]);
    printf("\n");

    WCHNET_GetMacAddr(MACAddr);                                                 /*获取芯片MAC地址*/
    printf("mac addr:");
    for (int i = 0; i < 6; i++)
        printf("%x ", MACAddr[i]);
    printf("\n");
    http_request = (st_http_request*) RecvBuffer;

    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);                         /*以太网库初始化*/
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
    WCHNET_CreatTcpSocketListen();                                              /*创建TCP socket*/

    DESPORT = Port_CfgBuf->des_port[0] * 256 + Port_CfgBuf->des_port[1];
    SRCPORT = Port_CfgBuf->src_port[0] * 256 + Port_CfgBuf->src_port[1];
    WCHNET_CreatCfgSocket(Port_CfgBuf->mode, Port_CfgBuf->des_ip, DESPORT, SRCPORT);
    Init_Para_Tab();

    while(1)
    {
        WCHNET_MainTask();                                                      /*以太网库主任务函数，需要循环调用*/
        if(WCHNET_QueryGlobalInt())                                             /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
        {
            WCHNET_HandleGlobalInt();
        }
        Web_Server();
    }
}

