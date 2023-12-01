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
void TAMPER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
 * @fn      TAMPER_IRQHandler
 *
 * @brief   This function handles SysTick Handler.
 *
 * @return  none
 */
void TAMPER_IRQHandler(void)
{
  if( BKP_GetITStatus() == SET )
  {
#if 0
    printf("TAMPER_IRQHandler\r\n");
    printf( "BKP_DR1:%08x\r\n", BKP->DATAR1 );
    printf( "BKP_DR2:%08x\r\n", BKP->DATAR2 );
    printf( "BKP_DR3:%08x\r\n", BKP->DATAR3 );
    printf( "BKP_DR4:%08x\r\n", BKP->DATAR4 );

#endif
  }

  BKP_ClearITPendingBit();
}





