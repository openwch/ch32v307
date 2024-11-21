/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/02/21
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * @note
 * Setting the PMP(Physical memory protection) Range.It will enter the hardfault once did operations
 * witch is limited.Since PMP can only be configured in machine mode, the program enters a software
 * interrupt to configure it. You can also configure it within the .s file, since it hasn't switched
 * back to user mode until the mret statement is executed in the .s file
 */

#include "debug.h"

/* Global define */
#define PMP_MODE_TOR 0x00000008
#define PMP_MODE_NA4 0x00000010
#define PMP_MODE_NAPOT 0x00000018

#define PMP_AUTHOR_R 0x00000001
#define PMP_AUTHOR_W 0x00000002
#define PMP_AUTHOR_X 0x00000004

#define PMP_LOCK 0x00000080

#define PMP_MODE PMP_MODE_NAPOT
/* Global Variable */
volatile uint32_t pmp_grop, pmp_mode, pmp_start_address, pmp_end_address;
volatile uint32_t *pmpaddrbase;

/*********************************************************************
 * @fn SW_Handler
 *
 * @brief The above function is an interrupt handler that sets the PMP (Physical Memory Protection) address
 *      and configuration based on certain conditions.
 *
 * @return none
 */
__attribute__((interrupt("WCH-Interrupt-fast"))) void SW_Handler()
{
    if ((pmp_mode >> (pmp_grop * 8) & 0x00000018) == PMP_MODE_TOR)
    {
        if (pmp_grop == 0)
        {
            __asm volatile("csrw pmpaddr0 , %0" ::"r"(pmp_end_address));
        }
        else if (pmp_grop == 1)
        {
            __asm volatile("csrw pmpaddr0 , %0" ::"r"(pmp_start_address));
            __asm volatile("csrw pmpaddr1 , %0" ::"r"(pmp_end_address));
        }
        else if (pmp_grop == 2)
        {
            __asm volatile("csrw pmpaddr1 , %0" ::"r"(pmp_start_address));
            __asm volatile("csrw pmpaddr2 , %0" ::"r"(pmp_end_address));
        }
        else if (pmp_grop == 3)
        {
            __asm volatile("csrw pmpaddr2 , %0" ::"r"(pmp_start_address));
            __asm volatile("csrw pmpaddr3 , %0" ::"r"(pmp_end_address));
        }
    }
    else if ((pmp_mode & (PMP_MODE_NA4 << (pmp_grop * 8))) || (pmp_mode & (PMP_MODE_NAPOT << (pmp_grop * 8))))
    {
        if (pmp_grop == 0)
        {
            __asm volatile("csrw pmpaddr0 , %0" ::"r"(pmp_start_address));
        }
        else if (pmp_grop == 1)
        {
            __asm volatile("csrw pmpaddr1 , %0" ::"r"(pmp_start_address));
        }
        else if (pmp_grop == 2)
        {
            __asm volatile("csrw pmpaddr2 , %0" ::"r"(pmp_start_address));
        }
        else if (pmp_grop == 3)
        {
            __asm volatile("csrw pmpaddr3 , %0" ::"r"(pmp_start_address));
        }
    }
    __asm volatile("csrw pmpcfg0 , %0" ::"r"(pmp_mode));
}

/*********************************************************************
 * @fn  PMP_Set_Range
 *
 * @brief The function `PMP_Set_Range` sets the range and mode for the Physical Memory Protection (PMP)
 *      feature.
 *
 * @param grop The parameter "grop" represents the group number for the PMP (Physical Memory
 *           Protection) setting. It is used to determine which PMP group the range setting will be applied to.
 *        md The parameter "md" stands for "mode" and it is used to specify the PMP mode.
 *        sd The parameter "sd" stands for "start address". It represents the starting address of the
 *      memory range for which the PMP (Physical Memory Protection) settings are being configured.
 *        ed The parameter "ed" in the function PMP_Set_Range represents the end address of the range
 *      for the PMP (Physical Memory Protection) setting.But when you set the PMP mode as PMP_MODE_NAPOT,
 *      the parameter en means the size of pmp is 2^(en+2).Please refer to the manual(QingKeV4_Processor_Manual.PDF)
 *      for details
 *
 * @return none
 */
void PMP_Set_Range(uint32_t grop, uint32_t md, uint32_t sd, uint32_t ed)
{
#warning "Please check the pmp setting carefally"

    pmp_start_address = sd >> 2;
    if ((md & PMP_MODE_NAPOT) == PMP_MODE_NAPOT)
    {
        uint32_t temp = 0xffffffff;
        for (int i = 0; i < ed; i++)
        {
            temp <<= 1;
        }
        pmp_start_address &= temp;
        temp = 0;
        for (int i = 0; i < ed - 1; i++)
        {
            temp<<= 1;
            temp |= 0x00000001;
        }
        pmp_start_address |= temp;
    }
    else
    {
        pmp_end_address = ed >> 2;
    }

    pmp_mode = md << (8 * grop);
    pmp_grop = grop;
    NVIC_EnableIRQ(Software_IRQn);
    NVIC_SetPendingIRQ(Software_IRQn);
}

volatile uint32_t ProtectSec[0x400] = {1, 2, 3, 4, 5, 6, 7};

uint32_t FinalOprateAddress;
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    printf("PMP TEST\r\n");

    /* The code snippet is setting up the PMP (Physical Memory Protection) range and mode. */
#if PMP_MODE == PMP_MODE_TOR
    /*USE PMP_MODE_TOR ProtectSec[0x0] - ProtectSec[0x200] is not writable nor excusable*/
    PMP_Set_Range(1, PMP_MODE_TOR | PMP_AUTHOR_R, (uint32_t)(ProtectSec),
                  (uint32_t)&ProtectSec[0x200]);
#elif PMP_MODE == PMP_MODE_NA4
    /*USE PMP_MODE_TOR (uint32_t)ProtectSec[0] is not writable nor excusable*/
    PMP_Set_Range(0, PMP_MODE_NA4 | PMP_AUTHOR_R, (uint32_t)(ProtectSec),
                  (uint32_t)0);
#elif PMP_MODE == PMP_MODE_NAPOT
    /*USE PMP_MODE_TOR ProtectSec[0]-ProtectSec[2^(2+2)] is not writable nor excusable*/
    PMP_Set_Range(0, PMP_MODE_NAPOT | PMP_AUTHOR_R, (uint32_t)(ProtectSec),
                  (uint32_t)2);
#endif

    /* The code snippet is initializing the memory range `ProtectSec` by setting all its elements to 0. */
    FinalOprateAddress = (uint32_t)&ProtectSec[sizeof(ProtectSec) / sizeof(*ProtectSec) - 1];
    for (uint32_t z = (uint32_t)&ProtectSec[sizeof(ProtectSec) / sizeof(*ProtectSec) - 1];
         z > (uint32_t)ProtectSec; z -= 1)
    {
        FinalOprateAddress = *(uint8_t *)z;
        *(uint8_t *)z = 0;
    }

    printf("Operation succeed!!\r\n");
    while (1)
    {
        Delay_Ms(1000);
    }
}
