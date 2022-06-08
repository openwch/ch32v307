/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/10
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "bsp_uart.h"
/*
 *@Note
ETH_UART例程，演示以太网与UART的数据透传。默认使用115200波特率（可在bsp_uart.h中更改）进行串口数据传输。
因串口传输速度较慢，ETH的数据发送速度不宜过快，否则会出现丢包现象。
*/

u8 MACAddr[6];                                                                            /*MAC地址*/
u8 IPAddr[4]   = {192,168,1,10};                                                          /*IP地址*/
u8 GWIPAddr[4] = {192,168,1,1};                                                           /*网关*/
u8 IPMask[4]   = {255,255,255,0};                                                         /*子网掩码*/
u8 DESIP[4]    = {192,168,1,100};                                                         /*目的IP地址*/

u8 SocketId;                                                                              /*socket id号*/
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];                                    /*socket缓冲区*/
u8 MyBuf[RECE_BUF_LEN];
u16 desport = 1000;                                                                       /*目的端口号*/
u16 srcport = 1000;                                                                       /*源端口号*/

u8 sendDataFlag = 0; //0 无效   1 有效
u8 tcpConnValid = 0; //0 断连   1 连接
/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;                                             /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                                               /* 显示错误 */
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
 * @fn      WCHNET_CreatTcpSocket
 *
 * @brief   Create TCP Socket
 *
 * @return  none
 */
void WCHNET_CreatTcpSocket(void)
{
   u8 i;
   SOCK_INF TmpSocketInf;                                                       /* 创建临时socket变量 */

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
   memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                                 /* 设置目的IP地址 */
   TmpSocketInf.DesPort  = desport;                                             /* 设置目的端口 */
   TmpSocketInf.SourPort = srcport;                                             /* 设置源端口 */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* 设置socket类型 */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN;                                      /* 设置接收缓冲区的接收长度 */
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);                             /* 创建socket，将返回的socket索引保存在SocketId中 ，从零开始分配*/
   printf("WCHNET_SocketCreat %d\r\n",SocketId);
   mStopIfError(i);                                                             /* 检查错误 */
   i = WCHNET_SocketConnect(SocketId);                                          /* TCP连接 */
   mStopIfError(i);                                                             /* 检查错误 */
}


/*********************************************************************
 * @fn      uartRxAndSendDataToETH
 *
 * @brief   uart receive data, and send these data to eth
 *
 * @return  none
 */
void uartRxAndSendDataToETH(void)
{
    uint32_t temp = 0;
    int ret = -1;

    uint32_t len = 0;
    uint32_t len_1 = 0;

    if(!sendDataFlag) return;

    temp = DMA1_Channel6->CNTR;

    /* uart rx dma CNTR not equal to last, indicating that data is received*/
    if(temp != uart_data_t.last_RX_DMA_length)
    {
        /* calculate the length of the received data */
        uart_data_t.rx_write += (uart_data_t.last_RX_DMA_length - temp) & (UART_RX_DMA_SIZE - 1);

        /* update last rx dma CNTR */
        uart_data_t.last_RX_DMA_length = temp;

        /*calculate the length of the RX_buffer */
        len = uart_data_t.rx_write - uart_data_t.rx_read;

        /* RX_buffer is full */
        if(len > UART_RX_DMA_SIZE)
        {
            printf("uart2 RX DMA Buff is full \n");
            len = 0;
            uart_data_t.rx_read = 0;
            uart_data_t.rx_write = 0;
            uart_data_t.last_RX_DMA_length = 0;
        }

        /* send data starting from rx_read to RX_buffer end */
        len_1 = MIN(len,UART_RX_DMA_SIZE-(uart_data_t.rx_read&(UART_RX_DMA_SIZE-1)));

        if(len_1 != 0)
        {
            ret = WCHNET_SocketSend(0, &uart_data_t.RX_buffer[uart_data_t.rx_read&(UART_RX_DMA_SIZE-1)], &len_1);
            if (ret == 0)
            {
                uart_data_t.rx_read += len_1;
            }
        }

        /* send the rest of the data(if any) at beginning of RX_buffer  */
        if( (len-len_1) != 0)
        {
            len_1 = len-len_1;
            ret = WCHNET_SocketSend(0, &uart_data_t.RX_buffer[0], &len_1);
            if (ret == 0)
            {
                uart_data_t.rx_read += len_1;
            }
        }
    }

}

/*********************************************************************
 * @fn      uartTx
 *
 * @brief   send data from TX_buffer by uart tx dma
 *
 * @return  none
 */
void uartTx(void)
{
    uint16_t read_buf = 0;

    /* uart tx dma is ideal */
    if(uart_data_t.uart_tx_dma_state == IDEL)
    {
        /* eth has received data  */
        if(uart_data_t.tx_read != uart_data_t.tx_write)
        {
            read_buf = (uart_data_t.tx_read)%UART_TX_BUF_NUM;
            DMA1_Channel7->MADDR = (uint32_t) &uart_data_t.TX_buffer[read_buf];
            DMA1_Channel7->CNTR = uart_data_t.TX_data_length[read_buf];
            uart_data_t.uart_tx_dma_state = BUSY;
            DMA_Cmd(DMA1_Channel7,ENABLE);
        }
    }
}

/*********************************************************************
 * @fn      ETHRx
 *
 * @brief   ETH receive data and save it to buff
 *
 * @return  none
 */
void ETHRx(u8 socketid)
{
    u32 len;
    uint8_t write_buf = 0;
    int8_t receive_state = -1;

    len = WCHNET_SocketRecvLen(socketid,NULL);                               /* query length */

    if( (uart_data_t.tx_write - uart_data_t.tx_read) < UART_TX_BUF_NUM )
    {
        write_buf = (uart_data_t.tx_write)%UART_TX_BUF_NUM;
        /* socket receive */
        receive_state = WCHNET_SocketRecv(socketid,uart_data_t.TX_buffer[write_buf],&len);
        if(receive_state == 0)
        {
            uart_data_t.TX_data_length[write_buf] = len;

            uart_data_t.tx_write++;
        }
    }
    else
    {
        printf("eth receive buff busy\n");
    }
    uartTx();
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
    if(initstat & SINT_STAT_RECV)                                                /* socket接收中断*/
    {
        ETHRx(socketid);
    }
    if(initstat & SINT_STAT_CONNECT)                                             /* socket连接成功中断*/
    {
        tcpConnValid = 1;
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                          /* socket连接断开中断*/
    {
        tcpConnValid = 0;
        printf("TCP Disconnect\r\n");
    }
    if(initstat & SINT_STAT_TIM_OUT)                                             /* socket连接超时中断*/
    {
        tcpConnValid = 0;
        printf("TCP Timeout\r\n");
        WCHNET_SocketConnect(socketid);                                          /* TCP连接 */
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
	USART_Printf_Init(115200);                                              /*串口打印初始化*/
	printf("ETH_UART\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);                             /*系统主频*/
    printf("net version:%x\n",WCHNET_GetVer());                             /*库版本号*/
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                             /*获取芯片MAC地址*/
    printf("mac addr:");
    for(int i=0;i<6;i++) printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    BSP_Uart_Init();                                                        /*uart init*/
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                        /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");

    WCHNET_CreatTcpSocket();                                                /*创建TCP socket*/

	while(1)
	{
        WCHNET_MainTask();                                                  /*以太网库主任务函数，需要循环调用*/
        if(WCHNET_QueryGlobalInt())                                         /*查询以太网全局中断，如果有中断，调用全局中断处理函数*/
        {
            WCHNET_HandleGlobalInt();
        }
        if(tcpConnValid){
            uartTx();
            uartRxAndSendDataToETH();
        }
    }
}
