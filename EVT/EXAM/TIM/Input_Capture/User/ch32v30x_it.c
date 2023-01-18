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
void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      TIM1_CC_IRQHandler
 *
 * @brief   This function handles TIM1  Capture Compare Interrupt exception.
 *
 * @return  none
 */
void TIM1_CC_IRQHandler(void)
{
	if( TIM_GetITStatus( TIM1, TIM_IT_CC1 ) != RESET )
	{
		printf( "CH1_Val:%d\r\n", TIM_GetCapture1( TIM1 ) );
		TIM_SetCounter( TIM1, 0 );
	}

	if( TIM_GetITStatus( TIM1, TIM_IT_CC2 ) != RESET )
	{
		printf( "CH2_Val:%d\r\n", TIM_GetCapture2( TIM1 ) );
	}

	TIM_ClearITPendingBit( TIM1, TIM_IT_CC1 | TIM_IT_CC2 );
}


