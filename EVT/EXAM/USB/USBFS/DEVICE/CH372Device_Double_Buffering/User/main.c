/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/03
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*@Note
 * This routine demonstrates the use of USBFS to emulate a custom device similar to CH372,
 * with endpoints 1/3/5 downlinking data and uploading it via endpoints 2/4/6 respectively.
 *
 * - Endpoint 1: Double buffering enabled, data downlink.
 * - Endpoint 2: Double buffering enabled, uploads data from endpoint 1.
 * - Endpoint 3: Double buffering enabled, data downlink.
 * - Endpoint 4: Double buffering enabled, uploads bit-inverted data from endpoint 3.
 * - Endpoint 5/6: Always ACK for speed tests.
 *
 * Test Tool:
 * https://www.wch.cn/downloads/USBEndpDebug_ZIP.html
 */

#include "ch32v30x_usbfs_device.h"
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
	SystemCoreClockUpdate( );
	Delay_Init( );
	USART_Printf_Init( 115200 );
		
	printf( "SystemClk:%d\r\n", SystemCoreClock );
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf( "CH372Device Running On usbfs-FS Controller\n" );

	/* USBFSD device init */
	USBFS_RCC_Init( );
	USBFS_Device_Init( ENABLE );

	while(1)
	{
        if(USBFS_DevEnumStatus)
        {
            // add your code here
        }
	}
}
