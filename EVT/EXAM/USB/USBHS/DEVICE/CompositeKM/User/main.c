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
   模拟HID键盘设备，短接PA0键盘打印wch.cn，鼠标先移动至坐标（360*360）,再移动至（1080*1080），再左键点击一下。
   使用杜邦线短接开发板上排针KEY和PA0
*/
#include "ch32v30x_usbhs_device.h"
#include "debug.h"

/* Value */
UINT8 KeyStatus[ 8 ] = { 0x00 };
UINT8 MouseStatus[ 7 ] = { 0x00 };

/* Function statement */
void GPIO_Config( void );
UINT8 Basic_Key_Handle( void );

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8  UpLoadFlag = 0x00;
    UINT16 x_pix_val = 0;/* 绝对鼠标x坐标值 */
    UINT16 y_pix_val = 0;/* 绝对鼠标y坐标值 */
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
	        if( UpLoadFlag == 0 )
	        {
                printf( "KeyDown\n" );
                /* KeyBoard */
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
	            /* w */
	            KeyStatus[ 2 ] = 0x1A;
	            Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
	            /* c */
	            KeyStatus[ 2 ] = 0x06;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
	            /* h */
                KeyStatus[ 2 ] = 0x0B;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
	            /* . */
                KeyStatus[ 2 ] = 0x37;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
	            /* c */
                KeyStatus[ 2 ] = 0x06;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
	            /* n */
                KeyStatus[ 2 ] = 0x11;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                /* 回车 */
                KeyStatus[ 2 ] = 0x28;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                KeyStatus[ 2 ] = 0x00;
                Ep1_Tx( (UINT8*)KeyStatus, 8 );
                Delay_Ms( 500 );
                /* Mouse */
                MouseStatus[ 0 ] = 0x02;
                /* Move around */
                x_pix_val = 360;
                for( y_pix_val=360; y_pix_val<1080; y_pix_val+=16 )
                {
                    MouseStatus[ 2 ] = (UINT8)x_pix_val;/* X_Val_l */
                    MouseStatus[ 3 ] = (UINT8)(x_pix_val>>8);/* X_Val_h */
                    MouseStatus[ 4 ] = (UINT8)y_pix_val;/* Y_Val_l */
                    MouseStatus[ 5 ] = (UINT8)(y_pix_val>>8);/* Y_Val_h */
                    Ep2_Tx( (UINT8*)MouseStatus, 7 );
                    MouseStatus[ 2 ] = 0x00;/* X_Val_l */
                    MouseStatus[ 3 ] = 0x00;/* X_Val_h */
                    MouseStatus[ 4 ] = 0x00;/* Y_Val_l */
                    MouseStatus[ 5 ] = 0x00;/* Y_Val_h */
                    Ep2_Tx( (UINT8*)MouseStatus, 7 );
                    x_pix_val += 16;
                }
                /* Left click */
                MouseStatus[ 1 ] = 0x01;
                Ep2_Tx( (UINT8*)MouseStatus, 7 );
                MouseStatus[ 1 ] = 0x00;
                Ep2_Tx( (UINT8*)MouseStatus, 7 );
                Delay_Ms( 100 );
	            UpLoadFlag = 1;
	            printf( "KeyDown Finish\n" );
	        }
	    }
	    else
	    {
	        if( UpLoadFlag )
	        {
	            UpLoadFlag = 0;
	            printf( "KeyUP\n" );
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
        Delay_Ms(30);
        if( ! GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0 ) )
        {
            keyval = 1;
        }
    }
    return keyval;
}
