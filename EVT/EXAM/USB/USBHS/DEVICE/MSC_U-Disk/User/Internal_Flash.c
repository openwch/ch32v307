/********************************** (C) COPYRIGHT *******************************
 * File Name          : Internal_Flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Internal Flash program
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x.h"
#include "Internal_Flash.h"
#include "ch32v30x_flash.h"

void IFlash_Prog_512(uint32_t address,uint32_t *pbuf)
{
    if (address < IFLASH_UDISK_START_ADDR || (address + 511) > IFLASH_UDISK_END_ADDR )
    {
        printf("Error Address %x\n",address);
        return;
    }
    address &= 0x00FFFFFF;
    address |= 0x08000000;
    __disable_irq();
    FLASH_Unlock_Fast();
    FLASH_ErasePage_Fast(address);
    FLASH_ProgramPage_Fast(address, pbuf);
    FLASH_ErasePage_Fast(address + INTERNAL_FLASH_PAGE_SIZE);
    FLASH_ProgramPage_Fast(address+ INTERNAL_FLASH_PAGE_SIZE, pbuf + INTERNAL_FLASH_PAGE_SIZE / 4);
    FLASH_Lock_Fast();
    __enable_irq();
}
