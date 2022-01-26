/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main Interrupt Service Routines.
*******************************************************************************/

#include "ch32v30x_it.h"
#include "WCHNET.H"
#include "bsp_uart.h"
#include "debug.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");


printf("mepc  :%08x\r\n",__get_MEPC());
printf("mcause:%08x\r\n",__get_MCAUSE());
printf("mtval :%08x\r\n",__get_MTVAL());
while(1);

}

volatile u32  LTime = 0;
/*******************************************************************************
* Function Name  : ETH_IRQHandler
* Description    : This function handles ETH exception.
* Input          : None
* Return         : None
*******************************************************************************/
void ETH_IRQHandler(void)
{
    WCHNET_ETHIsr();
}

void TIM2_IRQHandler(void)
{
    WCHNET_TimeIsr(WCHNETTIMERPERIOD);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );
}

#ifndef CH32V307_DEBUG
/**********************************************************************/
/* uart1  tx */
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

/**********************************************************************/
/* uart2  tx */
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

/**********************************************************************/
/* uart3  tx */
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





/**********************************************************************/
/* uart4  tx */
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




/**********************************************************************/
/* uart5  tx */
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



/**********************************************************************/
/* uart6  tx */
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




/**********************************************************************/
/* uart7  tx */
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




/**********************************************************************/
/* uart8  tx */
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
