/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/09/01
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/

/*
 * @Note
 * This example demonstrates the enumeration process of a USB host to a device that
 * supports MTP and PTP protocols, and reads its files.
*/

/*
 * @Note
 * Please select the corresponding macro definition (CH32V30x_D8C/CH32V30x_D8)
 * and startup_xxx.s file according to the chip model, otherwise the example may be abnormal.
 * In addition, when the system clock is selected as the USBFS clock source, only 144MHz/96MHz/48MHz
 * are supported.
 */

/*******************************************************************************/
/* Header Files */
#include "usb_host_config.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main( void )
{
    /* Initialize system configuration */
    Delay_Init( );
    USART_Printf_Init( 115200 );
    DUG_PRINTF( "SystemClk:%d\r\n", SystemCoreClock );
    DUG_PRINTF( "USBHS HOST MTP Test\r\n" );

    /* Initialize USBHS host */
    /* Note: Only CH32V305/CH32V307 support USB high-speed port. */
#if DEF_USBHS_PORT_EN
    DUG_PRINTF( "USBHS Host Init\r\n" );
    USBHS_RCC_Init( );
    USBHS_Host_Init( ENABLE );
    memset( &RootHubDev[ DEF_USBHS_PORT_INDEX ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USBHS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL ].InterfaceNum, 0, sizeof( HOST_CTL ) );
#endif

    /* Initialize USBFS host */
#if DEF_USBFS_PORT_EN
    DUG_PRINTF( "USBFS Host Init\r\n" );
    USBFS_RCC_Init( );
    USBFS_Host_Init( ENABLE );
    memset( &RootHubDev[ DEF_USBFS_PORT_INDEX ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USBFS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL ].InterfaceNum, 0, sizeof( HOST_CTL ) );
#endif

    while( 1 )
    {
        USBH_MainDeal( );
    }
}
