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
 * 可在Tool Settings -> GNU RISC_V Cross Compiler -> Perprocessor 中添加 CH32V307_DEBUG 宏定义，打开调试开关（默认为串口1）
 * */

#include "debug.h"
#include "WCHNET.H"
#include "bsp_uart.h"


#ifdef  CH32V307_DEBUG
#define PRINT(X...) printf(X)
#else
#define PRINT(X...)
#endif

/* Global Variable */
 __attribute__((__aligned__(4)))ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];  /* send descriptor table */
 __attribute__((__aligned__(4)))ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];  /* receive descriptor table */
 __attribute__((__aligned__(4)))uint8_t  MACRxBuf[ETH_RXBUFNB*ETH_MAX_PACKET_SIZE];
 __attribute__((__aligned__(4)))uint8_t  MACTxBuf[ETH_TXBUFNB*ETH_MAX_PACKET_SIZE];



__attribute__((__aligned__(4))) SOCK_INF SocketInf[WCHNET_MAX_SOCKET_NUM];     /* Socket table，4 byte alignment */
const uint16_t MemNum[8] = {WCHNET_NUM_IPRAW,
                         WCHNET_NUM_UDP,
                         WCHNET_NUM_TCP,
                         WCHNET_NUM_TCP_LISTEN,
                         WCHNET_NUM_TCP_SEG,
                         WCHNET_NUM_IP_REASSDATA,
                         WCHNET_NUM_PBUF,
                         WCHNET_NUM_POOL_BUF
                         };
const uint16_t MemSize[8] = {  WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IPRAW_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_UDP_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB_LISTEN),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_SEG),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IP_REASSDATA),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(0),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_POOL_BUF)
                         };
 __attribute__((__aligned__(4)))uint8_t Memp_Memory[WCHNET_MEMP_SIZE];
 __attribute__((__aligned__(4)))uint8_t Mem_Heap_Memory[WCHNET_RAM_HEAP_SIZE];
 __attribute__((__aligned__(4)))uint8_t Mem_ArpTable[WCHNET_RAM_ARP_TABLE_SIZE];


#define RECE_BUF_LEN      3200

#define SOCKET_NUM  8
uint8_t SocketId[SOCKET_NUM];
uint8_t SocketRecvBuf[WCHNET_NUM_TCP][RECE_BUF_LEN];

uint8_t MACAddr[6] = {0x02,0x03,0x04,0x05,0x06,0x17};
uint8_t IPAddr[4] = {192,168,1,10};
uint8_t GWIPAddr[4] = {192,168,1,1};
uint8_t IPMask[4] = {255,255,255,0};
uint8_t DESIP[4] = {192,168,1,100};

uint16_t desport=5000;
uint16_t srcport=1000;

volatile uint8_t tcp_connect_num = 0;


/*********************************************************************
 * @fn      Ethernet_LED_Configuration
 *
 * @brief   set eth data and link led pin
 *
 * @return  none
 */
void Ethernet_LED_Configuration(void)
{
    GPIO_InitTypeDef  GPIO={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO);
    Ethernet_LED_LINKSET(1);
    Ethernet_LED_DATASET(1);
}


/*********************************************************************
 * @fn      Ethernet_LED_LINKSET
 *
 * @brief   set eth link led,setbit 0 or 1,the link led turn on or turn off
 *
 * @return  none
 */
void Ethernet_LED_LINKSET(uint8_t setbit)
{
     if(setbit){
         GPIO_SetBits(GPIOB, GPIO_Pin_8);
     }
     else {
         GPIO_ResetBits(GPIOB, GPIO_Pin_8);
    }
}


/*********************************************************************
 * @fn      Ethernet_LED_DATASET
 *
 * @brief   set eth data led,setbit 0 or 1,the data led turn on or turn off
 *
 * @return  none
 */
void Ethernet_LED_DATASET(uint8_t setbit)
{
     if(setbit){
         GPIO_SetBits(GPIOB, GPIO_Pin_9);
     }
     else {
         GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    }
}

/*********************************************************************
 * @fn      WCHNET_LibInit
 *
 * @brief   WCH NET_Lib init.
 *
 * @return  none
 */
uint8_t WCHNET_LibInit(const uint8_t *ip,const uint8_t *gwip,const uint8_t *mask,const uint8_t *macaddr)
{
	uint8_t i;
	struct _WCH_CFG  cfg;

	cfg.RxBufSize = RX_BUF_SIZE;
	cfg.TCPMss   = WCHNET_TCP_MSS;
	cfg.HeapSize = WCH_MEM_HEAP_SIZE;
	cfg.ARPTableNum = WCHNET_NUM_ARP_TABLE;
	cfg.MiscConfig0 = WCHNET_MISC_CONFIG0;
	WCHNET_ConfigLIB(&cfg);
	i = WCHNET_Init(ip,gwip,mask,macaddr);
	#ifdef  KEEPLIVE_ENABLE
		net_initkeeplive( );
	#endif
	return (i);
}

/*********************************************************************
 * @fn      SET_MCO
 *
 * @brief   Set MCO wave output.
 *
 * @return  none
 */
void SET_MCO(void)
{
    RCC_PLL3Cmd(DISABLE);
    RCC_PREDIV2Config(RCC_PREDIV2_Div2);
    RCC_PLL3Config(RCC_PLL3Mul_15);
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
    RCC_PLL3Cmd(ENABLE);
    Delay_Ms(100);
    while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLL3RDY))
    {
    	PRINT("Wait for PLL3 ready.\n");
        Delay_Ms(500);
    }
    RCC_AHBPeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}


/*********************************************************************
 * @fn      TIM1_Init
 *
 * @brief   Ethernet send counter.
 *
 * @return  none
 */
void TIM2_Init( void )
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 100-1;
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update ,ENABLE);

	TIM_Cmd(TIM2, ENABLE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
	NVIC_SetPriority(TIM2_IRQn, 0x80);
	NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreatTcpSocket
 *
 * @brief   creat TCP socket, and connect
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
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* set socekt type */
	TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* set receive buf length */

	for(uint8_t i=0;i<8;i++)
	{
	    TmpSocketInf.SourPort = srcport++;                                       /* set source port */
	    TmpSocketInf.RecvStartPoint = (uint32_t)&socket_revbuf[i];               /* set receive buf address */
	    //PRINT("add =%08x \n",socket_revbuf[i]);
	    temp = WCHNET_SocketCreat(&SocketId[tcp_connect_num],&TmpSocketInf);     /* creat socket */
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
void WCHNET_HandleSockInt(uint8_t sockeid,uint8_t initstat)
{
    uint32_t len;
    uint8_t write_buf = 0;
    int8_t receive_state = -1;

    if(initstat & SINT_STAT_RECV)                                               /* socket receive */
    {
        len = WCHNET_SocketRecvLen(sockeid,NULL);                               /* query length */
        if( (uart_data_t[sockeid].tx_write - uart_data_t[sockeid].tx_read) < UART_TX_BUF_NUM )
        {
        	write_buf = (uart_data_t[sockeid].tx_write)%UART_TX_BUF_NUM;
        	/* socket receive */
        	receive_state = WCHNET_SocketRecv(sockeid,uart_data_t[sockeid].TX_buffer[write_buf],&len);
        	if(receive_state == 0)
        	{
        		uart_data_t[sockeid].TX_data_length[write_buf] = len;

        		uart_data_t[sockeid].tx_write++;
        	}
        }
        else
        {
        	PRINT("eth %d receive buf busy\n",sockeid);
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
		Delay_Ms(200);

		PRINT("TCP Timout\r\n");
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
	initstat = WCHNET_GetGlobalInt();                         /* get golbal interrupt, and clear */

	if(initstat & GINT_STAT_UNREACH)                          /* unreach */
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
 * @brief   uart receive datas, and send these datas to eth
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

	RCC_GetClocksFreq(&RCC_ClocksStatus);
	SET_MCO();
	TIM2_Init();

	PRINT("mac:");
	WCH_GetMac(MACAddr);
	for(i=0;i<6;i++)
	{
		PRINT(" %02x",MACAddr[i]);
	}
	PRINT("\r\n");

	BSP_Uart_Init(); /*uart init*/

	WCHNET_SetHostname(name);
	i=WCHNET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);
	if(i != WCHNET_ERR_SUCCESS)
	{
		PRINT("WCH NET LIB INIT ERROR\n");
		while(1);
	}
	while(!(WCHNET_GetPHYStatus()&PHY_LINK_SUCCESS))   /*wait PHY connect success*/
	{
		Delay_Ms(100);
	}

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










