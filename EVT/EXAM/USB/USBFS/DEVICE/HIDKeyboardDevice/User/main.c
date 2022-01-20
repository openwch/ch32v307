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
 模拟Keyboard例程：
 OTG_FS_DM(PA11)、OTG_FS_DP(PA12)
 本例程演示使用 USBHD 模拟Keyboard设备。
 
*/
#include "ch32v30x_usbotg_device.h"
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

    /* USBOTG_FS device init */
	printf( "Keyboard Running On USBOTG_FS Controller\n" );
	Delay_Ms(10);
	USBOTG_Init( );

    /* GPIO Config */
    GPIO_Config( );

	while(1)
	{
        if( Basic_Key_Handle( ) )
        {
            while( USBHD_Endp1_Up_Flag );
            printf( "u\n" );
            memcpy( pEP1_IN_DataBuf, (UINT8*)Key_W_Val, 8 );
            DevEP1_IN_Deal( 8 );
            UpLoadFlag = 1;
        }
        else
        {
            if( UpLoadFlag )
            {
                while( USBHD_Endp1_Up_Flag );
                printf( "n\n" );
                memcpy( pEP1_IN_DataBuf, (UINT8*)Key_NotPress, 8 );
                DevEP1_IN_Deal( 8 );
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

