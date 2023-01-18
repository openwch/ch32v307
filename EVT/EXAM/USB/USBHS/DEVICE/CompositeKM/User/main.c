/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/18
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * @Note
 * Composite Keyboard and Mouse Example:
 * This example uses PB12-PB15 and PA4-PA7 to simulate keyboard key pressing and mouse
 * movement respectively, active low.
 * At the same time, it also uses USART2(PA3) to receive the specified data sent from
 * the host to simulate the pressing and releasing of the following specific keyboard
 * keys. Data is sent in hexadecimal format and 1 byte at a time.
 * 'W' -> 0x1A
 * 'A' -> 0x04
 * 'S' -> 0x16
 * 'D' -> 0x07
 */

/*
 * @Note
 * Please select the corresponding macro definition (CH32V30x_D8/CH32V30x_D8C)
 * and startup_xxx.s file according to the chip model, otherwise the example may be abnormal.
 */


/*******************************************************************************/
/* Header Files */
#include "ch32v30x_usbhs_device.h"
#include "usbd_composite_km.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main( void )
{
    /* Initialize system configuration */
	SystemCoreClockUpdate( );
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
	Delay_Init( );
	USART_Printf_Init( 115200 );
		
	printf( "SystemClk:%d\r\n", SystemCoreClock );
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USBHS Composite KM Device Test\r\n" );

	/* Initialize USART2 for receiving the specified keyboard data */
	USART2_Init( 115200 );
	printf( "USART2 Init OK!\r\n" );

	/* Initialize GPIO for keyboard scan */
	KB_Scan_Init( );
	KB_Sleep_Wakeup_Cfg( );
	printf( "KB Scan Init OK!\r\n" );

	/* Initialize GPIO for mouse scan */
	MS_Scan_Init( );
	MS_Sleep_Wakeup_Cfg( );
	printf( "MS Scan Init OK!\r\n" );

	/* Initialize timer for Keyboard and mouse scan timing */
	TIM3_Init( 1, SystemCoreClock / 10000 - 1 );
	printf( "TIM3 Init OK!\r\n" );

	/* Initialize USBHD interface to communicate with the host  */
	USBHS_RCC_Init( );
	USBHS_Device_Init( ENABLE );
	USB_Sleep_Wakeup_CFG( );

	while( 1 )
    {
	    if( USBHS_DevEnumStatus )
	    {
	        /* Handle keyboard scan data */
	        KB_Scan_Handle(  );

	        /* Handle keyboard lighting */
	        KB_LED_Handle( );

            /* Handle mouse scan data */
            MS_Scan_Handle( );

            /* Handle USART2 receiving data */
            USART2_Receive_Handle( );
	    }
    }
}
