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
#include "ch32v30x_usbfs_device.h"
#include "debug.h"
#include "SPI_FLASH.h"
#include <SW_CDROM.h>
/* @Note
 * CD-ROM Example:
 * 1.This program provides examples of CD-ROM.The iso file TEST.iso used for testing is 
 * located in the TEST_ISO folder. 
 * 2.User needs to place the ISO at address 0 of the external Flash.The ISO file is in 
 * ISO 9660 Joliet Level 1 format and is suitable for a sector size of 512B. If other 
 * file systems are used, the sector size DEF_CFG_DISK_SEC_SIZE in the program needs to 
 * be adjusted.
 * 3.This Code Only Supports Winboard W25 Series flash, if user needs to use other type
 * of flash, you can add flash type by yourself in SPI_FLASH.c in user floder.
 * 4.External flash chip needs to be attached to SPI_1.
 * 5.CD_ROM iso file 'TEST.iso' contains wch serial-port driver, and autorun.inf.
 *  *//*********************************************************************
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
    printf( "USBD CD-ROM Demo\r\nStorage Medium: SPI FLASH \r\n" );

    /* SPI flash init */
    FLASH_Port_Init( );
    /* FLASH ID check */
    FLASH_IC_Check( );
    /* Enable CDROM */
    CDROM_Capability = Flash_Sector_Count;
    CDROM_Status |= DEF_CDROM_EN_FLAG;

	/* USBOTG_FS device init */
	USBFS_RCC_Init( );
	USBFS_Device_Init( ENABLE );

	while(1)
	{
	    ;
	}
}
