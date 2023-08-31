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
 *  This code emulates a CDC-ECM compliant 10M/100M/1G Ethernet adapter for linux, android or unix-like systems, Using the CH32V307 series
 *  monolithic i.e. built-in USBHS controller and 10M/100M/1G MAC. The Ethernet MAC of CH32V307 series monolithic i.e. has
 *  built-in 10Megabit MAC+PHY, 10M/100M/1G MAC with RMII or MII Interface and 1G MAC with RGMII Interface.
 *USB Features:
 *  1,Update ETH status via end-point 1 every 200ms.
 *  2,Uploading data received from the MAC via endpoint 2 in CDC-ECM format.
 *  3,Receive Ethernet packets in CDC-ECM format via endpoint 3 and send them via MAC.
 *  4,Supported ECM type commands: SetEthernetPacketFilter only.
 *ETH Features:
 *  1,Ethernet MAC address obtained from chip unique code.
 *  2,Using built-in 10M/100M/1G MAC with 10MBase/RMII/MII/RGMII Interface.
 *  3,Support auto negotiation, Adaptive 10M/100M/1G Ethernet.
 *How to Change Interface(Phy)
 *  1,Find header file : eth_driver.h.
 *  2,Find macro definition : #define PHY_MODE  USE_MAC_MII.
 *  3,Change it to the interface you want, Default setting is USE_MAC_MII
 *  #define PHY_MODE  USE_10M_BASE   // Use internal 10M base Phy
 *  #define PHY_MODE  USE_MAC_RMII   // Use external 100M Phy with RMII interface
 *  #define PHY_MODE  USE_MAC_MII    // Use external 100M Phy with MII interface
 *  #define PHY_MODE  USE_MAC_RGMII  // Use external 1000M Phy with RGMII interface
 *  4,CH32V307 supports 100M phy like CH182 or RTL8201F, 1000M phy like RTL8211FS.
 */

/*What changed:
 *  1,ReWrite CDC-ECM network status load logic, make it simple.
 *  2,Update new eth_driver, support 10MBase/RMII/MII/RGMII Interface, can be changed with macro definition.
 *  3,Simplify Ethernet initialization writing.
 *  4,Modify the location of relevant variables to make the code easier to port and modify.
 *  5,Add PackFliters in.
 *  6,Some code cleanup
 * */

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
    uint8_t i;
    uint8_t MACAddr[ 6 ];

    SystemCoreClockUpdate( );
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    Delay_Init( );
    USART_Printf_Init( 921600 );
    
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USB Simulated NetWork Adaptor, Based on CDC-ECM\r\n" );

    /* MAC&Phy Initialize */
    ETH_GetMacAddr( MACAddr );
    printf( "MAC address:" );
    for( i=0; i<6; i++ )
    {
        printf( "%x ",MACAddr[ i ] );
    }
    printf("\r\n");

    /* USB initialize */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );

    while(1)
    {
        if( USBHS_DevEnumStatus && ( PhyInit_Flag == 0 ) )
        {
            /* MAC&Phy Initialize  */
            PhyInit_Flag = 1;
            ETH_NETWork_Status = 0;
            ETH_DriverInit( MACAddr );
            ETH_PhyAbility_Set( );
        }
        USB2ETH_Trance( );
        ETH2USB_Trance( );
    }
}

