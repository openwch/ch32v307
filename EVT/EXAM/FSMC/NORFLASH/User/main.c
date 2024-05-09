/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/13
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2024 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   This example is used to demonstrate the FSMC interface operating NorFlash-MT28EW128ABA1LJS-0SIT.
  PIN:
   (Latches)
    FSMC_NADV     -    PB7
    FSMC_NOE      -    PD4  (OE)
    FSMC_NWE      -    PD5  (WE)
    FSMC_NWAIT    -    PD6  (RY/BY#)
    FSMC_NE1      -    PD7  (CE)
    FSMC_D/A0     -    PD14
    FSMC_D/A1     -    PD15
    FSMC_D/A2     -    PD0
    FSMC_D/A3     -    PD1
    FSMC_D/A4     -    PE7
    FSMC_D/A5     -    PE8
    FSMC_D/A6     -    PE9
    FSMC_D/A7     -    PE10
    FSMC_D/A8     -    PE11
    FSMC_D/A9     -    PE12
    FSMC_D/A10    -    PE13
    FSMC_D/A11    -    PE14
    FSMC_D/A12    -    PE15
    FSMC_D/A13    -    PD8
    FSMC_D/A14    -    PD9
    FSMC_D/A15    -    PD10
    FSMC_A16      -    PD11
    FSMC_A17      -    PD12
    FSMC_A18      -    PD13
    FSMC_A19      -    PE3
    FSMC_A20      -    PE4
    FSMC_A21      -    PE5
    FSMC_A22      -    PE6

  The address line and data line of this routine are multiplexed
  and need to use a latch.
*/

#include "debug.h"
#include "FSMC_NOR.h"

uint16_t TxBuffer[BUFFER_SIZE];
uint16_t RxBuffer[BUFFER_SIZE];
uint32_t WriteReadStatus = 0, Index = 0;
NOR_IDTypeDef NOR_ID;


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
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("NorFlash TEST\n");

    FSMC_NorFlash_Init();
    NOR_ReadID(&NOR_ID);
    NOR_ReturnToReadMode();
    NOR_EraseBlock(WRITE_READ_ADDR);
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x01);
    NOR_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    NOR_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    for (Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
    {
        if (RxBuffer[Index] != TxBuffer[Index])
        {
            WriteReadStatus = Index + 1;
            printf("RxBuffer[%d]=%x\r\n",Index,RxBuffer[Index]);
            printf("TxBuffer[%d]=%x\r\n",Index,TxBuffer[Index]);
        }
    }
    if (WriteReadStatus == 0)
    {
        /* OK */
        printf("sucess\r\n");
    }
    else
    {
        /* KO */
        printf("error\r\n");
    }
    while(1);
}
