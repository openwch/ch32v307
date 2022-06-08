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
#include "ModuleConfig.h"
/*
 *@Note
ETH_CFG例程，创建一个UDP Server用于与上位机通信，以配置WCHNET功能，包括各项参数以及创建一个新的通信.
*/

__attribute__((aligned(4))) u8 Default_cfg[MODULE_CFG_LEN]= {                  /*模块默认配置*/
  'W','C','H','N','E','T','M','O','D','U','L','E',0,0,0,0,0,0,0,0,0,           /*模块名称*/
  NET_MODULE_TYPE_NONE,                                                        /*标识模块处于默认模式(默认不开启任何模式)*/
  192,168,1,10,                                                                /*模块本身的IP地址  */
  255,255,255,0,                                                               /*模块本身的子网掩码 */
  192,168,1,1,                                                                 /*模块对应的网关地址 */
  1000%256,1000/256,                                                           /*模块源端口 */
  192,168,1,100,                                                               /*目的IP地址 */
  1000%256,1000/256 ,                                                          /*目的端口  */
  checkcode1,checkcode2                                                        /*验证码，用于验证配置信息*/
};

u8 Configbuf[MODULE_CFG_LEN];
pmodule_cfg CFG = (pmodule_cfg)Configbuf;
u8 socket_flag;

u8 MACAddr[6];                                                                 /*MAC地址*/
u8 IPAddr[4];                                                                  /*IP地址*/
u8 GWIPAddr[4];                                                                /*网关*/
u8 IPMask[4];                                                                  /*子网掩码*/
u8 DESIP[4];                                                                   /*目的IP地址*/

u8 SocketId;                                                                   /*socket id号*/
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];                        /*socket缓冲区*/
u8 MyBuf[RECE_BUF_LEN];

/*上位机通信参数*/
u8  brocastIp[4] = {255,255,255,255};                                          /*广播IP地址，不可改*/
u16 brocastPort = 60000;                                                       /*上位机通讯端口号*/
u16 localPort = 50000;                                                         /*本地通信端口号*/

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;                                  /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                                    /* 显示错误 */
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
 * @fn      WCHNET_UdpServerRecv
 *
 * @brief   UDP Receive data function
 *
 * @return  none
 */
void WCHNET_UdpServerRecv(struct _SCOK_INF *socinf,u32 ipaddr,u16 port,u8 *buf,u32 len)
{
    u8 i;

    i = ParseConfigbuf(buf) ;                                                    /*解析数据*/
    if(i){
        NVIC_SystemReset();
    }
}

void WCHNET_UdpSRecv(struct _SCOK_INF *socinf,u32 ipaddr,u16 port,u8 *buf,u32 len)
{
  u8 ip_addr[4],i;

  printf("ip:");
  for(i=0;i<4;i++){
    ip_addr[i]=ipaddr&0xff;
    printf("%-4d",ip_addr[i]);
    ipaddr=ipaddr>>8;
  }

  printf("port=%-8d  len=%-8d  sockeid=%-4d\n",port,len,socinf->SockIndex);
  WCHNET_SocketUdpSendTo(socinf->SockIndex,buf,&len,ip_addr,port);
}

void WCHNET_Creat_Communication_Socket(void)
{
   u8 i;
   u8 *desip = CFG->dest_ip;                                                    /* 目的IP地址 */
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,desip,4);                                 /* 设置目的IP地址 */
   TmpSocketInf.DesPort = ((u16)CFG->dest_port[0]+(u16)CFG->dest_port[1]*256);  /* 设置目的端口 */
   TmpSocketInf.SourPort = ((u16)CFG->src_port[0]+(u16)CFG->src_port[1]*256);   /* 设置源端口 */
   switch(CFG->type){
     case NET_MODULE_TYPE_TCP_S:
          TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                              /* 设置socket类型 */
          socket_flag= NET_MODULE_TYPE_TCP_S;
          break;
     case NET_MODULE_TYPE_TCP_C:
          TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
          socket_flag= NET_MODULE_TYPE_TCP_C;
          break;
     case NET_MODULE_TYPE_UDP_S:
          TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
          TmpSocketInf.AppCallBack = WCHNET_UdpSRecv;
          socket_flag= NET_MODULE_TYPE_UDP_S;
          break;
     case NET_MODULE_TYPE_UDP_C:
          TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
          socket_flag= NET_MODULE_TYPE_UDP_C;
          break;
     default:
          break;
   }
   TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf[1];                         /* 设置接收缓冲区的接收缓冲区 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 */
   mStopIfError(i);                                                             /* 检查错误 */
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
 * @fn      WCHNET_CreatUdpSocket
 *
 * @brief   Create UDP Socket
 *
 * @return  none
 */
void WCHNET_CreatUdpSocket(void)
{
   u8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   TmpSocketInf.SourPort = localPort;                                           /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_UDP;                                     /* 设置socket类型 */
   TmpSocketInf.RecvStartPoint = (u32)SocketRecvBuf[0];                         /* 设置接收缓冲区地址 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* 设置接收缓冲区的接收长度 */
   TmpSocketInf.AppCallBack = WCHNET_UdpServerRecv;                             /* UDP接收回调函数 ,UDP的接收在回调函数中处理*/
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 */
   printf("WCHNET_SocketCreat %d\r\n",SocketId);
   mStopIfError(i);                                                             /* 检查错误 */
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
    u32 totallen;
    u8 *p = MyBuf;

    if(initstat & SINT_STAT_RECV)                                                 /* socket接收中断*/
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);                                /* 查询长度 */
        printf("Receive Len = %02x\n",len);
        totallen = len;
        WCHNET_SocketRecv(socketid,MyBuf,&len);                                   /* 将接收缓冲区的数据读到MyBuf中*/
        while(1){
           len = totallen;
           WCHNET_SocketSend(socketid,p,&len);                                    /* 将MyBuf中的数据发送 */
           totallen -= len;                                                       /* 将总长度减去以及发送完毕的长度 */
           p += len;                                                              /* 将缓冲区指针偏移*/
           if(totallen)continue;                                                  /* 如果数据未发送完毕，则继续发送*/
           break;                                                                 /* 发送完毕，退出 */
        }
    }
    if(initstat & SINT_STAT_CONNECT)                                              /* socket连接成功中断*/
    {
        if(socket_flag==NET_MODULE_TYPE_TCP_S){
          WCHNET_ModifyRecvBuf(socketid,(u32)SocketRecvBuf[socketid],RECE_BUF_LEN);
        }
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                           /* socket连接断开中断*/
    {
        printf("TCP Disconnect\r\n");
        Delay_Ms(200);
        WCHNET_Creat_Communication_Socket();
    }
    if(initstat & SINT_STAT_TIM_OUT)                                              /* socket连接超时中断*/
    {
       printf("TCP Timeout\r\n");
       WCHNET_Creat_Communication_Socket();
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

    initstat = WCHNET_GetGlobalInt();                                           /* 获取全局中断标志*/
    if(initstat & GINT_STAT_UNREACH)                                            /* 不可达中断 */
    {
       printf("GINT_STAT_UNREACH\r\n");
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP冲突中断 */
   {
       printf("GINT_STAT_IP_CONFLI\r\n");
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY状态变化中断 */
   {
       i = WCHNET_GetPHYStatus();                                               /* 获取PHY连接状态*/
       if(i&PHY_Linked_Status)
       printf("PHY Link Success\r\n");
   }
   if(initstat & GINT_STAT_SOCKET)
   {
       for(i = 0; i < WCHNET_MAX_SOCKET_NUM; i ++)
       {
           socketinit = WCHNET_GetSocketInt(i);
           if(socketinit)WCHNET_HandleSockInt(i,socketinit);
       }
   }
}

void gpioInit(void)
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

	Delay_Init();
	USART_Printf_Init(115200);                                                  /*串口打印初始化*/
    gpioInit();
	printf("ETH_CFG\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }

    EEPROM_READ( PAGE_WRITE_START_ADDR,Configbuf,MODULE_CFG_LEN );              /* 读取EEPROM里保存的配置信息 */
    if(CFG->cfg_flag[0]!=checkcode1||CFG->cfg_flag[1]!=checkcode2){             /*校验EEPROM里的信息是否合法*/                                                                               /*若EEPROM里保存的信息不合法，或563还未经过上位机配置,那么就按照默认配置来初始化563*/
        EEPROM_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
        EEPROM_WRITE(PAGE_WRITE_START_ADDR,Default_cfg,MODULE_CFG_LEN );
        NVIC_SystemReset();
    }

    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) {                         /*按键按下后，初始化WCHNET，执行默认配置*/
       Delay_Ms(100);
       if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0){
           while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0);
           EEPROM_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
           EEPROM_WRITE( PAGE_WRITE_START_ADDR,Default_cfg,MODULE_CFG_LEN );
           NVIC_SystemReset();
       }
    }

    WCHNET_GetMacAddr(MACAddr);                                                 /*获取芯片MAC地址*/
    printf("mac addr:");
    for(int i=0;i<6;i++) printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    memcpy(IPAddr,CFG->src_ip,sizeof(CFG->src_ip));
    memcpy(GWIPAddr,CFG->getway,sizeof(CFG->getway));
    memcpy(IPMask,CFG->mask,sizeof(CFG->mask));
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                            /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");
    WCHNET_CreatUdpSocket();                                                    /*创建UDP socket*/
    if(CFG->type != NET_MODULE_TYPE_NONE){                                      /*默认配置下不开启任何模式*/
        WCHNET_Creat_Communication_Socket();                                    /*创建上位机配置模式下的socket*/
    }

	while(1)
	{
	  WCHNET_MainTask();                                                        /*以太网库主任务函数，需要循环调用*/
	  if(WCHNET_QueryGlobalInt())                                               /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
	  {
         WCHNET_HandleGlobalInt();
	  }
    }
}

