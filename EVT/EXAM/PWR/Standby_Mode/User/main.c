/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2026/10/14
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *low power, standby mode routine:
 *WKUP(PA0)
 *This routine demonstrates that WFI enters the standby mode, the rising edge of the WKUP (PA0) pin
 *exits the standby mode,Program reset after wake-up.
 *For the small package model of the chip, there are some pins that have not been led out 
 *compared to the largest package, or some pins that have been packaged but not used. 
 *These pins need to be set as pull-down\up inputs to reduce power consumption.Please refer 
 *to the routine configuration for details.
 */

#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(1000);
    /* To reduce power consumption, unused GPIOs need to be set as pull-down inputs. */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|
    RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    USART_Printf_Init(115200);	
    printf("SystemClk:%u\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("Standby Mode Test\r\n");

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        printf("\r\n Standby wake up reset \r\n");
    }
    else
    {
        printf("\r\n Power on reset \r\n");
        PWR_WakeUpPinCmd(ENABLE);
        PWR_EnterSTANDBYMode();
    }

    printf("\r\n ########## \r\n");
    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}
