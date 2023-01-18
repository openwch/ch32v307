/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
  1. This routine uses synchronous triggering to demonstrate the use of interrupt nesting.
  The maximum nesting level is 8, and the hardware stack only Save the lower three levels,
  high priority level 5 interrupts need to use software push stack, low priority level 3 can
  Use hardware push or software push.

  2. If you only use hardware push, you can configure the nesting level to be 4 and close
  the global when the hardware push overflows Interrupt (configure CSR (0x804) in the startup
  file, the configuration value is changed from 0x1f to 0x0b)

  3. If you don��t use hardware push stack, configure bit0 of CSR (0x804) to clear to 0,
  and remove the interrupt function declaration "WCH-Interrupt-fast" keyword

*/
#include "debug.h"

/* Global define */


/* Global Variable */ 


void WWDG_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void PVD_IRQHandler(void)    __attribute__((interrupt("WCH-Interrupt-fast")));
void TAMPER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTC_IRQHandler(void)    __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void FLASH_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void RCC_IRQHandler(void)    __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void EXTI0_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void EXTI1_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));

/*********************************************************************
 * @fn      Interrupt_Init
 *
 * @brief   Initializes interruption.
 *
 * @return  none
 */
void Interrupt_Init(void)
{
    NVIC_EnableIRQ(WWDG_IRQn);
    NVIC_EnableIRQ(PVD_IRQn);
    NVIC_EnableIRQ(TAMPER_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_EnableIRQ(FLASH_IRQn);
    NVIC_EnableIRQ(RCC_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);

    NVIC_SetPriority(WWDG_IRQn,  (7<<5) | (0x01<<4)); /* Group priority 7, sub-priority 1, lower overall priority */
    NVIC_SetPriority(PVD_IRQn,   (6<<5) | (0x01<<4));
    NVIC_SetPriority(TAMPER_IRQn,(5<<5) | (0x01<<4));
    NVIC_SetPriority(RTC_IRQn,   (4<<5) | (0x01<<4));
    NVIC_SetPriority(FLASH_IRQn, (3<<5) | (0x01<<4));
    NVIC_SetPriority(RCC_IRQn,   (2<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI0_IRQn, (1<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI1_IRQn, (0<<5) | (0x01<<4));/* Group priority 0, sub-priority 1, overall priority is higher */
}


uint8_t step=1;
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
	USART_Printf_Init(115200);
		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("Interrupt Nest Test\r\n");
	Interrupt_Init();
	printf("Enter lowest interrupt\r\n");
    NVIC_SetPendingIRQ(WWDG_IRQn);

    printf("Quit lowest interrupt\r\n");
    printf("Quit step:%d\r\n",step);

    while(1)
   {

   }
}

/*********************************************************************
 * @fn      WWDG_IRQHandler
 *
 * @brief   This function handles WWDG exception.
 *
 * @return  none
 */
void WWDG_IRQHandler(void)
{
    step++;
    printf(" 1.WWDG_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(PVD_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      PVD_IRQHandler
 *
 * @brief   This function handles PVD exception.
 *
 * @return  none
 */
void PVD_IRQHandler(void)
{
    step++;
    printf(" 2.PVD_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(TAMPER_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      TAMPER_IRQHandler
 *
 * @brief   This function handles TAMPER exception.
 *
 * @return  none
 */
void TAMPER_IRQHandler(void)
{
    step++;
    printf(" 3.TAMPER_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(RTC_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      RTC_IRQHandler
 *
 * @brief   This function handles RTC exception.
 *
 * @return  none
 */
void RTC_IRQHandler(void)
{
    step++;
    printf(" 4.RTC_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(FLASH_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      FLASH_IRQHandler
 *
 * @brief   This function handles FLASH exception.
 *
 * @return  none
 */
void FLASH_IRQHandler(void)
{
    step++;
    printf(" 5.RTC_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(RCC_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      RCC_IRQHandler
 *
 * @brief   This function handles RCC exception.
 *
 * @return  none
 */
void RCC_IRQHandler(void)
{
    step++;
    printf(" 6.RCC_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(EXTI0_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      EXTI0_IRQHandler
 *
 * @brief   This function handles EXTI0 exception.
 *
 * @return  none
 */
void EXTI0_IRQHandler(void)
{
    step++;
    printf(" 7.EXTI0_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(EXTI1_IRQn);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}

/*********************************************************************
 * @fn      EXTI1_IRQHandler
 *
 * @brief   This function handles EXTI1 exception.
 *
 * @return  none
 */
void EXTI1_IRQHandler(void)
{
    step++;
    printf(" 8.EXTI1_IRQHandler");
    printf("  step:%d\r\n",step);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	printf("GISR:%08x\r\n",PFIC->GISR);
}




