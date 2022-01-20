/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/24
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
     待机模式开启LV时，RAM 2K、30K低功耗数据保持例程：
    本例程演示在 2K RAM和30K RAM指定位置处写入数据，之后WFI进入STANDBY睡眠模式并开启LV，
    通过PA0(wakeup)引脚输入高电平退出待机模式，唤醒后打印RAM数据，测试RAM是否保持住数据。
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
        *(uint32_t volatile *)(myAddr+(i<<2)) = 0x44444444;
    }
    for(i=0;i<1024;i++)//Check 30K RAM
    {
        if((*(uint32_t volatile *)(myAddr+(i<<2))) != 0x44444444)
        {
            printf("Write 30K RAM Error!\r\n");
            break;
        }
    }
    printf("Write 30K RAM Successfully!\r\n");
    for(i=0;i<10;i++)//Write 2K RAM
    {
        *(uint32_t volatile *)(myAddr2+(i<<2)) = 0x44444444;
    }
    for(i=0;i<10;i++)//Check 30K RAM
    {
        if((*(uint32_t volatile *)(myAddr2+(i<<2))) != 0x44444444)
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

    /* Configure unused GPIO as IPU or IPD to reduce power consumption */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|
             RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Delay 1.5s to avoid entering low power mode immediately after reset*/
    Delay_Ms(1500);
    while(1)
    {
        TestDataRead();
        printf("4.Standby Mode RAM LV Test\r\n");
        TestDataWrite();
        PWR_WakeUpPinCmd(ENABLE);
        PWR_EnterSTANDBYMode_RAM_LV();
        printf("\r\n4.Out \r\n");
        printf("\r\n ########## \r\n");

    }
}
