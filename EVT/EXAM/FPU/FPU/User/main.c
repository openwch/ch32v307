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
 FPU hardware floating point operation routine:
   This example demonstrates hardware floating-point arithmetic.

       Enable hardware floating point M-RS configuration reference This example configuration
    Specific configuration-Properties -> C/C++ Build -> Setting -> Target Processor
    -> The Floating point option is configured as Single precision extension (RVF)
       Floating point ABI option configured as Single precision(f)

*/

#include "debug.h"

float val1=33.14;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    int t,t1;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	USART_Printf_Init(115200);
	SystemCoreClockUpdate();
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	val1 = (val1/2+11.12)*2;
    t=(int)(val1*10)%10;
    t1=(int)(val1*100)%10;

	printf("%d.%d%d\n", (int)val1, t, t1);

	while(1);
}


