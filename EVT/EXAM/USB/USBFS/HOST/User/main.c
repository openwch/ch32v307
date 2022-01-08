/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/10/26
* Description        : Main program body.
*******************************************************************************/
/*
 *@Note
  USBFS的简易枚举过程例程 (仅适用于CH32V305,CH32V307)：
  OTG_FS_DM(PA11)、OTG_FS_DP(PA12)。
  FS的OTG和H/D基地址一致，H/D不支持OTG功能。
*/
#include "stdio.h"
#include "string.h"
#include <ch32vf30x_usbfs_host.h>
#include "debug.h"

extern PUSBHS_HOST pOTGHost;
extern USBHS_HOST ThisUsbDev;
__attribute__ ((aligned(4))) UINT8 ComParebuff[sizeof(USBHS_HOST)];

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8 s;
    USART_Printf_Init(921600);
    Delay_Init();
    printf("SystemClk:%d\r\n",SystemCoreClock);
    USBOTG_HostInit(ENABLE);
    pOTGHost = &ThisUsbDev;

    while(1)
    {
        s = ERR_SUCCESS;
        if ( USBOTG_H_FS->INT_FG & USBHD_UIF_DETECT )
        {
            USBOTG_H_FS->INT_FG = USBHD_UIF_DETECT ;

            s = AnalyzeRootHub( );
            if ( s == ERR_USB_CONNECT )
            {
                printf( "New Device In\r\n" );
                FoundNewDev = 1;
            }
            if( s == ERR_USB_DISCON )
            {
                printf( "Device Out\r\n" );
            }
        }

        if ( FoundNewDev || s == ERR_USB_CONNECT )
        {
            FoundNewDev = 0;
            Delay_Ms( 200 );
            s = USBOTG_HostEnum( pOTGHost );
            if ( s == ERR_SUCCESS )
            {
                printf( "Enum Succeed\r\n" );
            }
            else printf( "Enum Failed:%02x\r\n", s );
        }
     }
}
