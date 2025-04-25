/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/01/09
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * IAP routine: this routine support USB and UART mode,
 * and you can choose the command method or the IO method to jump to the APP .
 * Key  parameters: CalAddr - address in flash (same in APP), note that this address needs to be unused.
 *                  CheckNum - The value of 'CalAddr' that needs to be modified.
 * Tips :the routine need IAP software version 1.50.
 */

#include "debug.h"
#include "ch32v30x_usbfs_device.h"
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_gpio.h"
#include "iap.h"
extern u8 End_Flag;

#define UPGRADE_MODE_COMMAND   0
#define UPGRADE_MODE_IO        1

#define UPGRADE_MODE   UPGRADE_MODE_COMMAND
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
    USBFSD->BASE_CTRL=0x06;
    USBFSD->INT_EN=0x00;
    Delay_Ms(50);
    printf("jump APP\r\n");
    GPIO_DeInit(GPIOA);
    GPIO_DeInit( GPIOB);
    USART_DeInit(USART3);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, DISABLE);
    Delay_Ms(10);
    NVIC_DisableIRQ(USBFS_IRQn);
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
	printf("IAP\r\n");

#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
    if(*(uint32_t*)FLASH_Base  != 0xe339e339 )
    {
        if(*(uint32_t*)CalAddr != CheckNum)
        {
            IAP_2_APP();
            while(1);
        }
    }
#elif UPGRADE_MODE == UPGRADE_MODE_IO
    if(PA0_Check() == 0)
    {
        IAP_2_APP();
        while(1);
    }
#endif

    USART3_CFG(460800);
    /* USB20 device init */
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );
    NVIC_EnableIRQ( USBHS_IRQn );

    USBFS_Init( );
	while(1)
	{
        if( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET){

            UART_Rx_Deal();
        }
        IWDG_ReloadCounter();

#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
        if (End_Flag)
         {
            IAP_2_APP();
            while(1);
         }
#endif
	}
}

