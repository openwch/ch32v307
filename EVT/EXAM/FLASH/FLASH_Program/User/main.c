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
 FLASH的擦/读/写例程：
   包括标准擦除和编程、快速擦除和编程。

   注意：擦除成功部分读非0xFF：
              字读——0xe339e339
              半字读——0xe339
              字节读——0x39
              偶地址字节读——0x39
              奇地址字节读——0xe3
*/

#include "debug.h"

/* Global define */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;
#define PAGE_WRITE_START_ADDR            ((uint32_t)0x0800F000) /* Start from 60K */
#define PAGE_WRITE_END_ADDR              ((uint32_t)0x08010000) /* End at 63K */
#define FLASH_PAGE_SIZE                  4096
#define FLASH_PAGES_TO_BE_PROTECTED      FLASH_WRProt_Pages60to63

/* Fast Mode define */
#define FAST_FLASH_PROGRAM_START_ADDR    ((uint32_t)0x08010000)
#define FAST_FLASH_PROGRAM_END_ADDR      ((uint32_t)0x08020000)
#define FAST_FLASH_SIZE                  (64 * 1024)

/* Global Variable */
uint32_t              EraseCounter = 0x0, Address = 0x0;
uint16_t              Data = 0xAAAA;
uint32_t              WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x0;
uint32_t              NbrOfPage;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

volatile TestStatus MemoryProgramStatus = PASSED;
volatile TestStatus MemoryEraseStatus = PASSED;

/*********************************************************************
 * @fn      Flash_Test
 *
 * @brief   Flash Program Test.
 *
 * @return  none
 */
void Flash_Test(void)
{
    printf("FLASH Test\n");

    FLASH_Unlock();

    NbrOfPage = (PAGE_WRITE_END_ADDR - PAGE_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter)); //Erase 4KB

        if(FLASHStatus != FLASH_COMPLETE)
        {
            printf("FLASH Erase Fail\r\n");
        }
        printf("FLASH Erase Suc\r\n");
    }

    Address = PAGE_WRITE_START_ADDR;
    printf("Programing...\r\n");
    while((Address < PAGE_WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
    {
        FLASHStatus = FLASH_ProgramHalfWord(Address, Data);
        Address = Address + 2;
    }

    Address = PAGE_WRITE_START_ADDR;

    printf("Program Cheking...\r\n");
    while((Address < PAGE_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
    {
        if((*(__IO uint16_t *)Address) != Data)
        {
            MemoryProgramStatus = FAILED;
        }
        Address += 2;
    }

    if(MemoryProgramStatus == FAILED)
    {
        printf("Memory Program FAIL!\r\n");
    }
    else
    {
        printf("Memory Program PASS!\r\n");
    }

    FLASH_Lock();
}

/*********************************************************************
 * @fn      Flash_Test_Fast
 *
 * @brief   Flash Fast Program Test.
 *
 * @return  none
 */
void Flash_Test_Fast(void)
{
    u16 i, j, flag;
    u32 buf[64];

    for(i = 0; i < 64; i++)
    {
        buf[i] = i;
    }

    printf("FLASH Fast Mode Test\n");
    FLASH_Unlock_Fast();

    FLASH_EraseBlock_64K_Fast(FAST_FLASH_PROGRAM_START_ADDR);

    FLASH_ProgramPage_Fast(FAST_FLASH_PROGRAM_START_ADDR, buf);

    printf("Program 64KByte start\n");
    for(i = 0; i < 256; i++)
    {
        FLASH_ProgramPage_Fast(FAST_FLASH_PROGRAM_START_ADDR + 256 * i, buf);
    }

    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < 256; j++)
        {
            if(*(u32 *)(FAST_FLASH_PROGRAM_START_ADDR + 256 * i + 4 * j) != i)
            {
                flag = 1;
                break;
            }
            else
            {
                flag = 0;
            }
        }
    }

    if(flag)
    {
        printf("Program 64KByte suc\n");
    }
    else
        printf("Program fail\n");

    printf("Erase 256Byte...\n");
    FLASH_ErasePage_Fast(FAST_FLASH_PROGRAM_START_ADDR);

    printf("Read 4KByte...\n");
    for(i = 0; i < 1024; i++)
    {
        printf("%08x ", *(u32 *)(FAST_FLASH_PROGRAM_START_ADDR + 4 * i));
    }
    printf("\n");

    printf("Erase 4KByte...\n");
    FLASH_ErasePage(FAST_FLASH_PROGRAM_START_ADDR);

    printf("Read 8KByte...\n");
    for(i = 0; i < 2048; i++)
    {
        printf("%08x ", *(u32 *)(FAST_FLASH_PROGRAM_START_ADDR + 4 * i));
    }
    printf("\n");

    printf("Erase 32KByte...\n");
    FLASH_EraseBlock_32K_Fast(FAST_FLASH_PROGRAM_START_ADDR);

    printf("Read 36KByte...\n");
    for(i = 0; i < (1024 * 9); i++)
    {
        printf("%08x ", *(u32 *)(FAST_FLASH_PROGRAM_START_ADDR + 4 * i));
    }
    printf("\n");

    printf("Erase 64KByte...\n");
    FLASH_EraseBlock_64K_Fast(FAST_FLASH_PROGRAM_START_ADDR);

    printf("Read 64KByte...\n");
    for(i = 0; i < (1024 * 16); i++)
    {
        printf("%08x ", *(u32 *)(FAST_FLASH_PROGRAM_START_ADDR + 4 * i));
    }
    printf("\n");

    FLASH_Lock_Fast();
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
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    printf("Flash Program Test\r\n");

    Flash_Test();
    Flash_Test_Fast();

    while(1)
        ;
}
