/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/04/18
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include <wchnet.h>
#include "eth_driver.h"
#include "ch32v30x_it.h"
#include "app_iochub.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));

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

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");

    printf("mepc  :%08x\r\n", __get_MEPC());
    printf("mcause:%08x\r\n", __get_MCAUSE());
    printf("mtval :%08x\r\n", __get_MTVAL());
    while(1);
}

/*********************************************************************
 * @fn      EXTI9_5_IRQHandler
 *
 * @brief   This function handles GPIO exception.
 *
 * @return  none
 */
void EXTI9_5_IRQHandler(void)
{
    // ETH_PHYLink( );
    EXTI_ClearITPendingBit(EXTI_Line7);     /* Clear Flag */
}

/*********************************************************************
 * @fn      ETH_IRQHandler
 *
 * @brief   This function handles ETH exception.
 *
 * @return  none
 */
void ETH_IRQHandler(void)
{
    WCHNET_ETHIsr();
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 exception.
 *
 * @return  none
 */
 extern volatile uint8_t speedflg;
void TIM2_IRQHandler(void)
{
	static uint8_t counter = 0;
	static uint16_t speedcnt = 0;
	if(counter>=10)
	{
		counter = 0;
		WCHNET_TimeIsr(WCHNETTIMERPERIOD);
	}
    if (speedcnt > 1000) {
        speedflg = 1;
        speedcnt = 0;
    }
	speedcnt++;
	counter++;
	WCHIOCHUB_TimeIsr();
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
