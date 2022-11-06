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
用于IAP实验例程：
可支持串口与USB进行FLASH烧录
注：
1. 用IAP下载工具，实现下载 PA0悬空(默认上拉输入)
2. 在下载完APP后，PA0接地（低电平输入），按复位键即可运行APP程序。
 
*/

#include "debug.h"
#include "ch32v30x_usbotg_device.h"
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_gpio.h"
#include "iap.h"
extern u8 End_Flag;
/*********************************************************************
 * @fn      IAP_2_APP
 *
 * @brief   IAP_2_APP program.
 *
 * @return  none
 */
void IAP_2_APP(void)
{
    USBHS_Device_Init( DISABLE );
    NVIC_EnableIRQ( USBHS_IRQn );
    USBHSD->HOST_CTRL=0x00;
    USBHSD->CONTROL=0x00;
    USBHSD->INT_EN=0x00;
    USBHSD->ENDP_CONFIG=0xFFffffff;
    USBHSD->CONTROL&=~USBHS_DEV_PU_EN;
    USBHSD->CONTROL|=USBHS_ALL_CLR|USBHS_FORCE_RST;
    USBHSD->CONTROL=0x00;
    Delay_Ms(50);
    printf("jump APP\r\n");
    Delay_Ms(10);
    NVIC_EnableIRQ(Software_IRQn);
    NVIC_SetPendingIRQ(Software_IRQn);
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{

	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

    USART3_CFG(57600);
    /* USB20 device init */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );
    NVIC_EnableIRQ( USBHS_IRQn );

    USBOTG_Init( );
	while(1)
	{

        if( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET){

            UART_Rx_Deal();
        }
        IWDG_ReloadCounter();
        if (End_Flag)
         {
             Delay_Ms(10);
             IAP_2_APP();
             while(1);
         }
	}
}
