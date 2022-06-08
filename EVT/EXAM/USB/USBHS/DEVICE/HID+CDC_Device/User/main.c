/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
  CDC + HID(Keyboard)
  USBHS - PB6(D-) PB7(D+)
  UART  - PA2(Tx) PA3(Rx)
  Keyboard - PB15-GND  Upload W key value
 
*/
#include "ch32v30x_usbotg_device.h"
#include "debug.h"
#include "my_dma.h"

u16 rx_len = 0;
u8 SendBuffer[UART_REV_LEN];
u8 ReceiveBuffer[UART_REV_LEN];
u16 BufferLen=512;
u8 rx_flag = 0;
u16 Sendlen_backup;
/* Function statement */
void GPIO_Config( void );
UINT8 Basic_Key_Handle( void );


/* const value definition */
const UINT8 Key_W_Val[ 8 ]    = { 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00  };
const UINT8 Key_NotPress[ 8 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
int8_t t_flag=0;



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    UINT8 Uart_Timeout = 0;
    UINT8 lenth;
    UINT8 UpLoadFlag = 0x00;
	Delay_Init();
	USART_Printf_Init(115200);
	Usart_Rx_Config();
	USART2_CFG(115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No);
	printf("SystemClk:%d\r\n",SystemCoreClock);

    /* USBOTG_FS device init */
	printf( "CH372Device Running On USBOTG_FS Controller\n" );
	Delay_Ms(10);
	USBOTG_Init( );
    /* GPIO Config */
    GPIO_Config( );
    USBHD_UEP0_T_LEN=0;
    USBHD_UEP2_T_LEN=0;
    USBHD_UEP1_T_LEN=0;
    USBHD_UEP4_T_LEN=0;

	while(1)
	{
        if( Basic_Key_Handle( ) )
        {
            while( USBHD_Endp3_Up_Flag );
            memcpy( pEP3_IN_DataBuf, (UINT8*)Key_W_Val, 8 );
            DevEP3_IN_Deal( 8 );
            UpLoadFlag = 1;
        }
        else
        {
            if( UpLoadFlag )
            {
                while( USBHD_Endp3_Up_Flag );
                memcpy( pEP3_IN_DataBuf, (UINT8*)Key_NotPress, 8 );
                DevEP3_IN_Deal( 8 );
                UpLoadFlag = 0;
            }
        }

        if(USBByteCount)
        {
            uart2_Send(EP4_DatabufHD,USBByteCount);
            USBByteCount=0;
            if(USBByteCount==0)
            {
                USBOTG_FS->UEP4_RX_CTRL  = (USBOTG_FS->UEP4_RX_CTRL & ~USBHD_UEP_R_RES_MASK)| USBHD_UEP_R_RES_ACK;
                USBOTG_FS->UEP4_RX_CTRL ^= USBHD_UEP_R_TOG;
                USBBufOutPoint=0;
            }

        }
        if(rx_len)
            Uart_Timeout++;

        if(!UpPoint4_Busy)
        {
            lenth = rx_len;

            if(lenth>0)
            {
                if(t_flag==1||Uart_Timeout>200)
                {
                    t_flag=0;
                    if(Uart_Output_Point+lenth>UART_REV_LEN)
                        lenth = UART_REV_LEN-Uart_Output_Point;
                    rx_len -= lenth;
                    while( UpPoint4_Busy )
                    {
                        printf( "Busy\n");
                    }
                    UpPoint4_Busy = 1;
                    memcpy(EP4_DatabufHD+MAX_PACKET_SIZE,&SendBuffer[Uart_Output_Point],lenth);
                    DevEP4_IN_Deal(lenth);
                }
            }
        }

	}
}

/*********************************************************************
 * @fn      GPIO_Config
 *
 * @brief   GPIO Configuration Program
 *
 * @return  none
 */
void GPIO_Config( void )
{
    GPIO_InitTypeDef GPIO_InitTypdefStruct={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
    GPIO_InitTypdefStruct.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitTypdefStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init( GPIOB, &GPIO_InitTypdefStruct );
}

/*********************************************************************
 * @fn      Basic_Key_Handle
 *
 * @brief   Basic Key Handle
 *
 * @return  0 - no key press
 *          1 - key press down
 */
UINT8 Basic_Key_Handle( void )
{
    UINT8 keyval = 0;
    if( ! GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_15 ) )
    {
        Delay_Ms(20);
        if( ! GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_15 ) )
        {
            keyval = 1;
        }
    }

    return keyval;
}
