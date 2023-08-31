/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/07/31
* Description        : main program body.
*********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 *General:
 *  This code emulates a Remote NDIS compliant 10M/100M/1G Ethernet adapter for windows10/11 or linux if driver supported, Using the CH32V307 series
 *  monolithic i.e. built-in USBHS controller and 10M/100M/1G MAC. The Ethernet MAC of CH32V307 series monolithic i.e. has
 *  built-in 10Megabit MAC+PHY, 10M/100M/1G MAC with RMII or MII Interface and 1G MAC with RGMII Interface.
 *USB Features:
 *  1,Update ETH status every 200ms, load status via REMOTE_NDIS_INDICATE_STATUS_MSG.
 *  2,Uploading data received from the MAC via endpoint 2 in Remote NDIS format.
 *  3,Receive Ethernet packets in CDC-ECM format via endpoint 3 and send them via MAC.
 *ETH Features:
 *  1,Ethernet MAC address obtained from chip unique code.
 *  2,Using built-in 10M/100M/1G MAC with 10MBase/RMII/MII/RGMII Interface.
 *  3,Support auto negotiation, Adaptive 10M/100M/1000M Ethernet.
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
#include "usbhs_rndis.h"

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

    SystemCoreClockUpdate( );
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    Delay_Init( );
    USART_Printf_Init( 921600 );
    
    printf( "SystemClk:%d\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USB Simulated NetWork Adaptor, Based on RNDIS\r\n" );

    /* MAC&Phy Initialize */
    ETH_GetMacAddr( MAC_Address );
    printf( "MAC address:\r\n" );
    for( i = 0; i < 5; i++ )
    {
        printf( "%02x:", MAC_Address[ i ] );
    }
    printf("%02x\r\n", MAC_Address[ i ] );

    /* USB initialize */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );

    while( 1 )
    {
        if( USBHS_DevEnumStatus && ( PhyInit_Flag == 0 ) )
        {
            /* MAC&Phy Initialize  */
            PhyInit_Flag = 1;
            ETH_NETWork_Status = 0;
            ETH_DriverInit( MAC_Address );
            ETH_PhyAbility_Set( );
        }
        USB2ETH_Trance( );
        ETH2USB_Trance( );
    }
}

