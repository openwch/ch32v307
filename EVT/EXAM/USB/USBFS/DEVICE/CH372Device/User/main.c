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
  模拟自定义USB设备（CH372设备）例程：
 OTG_FS_DM(PA11)、OTG_FS_DP(PA12)
  本例程演示使用 USBOTG-FS模拟自定义设备 CH372，端点1~7可自由上下传，下传数据将复制到上传，批量设备，最大包长64，
  可使用Bushund或其它上位机软件对设备进行操作。
*/

#include "debug.h"
#include "ch32v30x_usbotg_device.h"

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

    /* USBOTG_FS device init */
	printf( "CH372Device Running On USBOTG-FS Controller\n" );
	Delay_Ms(10);
	USBOTG_Init( );

	while(1)
	{ }
}
