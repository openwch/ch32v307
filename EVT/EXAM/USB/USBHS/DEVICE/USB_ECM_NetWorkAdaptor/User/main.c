/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/20
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 *General:
 *  This code emulates a CDC-ECM compliant 100Mbps Ethernet adapter for linux, android or unix-like systems, Using the CH32V307 series
 *  monolithic i.e. built-in USBHS controller and 100Megabit Ethernet MAC. The Ethernet MAC of CH32V307 series monolithic i.e. has
 *  built-in 10Megabit MAC+PHY, 100Megabit Ethernet MAC with RMII or MII Interface and 1-Gigabit Ethernet MAC with RGMI Interface.
 *USB Features:
 *  1,Update ETH status via end-point 1 every 200ms.
 *  2,Uploading data received from the MAC via endpoint 2 in CDC-ECM format.
 *  3,Receive Ethernet packets in CDC-ECM format via endpoint 3 and send them via MAC.
 *  4,Supported ECM type commands: SetEthernetPacketFilter only.
 *  5,not support remote wake-up by Ethernet packs or Ethernet link changes.
 *  6,USBHS Only.
 *ETH Features:
 *  1,Ethernet MAC address obtained from chip unique code.
 *  2,Using built-in 100Megabit Ethernet MAC with RMII Interface.
 *  3,Support auto negotiation, Adaptive 10M/100M Ethernet.
 */

#include "cdc_ecm.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    uint8_t  i;
    uint8_t MACAddr[ 6 ];

    SystemCoreClockUpdate( );
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    Delay_Init( );
    USART_Printf_Init( 115200 );
    
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USB Simulated NetWork Adaptor, Based on CDC-ECM\r\n" );

    /* MAC&Phy Initialize */
    PHY_GetMacAddr( MACAddr );
    printf( "MAC address:" );
    for( i=0; i<6; i++ )
    {
        printf( "%x ",MACAddr[ i ] );
    }
    printf("\r\n");
    MACAddr_Change_To_SNDesc( MACAddr );
    RB_Init( );
    TIM2_Init( );
    ETH_Init( MACAddr );

    /* USB initialize */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );

    while(1)
    {
        USB2ETH_Trance( );
        ETH2USB_Trance( );
    }
}

