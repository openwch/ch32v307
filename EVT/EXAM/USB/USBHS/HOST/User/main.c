/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note 
  USBHS设备的简易枚举过程例程：
  USBHS_DM(PB6)、USBHS_DM(PB7)

*/

#include "debug.h"
#include "ch32v30x_usbhs_host.h"

__attribute__ ((aligned(4))) UINT8   endpTXbuf[ MAX_PACKET_SIZE ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpRXbuf[ MAX_PACKET_SIZE ];  // OUT, must even addres



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8 ret;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	USBHS_HostInit(ENABLE);

	while(1)
	{
        if( USBHSH->INT_FG & USBHS_DETECT_FLAG )
        {
            USBHSH->INT_FG = USBHS_DETECT_FLAG;
            if( USBHSH->MIS_ST & USBHS_ATTCH )
            {
                ret = USBHS_HostEnum( endpRXbuf);
                if( ret == ERR_SUCCESS )
                {
                }
                else
                {
                    printf("enum error\n");
                }
            }
            else
            {
                USBHS_HostInit(DISABLE);
                USBHS_HostInit(ENABLE);
                printf("disconnect\n");
            }
        }
	}
}




