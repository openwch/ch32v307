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
 BKP侵入检测例程：
   侵入检测引脚(PC13)，当产生侵入检测事件时，所有数据备份寄存器内容被清除，
   并触发侵入检测中断。

*/

#include "debug.h"

/*********************************************************************
 * @fn      BKP_Tamper_Init
 *
 * @brief   Initializes the BKP Tamper.
 *
 * @return  none
 */
void BKP_Tamper_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure={0};

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );

	BKP_TamperPinCmd( DISABLE );
    PWR_BackupAccessCmd( ENABLE );
    BKP_ClearFlag();

	BKP_WriteBackupRegister( BKP_DR1, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR2, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR3, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR4, 0x3456 );

	printf( "BKP_DR1:%08x\r\n", BKP->DATAR1 );
	printf( "BKP_DR2:%08x\r\n", BKP->DATAR2 );
	printf( "BKP_DR3:%08x\r\n", BKP->DATAR3 );
    printf( "BKP_DR4:%08x\r\n", BKP->DATAR4 );

	BKP_TamperPinLevelConfig( BKP_TamperPinLevel_High );  //TPAL:0，PC13 set input-pull-down
//	BKP_TamperPinLevelConfig( BKP_TamperPinLevel_Low );	 //TPAL:1，PC13 input-pull-up

	NVIC_InitStructure.NVIC_IRQChannel = TAMPER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	BKP_ITConfig( ENABLE );
	BKP_TamperPinCmd( ENABLE );
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

	BKP_Tamper_Init();

	while(1);
}


