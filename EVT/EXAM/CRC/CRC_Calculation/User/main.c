/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/05
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *CRC routine:
 *Use CRC-32 polynomial 0x4C11DB7.
 *
 */

#include "debug.h"

/* Global define */
#define Buf_Size 32

/* Global Variable */
u32 SRC_BUF[Buf_Size] = {0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
						 0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20, 0x21222324, 0x25262728,
						 0x292A2B2C, 0x2D2E2F30, 0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
						 0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50, 0x51525354, 0x55565758,
						 0x595A5B5C, 0x5D5E5F60, 0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
						 0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80};

u32 CRCValue = 0;

/*********************************************************************
 * @fn	RecalculateCRC
 *
 * @brief The function RecalculateCRC calculates the CRC value of a given buffer and sets the ID register to a
 * 		specific value.
 *
 * @param SRC_BUF SRC_BUF is the source buffer, which is a pointer to the start of the data that needs
 * 		to be used for CRC calculation.
 *        suze The parameter "suze" is likely a typo and should be "size". It represents the size of
 * 		the source buffer in bytes.
 *
 * @return the calculated CRC value as a uint32_t.
 */
uint32_t RecalculateCRC(uint32_t *SRC_BUF, uint32_t suze)
{
	uint32_t temp;
	if (CRC_GetIDRegister() == 0xaa)
		CRC_ResetDR();
	temp = CRC_CalcBlockCRC((u32 *)SRC_BUF, suze);
	CRC_SetIDRegister(0xaa);
	return temp;
}

/*********************************************************************
 * @fn CRC_Is_Used
 *
 * @brief The function checks if the CRC module is being used by comparing the ID register value to 0xaa and
 * returns 1 if it is being used, otherwise it returns 0.
 *
 * @return a value of type uint8_t, which is an 8-bit unsigned integer. The function is checking if the
 * value returned by the CRC_GetIDRegister() function is equal to 0xaa. If it is, the function returns
 * 1. If it is not, the function returns 0.
 */
uint8_t CRC_Is_Used()
{
	if (CRC_GetIDRegister() == 0xaa)
		return 1;
	else
		return 0;
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n", SystemCoreClock);
	printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
	printf("CRC TEST\r\n");
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

	CRC_Is_Used() ? printf("CRC is not clear\r\n") : printf("CRC is clear\r\n");
	/* The code is calling the function `RecalculateCRC` with the arguments `SRC_BUF` and `Buf_Size`. */
	CRCValue = RecalculateCRC(SRC_BUF, Buf_Size);

	printf("CRCValue: 0x%08X\r\n", CRCValue); /* CRCValue should be 0x199AC3CA in this example */

	CRC_Is_Used() ? printf("CRC is not clear\r\n") : printf("CRC is clear\r\n");
	while (1)
		;
}
