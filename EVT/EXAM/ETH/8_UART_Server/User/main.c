/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/16
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * @note
 * Based on CH32V307 to implement 8 serial port network server
 * The default baud rate is 921600
 *
 * note:Due to pin multiplexing, this example only supports 10M networks.
 *
 * The network is in TCP client mode, the destination IP address is 192.168.1.100,
 * and the destination port number is 1000
 *
 * uart1 <-> socket0  |  uart2 <-> socket1 | ... | uart8 <-> socket7
 *
 * In this example, the RAM configuration of CH32V307 is 128KB and the Flash
 * configuration is 192KB (configured using the WCHISPTool tool)
 *
 * CH32V307_DEBUG macro definitions can be added in Tool Settings -> GNU RISC_V Cross
 * Compiler -> Preprocessor to turn on the debug switch (serial port 1 by default)
 *
 * For details on the selection of engineering chips,
 * please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 * */
#include "string.h"
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

uint8_t MACAddr[6];
uint8_t IPAddr[4]   = {192, 168, 1, 10};
uint8_t GWIPAddr[4] = {192, 168, 1, 1};
uint8_t IPMask[4]   = {255, 255, 255, 0};
uint8_t DESIP[4]    = {192, 168, 1, 100};
uint16_t desport = 1000;
uint16_t srcport = 1000;
volatile uint8_t tcp_connect_num = 0;
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
    PRINT("Error: %02x\r\n", (u16)iError);
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
 * @fn      WCHNET_CreateTcpSocket
 *
 * @brief   create TCP socket, and connect
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket(uint8_t *socket_revbuf)
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
		WCHNET_SocketConnect(SocketId[tcp_connect_num]);                         /* TCP connect */
	}
}

/*********************************************************************
 * @fn      ETHRx
 *
 * @brief   ETH receive data and save it to buff.
 *
 * @param   socketid - socket id.
 *
 * @return  none
 */
void ETHRx(u8 socketid)
{
    u32 len;
    int8_t receive_state = -1;

    len = WCHNET_SocketRecvLen(socketid, NULL);                               /* query length */
#ifdef CH32V307_DEBUG
    if(socketid == 0)
    {
        WCHNET_SocketRecv(socketid, NULL, &len);
        return;
    }
#endif
    if(len > ETH_RECEIVE_SIZE){
        len = ETH_RECEIVE_SIZE;
    }

    if( uart_data_t[socketid].tx_remainBuffNum > 0)
    {
        /* socket receive */
        receive_state = WCHNET_SocketRecv(socketid, uart_data_t[socketid].TX_buffer[uart_data_t[socketid].tx_write], &len);
        if(receive_state == WCHNET_ERR_SUCCESS)
        {
            uart_data_t[socketid].TX_data_length[uart_data_t[socketid].tx_write] = len;

            uart_data_t[socketid].tx_write++;
            /* Prevent access from out-of-bounds */
            uart_data_t[socketid].tx_write = (uart_data_t[socketid].tx_write)%UART_TX_BUF_NUM;

            uart_data_t[socketid].tx_remainBuffNum--;
        }
    }
    else
    {
        PRINT("eth receive buff busy\n");
    }
    uartTx();
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
void WCHNET_HandleSockInt(uint8_t socketid,uint8_t intstat)
{
    uint32_t len;
    uint8_t write_buf = 0;
    int8_t receive_state = -1;

    if(intstat & SINT_STAT_RECV)                                               /* socket receive */
    {
        ETHRx(socketid);
    }
    if(intstat & SINT_STAT_CONNECT)                         /* TCP connect */
    {

    	PRINT("TCP Connect Success\n");
    }
    if(intstat & SINT_STAT_DISCONNECT)                      /* TCP disconnect  */
    {
		PRINT("TCP Disconnect\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT)                         /* TCP time out */
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
	uint8_t intstat;
	uint16_t i;
	uint8_t socketint;
	intstat = WCHNET_GetGlobalInt();                         /* get global interrupt, and clear */

	if(intstat & GINT_STAT_UNREACH)                          /* unreachable */
	{
		PRINT("GINT_STAT_UNREACH\r\n");
	}

	if(intstat & GINT_STAT_IP_CONFLI)                        /* IP conflict */
	{
		PRINT("GINT_STAT_IP_CONFLI\r\n");
	}

	if(intstat & GINT_STAT_PHY_CHANGE)                       /* PHY change */
	{
		i = WCHNET_GetPHYStatus();                            /*get PHY state*/
		if(i&PHY_Linked_Status)
		PRINT("PHY_Linked_Status\r\n");
		PRINT("GINT_STAT_PHY_CHANGE %x\n",i);
	}

	if(intstat & GINT_STAT_SOCKET)                           /*Socket interrupt*/
	{
		for(i = 0; i < SOCKET_NUM; i++)
		{
		    socketint = WCHNET_GetSocketInt(i);              /* get Socket interrupt*/
			if(socketint)
			{
				WCHNET_HandleSockInt(i,socketint);           /* IQR */
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

	for(uint8_t i=0;i<8;i++)
	{
		/* uart tx dma is idel */
		if(uart_data_t[i].uart_tx_dma_state == IDEL)
		{
			/* eth has received data  */
		    if(uart_data_t[i].tx_remainBuffNum < UART_TX_BUF_NUM)
			{
				read_buf = uart_data_t[i].tx_read;
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
 * @fn      uartRxAndSendDataToETH
 *
 * @brief   uart receive data, and send these data to eth
 *
 * @return  none
 */
void uartRxAndSendDataToETH(void)
{
    u8 *p;
    uint32_t temp = 0;
    uint32_t len = 0;
    uint32_t readindex = 0;

	for(uint8_t i = 0; i < 8; i++)
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
		if(temp != uart_data_t[i].last_RX_DMA_length)
		{
	        if(temp < uart_data_t[i].last_RX_DMA_length)
	        {
	            len = uart_data_t[i].last_RX_DMA_length - temp;
	        }
	        else {
	            len = uart_data_t[i].last_RX_DMA_length;
	        }
	        readindex = UART_RX_DMA_SIZE - uart_data_t[i].last_RX_DMA_length;
	        p = &uart_data_t[i].RX_buffer[readindex];
	        WCHNET_SocketSend(i, p, &len);
	        uart_data_t[i].last_RX_DMA_length -= len;
	        if(uart_data_t[i].last_RX_DMA_length == 0)
	            uart_data_t[i].last_RX_DMA_length = UART_RX_DMA_SIZE;
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
	uint8_t *name = "WCHNET";

	SystemCoreClockUpdate();
	Delay_Init();
#ifdef CH32V307_DEBUG
	USART_Printf_Init(115200);
#endif
    RCC_GetClocksFreq(&RCC_ClocksStatus);
	PRINT("8 UART Server Test\r\n");
	PRINT("SystemClk:%d\r\n",SystemCoreClock);
	PRINT("net version:%x\r\n",WCHNET_GetVer());
    if(WCHNET_LIB_VER != WCHNET_GetVer())
    {
        PRINT("version error.\r\n");
    }
    WCHNET_GetMacAddr(MACAddr);                          //get the chip MAC address
    PRINT("mac addr:");
    for(i = 0; i < 6; i++) 
        PRINT("%x ", MACAddr[i]);
    PRINT("\r\n");
    TIM2_Init();
    BSP_Uart_Init();                                     /*uart init*/
    WCHNET_DHCPSetHostname(name);                        //Configure DHCP host name
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);     //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) PRINT("WCHNET_LibInit Success\r\n");
	WCHNET_CreateTcpSocket(&SocketRecvBuf[0]);            //Create  UDP Socket

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
		uartTx();
		uartRxAndSendDataToETH();
	}
}










