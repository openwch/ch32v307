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
 CRC routine:
 Use CRC-32 polynomial 0x4C11DB7.

*/

#include "debug.h"


/* Global define */
#define Buf_Size 32

/* Global Variable */
u32  SRC_BUF[Buf_Size]={0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                     	0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                        0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                        0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
	                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                        0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                        0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                        0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};

u32 CRCValue = 0;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("CRC TEST\r\n");
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	CRCValue = CRC_CalcBlockCRC((u32 *)SRC_BUF, Buf_Size);

	printf("CRCValue: 0x%08x\r\n",CRCValue);      /* CRCValue should be 0x199AC3CA in this example */
	while(1);
}
