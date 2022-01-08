/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
//#include "FreeRTOSConfig.h"


#include <stdio.h>

#include "Config.h"
#include "core_riscv.h"

/* Standard Includes */
#include <stdlib.h>
#include <unistd.h>

void SysTick_Handler(void) __attribute__((interrupt()));

/* Each task maintains its own interrupt status in the critical nesting
variable. */
UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
//UBaseType_t uxCriticalNesting = 0;

#if USER_MODE_TASKS
	unsigned long MSTATUS_INIT = 0x00000088;
#else
	unsigned long MSTATUS_INIT = (0x00001800 | 0x00000088);
#endif


/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );


/*---------------进入临界区---------------------*/
void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}
/*------------------退出临界区----------------------------*/

void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
	    portENABLE_INTERRUPTS();
	}
	return;
}
/*-----------------------------------------------------------*/

/* Clear current interrupt mask and set given mask */
void vPortClearInterruptMask(int int_mask)
{
    NVIC->ITHRESDR = int_mask;
}
/*-----------------------------------------------------------*/

/* Set interrupt mask and return current interrupt enable register */
int xPortSetInterruptMask()
{
	int int_mask=0;
	int_mask = NVIC->ITHRESDR;
	NVIC->ITHRESDR = (configMAX_SYSCALL_INTERRUPT_PRIORITY)<<4;
	return int_mask;
}

/*-----------------------------------------------------------*/
/*
 * 初始化任务栈，若出入栈顺序和内容发生改变，该函数需要调整.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	pxTopOfStack--;
	*pxTopOfStack = (portSTACK_TYPE)pxCode;			/* Start address */
	pxTopOfStack--;
	*pxTopOfStack = MSTATUS_INIT;
	pxTopOfStack -= 22;
	*pxTopOfStack = (portSTACK_TYPE)pvParameters;	/* Register a0 */
	pxTopOfStack -=9;
	*pxTopOfStack = (portSTACK_TYPE)prvTaskExitError; /* Register ra */
	pxTopOfStack--;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/


void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).
	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/



/*Entry Point for Machine Timer Interrupt Handler*/
//Bob: add the function argument int_num

//uint32_t vPortSysTickHandler()
//{
//	/* Increment the RTOS tick. */
//	if( xTaskIncrementTick() != pdFALSE )
//	{
//		//portYIELD();
//		vTaskSwitchContext();
//	}
//}
/*----------------使用timer4为系统节拍定时器(可以替换回systicker，主要是因为systicker调试时不停止)-----------------------*/
void SysTick_Handler(void)
{
    /* Increment the RTOS tick. */
    portDISABLE_INTERRUPTS();
    if( xTaskIncrementTick() != pdFALSE )
    {
        portYIELD();     //----调用软中断，完成切换----
    }
    portENABLE_INTERRUPTS();
    SysTick->SR=0;
    //    TIM_ClearITPendingBit( TIM4, TIM_IT_Update);    //清除中断标志位
}

void vPortSetupTimer()
{
    init_timer();
}
/*-----------------------------------------------------------*/
void vPortSetup()	{
	vPortSetupTimer();
	uxCriticalNesting = 0;
	//portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/
BaseType_t xPortStartScheduler( void )
{
    vPortSetup();
    //-------使能软中断-------
    NVIC_SetPriority(Software_IRQn, 0xff);
    NVIC_ClearPendingIRQ(Software_IRQn);
    NVIC_EnableIRQ(Software_IRQn);
    asm("ecall");       //---硬件异常中开启首个任务---
    return 0;
}
