/********************************** (C) COPYRIGHT *******************************
 * File Name          : system_ch32v30x.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : CH32V30x Device Peripheral Access Layer System Source File.
 *                      For HSE = 8Mhz
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *********************************************************************************/
#include "ch32v30x.h" 

/* 
 * Uncomment the line corresponding to the desired System clock (SYSCLK) frequency (after
 * reset the HSI is used as SYSCLK source).
 * If none of the define below is enabled, the HSI is used as System clock source.
 */


/* Clock Definitions */

uint32_t SystemCoreClock = HSI_VALUE; /* System Clock Frequency (Core Clock) */


__I uint8_t AHBPrescTable[16] =
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };

/* system_private_function_proto_types */
static void SetSysClock(void);

/*********************************************************************
 * @fn      SystemInit
 *
 * @brief   Setup the microcontroller system Initialize the Embedded Flash Interface,
 *        the PLL and update the SystemCoreClock variable.
 *
 * @return  none
 */
void SystemInit(void) {
    RCC->CTLR |= (uint32_t) 0x00000001;

#ifdef CH32V30x_D8C
    RCC->CFGR0 &= (uint32_t) 0xF8FF0000;
#else
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
#endif 

    RCC->CTLR &= (uint32_t) 0xFEF6FFFF;
    RCC->CTLR &= (uint32_t) 0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t) 0xFF80FFFF;

#ifdef CH32V30x_D8C
    RCC->CTLR &= (uint32_t) 0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;
#else
    RCC->INTR = 0x009F0000;
#endif   
    SetSysClock();
}

/*********************************************************************
 * @fn      SystemCoreClockUpdate
 *
 * @brief   Update SystemCoreClock variable according to Clock Register Values.
 *
 * @return  none
 */
void SystemCoreClockUpdate(void) {
    uint32_t tmp = 0, pllmull = 0, pllsource = 0, Pll_6_5 = 0;

    tmp = RCC->CFGR0 & RCC_SWS;

    switch (tmp) {
    case 0x00:
        SystemCoreClock = HSI_VALUE;
        break;
    case 0x04:
        SystemCoreClock = HSE_VALUE;
        break;
    case 0x08:
        pllmull = RCC->CFGR0 & RCC_PLLMULL;
        pllsource = RCC->CFGR0 & RCC_PLLSRC;
        pllmull = (pllmull >> 18) + 2;

#ifdef CH32V30x_D8
        if(pllmull == 17) pllmull = 18;
#else
        if (pllmull == 2)
            pllmull = 18;
        if (pllmull == 15) {
            pllmull = 13; /* *6.5 */
            Pll_6_5 = 1;
        }
        if (pllmull == 16)
            pllmull = 15;
        if (pllmull == 17)
            pllmull = 16;
#endif

        if (pllsource == 0x00) {
            SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
        } else {
            if ((RCC->CFGR0 & RCC_PLLXTPRE) != (uint32_t) RESET) {
                SystemCoreClock = (HSE_VALUE >> 1) * pllmull;
            } else {
                SystemCoreClock = HSE_VALUE * pllmull;
            }
        }

        if (Pll_6_5 == 1)
            SystemCoreClock = (SystemCoreClock / 2);

        break;
    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }

    tmp = AHBPrescTable[((RCC->CFGR0 & RCC_HPRE) >> 4)];
    SystemCoreClock >>= tmp;
}

/*********************************************************************
 * @fn      SetSysClock
 *
 * @brief   Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClock(void) {

    /* If none of the define above is enabled, the HSI is used as System clock
     * source (default after reset)
     */
}

