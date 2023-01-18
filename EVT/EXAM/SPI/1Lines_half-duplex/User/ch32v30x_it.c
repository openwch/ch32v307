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
void SPI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
 * @fn      SPI1_IRQHandler
 *
 * @brief   This function handles SPI1 exception.
 *
 * @return  none
 */
void SPI1_IRQHandler(void)
{
#if (SPI_MODE == HOST_MODE)
	if( SPI_I2S_GetITStatus( SPI1, SPI_I2S_IT_TXE ) != RESET )
	{
		SPI_I2S_SendData( SPI1, TxData[Txval++] );
		if( Txval == 18 )
		{
			SPI_I2S_ITConfig( SPI1, SPI_I2S_IT_TXE , DISABLE );
		}
	}

#elif (SPI_MODE == SLAVE_MODE)
	RxData[Rxval++] = SPI_I2S_ReceiveData( SPI1 );

#endif
}



