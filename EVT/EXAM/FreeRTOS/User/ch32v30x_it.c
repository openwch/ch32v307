/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main Interrupt Service Routines.
*******************************************************************************/
#include "ch32v30x_it.h"

void NMI_Handler(void) __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    while(1)
    {

    }
}



