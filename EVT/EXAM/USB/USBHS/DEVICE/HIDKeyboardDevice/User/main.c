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
   模拟HID键盘设备，PB15按钮为W健
*/
#include "ch32v30x_usbhs_device.h"
#include "debug.h"

/* Function statement */
void GPIO_Config( void );
UINT8 Basic_Key_Handle( void );

/* const value definition */
const UINT8 Key_W_Val[ 8 ]    = { 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00  };
const UINT8 Key_NotPress[ 8 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8 UpLoadFlag = 0x00;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "HIDKeyBoard Running On USBHS Controller\n" );

    /* USB20 device init */
    USBHS_RCC_Init( );                                                         /* USB2.0高速设备RCC初始化 */
    USBHS_Device_Init( ENABLE );
    NVIC_EnableIRQ( USBHS_IRQn );

    /* GPIO Config */
    GPIO_Config( );

	while(1)
	{
	    if( Basic_Key_Handle( ) )
	    {
	        while( USBHS_Endp1_Up_Flag );
	        Ep1_Tx( (UINT8*)Key_W_Val, 8 );
	        UpLoadFlag = 1;
	    }
	    else
	    {
	        if( UpLoadFlag )
	        {
	            while( USBHS_Endp1_Up_Flag );
	            Ep1_Tx( (UINT8*)Key_NotPress, 8 );
	            UpLoadFlag = 0;
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
    GPIO_InitTypeDef GPIO_InitTypdefStruct;

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
