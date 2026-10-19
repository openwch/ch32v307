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

#include "eth_driver.h"
#include "ch32v30x_it.h"
#include "app_iochub.h"
#include "app_uart.h"

uint32_t g_DmaTxCount[2] = {0};
uint32_t g_DmaRxCount[6] = {0};
uint32_t g_DmaTxErrCount[4] = {0};

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    /*An NMI source that is not cleared would re-enter this handler forever,
     *so keep the core halted here instead of returning into the main loop.*/
    while (1)
    {
    }
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
    NVIC_SystemReset();
    while(1);
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
void TIM2_IRQHandler (void) {
    static uint8_t counter = 0;

    counter++;
    if (counter >= WCHNETTIMERPERIOD) {
        counter = 0;
        WCHNET_TimeIsr (WCHNETTIMERPERIOD);
    }

    WCHIOCHUB_TimeIsr();
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
}

/*********************************************************************
 * @fn      DMA1_Channel2_IRQHandler
 *
 * @brief   uart3 tx
 *
 * @return  none
 */
void DMA1_Channel2_IRQHandler (void) {
    if (DMA_GetITStatus (DMA1_IT_TE2)) {
        DMA_ClearFlag (DMA1_IT_TE2);
    }

    if (DMA_GetITStatus (DMA1_IT_TC2)) {
        uart_dmatx_done_isr (DEV_UART3);
        DMA_ClearFlag (DMA1_FLAG_TC2);
        DMA_Cmd (DMA1_Channel2, DISABLE);
    }
}

/*********************************************************************
 * @fn      DMA1_Channel3_IRQHandler
 *
 * @brief   uart3 rx
 *
 * @return  none
 */
void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TE3))
	{
		DMA_ClearFlag(DMA1_IT_TE3);
	}
	
    if(DMA_GetITStatus(DMA1_IT_HT3))
	{
		DMA_ClearFlag(DMA1_FLAG_HT3);
		uart_dmarx_half_done_isr(DEV_UART3);
	}
	if(DMA_GetITStatus(DMA1_IT_TC3))
	{
		DMA_ClearFlag(DMA1_FLAG_TC3);
		uart_dmarx_done_isr(DEV_UART3);
	}
}

/*********************************************************************
 * @fn      USART3_IRQHandler
 *
 * @brief   uart3 IRQHandler
 *
 * @return  none
 */
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		uart_dmarx_idle_isr(DEV_UART3);
		USART_ReceiveData(USART3);//clear idle flag
	}
}