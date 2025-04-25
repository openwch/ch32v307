/********************************** (C) COPYRIGHT *******************************
 * File Name          : FSMC_NOR.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/04/13
 * Description        : This file contains the headers of the FSMC_NOR.
*********************************************************************************
* Copyright (c) 2024 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/******************************************************************************/
#include "debug.h"
#include "string.h"
#include "FSMC_NOR.h"

/*********************************************************************
 * @fn      FSMC_NorFlash_Init
 *
 * @brief   Init FSMC
 *
 * @return  None
 */

void FSMC_NorFlash_Init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure={0};
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming={0};
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);

    /*Data line*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                        GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                        GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                        GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /*Address line */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* NOE and NWE configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FSMC_NADV */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* FSMC_NWAIT */
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FSMC_NE1 */
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    readWriteTiming.FSMC_AddressSetupTime = 0x08;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x0A;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_C;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
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
 * @fn      NOR_GetStatus
 *
 * @brief   Returns the NOR operation status
 *
 * @param   Timeout - NOR progamming Timeout
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *         or NOR_TIMEOUT
 */

NOR_Status NOR_GetStatus(uint32_t Timeout)
{
    uint16_t val1 = 0x00, val2 = 0x00;
    NOR_Status status = NOR_ONGOING;
    uint32_t timeout = Timeout;

    /*!< Poll on NOR memory Ready/Busy signal ----------------------------------*/
    while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) != RESET) && (timeout > 0))
    {
        timeout--;
    }
    timeout = Timeout;

    while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == RESET) && (timeout > 0))
    {
        timeout--;
    }

    /*!< Get the NOR memory operation status -----------------------------------*/
    while((Timeout != 0x00) && (status != NOR_SUCCESS))
    {
        Timeout--;

        /*!< Read DQ6 and DQ5 */
        val1 = *(__IO uint16_t *)(Bank1_NOR_ADDR);
        val2 = *(__IO uint16_t *)(Bank1_NOR_ADDR);

        /*!< If DQ6 did not toggle between the two reads then return NOR_Success */
        if((val1 & 0x0040) == (val2 & 0x0040))
        {
            return NOR_SUCCESS;
        }

        if((val1 & 0x0020) != 0x0020)
        {
            status = NOR_ONGOING;
        }

        val1 = *(__IO uint16_t *)(Bank1_NOR_ADDR);
        val2 = *(__IO uint16_t *)(Bank1_NOR_ADDR);

        if((val1 & 0x0040) == (val2 & 0x0040))
        {
            return NOR_SUCCESS;
        }
        else if((val1 & 0x0020) == 0x0020)
        {
            return NOR_ERROR;
        }
    }

    if(Timeout == 0x00)
    {
        status = NOR_TIMEOUT;
    }

    /*!< Return the operation status */
    return (status);
}

/*********************************************************************
 * @fn      NOR_Read_CFI
 *
 * @brief   Returns the NOR memory to Read  CFI mode
 *
 * @param   None
 *
 * @return  None
 */

void NOR_Read_CFI()
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x98);
}

/*********************************************************************
 * @fn      NOR_ReadID
 *
 * @brief   Reads NOR memory's Manufacturer and Device Code
 *
 * @param   NOR_ID: pointer to a NOR_IDTypeDef structure which will hold the
 *          Manufacturer and Device Code.
 *
 * @return  None
 */

void NOR_ReadID(NOR_IDTypeDef* NOR_ID)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0090);

    NOR_ID->Manufacturer_Code = *(__IO uint16_t *) ADDR_SHIFT(0x0000);
    NOR_ID->Device_Code1 = *(__IO uint16_t *) ADDR_SHIFT(0x0001);
    NOR_ID->Device_Code2 = *(__IO uint16_t *) ADDR_SHIFT(0x000E);
    NOR_ID->Device_Code3 = *(__IO uint16_t *) ADDR_SHIFT(0x000F);
    printf("NOR_ID->Manufacturer_Code=%x\r\n",NOR_ID->Manufacturer_Code);
    printf("NOR_ID->Device_Code1=%x\r\n",NOR_ID->Device_Code1);
    printf("NOR_ID->Device_Code2=%x\r\n",NOR_ID->Device_Code2);
    printf("NOR_ID->Device_Code3=%x\r\n",NOR_ID->Device_Code3);
}

/*********************************************************************
 * @fn      NOR_ReturnToReadMode
 *
 * @brief   Returns the NOR memory to Read mode
 *
 * @param   none
 *
 * @return  NOR_SUCCESS
 */

NOR_Status NOR_ReturnToReadMode(void)
{
    NOR_WRITE(Bank1_NOR_ADDR, 0x00F0);

    return (NOR_SUCCESS);
}

/*********************************************************************
 * @fn      NOR_EraseBlock
 *
 * @brief   Erases the specified Nor memory block
 *
 * @param   BlockAddr: address of the block to erase
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *          or NOR_TIMEOUT
 */

NOR_Status NOR_EraseBlock(uint32_t BlockAddr)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE((Bank1_NOR_ADDR + BlockAddr), 0x30);

    return (NOR_GetStatus(BlockErase_Timeout));
}

/*********************************************************************
 * @fn      NOR_EraseChip
 *
 * @brief   Erases the entire chip
 *
 * @param   None
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *          or NOR_TIMEOUT
 */

NOR_Status NOR_EraseChip(void)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x0010);

    return (NOR_GetStatus(ChipErase_Timeout));
}

/*********************************************************************
 * @fn      Fill_Buffer
 *
 * @brief   pBuffer: pointer on the Buffer to fill
 *          BufferSize: size of the buffer to fill
 *          Offset: first value to fill on the Buffer
 *
 * @param   None
 *
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *          or NOR_TIMEOUT
 */

void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
    {
        pBuffer[IndexTmp] =IndexTmp + Offset;
    }
}

/*********************************************************************
 * @fn      NOR_WriteHalfWord
 *
 * @brief   Writes a half-word to the NOR memory
 *
 * @param   WriteAddr: NOR memory internal address to write to
 *          Data: Data to write
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *         or NOR_TIMEOUT
 */

NOR_Status NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00A0);
    NOR_WRITE((Bank1_NOR_ADDR + WriteAddr), Data);

    return (NOR_GetStatus(Program_Timeout));
}

/*********************************************************************
 * @fn      NOR_WriteBuffer
 *
 * @brief   Writes a half-word buffer to the FSMC NOR memory
 *
 * @param   pBuffer: pointer to buffer
 *          WriteAddr: NOR memory internal address from which the data will be
 *          written
 *          NumHalfwordToWrite: number of Half words to write
 * @return  NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
 *         or NOR_TIMEOUT
 */

NOR_Status NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
    NOR_Status status = NOR_ONGOING;

    do
    {
        /*!< Transfer data to the memory */
        status = NOR_WriteHalfWord(WriteAddr, *pBuffer++);
        WriteAddr = WriteAddr + 2;
        NumHalfwordToWrite--;
    }
    while((status == NOR_SUCCESS) && (NumHalfwordToWrite != 0));

    return (status);
}

/*********************************************************************
 * @fn      NOR_ReadBuffer
 *
 * @brief   Reads a block of data from the FSMC NOR memory
 *
 * @param   pBuffer: pointer to the buffer that receives the data read from the
 *          NOR memory
 *          ReadAddr: NOR memory internal address to read from
 *          NumHalfwordToRead : number of Half word to read
 * @return  None
 */

void NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
    NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
    NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
    NOR_WRITE((Bank1_NOR_ADDR + ReadAddr), 0x00F0);

    while( NumHalfwordToRead != 0 ) /*!< while there is data to read */
    {
        /*!< Read a Halfword from the NOR */
        *pBuffer++ = *(__IO uint16_t *)((Bank1_NOR_ADDR + ReadAddr));
        ReadAddr = ReadAddr + 2;
        NumHalfwordToRead--;
    }
}
