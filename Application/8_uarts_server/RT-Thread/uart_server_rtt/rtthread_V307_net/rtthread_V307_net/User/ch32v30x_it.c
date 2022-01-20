/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v10x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : Main Interrupt Service Routines.
*******************************************************************************/
#include "ch32v30x_it.h"
#include "board.h"
#include <rtthread.h>
#include "bsp_uart.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    rt_kprintf(" NMI Handler\r\n");
    rt_interrupt_leave();
    FREE_INT_SP();
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();
    rt_kprintf(" hardfult\r\n");
    printf("mepc:%08x\r\n",__get_MEPC());
    printf("mcause:%08x\r\n",__get_MCAUSE());
    printf("mtval:%08x\r\n",__get_MTVAL());
    while(1);
    rt_interrupt_leave();
    FREE_INT_SP();
}


/**********************************************************************/
/* uart2  tx */
void DMA1_Channel7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel7_IRQHandler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_Cmd(DMA1_Channel7,DISABLE);
        uart_data_t[1].uart_tx_dma_state = IDEL;
        uart_data_t[1].tx_read++;
        DMA_ClearITPendingBit(DMA1_IT_TC7);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}




/**********************************************************************/
/* uart3  tx */
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_Cmd(DMA1_Channel2,DISABLE);
        uart_data_t[2].uart_tx_dma_state = IDEL;
        uart_data_t[2].tx_read++;
        DMA_ClearITPendingBit(DMA1_IT_TC2);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}





/**********************************************************************/
/* uart4  tx */
void DMA2_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel5_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC5))
    {
        DMA_Cmd(DMA2_Channel5,DISABLE);
        uart_data_t[3].uart_tx_dma_state = IDEL;
        uart_data_t[3].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC5);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}




/**********************************************************************/
/* uart5  tx */
void DMA2_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel4_IRQHandler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC4))
    {
        DMA_Cmd(DMA2_Channel4,DISABLE);
        uart_data_t[4].uart_tx_dma_state = IDEL;
        uart_data_t[4].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC4);

        //rt_kprintf("uart4 TC DMA2_Channel5->CNTR: %d \n",DMA2_Channel5->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}



/**********************************************************************/
/* uart6  tx */
void DMA2_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel6_IRQHandler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();


    if(DMA_GetITStatus(DMA2_IT_TC6))
    {
        DMA_Cmd(DMA2_Channel6,DISABLE);
        uart_data_t[5].uart_tx_dma_state = IDEL;
        uart_data_t[5].tx_read++;

        DMA_ClearITPendingBit(DMA2_IT_TC6);
        //rt_kprintf("uart6 TC DMA2_Channel6->CNTR: %d \n",DMA2_Channel6->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}




/**********************************************************************/
/* uart7  tx */
void DMA2_Channel8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel8_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC8))
    {
        DMA_Cmd(DMA2_Channel8,DISABLE);
        uart_data_t[6].uart_tx_dma_state = IDEL;
        uart_data_t[6].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC8);
        //rt_kprintf("uart7 TC DMA2_Channel8->CNTR: %d \n",DMA2_Channel8->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}




/**********************************************************************/
/* uart8  tx */
void DMA2_Channel10_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel10_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC10))
    {
        DMA_Cmd(DMA2_Channel10,DISABLE);
        uart_data_t[7].uart_tx_dma_state = IDEL;
        uart_data_t[7].tx_read++;

        DMA_ClearITPendingBit(DMA2_IT_TC10);

        //rt_kprintf("uart4 TC DMA2_Channel5->CNTR: %d \n",DMA2_Channel5->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}







