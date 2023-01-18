/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 CAN test mode, including silent mode, loopback mode and loopback silent mode:
 CAN_Tx(PB9),CAN_Rx(PB8)
 Standard_Frame: includes a 32bit filter mask bit pattern.

*/

#include "debug.h"

/* CAN Test Mode Definition */
#define SILENT_MODE             1
#define LOOPBACK_MODE           2
#define SILENT_LOOPBACK_MODE    3

/* Test Mode Selection */
//#define TEST_MODE   SILENT_MODE
//#define TEST_MODE   LOOPBACK_MODE
#define TEST_MODE   SILENT_LOOPBACK_MODE

/*********************************************************************
 * @fn      CAN_Mode_Init
 *
 * @brief   Initializes CAN communication test mode.
 *          Bps =Fpclk1/((tpb1+1+tbs2+1+1)*brp)
 *
 * @param   tsjw - CAN synchronisation jump width.
 *          tbs2 - CAN time quantum in bit segment 1.
 *          tbs1 - CAN time quantum in bit segment 2.
 *          brp - Specifies the length of a time quantum.
 *          mode - Test mode.
 *            CAN_Mode_Normal.
 *            CAN_Mode_LoopBack.
 *            CAN_Mode_Silent.
 *            CAN_Mode_Silent_LoopBack.
 *
 * @return  none
 */
void CAN_Test_Mode_Init( u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode )
{
	GPIO_InitTypeDef GPIO_InitSturcture={0};
	CAN_InitTypeDef CAN_InitSturcture={0};
	CAN_FilterInitTypeDef CAN_FilterInitSturcture={0};
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE ); 
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1, ENABLE );	
	
	GPIO_PinRemapConfig( GPIO_Remap1_CAN1, ENABLE);	
	
	GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitSturcture);
	
	GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init( GPIOB, &GPIO_InitSturcture);
	
	CAN_InitSturcture.CAN_TTCM = DISABLE;		
	CAN_InitSturcture.CAN_ABOM = DISABLE;		
	CAN_InitSturcture.CAN_AWUM = DISABLE;		
	CAN_InitSturcture.CAN_NART = ENABLE;		
	CAN_InitSturcture.CAN_RFLM = DISABLE;		
	CAN_InitSturcture.CAN_TXFP = DISABLE;	
	CAN_InitSturcture.CAN_Mode = mode;
	CAN_InitSturcture.CAN_SJW = tsjw;		
	CAN_InitSturcture.CAN_BS1 = tbs1;		
	CAN_InitSturcture.CAN_BS2 = tbs2;		
	CAN_InitSturcture.CAN_Prescaler = brp;			
	CAN_Init( CAN1, &CAN_InitSturcture );
	
	CAN_FilterInitSturcture.CAN_FilterNumber = 0;		
	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;		
	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;	
	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0;
	CAN_FilterInitSturcture.CAN_FilterIdLow = 0;
	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh =0; 
	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0x0006;
	CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
	CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;		
	CAN_FilterInit( &CAN_FilterInitSturcture );
		
}

/*********************************************************************
 * @fn      CAN_Send_Msg
 *
 * @brief   CAN Transmit function.
 *
 * @param   msg - Transmit data buffer.
 *          len - Data length.
 *
 * @return  0 - Send successful.
 *          1 - Send failed.
 */
u8 CAN_Send_Msg( u8 *msg, u8 len )
{
	u8 mbox;
	u16 i = 0;
	
	CanTxMsg CanTxStructure;

	CanTxStructure.StdId = 0x317;	
	CanTxStructure.IDE = CAN_Id_Standard;	  
	CanTxStructure.RTR = CAN_RTR_Data;
	CanTxStructure.DLC = len;
	
	for( i=0; i<len; i++ )
	{
		CanTxStructure.Data[i] = msg[i];	
	}
	
	mbox = CAN_Transmit(CAN1, &CanTxStructure);
	
	i = 0;
	
	while( ( CAN_TransmitStatus( CAN1, mbox ) != CAN_TxStatus_Ok ) && (i < 0xFFF) ) 
	{
		i++;
	}
	
	if( i == 0xFFF )
	{
		return 1;
	}
	else 
	{
		return 0;
	}	
}

/*********************************************************************
 * @fn      CAN_Receive_Msg
 *
 * @brief   CAN Receive function.
 *
 * @param   buf - Receive data buffer.
 *          len - Data length.
 *
 * @return  CanRxStructure.DLC - Receive data length.
 */
u8 CAN_Receive_Msg( u8 *buf )
{
	u8 i;
	
	CanRxMsg CanRxStructure;
	
	if( CAN_MessagePending( CAN1, CAN_FIFO1 ) == 0)		
	{
		return 0;
	}
	
	CAN_Receive( CAN1, CAN_FIFO1, &CanRxStructure );	
	
	for( i=0; i<8; i++ )
	{
		buf[i] = CanRxStructure.Data[i];
	}
	
	return CanRxStructure.DLC;	
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
	u8 i;
	u8 cnt=0;
	u8 tx, rx;
	u8 txbuf[8];
	u8 rxbuf[8];
	
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

/* Bps = 333Kbps */
#if (TEST_MODE == SILENT_MODE)	
	CAN_Test_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 12, CAN_Mode_Silent );		
    printf("Slient Mode\r\n");
	
#elif (TEST_MODE == LOOPBACK_MODE)
	CAN_Test_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 12, CAN_Mode_LoopBack );	
    printf("LoopBack Mode\r\n");
	
#elif (TEST_MODE == SILENT_LOOPBACK_MODE)
	CAN_Test_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 12, CAN_Mode_Silent_LoopBack );
	printf("Silent_LoopBack Mode\r\n");	
		
#endif	
	
	while(1)
	{
		for(i=0; i<8; i++)
		{
			txbuf[i] = cnt+i;	
		}
		
		tx = CAN_Send_Msg( txbuf, 8 );	
		
		if( tx )	
		{
			printf( "Send Failed\r\n" );
		}
		else	
		{
			printf( "Send Success\r\n" );
			printf( "Send Data:\r\n" );
			
			for(i=0; i<8; i++)
			{
				printf( "%02x\r\n", txbuf[i] );
			}
		}
		
		rx = CAN_Receive_Msg( rxbuf );
		
		if( rx )
		{
			printf( "Receive Data:\r\n" );
			
			for(i=0; i<8; i++)
			{
				printf( "%02x\r\n", txbuf[i] );
			}
		}
		else	
		{
			printf( "No Receive Data\r\n" );
		}
		
		Delay_Ms(1000);
		cnt++;
		if( cnt == 0xFF)
		{
			cnt =0;
		}	
	}
}

