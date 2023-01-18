/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/24
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 when LV is not enabled in standby mode, RAM 2k and 30K low-power data holding routines:
 This routine demonstrates writing data at the specified location of 2K RAM and 30K RAM, and then WFI enters
 STANDBY sleep mode, through the PA0 (wakeup) pin Input high level to exit standby mode, print RAM data after
 waking up, and test whether RAM is maintained data.

*/

#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      TestDataWrite
 *
 * @brief   Write 0x11111111 to certain address of 2K RAM and 30K RAM.
 *
 * @return  none
 */
void TestDataWrite(void)
{
    uint32_t myAddr=0x20000000+10*1024;
    uint32_t myAddr2=0x20000000+1*1024+512;
    uint32_t i=0;
    for(i=0;i<1024;i++)//Write 30K RAM
    {
        *(uint32_t volatile *)(myAddr+(i<<2)) = 0x33333333;
    }
    for(i=0;i<1024;i++)//Check 30K RAM
    {
        if((*(uint32_t volatile *)(myAddr+(i<<2))) != 0x33333333)
        {
            printf("Write 30K RAM Error!\r\n");
            break;
        }
    }
    printf("Write 30K RAM Successfully!\r\n");
    for(i=0;i<10;i++)//Write 2K RAM
    {
        *(uint32_t volatile *)(myAddr2+(i<<2)) = 0x33333333;
    }
    for(i=0;i<10;i++)//Check 30K RAM
    {
        if((*(uint32_t volatile *)(myAddr2+(i<<2))) != 0x33333333)
        {
            printf("Write 2K RAM Error!\r\n");
            break;
        }
    }
    printf("Write 2K RAM Successfully!\r\n");

}

/*********************************************************************
 * @fn      TestDataRead
 *
 * @brief   Print the data of certain address of 2K RAM and 30K RAM.
 *
 * @return  none
 */
void TestDataRead(void)
{
    uint32_t myAddr=0x20000000+10*1024;
    uint32_t myAddr2=0x20000000+1*1024+512;
    uint32_t i=0;
    printf("2K RAM:\r\n");
    for(i=0;i<10;i++)
    {
        printf("0x%08lx=0x%08lx\r\n",myAddr2+(i<<2),*(uint32_t volatile *)(myAddr2+(i<<2)));
    }
    printf("32K RAM:\r\n");
    for(i=0;i<1024;i++)
    {
        printf("0x%08lx=0x%08lx\r\n",myAddr+(i<<2),*(uint32_t volatile *)(myAddr+(i<<2)));
    }

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
    GPIO_InitTypeDef GPIO_InitStructure={0};
    SystemCoreClockUpdate();
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    /* Configure unused GPIO as IPD to reduce power consumption */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|
             RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Delay 1.5s to avoid entering low power mode immediately after reset*/
    Delay_Ms(1500);
    while(1)
    {
        TestDataRead();
        printf("3.Stop RAM LV Mode Test\r\n");
        TestDataWrite();
        PWR_WakeUpPinCmd(ENABLE);
        PWR_EnterSTANDBYMode_RAM();
        printf("\r\n3.Out \r\n");
        printf("\r\n ########## \r\n");

    }
}

