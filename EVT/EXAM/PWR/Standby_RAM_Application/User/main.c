/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/07/09
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 * In STANDBY mode, 2K bytes RAM and 30K bytes RAM data holding routines:
 * Address range of  2K bytes RAM data holding: 0x20000000 ¡ª 0x20000000+2K .
 * Address range of 30K bytes RAM data holding: 0x20000000+2K ¡ª 0x20000000+2K+30K .
 * This example demonstrates the RAM data holding in standby mode (chip VDD power supply or VBAT power
 * supply,and RAM work at different voltages). Write data to 2K RAM and 30K RAM respectively, then
 * execute WFI to enter STANDBY mode, exit STANDBY mode by inputting a high level to the PA0 (wake-up)
 * pin, print RAM data after waking up, and test whether RAM retains data.
 *
 */

#include "debug.h"


#define KEEP_2kRAM   __attribute__((section(".keep_2kram"))) //Address space for  2K bytes of RAM.
#define KEEP_30kRAM  __attribute__((section(".keep_30kram")))//Address space for 30K bytes of RAM.

/* Power supply mode definition */
#define  VDD_POWER_MODE  0
#define  VBAT_POWER_MODE 1

#define  POWER_MODE  VDD_POWER_MODE

/* RAM low voltage working mode definition */
#define  RAMLV 1


/* Global Variable */
KEEP_2kRAM   u32 DataBuf0[256];
KEEP_30kRAM  u32 DataBuf1[1024];


/*********************************************************************
 * @fn      TestDataWrite
 *
 * @brief   Write data in 2K RAM and 30K RAM.
 *
 * @return  none
 */
void TestDataWrite(void)
{
    uint32_t i=0;
    /* Write data in 2K RAM */
    for( i=0; i<256; i++ )
    {
       DataBuf0[i] = 0x22222222;
    }
    printf("Write data in 2K RAM Successfully!\r\n");
    /* Write data in 30K RAM */
    for( i=0; i<1024; i++ )
    {
        DataBuf1[i] = 0x33333333;
    }
    printf("Write data in 30K RAM Successfully!\r\n");
}

/*********************************************************************
 * @fn      TestDataRead
 *
 * @brief   Read data written in 2K RAM and 30K RAM.
 *
 * @return  none
 */
void TestDataRead(void)
{
    uint32_t i=0;
    printf("Read data in 2K RAM:\r\n");
    for( i=0; i<256; i++ )
    {
        printf("DataBuf0#%d = 0x%08lx\r\n",i,DataBuf0[i]);
    }
    printf("Read data in 30K RAM:\r\n");
    for( i=0; i<1024; i++ )
    {
        printf("DataBuf1#%d = 0x%08lx\r\n",i,DataBuf1[i]);
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

    /* Configure unused GPIO as IPD to reduce power consumption */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
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

    TestDataRead();
    TestDataWrite();
    PWR_WakeUpPinCmd(ENABLE);
#if(POWER_MODE == VDD_POWER_MODE)
    #if RAMLV
        printf("\r\n***Start RAM_LV Data Hold Test \r\n");
        PWR_EnterSTANDBYMode_RAM_LV();
    #else
        printf("\r\n***Start RAM Data Hold Test \r\n");
        PWR_EnterSTANDBYMode_RAM();
    #endif
#elif(POWER_MODE == VBAT_POWER_MODE)
    #if RAMLV
        printf("\r\n***Start VBAT_EN RAM_LV Data Hold Test \r\n");
        PWR_EnterSTANDBYMode_RAM_LV_VBAT_EN();
    #else
        printf("\r\n***Start VBAT_EN RAM Data Hold Test \r\n");
        PWR_EnterSTANDBYMode_RAM_VBAT_EN();
    #endif
#endif
    while(1);
}

