/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
  1.本例程采用同步触发，演示中断嵌套的运用，最大嵌套8级，硬件压栈只
           保存低三级，高优先级的5级中断需要使用软件压栈，低优先级的3级可以
           使用硬件压栈或软件压栈。

  2.若只使用硬件压栈，可配置嵌套等级为4和硬件压栈溢出时关闭全局
           中断(启动文件中配置CSR(0x804),配置值由0x1f修改为0x0b)

  3.若不使用硬件压栈，配置CSR(0x804)的bit0清0，且中断函数声明去掉
   "WCH-Interrupt-fast"关键字

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

    NVIC_SetPriority(WWDG_IRQn,  (7<<5) | (0x01<<4)); /* 组优先级7，子优先级1，总优先级较低 */
    NVIC_SetPriority(PVD_IRQn,   (6<<5) | (0x01<<4));
    NVIC_SetPriority(TAMPER_IRQn,(5<<5) | (0x01<<4));
    NVIC_SetPriority(RTC_IRQn,   (4<<5) | (0x01<<4));
    NVIC_SetPriority(FLASH_IRQn, (3<<5) | (0x01<<4));
    NVIC_SetPriority(RCC_IRQn,   (2<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI0_IRQn, (1<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI1_IRQn, (0<<5) | (0x01<<4));/* 组优先级0，子优先级1，总优先级较高 */
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
    Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
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




