/********************************** (C) COPYRIGHT *******************************
 * File Name          : SPI_FLAH.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : header file for SPI_FLAH.c
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif 

/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/******************************************************************************/
/* SPI FLASH相关引脚 */
#define PIN_FLASH_CS_LOW( )       ( GPIOA->BCR = GPIO_Pin_2 )
#define PIN_FLASH_CS_HIGH( )      ( GPIOA->BSHR = GPIO_Pin_2 )

/******************************************************************************/
/* SPI Serial Flash OPERATION INSTRUCTIONS */
#define CMD_FLASH_READ             0x03                                         /* Read Memory at 25 MHz */
#define CMD_FLASH_SECTOR_ERASE     0x20                                         /* Erase 4 KByte of memory array */
#define CMD_FLASH_BYTE_PROG        0x02                                         /* To Program One Data Byte */
#define CMD_FLASH_RDSR             0x05                                         /* Read-Status-Register */
#define CMD_FLASH_EWSR             0x50                                         /* Enable-Write-Status-Register */
#define CMD_FLASH_WREN             0x06                                         /* Write-Enable */
#define CMD_FLASH_WRDI             0x04                                         /* Write-Disable */
#define CMD_FLASH_JEDEC_ID         0x9F                                         /* JEDEC ID read */

/******************************************************************************/
/* FLASH操作填充自己定义 */
#define DEF_DUMMY_BYTE             0xFF

/******************************************************************************/
/* FLASH操作相关参数定义 */
#define SPI_FLASH_SectorSize       4096
#define SPI_FLASH_PageSize         256
#define SPI_FLASH_PerWritePageSize 256

/******************************************************************************/
/* SPI FLASH芯片类型定义 */
#define DEF_TYPE_W25XXX            0                                            /* W25XXX系列 */

/******************************************************************************/
/* SPI FLASH芯片型号定义 */

/* W25XXX系列 */
#define W25X10_FLASH_ID            0xEF3011                                     /* 1M bit */
#define W25X20_FLASH_ID            0xEF3012                                     /* 2M bit */
#define W25X40_FLASH_ID            0xEF3013                                     /* 4M bit */
#define W25X80_FLASH_ID            0xEF4014                                     /* 8M bit */
#define W25Q16_FLASH_ID1           0xEF3015                                     /* 16M bit */
#define W25Q16_FLASH_ID2           0xEF4015                                     /* 16M bit */
#define W25Q32_FLASH_ID1           0xEF4016                                     /* 32M bit */
#define W25Q32_FLASH_ID2           0xEF6016                                     /* 32M bit */
#define W25Q64_FLASH_ID1           0xEF4017                                     /* 64M bit */
#define W25Q64_FLASH_ID2           0xEF6017                                     /* 64M bit */
#define W25Q128_FLASH_ID1          0xEF4018                                     /* 128M bit */
#define W25Q128_FLASH_ID2          0xEF6018                                     /* 128M bit */
#define W25Q256_FLASH_ID1          0xEF4019                                     /* 256M bit */
#define W25Q256_FLASH_ID2          0xEF6019                                     /* 256M bit */

/******************************************************************************/
/* 常、变量外扩 */
extern volatile uint8_t  Flash_Type;                                            /* FLASH芯片类型: 0: W25XXX系列;  */
extern volatile uint32_t Flash_ID;                                              /* FLASH芯片ID号 */
extern volatile uint32_t Flash_Sector_Count;                                    /* FLASH芯片扇区数 */
extern volatile uint16_t Flash_Sector_Size;                                     /* FLASH芯片扇区大小 */

/******************************************************************************/
/* 函数外扩 */
extern void FLASH_Port_Init( void );
extern uint8_t SPI_FLASH_SendByte( uint8_t byte );
extern uint8_t SPI_FLASH_ReadByte( void );
extern uint32_t FLASH_ReadID( void );
extern void FLASH_WriteEnable( void );
extern void FLASH_WriteDisable( void );
extern uint8_t FLASH_ReadStatusReg( void );
extern void FLASH_IC_Check( void );
extern void FLASH_Erase_Sector( uint32_t address );
extern void FLASH_RD_Block_Start( uint32_t address );
extern void FLASH_RD_Block( uint8_t *pbuf, uint32_t len );
extern void FLASH_RD_Block_End( void );
extern void W25XXX_WR_Block( uint8_t *pbuf, uint32_t address, uint32_t len );

#ifdef __cplusplus
}
#endif

#endif
