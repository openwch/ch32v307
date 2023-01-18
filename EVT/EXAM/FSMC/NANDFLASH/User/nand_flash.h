/********************************** (C) COPYRIGHT *******************************
* File Name          : nand_flash.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the NANDFLASH.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __NAND_FLASH_H
#define __NAND_FLASH_H

#include "debug.h"

#define NAND_ECC_SECTOR_SIZE        512

//NANDFLASH parameter
typedef struct
{
    u16 page_totalsize;
    u16 page_mainsize;
    u16 page_sparesize;
    u8  block_pagenum;
    u16 plane_blocknum;
    u16 block_totalnum;
    u32 id;
}nand_attriute;

extern nand_attriute nand_dev;

#define NAND_ADDRESS            0X70000000
#define NAND_CMD                1<<16
#define NAND_ADDR               1<<17

//NAND FLASH����
#define NAND_READID             0X90
#define NAND_RESET              0XFF
#define NAND_READSTA            0X70
#define NAND_AREA_A             0X00
#define NAND_AREA_TRUE1         0X30
#define NAND_WRITE0             0X80
#define NAND_WRITE_TURE1        0X10
#define NAND_ERASE0             0X60
#define NAND_ERASE1             0XD0

//NANDFLASH state
#define NSTA_READY              0X40
#define NSTA_ERROR              0X01
#define NSTA_TIMEOUT            0X02


//NANDFLASH ID
#define FS33ND01GS108TF             0xF1009542 //  0x429500F1


u8 NAND_Init(void);
u32 NAND_ReadID(void);
u8 NAND_ReadStatus(void);
u8 NAND_WaitForReady(void);
u8 NAND_Reset(void);
u8 NAND_WaitRB(vu8 rb);
void NAND_Delay(vu32 i);
u8 NAND_ReadPage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead);
u8 NAND_WritePage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToWrite);
u8 NAND_EraseBlock(u32 BlockNum);

#endif 
