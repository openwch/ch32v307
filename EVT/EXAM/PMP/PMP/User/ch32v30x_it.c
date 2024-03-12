/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32v30x_it.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SW_Handler() __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
}

extern uint32_t FinalOprateAddress;
/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    /* The code is printing out information related to a hard fault exception. */
    printf("I'm in the hardfault\r\n");
    printf("Last Oprate Address is %#p\r\n", FinalOprateAddress);
    printf("MCAUSE: %d\r\n", __get_MCAUSE());
    printf("MEPC: %d\r\n", __get_MEPC());
    /* The code is using a switch statement to check the value of the `__get_MCAUSE()` function.
    Depending on the value returned by this function, different actions are taken. */
    switch (__get_MCAUSE())
    {
        case 1:
            printf("The address you excuse is in pmp range!!\r\n");
            break;
        case 7:
            printf("The address you Store is in pmp range!!\r\n");
            break;
        case 5:
            printf("The address you read is in pmp range!!\r\n");
            break;

        default:
            break;
    }
    while (1)
    {
    }
}

