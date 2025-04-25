/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/07/20
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 *General:
 *  This code emulates a CDC-NCM compliant 10M/100M/1G Ethernet adapter for linux, android or unix-like systems, Using the CH32V307 series
 *  monolithic i.e. built-in USBHS controller and 10M/100M/1G MAC. The Ethernet MAC of CH32V307 series monolithic i.e. has
 *  built-in 10Megabit MAC+PHY, 10M/100M/1G MAC with RMII or MII Interface and 1G MAC with RGMII Interface.
 *  !!!!!Important!!!!!
 *  This Project Must Be Set To FLASH-192K + RAM-128K,
 *  Link.ld File Needs To Be Changed To
 *  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 192K
 *  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
 *USB Features:
 *  1,Update ETH status via end-point 1 every 200ms.
 *  2,Uploading data received from the MAC via endpoint 2 in CDC-NCM format.
 *  3,Receive Ethernet packets in CDC-NCM format via endpoint 3 and send them via MAC.
 *  4,Supported NCM type commands: SetEthernetPacketFilter only.
 *ETH Features:
 *  1,Ethernet MAC address obtained from chip unique code.
 *  2,Using built-in 10M/100M/1G MAC with 10MBase/RMII/MII/RGMII Interface.
 *  3,Support auto negotiation, Adaptive 10M/100M/1G Ethernet.
 *How to Change Interface(Phy)
 *  1,There are 5 types of interfaces, they are 10M internal, CH32V317(100M internal), MII, RMII and RGMII;
 *  2,If you need to use one of them, such as eth_driver_10M.c, right-click on the corresponding file, select 'Include/Exclude From Build', 
 *  include it to bulid, and ensure that the others are not included. 
 *  3,Rebuild the project;
 */

/*What changed: 
 * 1,Update eth-drivers, use the same way as Ethernet Evts.
 * 2,Adjust the writing of other code according to the new stye of eth-drivers;
 * */

#include "cdc_ncm.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    uint8_t i;
    uint8_t MACAddr[ 6 ];

    SystemCoreClockUpdate( );
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    Delay_Init( );
    USART_Printf_Init( 115200 );
    
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USB Simulated NetWork Adaptor, Based on CDC-NCM\r\n" );

    /* Get MacAddress */
    ETH_GetMacAddr( MACAddr );
    printf( "MAC address:" );
    for( i=0; i<6; i++ )
    {
        printf( "%x ",MACAddr[ i ] );
    }
    printf("\r\n");
    /* change mac-addr to ecm/ncm format */
    MACAddr_Change_To_SNDesc( MACAddr );

    /* USB initialize */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );

    while(1)
    {
        USBETH_Main( );
    }
}

