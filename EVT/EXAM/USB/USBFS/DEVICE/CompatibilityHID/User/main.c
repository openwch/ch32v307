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
   模拟HID兼容设备，支持中断上下传，支持控制端点上下传，支持设置全速，低速，可使用BusHound等软件调试
*/
#include "ch32v30x_usbotg_device.h"
#include "debug.h"

/* Function statement */
void GPIO_Config( void );
UINT8 Basic_Key_Handle( void );

/* Value */
UINT8 TestValue[ DEF_USBD_UEP0_SIZE ] = { 0x00 };

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
    UINT8 i;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

    /* USBOTG_FS device init */
	printf( "Keyboard Running On USBOTG_FS Controller\n" );
	Delay_Ms(10);
	USBOTG_Init( );

    /* GPIO Config */
    GPIO_Config( );

    /* Prepare Data For Test */
    for( i=0; i<DEF_USBD_UEP0_SIZE; i++ )
    {
        TestValue[ i ] = i;
    }

    while(1)
    {
        /* HID Tx */
        if( Basic_Key_Handle( ) )
        {
            if( UpLoadFlag == 0 )
            {
                UpLoadFlag = 1;
                Ep1_Tx( TestValue, DEF_USBD_UEP0_SIZE );
                printf( "Tx Data\n" );
            }
        }
        else
        {
            if( UpLoadFlag )
            {
                UpLoadFlag = 0;
            }
        }
        /* HID Rx */
        if( USBHD_Endp1_Down_Flag )
        {
            USBHD_Endp1_Down_Flag = 0;
            printf( "Rx Data\n" );
            for( i=0; i<DEF_USBD_UEP0_SIZE; i++ )
            {
                printf( "%02x ", pEP1_OUT_DataBuf[ i ] );
            }
            printf( "\n" );
            USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~USBHD_UEP_R_RES_MASK) | USBHD_UEP_R_RES_ACK;
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

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    GPIO_InitTypdefStruct.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitTypdefStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitTypdefStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init( GPIOA, &GPIO_InitTypdefStruct );
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
    if( ! GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0 ) )
    {
        Delay_Ms(20);
        if( ! GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0 ) )
        {
            keyval = 1;
        }
    }
    return keyval;
}

