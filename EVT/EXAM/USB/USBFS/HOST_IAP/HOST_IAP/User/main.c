/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
  USB host IAP demo code.
  USBFS HOST USB drive enumeration and operation, read the APP.BIN file in the root directory inside the USB drive, write to location 0x08006000, after a successful check
  Write the flag data, then jump to the user code. The code will run IAP first to check the flag data, if the preparation data is normal, it will jump to the user code, otherwise
  If the compiled data is OK, it will jump to the user code, if not, it will stay in IAP again.
    Support: FAT12/FAT16/FAT32
*/

#include "usb_host_iap.h"
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
    /* System Debug Initialization */
    SystemCoreClockUpdate( );
    Delay_Init( );
    USART_Printf_Init (115200 );
    
    printf( "SystemClk:%d.\r\n", SystemCoreClock );
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf( "USBFS HOST, UDisk IAP.\r\n" );

    /* IAP initialization */
    IAP_Initialization( );

    while(1)
    {
        IAP_Main_Deal( );
    }
}

