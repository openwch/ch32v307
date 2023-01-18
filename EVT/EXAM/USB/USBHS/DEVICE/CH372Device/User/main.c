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
  Example routine to emulate a custom USB device (CH372 device).
  This routine demonstrates the use of a USBHS Device to emulate a custom device, the CH372, 
  with endpoints 1/3/5 downlinking data and uploading via endpoints 1/4/6 respectively
  Endpoint 1 uploads and downlinks via ring buffer with no data reversal, endpoints 3/4, and endpoints 5/6 copy and upload.
  The device can be operated using Bushund or other upper computer software.
  Note: This routine needs to be demonstrated in conjunction with the host software.
*/

#include "ch32v30x_usbhs_device.h"
#include "debug.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint8_t ret;
	SystemCoreClockUpdate( );
	Delay_Init( );
	USART_Printf_Init( 115200 );
    	
	printf( "SystemClk:%d\r\n",SystemCoreClock) ;
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf( "CH372Device Running On USBHS Controller\n" );
	
    /* USB20 device init */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );
    NVIC_EnableIRQ( USBHS_IRQn );

	while(1)
	{
        /* Determine if enumeration is complete, perform data transfer if completed */
        if(USBHS_DevEnumStatus)
        {
            /* Data Transfer */
            if(RingBuffer_Comm.RemainPack)
            {
                ret = USBHS_Endp_DataUp(DEF_UEP1, &Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_HS_PACK_SIZE], RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr], DEF_UEP_DMA_LOAD);
                if(ret == 0)
                {
                    NVIC_DisableIRQ(USBHS_IRQn);
                    RingBuffer_Comm.RemainPack--;
                    RingBuffer_Comm.DealPtr++;
                    if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                    {
                        RingBuffer_Comm.DealPtr = 0;
                    }
                    NVIC_EnableIRQ(USBHS_IRQn);
                }
            }

            /* Monitor whether the remaining space is available for further downloads */
            if(RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART))
            {
                if(RingBuffer_Comm.StopFlag)
                {
                    RingBuffer_Comm.StopFlag = 0;
                    USBHSD->UEP1_RX_CTRL = (USBHSD->UEP1_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                }
            }
        }
	}
}
