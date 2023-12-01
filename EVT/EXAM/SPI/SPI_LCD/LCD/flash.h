/*********************************************************************
 * File Name          : flash.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for spi flash.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H
#include "debug.h"
#include "spi.h"
#include "config.h"

#define W25X_WriteEnable      0x06
#define W25X_WriteDisable     0x04
#define W25X_ReadStatusReg    0x05
#define W25X_WriteStatusReg   0x01
#define W25X_ReadData         0x03
#define W25X_FastReadData     0x0B
#define W25X_FastReadDual     0x3B
#define W25X_PageProgram      0x02
#define W25X_BlockErase       0xD8
#define W25X_SectorErase      0x20
#define W25X_ChipErase        0xC7
#define W25X_PowerDown        0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID         0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID    0x9F

#define FLASH_CS_SET          (FLASH_CS_PORT->BSHR = FLASH_CS_PIN)
#define FLASH_CS_CLR          (FLASH_CS_PORT->BCR = FLASH_CS_PIN)

void SPI_FLASH_Init(void);
uint8_t SPI_Flash_ReadSR(void);
void SPI_FLASH_Write_SR(uint8_t sr);
void SPI_Flash_Wait_Busy(void);
void SPI_FLASH_Write_Enable(void);
void SPI_FLASH_Write_Disable(void);
uint16_t SPI_Flash_ReadID(void);
uint32_t SPI_Flash_ReadJedecDeviceID(void);
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr);
void SPI_Flash_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t size);
void SPI_Flash_Write_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t size);
void SPI_Flash_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t size);
void SPI_Flash_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t size);
void SPI_Flash_Erase_Chip(void);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);
void SPI_Flash_Read_dma_start(uint32_t ReadAddr);
void SPI_Flash_Read_dma_end(void);

#endif
