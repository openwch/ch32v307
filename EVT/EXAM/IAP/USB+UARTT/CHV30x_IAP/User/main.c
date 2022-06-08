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
2.在下载完APP后，PA0接地（低电平输入），按复位键即可运行APP程序。
 
*/

#include "debug.h"
#include "ch32v30x_usbotg_device.h"
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_gpio.h"
#include "iap.h"
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
	USART1_CFG(57600);
    if (PA0_Check() == 0) {

         printf("jump APP\r\n");

         NVIC_EnableIRQ(Software_IRQn);

         NVIC_SetPendingIRQ(Software_IRQn);

         while(1);
     }
    /* USB20 device init */
    USBHS_RCC_Init( );                                                         /* USB2.0高速设备RCC初始化 */
    USBHS_Device_Init( ENABLE );
    NVIC_EnableIRQ( USBHS_IRQn );

    USBOTG_Init( );
	while(1)
	{
        if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){
            UART_Rx_Deal();
        }
        IWDG_ReloadCounter();
	}
}
