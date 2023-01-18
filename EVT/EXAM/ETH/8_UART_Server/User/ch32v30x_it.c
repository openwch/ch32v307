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
#include "eth_driver.h"
#include "WCHNET.H"
#include "bsp_uart.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
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
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 exception.
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    WCHNET_TimeIsr(WCHNETTIMERPERIOD);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

#ifndef CH32V307_DEBUG
/*********************************************************************
 * @fn      DMA1_Channel4_IRQHandler
 *
 * @brief   uart1 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel4_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
    	/* disable uart tx dma */
        DMA_Cmd(DMA1_Channel4,DISABLE);

        /* set uart tx dma state idel */
        uart_data_t[0].uart_tx_dma_state = IDEL;

        /* update tx_read */
        uart_data_t[0].tx_read++;

        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }

}
#endif

/*********************************************************************
 * @fn      DMA1_Channel7_IRQHandler
 *
 * @brief   uart2 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA1_Channel7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel7_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_Cmd(DMA1_Channel7,DISABLE);
        uart_data_t[1].uart_tx_dma_state = IDEL;
        uart_data_t[1].tx_read++;
        DMA_ClearITPendingBit(DMA1_IT_TC7);
    }

}

/*********************************************************************
 * @fn      DMA1_Channel2_IRQHandler
 *
 * @brief   uart3 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_Cmd(DMA1_Channel2,DISABLE);
        uart_data_t[2].uart_tx_dma_state = IDEL;
        uart_data_t[2].tx_read++;
        DMA_ClearITPendingBit(DMA1_IT_TC2);
    }
}


/*********************************************************************
 * @fn      DMA2_Channel5_IRQHandler
 *
 * @brief   uart4 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA2_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel5_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA2_IT_TC5))
    {
        DMA_Cmd(DMA2_Channel5,DISABLE);
        uart_data_t[3].uart_tx_dma_state = IDEL;
        uart_data_t[3].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC5);
    }

}


/*********************************************************************
 * @fn      DMA2_Channel4_IRQHandler
 *
 * @brief   uart5 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA2_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel4_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA2_IT_TC4))
    {
        DMA_Cmd(DMA2_Channel4,DISABLE);
        uart_data_t[4].uart_tx_dma_state = IDEL;
        uart_data_t[4].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC4);
    }
}


/*********************************************************************
 * @fn      DMA2_Channel6_IRQHandler
 *
 * @brief   uart6 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA2_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel6_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA2_IT_TC6))
    {
        DMA_Cmd(DMA2_Channel6,DISABLE);
        uart_data_t[5].uart_tx_dma_state = IDEL;
        uart_data_t[5].tx_read++;

        DMA_ClearITPendingBit(DMA2_IT_TC6);
    }

}

/*********************************************************************
 * @fn      DMA2_Channel8_IRQHandler
 *
 * @brief   uart7 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA2_Channel8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel8_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA2_IT_TC8))
    {
        DMA_Cmd(DMA2_Channel8,DISABLE);
        uart_data_t[6].uart_tx_dma_state = IDEL;
        uart_data_t[6].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC8);
    }

}

/*********************************************************************
 * @fn      DMA2_Channel10_IRQHandler
 *
 * @brief   uart8 dma tx completion interrupt.
 *
 * @return  none
 */
void DMA2_Channel10_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel10_IRQHandler(void)
{


    if(DMA_GetITStatus(DMA2_IT_TC10))
    {
        DMA_Cmd(DMA2_Channel10,DISABLE);
        uart_data_t[7].uart_tx_dma_state = IDEL;
        uart_data_t[7].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC10);
    }

}
