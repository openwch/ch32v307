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
IAP upgrade routine:
Support serial port and USB for FLASH burning

1. Use the IAP download tool to realize the download PA0 floating (default pull-up input)
2. After downloading the APP, connect PA0 to ground (low level input), and press the reset
button to run the APP program.
Note: FLASH operation keeps the frequency below 100Mhz, it is recommended that the main
frequency of IAP be below 100Mhz

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
    NVIC_DisableIRQ( USBHS_IRQn );
    USBHSD->HOST_CTRL=0x00;
    USBHSD->CONTROL=0x00;
    USBHSD->INT_EN=0x00;
    USBHSD->ENDP_CONFIG=0xFFffffff;
    USBHSD->CONTROL&=~USBHS_DEV_PU_EN;
    USBHSD->CONTROL|=USBHS_ALL_CLR|USBHS_FORCE_RST;
    USBHSD->CONTROL=0x00;
    USBOTG_FS->BASE_CTRL=0x06;
    USBOTG_FS->INT_EN=0x00;
    Delay_Ms(50);
    printf("jump APP\r\n");
    GPIO_DeInit(GPIOA);
    GPIO_DeInit( GPIOB);
    USART_DeInit(USART3);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, DISABLE);
    Delay_Ms(10);
    NVIC_DisableIRQ(OTG_FS_IRQn);
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

	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    if(PA0_Check() == 0)
    {
        IAP_2_APP();
        while(1);
    }
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
