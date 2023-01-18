/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/10
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
ETH_UART example demonstrates data transparency between Ethernet and UART.
By default, 1000000 baud rate (can be changed in bsp_uart.h) is used for serial port data transmission.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
*/
#include "string.h"
#include "debug.h"
#include "wchnet.h"
#include "eth_driver.h"
#include "bsp_uart.h"

u8 MACAddr[6];                                          //MAC address
u8 IPAddr[4]   = {192,168,1,10};                        //IP address
u8 GWIPAddr[4] = {192,168,1,1};                         //Gateway IP address
u8 IPMask[4]   = {255,255,255,0};                       //subnet mask
u8 DESIP[4]    = {192,168,1,100};                       //destination IP address

u8 SocketId;                                            //socket id
u8 SocketRecvBuf[RECE_BUF_LEN];                         //socket data buff
u16 desport = 1000;                                     //destination port
u16 srcport = 1000;                                     //source port

u8 TCPConnValid = 0;                                    //0 disconnected  1 connect

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
 * @fn      WCHNET_CreateTcpSocket
 *
 * @brief   Create TCP Socket
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket(void)
{
   u8 i;
   SOCK_INF TmpSocketInf;

   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));
   memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);
   TmpSocketInf.DesPort  = desport;
   TmpSocketInf.SourPort = srcport;
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
   i = WCHNET_SocketCreat(&SocketId,&TmpSocketInf);
   printf("WCHNET_SocketCreat %d\r\n",SocketId);
   mStopIfError(i);
   i = WCHNET_SocketConnect(SocketId);
   mStopIfError(i);
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
    int ret = -1;
    uint32_t temp = 0;
    uint32_t len = 0;
    uint32_t len_1 = 0;

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
            printf("uart2 RX DMA Buff is full!\r\n");
            len = 0;
            uart_data_t.rx_read = 0;
            uart_data_t.rx_write = 0;
            uart_data_t.last_RX_DMA_length = 0;
        }

        /* send data starting from rx_read to RX_buffer end */
        len_1 = MIN(len,UART_RX_DMA_SIZE-(uart_data_t.rx_read&(UART_RX_DMA_SIZE-1)));

        if(len_1 != 0)
        {
            ret = WCHNET_SocketSend(SocketId, &uart_data_t.RX_buffer[uart_data_t.rx_read&(UART_RX_DMA_SIZE-1)], &len_1);
            if (ret == 0)
            {
                uart_data_t.rx_read += len_1;
            }
        }

        /* send the rest of the data(if any) at beginning of RX_buffer  */
        if( (len-len_1) != 0)
        {
            len_1 = len-len_1;
            ret = WCHNET_SocketSend(SocketId, &uart_data_t.RX_buffer[0], &len_1);
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

    if(uart_data_t.uart_tx_dma_state == IDLE)
    {
        /* eth has received data  */
        if(uart_data_t.tx_remainBuffNum < UART_TX_BUF_NUM)
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
 * @brief   ETH receive data and save it to buff.
 *
 * @param   socketid - socket id.
 *
 * @return  none
 */
void ETHRx(u8 socketid)
{
    u32 len;
    uint8_t write_buf = 0;
    int8_t receive_state = -1;

    len = WCHNET_SocketRecvLen(socketid, NULL);                               /* query length */

    if( uart_data_t.tx_remainBuffNum > 0)
    {
        write_buf = (uart_data_t.tx_write)%UART_TX_BUF_NUM;
        /* socket receive */
        receive_state = WCHNET_SocketRecv(socketid, uart_data_t.TX_buffer[write_buf], &len);
        if(receive_state == WCHNET_ERR_SUCCESS)
        {
            uart_data_t.TX_data_length[write_buf] = len;

            uart_data_t.tx_write++;

            uart_data_t.tx_remainBuffNum--;
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
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid,u8 intstat)
{
    if(intstat & SINT_STAT_RECV)                        //receive data
    {
        ETHRx(socketid);
    }
    if(intstat & SINT_STAT_CONNECT)                     //connect successfully
    {
        TCPConnValid = 1;
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf, RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if(intstat & SINT_STAT_DISCONNECT)                  //disconnect
    {
        TCPConnValid = 0;
        printf("TCP Disconnect\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT)                     //timeout disconnect
    {
        TCPConnValid = 0;
        printf("TCP Timeout\r\n");
        WCHNET_CreateTcpSocket();
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
	USART_Printf_Init(115200);                                              //USART initialize
	printf("ETH_UART\r\n");	
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("net version:%x\n",WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer() ){
      printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr);                                             //get the chip MAC address
    printf("mac addr:");
    for(i = 0; i < 6; i++) 
        printf("%x ",MACAddr[i]);
    printf("\n");
    TIM2_Init();
    BSP_Uart_Init();                                                        //USART2 initialize
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                        //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf("WCHNET_LibInit Success\r\n");

    WCHNET_CreateTcpSocket();                                               //Create TCP Socket

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
        if(TCPConnValid){
            uartTx();
            uartRxAndSendDataToETH();
        }
    }
}
