/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/04/11
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *CAN normal mode, standard frame and expanded frame data transceiver:
 *CAN_Tx(PB9),CAN_Rx(PB8)
 *This routine requires an external CAN transceiver to demonstrate Standard_Frame and Extended_Frame.
 *Standard_Frame: including 1 32bit filter mask bit pattern, 2 16bit filter mask bit patterns,
 *1 32bit identifier list mode, 2 16bit identifier list modes;
 *Extended_Frame: 1 32bit filter mask bit pattern.
 *
 */

 #include "debug.h"

 /* CAN Mode Definition */
 #define TX_MODE   0
 #define RX_MODE   1
 
 /* Frame Format Definition */
 #define Standard_Frame   0
 #define Extended_Frame   1
 
 /* CAN Communication Mode Selection */
 // #define CAN_MODE   TX_MODE
 #define CAN_MODE   RX_MODE
 
 /* Frame Formate Selection */
 #define Frame_Format   Standard_Frame
 //#define Frame_Format   Extended_Frame
 
 /* Receive can data in interrupt */
 #define USE_INTERRUPT
 
 #define USE_SOFT_FILTER
 
 #define CANSOFTFILTER_MAX_GROUP_NUM 2           // The maximum recommended configuration is 14. 
												 //Configure only what you need to prevent excessive RAM usage or an increase in the software's filtering time.
 
 #define CANSOFTFILER_PREDEF_CTRLBYTE_MASK32 ((CAN_FilterScale_32bit << 5) | (CAN_FilterMode_IdMask << 1))
 #define CANSOFTFILER_PREDEF_CTRLBYTE_ID32   ((CAN_FilterScale_32bit << 5) | (CAN_FilterMode_IdList << 1))
 
 /* 
 This is the structure of the software filtering table. It can be configured through the CAN_SoftFilterInit function,
 or you can directly set the configuration values. The configured values can be modified directly during runtime.
 However, when using the interrupt mode for reception, you need to be aware that if the modification is interrupted, 
 it may affect the filtering results during this period. 
 */
 struct CANFilterStruct_t
 {
	 union
	 {
		 union
		 {
			 struct
			 {
				 uint32_t :1;
				 uint32_t RTR :1;
				 uint32_t IDE :1;
				 uint32_t ExID :29;
			 }Access_Ex;
			 struct
			 {
				 uint32_t :1;
				 uint32_t RTR :1;
				 uint32_t IDE :1;
				 uint32_t :18;
				 uint32_t StID :11;
			 }Access_St;
		 };
		 union{
			 struct {
				 uint16_t FR_16_L;
				 uint16_t FR_16_H;
			 };
			 uint32_t FR_32;
		 };
	 }FR[2];
	 union
	 {
		 struct
		 {
			 uint16_t en :1;
			 uint16_t mode :4;
			 uint16_t scale :3;
		 };
		 uint16_t ctrl_byte;
	 };
 }CANFilterStruct[CANSOFTFILTER_MAX_GROUP_NUM];

 int CAN2FilterStartBank = CANSOFTFILTER_MAX_GROUP_NUM;
 
 uint8_t interrupt_rx_flag = 0;
 volatile u8 canexbuf_interrupt[8];
 
 void CAN_SoftSlaveStartBank(uint8_t CAN_BankNumber);
 void CAN_SoftFilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);
 void CAN_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode);
 void CAN_ReceiveViaSoftFilter(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage);
 u8 CAN_Send_Msg(u8 *msg, u8 len);
 u8 CAN_Receive_Msg(u8 *buf);
 
 void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
 void USB_LP_CAN1_RX0_IRQHandler()
 {
	 int i;
	 uint8_t px,pbuf[8];
	 if (CAN_GetITStatus(CAN1,CAN_IT_FMP0))
	 {
		 px = CAN_Receive_Msg(pbuf);
		 for ( i = 0; i < px; i++) 
		 {
			 canexbuf_interrupt[i] = pbuf[i];
		 }
		 if(px)
		 {
			 interrupt_rx_flag = 1;
		 }
		 CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	 }
 }
 
 /*********************************************************************
  * @fn      CAN_SoftFilterInit
  *
  * @brief   Initializes the CAN peripheral according to the specified
  *        parameters in the CAN_FilterInitStruct.
  *
  * @param   CAN_FilterInitStruct - pointer to a CAN_FilterInitTypeDef
  *        structure that contains the configuration information.
  *
  * @return  none
  */
 void CAN_SoftFilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct)
 {
	if(CAN_FilterInitStruct->CAN_FilterNumber > CANSOFTFILTER_MAX_GROUP_NUM){
		return;
	}
	if(CAN_FilterInitStruct->CAN_FilterActivation)
	{
		CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].en = 1;
	}else
	{
		CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].en = 0;
	}
	CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[0].FR_16_H = CAN_FilterInitStruct->CAN_FilterIdHigh;
	CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[0].FR_16_L = CAN_FilterInitStruct->CAN_FilterIdLow;
	CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[1].FR_16_H = CAN_FilterInitStruct->CAN_FilterMaskIdHigh;
	CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[1].FR_16_L = CAN_FilterInitStruct->CAN_FilterMaskIdLow;
	CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].mode = CAN_FilterInitStruct->CAN_FilterMode;
	CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].scale = CAN_FilterInitStruct->CAN_FilterScale;
 }
 
 /*********************************************************************
  * @fn      CAN_ReceiveViaSoftFilter
  *
  * @brief   Receives a message via soft filter.
  *
  * @param   CANx - where x can be 1 to select the CAN peripheral.
  *          FIFONumber - Receive FIFO number.
  *            CAN_FIFO0.
  *          RxMessage -  pointer to a structure receive message which contains
  *        CAN Id, CAN DLC, CAN datas and FMI number.
  *
  * @return  none
  */
 void CAN_ReceiveViaSoftFilter(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage)
 {
     int group, start_bank, end_bank;
    if(CANx == CAN1)
    {
        start_bank = 0;
        end_bank = CAN2FilterStartBank;
    }
    else
    {
        start_bank = CAN2FilterStartBank;
        end_bank = CANSOFTFILTER_MAX_GROUP_NUM;
    }

     for ( group = start_bank; group < end_bank; group++) 
     {
         if (CANFilterStruct[group].en) 
         {
             uint32_t temp = CANx->sFIFOMailBox[0].RXMIR & (~0x1);
             switch ((uint8_t)CANFilterStruct[group].ctrl_byte & ~0x1) 
             {
 
                 case CANSOFTFILER_PREDEF_CTRLBYTE_ID32:
                     if((CANFilterStruct[group].FR[0].FR_32 != temp) && (CANFilterStruct[group].FR[1].FR_32 != temp))
                     {
                         continue;
                     }
                     else
                     {
                         CAN_Receive(CANx, CAN_FIFO0, RxMessage);
                         return;
                     }
                     break;
 
                 case CANSOFTFILER_PREDEF_CTRLBYTE_MASK32:
                     if((CANFilterStruct[group].FR[0].FR_32 & CANFilterStruct[group].FR[1].FR_32) ^ (temp & CANFilterStruct[group].FR[1].FR_32))
                     {
                         continue;
                     }
                     else 
                     {
                         CAN_Receive(CANx, CAN_FIFO0, RxMessage);
                         return;
                     }
                     break;
 
                 default:
                     return;
                     break;
             }
         }
     }
     CAN_FIFORelease(CANx,CAN_FIFO0);
 }
 
 /*********************************************************************
 * @fn      CAN_SoftSlaveStartBank
 *
 * @brief   This function applies only to CH32 Connectivity line devices.
 *
 * @param   CAN_BankNumber - Select the start slave bank filter from 1...size of CANFilterStruct
 *
 * @return  none
 */
void CAN_SoftSlaveStartBank(uint8_t CAN_BankNumber)
{
   CAN2FilterStartBank = CAN_BankNumber;
}

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
 void CAN_Mode_Init( u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode )
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
 
 #if (Frame_Format == Standard_Frame)
 /* identifier/mask mode, One 32-bit filter, StdId: 0x317 */
	 CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;	 
	 CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit; 
	 CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;	 
	 CAN_FilterInitSturcture.CAN_FilterIdLow = 0; 
	 CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFE0;  	
	 CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0x0006;	 
	 
#ifndef USE_SOFT_FILTER
 /* identifier/mask mode, Two 16-bit filters, StdId: 0x317,0x316 */
 //	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;	
 //	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_16bit;	
 //	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;	
 //	CAN_FilterInitSturcture.CAN_FilterIdLow = 0xFFF8;		
 //	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0x62C0; 	
 //	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0xFFF8;
 #endif		
	 
 /* identifier list mode, One 32-bit filter, StdId: 0x317,0x316 */
 //	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdList;	
 //	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;	
 //	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;	
 //	CAN_FilterInitSturcture.CAN_FilterIdLow = 0;
 //	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0x62C0; 	
 //	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0;	

 #ifndef USE_SOFT_FILTER
 /* identifier list mode, Two 16-bit filters, StdId: 0x317,0x316,0x315,0x314 */
 //	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdList;	
 //	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_16bit;	
 //	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;	
 //	CAN_FilterInitSturcture.CAN_FilterIdLow = 0x62C0;	
 //	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0x62A0; 	
 //	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0x6280;	
 #endif		
 
 #elif (Frame_Format == Extended_Frame)
 /* identifier/mask mode, One 32-bit filter, ExtId: 0x12124567 */
	 CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;		
	 CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;	
	 CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x9092;	
	 CAN_FilterInitSturcture.CAN_FilterIdLow = 0x2B3C;	
	 CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFFF; 	
	 CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0xFFFE;	
	 
 #endif
 
	 CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	 CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;
 
 #ifdef USE_INTERRUPT
	 CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	 NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
 #endif
 
 #ifdef USE_SOFT_FILTER
	 CAN_SoftFilterInit( &CAN_FilterInitSturcture );
 #else
	 CAN_FilterInit( &CAN_FilterInitSturcture );
 #endif // USE_SOFT_FILTER	
	 
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
 
 #if (Frame_Format == Standard_Frame)
	 CanTxStructure.StdId = 0x317;		
	 CanTxStructure.IDE = CAN_Id_Standard;	
   
 #elif (Frame_Format == Extended_Frame)
	 CanTxStructure.ExtId = 0x12124567;		
	 CanTxStructure.IDE = CAN_Id_Extended;	
	 
 #endif	
	 
	 CanTxStructure.RTR = CAN_RTR_Data;		
	 CanTxStructure.DLC = len;
	 
	 for( i=0; i<len; i++ )
	 {
		 CanTxStructure.Data[i] = msg[i];	
	 }
	 
	 mbox = CAN_Transmit( CAN1, &CanTxStructure);
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
	 
	 if( CAN_MessagePending( CAN1, CAN_FIFO0 ) == 0)	 
	 {
		 return 0;
	 }
 
 #ifdef USE_SOFT_FILTER
	 CAN_ReceiveViaSoftFilter( CAN1, CAN_FIFO0, &CanRxStructure );
 #else
	 CAN_Receive( CAN1, CAN_FIFO0, &CanRxStructure );	
 #endif // USE_SOFT_FILTER
	 
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
	 u8 i=0;
	 u8 cnt=1;
	 u8 px;
	 u8 pxbuf[8];
	 
	 SystemCoreClockUpdate();
	 Delay_Init();
	 USART_Printf_Init(115200);	
	 printf("SystemClk:%d\r\n",SystemCoreClock);
	 printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	 
 #if (CAN_MODE == TX_MODE)	
	 printf( "Tx Mode\r\n" );
	 
 #elif (CAN_MODE == RX_MODE)	
	 printf( "Rx Mode\r\n" );
	 
 #endif	
 /* Bps = 250Kbps */
	 CAN_Mode_Init( CAN_SJW_1tq, CAN_BS2_5tq, CAN_BS1_6tq, 12, CAN_Mode_Normal );
 
	 while(1)
	 {	
		 
 #if (CAN_MODE == TX_MODE)		
		 for( i=0; i<8; i++ )
		 {
			 pxbuf[i] = cnt+i;	
		 }
		 
		 px = CAN_Send_Msg( pxbuf, 8);	
		 
		 if( px )	
		 {
			 printf( "Send Failed\r\n" );
		 }
		 else	
		 {
			 printf( "Send Success\r\n" );
			 printf( "Send Data:\r\n" );
			 
			 for(i=0; i<8; i++)
			 {
				 printf( "%02x\r\n", pxbuf[i] );
			 }
		 }
		 
 #elif (CAN_MODE == RX_MODE)		
 
 #ifndef USE_INTERRUPT
		 px = CAN_Receive_Msg(pxbuf);
 
		 if(px)
		 {
			 printf("Receive Data:\r\n");
 
			 for(i = 0; i < 8; i++) {
				 printf("%02x\r\n", pxbuf[i]);
			 }
		 }
		 Delay_Ms(1000);
 #else
		 if(interrupt_rx_flag)
		 {
			 interrupt_rx_flag = 0;
			 printf("Receive Data: ");
			 for(i = 0; i < 8; i++) {
				 printf("%02x\t", canexbuf_interrupt[i]);
			 }
			 printf("\n");
		 }
 #endif
 #endif		
		 cnt++;
		 
		 if(cnt == 0xFF)
		 {
			 cnt = 0;
		 }	
	 }
 }
 
 
