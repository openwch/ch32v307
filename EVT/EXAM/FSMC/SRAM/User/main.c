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
   FSMC routine to operate NOR_ SRAM:
      This routine demonstrates the operation of IS62WV25616BLL model
      NOR_SRAM erase-read through FSMC interface.
  PIN:
   (Latches)
    FSMC_NADV   -    PB7
    SRAM
    FSMC_A18    -    PD13 (CS1)
    FSMC_NOE    -    PD4  (OE)
    FSMC_NWE    -    PD5  (WE)
    FSMC_NBL1   -    PE1  (UB)
    FSMC_NBL0   -    PE0  (LB)
    FSMC_D0     -    PD14
    FSMC_D1     -    PD15
    FSMC_D2     -    PD0
    FSMC_D3     -    PD1
    FSMC_D4     -    PE7
    FSMC_D5     -    PE8
    FSMC_D6     -    PE9
    FSMC_D7     -    PE10
    FSMC_D8     -    PE11
    FSMC_D9     -    PE12
    FSMC_D10    -    PE13
    FSMC_D11    -    PE14
    FSMC_D12    -    PE15
    FSMC_D13    -    PD8
    FSMC_D14    -    PD9
    FSMC_D15    -    PD10
    FSMC_A16    -    PD11
    FSMC_A17    -    PD12

  The address line and data line of this routine are multiplexed
  and need to use a latch.
*/

#include "debug.h"

#define Bank1_SRAM1_ADDR    ((u32)(0x60000000))

/*********************************************************************
 * @fn      FSMC_SRAM_Init
 *
 * @brief   Init FSMC
 *
 * @return  none
 */
void FSMC_SRAM_Init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure={0};
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming={0};
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);

    /* FSMC_NADV */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);

    readWriteTiming.FSMC_AddressSetupTime = 0x00;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x03;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/*********************************************************************
 * @fn      FSMC_SRAM_WriteBuffer
 *
 * @brief   Write data to NOR_SRAM
 *
 * @param   pBuffer - data pointer
 *          WriteAddr - Start address
 *          n - data number
 *
 * @return  none
 */
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *(vu8*)(Bank1_SRAM1_ADDR+WriteAddr)=*pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

/*********************************************************************
 * @fn      FSMC_SRAM_ReadBuffer
 *
 * @brief   Read data from NOR_SRAM
 *
 * @param   pBuffer - data pointer
 *          WriteAddr - Start address
 *          n - data number
 *
 * @return  none
 */
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
    for(;n!=0;n--)
    {
        *pBuffer++=*(vu8*)(Bank1_SRAM1_ADDR+ReadAddr);
        ReadAddr++;
    }
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
    u32 i=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("SRAM TEST\n");

	FSMC_SRAM_Init();

	printf("Write data:\n");
	for(i=0; i<1024; i++){
      *(u32*)(Bank1_SRAM1_ADDR + 4*i) = i;
	}

	printf("Read data:\n");
	for(i=0; i<1024; i++){
      printf("%08x ", *(u32*)(Bank1_SRAM1_ADDR + 4*i));
    } printf("\n");

	while(1);
}
