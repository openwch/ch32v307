/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/16
* Description        : Main program body.
* Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 * @note
 * 基于CH32V307实现8串口网络服务器
 * 默认波特率为115200
 * 网络处于TCP client模式，目的IP为192.168.1.100，目的端口号为5000
 *
 * uart1 <-> socket0  |  uart2 <-> socket1 | ... | uart8 <-> socket7
 *
 * 该例程中，CH32V307 的RAM配置为128KB，Flash配置为192KB（使用WCHISPTool工具进行配置）
 *
 * 可在Tool Settings -> GNU RISC_V Cross Compiler -> Preprocessor 中添加 CH32V307_DEBUG 宏定义，打开调试开关（默认为串口1）
 * */
#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "bsp_uart.h"

#ifdef  CH32V307_DEBUG
#define PRINT(X...) printf(X)
#else
#define PRINT(X...)
#endif

#define SOCKET_NUM  8
uint8_t SocketId[SOCKET_NUM];
uint8_t SocketRecvBuf[WCHNET_NUM_TCP][RECE_BUF_LEN];

uint8_t MACAddr[6]  = {0x02,0x03,0x04,0x05,0x06,0x17};
uint8_t IPAddr[4]   = {192,168,1,10};
uint8_t GWIPAddr[4] = {192,168,1,1};
uint8_t IPMask[4]   = {255,255,255,0};
uint8_t DESIP[4]    = {192,168,1,100};

uint16_t desport = 5000;
uint16_t srcport = 1000;

volatile uint8_t tcp_connect_num = 0;

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;                                   /* 操作成功 */
    printf("Error: %02X\r\n", (u16)iError);                                     /* 显示错误 */
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Ethernet send counter.
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
 * @brief   create TCP socket, and connect
 *
 * @return  none
 */
void WCHNET_CreatTcpSocket(uint8_t *socket_revbuf)
{
	uint8_t temp;

	SOCK_INF TmpSocketInf;

	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
	memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                                 /* set the destination IP  */
	TmpSocketInf.DesPort  = desport;                                             /* set the destination port */
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* set socket type */
	TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* set receive buff length */

	for(uint8_t i=0;i<8;i++)
	{
	    TmpSocketInf.SourPort = srcport++;                                       /* set source port */
	    TmpSocketInf.RecvStartPoint = (uint32_t)&socket_revbuf[i*RECE_BUF_LEN];  /* set receive buff address */
	    temp = WCHNET_SocketCreat(&SocketId[tcp_connect_num],&TmpSocketInf);     /* create socket */
	    //PRINT("WCHNET_SocketCreat  %x\r\n",SocketId) ;
		WCHNET_SocketConnect(SocketId[tcp_connect_num]);                         /* TCP connect */
	}

}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleSockInt(uint8_t socketid,uint8_t initstat)
{
    uint32_t len;
    uint8_t write_buf = 0;
    int8_t receive_state = -1;

    if(initstat & SINT_STAT_RECV)                                               /* socket receive */
    {
        len = WCHNET_SocketRecvLen(socketid,NULL);                               /* query length */
        if( (uart_data_t[socketid].tx_write - uart_data_t[socketid].tx_read) < UART_TX_BUF_NUM )
        {
        	write_buf = (uart_data_t[socketid].tx_write)%UART_TX_BUF_NUM;
        	/* socket receive */
        	receive_state = WCHNET_SocketRecv(socketid,uart_data_t[socketid].TX_buffer[write_buf],&len);
        	if(receive_state == 0)
        	{
        		uart_data_t[socketid].TX_data_length[write_buf] = len;

        		uart_data_t[socketid].tx_write++;
        	}
        }
        else
        {
        	PRINT("eth %d receive buff busy\n",socketid);
		}
    }
    if(initstat & SINT_STAT_CONNECT)                         /* TCP connect */
    {

    	PRINT("TCP Connect Success\n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                      /* TCP disconnect  */
    {
		PRINT("TCP Disconnect\r\n");
    }
    if(initstat & SINT_STAT_TIM_OUT)                         /* TCP time out */
    {
		PRINT("TCP Timeout\r\n");
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
	uint8_t initstat;
	uint16_t i;
	uint8_t socketinit;
	initstat = WCHNET_GetGlobalInt();                         /* get global interrupt, and clear */

	if(initstat & GINT_STAT_UNREACH)                          /* unreachable */
	{
		PRINT("GINT_STAT_UNREACH\r\n");
	}

	if(initstat & GINT_STAT_IP_CONFLI)                        /* IP conflict */
	{
		PRINT("GINT_STAT_IP_CONFLI\r\n");
	}

	if(initstat & GINT_STAT_PHY_CHANGE)                       /* PHY change */
	{
		i = WCHNET_GetPHYStatus();                            /*get PHY state*/
		if(i&PHY_Linked_Status)
		PRINT("PHY_Linked_Status\r\n");
		PRINT("GINT_STAT_PHY_CHANGE %x\n",i);
	}

	if(initstat & GINT_STAT_SOCKET)                           /*Socket interrupt*/
	{
		for(i = 0; i < SOCKET_NUM; i++)
		{
			socketinit = WCHNET_GetSocketInt(i);              /* get Socket interrupt*/
			if(socketinit)
			{
				WCHNET_HandleSockInt(i,socketinit);           /* IQR */
			}
		}
	}
}

/*********************************************************************
 * @fn      uart_tx
 *
 * @brief   send data from TX_buffer by uart tx dma
 *
 * @return  none
 */
void uart_tx(void)
{
	uint16_t read_buf = 0;

	for(uint8_t i=0;i<8;i++)
	{
		/* uart tx dma is idel */
		if(uart_data_t[i].uart_tx_dma_state == IDEL)
		{
			/* eth has received data  */
			if(uart_data_t[i].tx_read != uart_data_t[i].tx_write)
			{
				read_buf = (uart_data_t[i].tx_read)%UART_TX_BUF_NUM;
				switch(i)
				{
#ifndef CH32V307_DEBUG
					/*uart1*/
					case 0:
						/* update uart tx dma address */
						DMA1_Channel4->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];

						/* update uart tx dma length */
						DMA1_Channel4->CNTR = uart_data_t[i].TX_data_length[read_buf];

						/* set uart tx dma state busy */
						uart_data_t[i].uart_tx_dma_state = BUSY;

						/* enable uart tx dma */
						DMA_Cmd(DMA1_Channel4,ENABLE);
					break;
#endif
					/*uart2*/
					case 1:
						DMA1_Channel7->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA1_Channel7->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA1_Channel7,ENABLE);
					break;

					/*uart3*/
					case 2:
						DMA1_Channel2->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];;
						DMA1_Channel2->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA1_Channel2,ENABLE);
					break;

					/*uart4*/
					case 3:
						DMA2_Channel5->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];;
						DMA2_Channel5->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel5,ENABLE);
					break;

					/*uart5*/
					case 4:
						DMA2_Channel4->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];;
						DMA2_Channel4->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel4,ENABLE);
					break;

					/*uart6*/
					case 5:
						DMA2_Channel6->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel6->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel6,ENABLE);
					break;

					/*uart7*/
					case 6:
						DMA2_Channel8->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel8->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel8,ENABLE);
					break;

					/*uart8*/
					case 7:
						DMA2_Channel10->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel10->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel10,ENABLE);
					break;

					default:
						break;
				}
			}
		}
	}
}

/*********************************************************************
 * @fn      uart_tx
 *
 * @brief   uart receive data, and send these data to eth
 *
 * @return  none
 */
void uart_rx(void)
{
	uint32_t temp = 0;
	int ret = -1;

	uint32_t len = 0;
	uint32_t len_1 = 0;

	for(uint8_t i=0;i<8;i++)
	{
		switch(i)
		{
#ifndef CH32V307_DEBUG
			/*uart1*/
			case 0:
				temp = DMA1_Channel5->CNTR;
			break;
#endif
			/*uart2*/
			case 1:
				temp = DMA1_Channel6->CNTR;
			break;

			/*uart3*/
			case 2:
				temp = DMA1_Channel3->CNTR;
			break;

			/*uart4*/
			case 3:
				temp = DMA2_Channel3->CNTR;
			break;

			/*uart5*/
			case 4:
				temp = DMA2_Channel2->CNTR;
			break;

			/*uart6*/
			case 5:
				temp = DMA2_Channel7->CNTR;
			break;

			/*uart7*/
			case 6:
				temp = DMA2_Channel9->CNTR;
			break;

			/*uart8*/
			case 7:
				temp = DMA2_Channel11->CNTR;
			break;
			default:
				break;
		}

		/* uart rx dma CNTR not equal to last, indicating that data is received*/
		if(temp!=uart_data_t[i].last_RX_DMA_length)
		{


			/* calculate the length of the received data */
			uart_data_t[i].rx_write += (uart_data_t[i].last_RX_DMA_length - temp) & (UART_RX_DMA_SIZE - 1);

			/* update last rx dma CNTR */
			uart_data_t[i].last_RX_DMA_length = temp;

			/*calculate the length of the RX_buffer */
			len = uart_data_t[i].rx_write - uart_data_t[i].rx_read;

			/* RX_buffer is full */
			if(len > UART_RX_DMA_SIZE)
			{
				PRINT("uart%d RX DMA Buf is full \n",i);
				len = 0;
				uart_data_t[i].rx_read = 0;
				uart_data_t[i].rx_write = 0;
				uart_data_t[i].last_RX_DMA_length = 0;
			}

			/* send data starting from rx_read to RX_buffer end */
			len_1 = MIN(len,UART_RX_DMA_SIZE-(uart_data_t[i].rx_read&(UART_RX_DMA_SIZE-1)));

            if(len_1 != 0)
            {
                ret = WCHNET_SocketSend(i, &uart_data_t[i].RX_buffer[uart_data_t[i].rx_read&(UART_RX_DMA_SIZE-1)], &len_1);
                if (ret == 0)
                {
                    uart_data_t[i].rx_read += len_1;
                }
            }

            /* send the rest of the data(if any) at beginning of RX_buffer  */
            if( (len-len_1) != 0)
            {
                len_1 = len-len_1;
                ret = WCHNET_SocketSend(i, &uart_data_t[i].RX_buffer[0], &len_1);
                if (ret == 0)
                {
                    uart_data_t[i].rx_read += len_1;
                }
            }
		}
	}

}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	RCC_ClocksTypeDef RCC_ClocksStatus;
	uint8_t i;
	uint32_t len;
	uint8_t *name="ch32v307";


	Delay_Init();

#ifdef CH32V307_DEBUG
	USART_Printf_Init(256000);
#endif

	PRINT("SystemClk:%d\r\n",SystemCoreClock);

	RCC_GetClocksFreq(&RCC_ClocksStatus);
	PRINT("SystemClk:%d\r\n",SystemCoreClock);
	PRINT("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() )
    {
        PRINT("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                          /*获取芯片Mac地址*/
    PRINT("mac addr:");
    for(int i=0;i<6;i++) PRINT("%x ",MACAddr[i]);
    PRINT("\n");
    TIM2_Init();
    BSP_Uart_Init(); /*uart init*/
    WCHNET_DHCPSetHostname(name);                        /*设置DHCP主机名称*/
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);     /*以太网库初始化*/
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) PRINT("WCHNET_LibInit Success\r\n");
	WCHNET_CreatTcpSocket(&SocketRecvBuf[0]);

	while(1)
	{
		WCHNET_MainTask();

		if(WCHNET_QueryGlobalInt())
		{
			WCHNET_HandleGlobalInt();
		}
		uart_tx();
		uart_rx();
	}
}










