/*********************************************************************
 * File Name          : flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for spi flash.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "flash.h"
#include "spi.h"

static uint8_t FLASH_readWrite(uint8_t TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI1, TxData);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;

    return SPI_I2S_ReceiveData(SPI1);
}

/*********************************************************************
 * @fn      cs_pin_init
 *
 * @brief   gpio cs pin init
 *
 * @return  none
 */
static void cs_pin_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(FLASH_CS_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(FLASH_CS_PORT, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      SPI_FLASH_Init
 *
 * @brief   SPI FLASH Init
 *
 * @return  none
 */
void SPI_FLASH_Init(void)
{
    SPI1_Init();
    SPI1_Read_DMA_Init();
    cs_pin_init();
    FLASH_CS_SET;
}

/*********************************************************************
 * @fn      SPI_Flash_ReadSR
 *
 * @brief   Read W25Qxx status register. 
 *        ！！BIT7  6   5   4   3   2   1   0
 *        ！！SPR   RV  TB  BP2 BP1 BP0 WEL BUSY
 *
 * @return  byte - status register value.
 */
uint8_t SPI_Flash_ReadSR(void)
{
    uint8_t byte = 0;

    FLASH_CS_CLR;
    FLASH_readWrite(W25X_ReadStatusReg);
    byte = FLASH_readWrite(0Xff);
    FLASH_CS_SET;

    return byte;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_SR
 *
 * @brief   Write W25Qxx status register.
 *
 * @param   sr - status register value.
 *
 * @return  none
 */
void SPI_FLASH_Write_SR(uint8_t sr)
{
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_WriteStatusReg);
    FLASH_readWrite(sr);
    FLASH_CS_SET;
}

/*********************************************************************
 * @fn      SPI_Flash_Wait_Busy
 *
 * @brief   Wait flash free.
 *
 * @return  none
 */
void SPI_Flash_Wait_Busy(void)
{
    while ((SPI_Flash_ReadSR() & 0x01) == 0x01)
        ;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Enable
 *
 * @brief   Enable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Enable(void)
{
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_WriteEnable);
    FLASH_CS_SET;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Disable
 *
 * @brief   Disable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Disable(void)
{
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_WriteDisable);
    FLASH_CS_SET;
}

/*********************************************************************
 * @fn      SPI_Flash_ReadID
 *
 * @brief   Read flash ID.
 *
 * @return  Temp - FLASH ID.
 */
uint16_t SPI_Flash_ReadID(void)
{
    uint16_t Temp = 0;

    FLASH_CS_CLR;
    FLASH_readWrite(W25X_ManufactDeviceID);
    FLASH_readWrite(0x00);
    FLASH_readWrite(0x00);
    FLASH_readWrite(0x00);
    Temp |= FLASH_readWrite(0xFF) << 8;
    Temp |= FLASH_readWrite(0xFF);
    FLASH_CS_SET;

    return Temp;
}

/*********************************************************************
 * @fn      SPI_Flash_ReadJedecDeviceID
 *
 * @brief   Read JedecDevice ID.
 *
 * @return  JedecDevice ID.
 */
uint32_t SPI_Flash_ReadJedecDeviceID(void)
{
    uint32_t Temp = 0;
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_JedecDeviceID);
    Temp |= FLASH_readWrite(0xFF) << 16;
    Temp |= FLASH_readWrite(0xFF) << 8;
    Temp |= FLASH_readWrite(0xFF);
    FLASH_CS_SET;
    return Temp;
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Sector
 *
 * @brief   Erase one sector(4Kbyte).
 *
 * @param   Dst_Addr - 0 ！！ 2047
 *
 * @return  none
 */
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr)
{
    Dst_Addr *= 4096;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_SectorErase);
    FLASH_readWrite((uint8_t)((Dst_Addr) >> 16));
    FLASH_readWrite((uint8_t)((Dst_Addr) >> 8));
    FLASH_readWrite((uint8_t)Dst_Addr);
    FLASH_CS_SET;
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Read
 *
 * @brief   Read data from flash.
 *
 * @param   pBuffer -
 *          ReadAddr -Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t size)
{
    uint16_t i;

    FLASH_CS_CLR;
    FLASH_readWrite(W25X_ReadData);
    FLASH_readWrite((uint8_t)((ReadAddr) >> 16));
    FLASH_readWrite((uint8_t)((ReadAddr) >> 8));
    FLASH_readWrite((uint8_t)ReadAddr);

    for (i = 0; i < size; i++)
    {
        pBuffer[i] = FLASH_readWrite(0XFF);
    }

    FLASH_CS_SET;
}

/*********************************************************************
 * @fn      SPI_Flash_Write_Page
 *
 * @brief   Write data by one page.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t size)
{
    uint16_t i;

    SPI_FLASH_Write_Enable();
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_PageProgram);
    FLASH_readWrite((uint8_t)((WriteAddr) >> 16));
    FLASH_readWrite((uint8_t)((WriteAddr) >> 8));
    FLASH_readWrite((uint8_t)WriteAddr);

    for (i = 0; i < size; i++)
    {
        FLASH_readWrite(pBuffer[i]);
    }

    FLASH_CS_SET;
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Write_NoCheck
 *
 * @brief   Write data to flash.(need Erase)
 *          All data in address rang is 0xFF.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t size)
{
    uint16_t pageremain;

    pageremain = 256 - WriteAddr % 256;

    if (size <= pageremain)
        pageremain = size;

    while (1)
    {
        SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);

        if (size == pageremain)
        {
            break;
        }
        else
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            size -= pageremain;

            if (size > 256)
                pageremain = 256;
            else
                pageremain = size;
        }
    }
}

static uint8_t W25QXX_BUFFER[4096];
/*********************************************************************
 * @fn      SPI_Flash_Write
 *
 * @brief   Write data to flash.(no need Erase)
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t size)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;

    secpos    = WriteAddr / 4096;
    secoff    = WriteAddr % 4096;
    secremain = 4096 - secoff;

    if (size <= secremain)
        secremain = size;

    while (1)
    {
        SPI_Flash_Read(W25QXX_BUFFER, secpos * 4096, 4096);

        for (i = 0; i < secremain; i++)
        {
            if (W25QXX_BUFFER[secoff + i] != 0XFF)
                break;
        }

        if (i < secremain)
        {
            SPI_Flash_Erase_Sector(secpos);

            for (i = 0; i < secremain; i++)
            {
                W25QXX_BUFFER[i + secoff] = pBuffer[i];
            }

            SPI_Flash_Write_NoCheck(W25QXX_BUFFER, secpos * 4096, 4096);
        }
        else
        {
            SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, secremain);
        }

        if (size == secremain)
        {
            break;
        }
        else
        {
            secpos++;
            secoff = 0;

            pBuffer += secremain;
            WriteAddr += secremain;
            size -= secremain;

            if (size > 4096)
            {
                secremain = 4096;
            }
            else
            {
                secremain = size;
            }
        }
    }
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Chip
 *
 * @brief   Erase all FLASH pages.
 *
 *
 * @return  none
 */
void SPI_Flash_Erase_Chip(void)
{
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_ChipErase);
    FLASH_CS_SET;
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_PowerDown
 *
 * @brief   Enter power down mode.
 *
 * @return  none
 */
void SPI_Flash_PowerDown(void)
{
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_PowerDown);
    FLASH_CS_SET;
    Delay_Us(3);
}

/*********************************************************************
 * @fn      SPI_Flash_WAKEUP
 *
 * @brief   Power down wake up.
 *
 * @return  none
 */
void SPI_Flash_WAKEUP(void)
{
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_ReleasePowerDown);
    FLASH_CS_SET;
    Delay_Us(3);
}

/*********************************************************************
 * @fn      SPI_Flash_Read_dma_start
 *
 * @brief   enable spi dma and set read address
 *   
 * @param    ReadAddr - the address in spi flash
 *
 * @return  none
 */
void SPI_Flash_Read_dma_start(uint32_t ReadAddr)
{
    FLASH_CS_CLR;
    FLASH_readWrite(W25X_FastReadData);
    FLASH_readWrite((uint8_t)((ReadAddr) >> 16));
    FLASH_readWrite((uint8_t)((ReadAddr) >> 8));
    FLASH_readWrite((uint8_t)ReadAddr);
    FLASH_readWrite(0xFF);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
}

/*********************************************************************
 * @fn      SPI_Flash_Read_dma_end
 *
 * @brief   disable spi dma
 *
 * @return  none
 */
void SPI_Flash_Read_dma_end(void)
{
    FLASH_CS_SET;
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE);
}
