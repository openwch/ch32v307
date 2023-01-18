/********************************** (C) COPYRIGHT *******************************
 * File Name          : SPI_FLAH.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : SPI FLASH芯片操作相关文件
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include <SPI_FLASH.h>
#include <SW_UDISK.h>
#include "ch32v30x_spi.h"

/******************************************************************************/
/* 常、变量定义 */
volatile uint8_t   Flash_Type = 0x00;                                           /* FLASH芯片类型: 0: W25XXX系列 */
volatile uint32_t  Flash_ID = 0x00;                                             /* FLASH芯片ID号 */
volatile uint32_t  Flash_Sector_Count = 0x00;                                   /* FLASH芯片扇区数 */
volatile uint16_t  Flash_Sector_Size = 0x00;                                    /* FLASH芯片扇区大小 */

/*******************************************************************************
* Function Name  : FLASH_Port_Init
* Description    : FLASH芯片操作相关引脚和硬件初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_Port_Init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 , ENABLE);

    /* CS# */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_2);

    /* SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);
//    SPI1->HSCR |= 0x01;                                                         // Enable High Speed Read
    SPI_Cmd(SPI1, ENABLE);
}

/*********************************************************************
 * @fn      SPI1_ReadWriteByte
 *
 * @brief   SPI1 read or write one byte.
 *
 * @param   TxData - write one byte data.
 *
 * @return  Read one byte data.
 */
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
    uint8_t i = 0;

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        i++;
        if(i > 200)
            return 0;
    }

    SPI_I2S_SendData(SPI1, TxData);
    i = 0;

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        i++;
        if(i > 200)
            return 0;
    }

    return SPI_I2S_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : SPI发送一个字节数据
* Input          : byte: 要发送的字节
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t SPI_FLASH_SendByte( uint8_t byte )
{
    return SPI1_ReadWriteByte( byte );
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : SPI接收一个字节数据
* Input          : None
* Output         : None
* Return         : 返回接收的字节数据
*******************************************************************************/
uint8_t SPI_FLASH_ReadByte( void )
{
    return SPI1_ReadWriteByte( 0xFF );
}

/*******************************************************************************
* Function Name  : FLASH_ReadID
* Description    : 读取FLASH芯片ID
* Input          : None
* Output         : None
* Return         : 返回4个字节,最高字节为0x00,
*                  次高字节为Manufacturer ID(0xEF),
*                  次低字节为Memory Type ID
*                  最低字节为Capacity ID
*                  W25X40BL返回: 0xEF、0x30、0x13
*                  W25X10BL返回: 0xEF、0x30、0x11
*******************************************************************************/
uint32_t FLASH_ReadID( void )
{
    uint32_t dat;
    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_JEDEC_ID );
    dat = ( uint32_t )SPI_FLASH_SendByte( DEF_DUMMY_BYTE ) << 16;
    dat |= ( uint32_t )SPI_FLASH_SendByte( DEF_DUMMY_BYTE ) << 8;
    dat |= SPI_FLASH_SendByte( DEF_DUMMY_BYTE );
    PIN_FLASH_CS_HIGH( );
    return( dat );
}

/*******************************************************************************
* Function Name  : FLASH_WriteEnable
* Description    : FLASH芯片允许写操作
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_WriteEnable( void )
{
    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_WREN );
    PIN_FLASH_CS_HIGH( );
}

/*******************************************************************************
* Function Name  : FLASH_WriteDisable
* Description    : FLASH芯片禁止写操作
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_WriteDisable( void )
{
    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_WRDI );
    PIN_FLASH_CS_HIGH( );
}

/*******************************************************************************
* Function Name  : FLASH_ReadStatusReg
* Description    : FLASH芯片读取状态寄存器 
* Input          : None
* Output         : None
* Return         : 返回寄存器值
*******************************************************************************/
uint8_t FLASH_ReadStatusReg( void )
{
    uint8_t  status;

    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_RDSR );
    status = SPI_FLASH_ReadByte( );
    PIN_FLASH_CS_HIGH( );
    return( status );
}

/*******************************************************************************
* Function Name  : FLASH_Erase_Sector
* Description    : FLASH扇区数据擦除
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/ 
void FLASH_Erase_Sector( uint32_t address )
{
    uint8_t  temp;
    FLASH_WriteEnable( );
    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_SECTOR_ERASE );
    SPI_FLASH_SendByte( (uint8_t)( address >> 16 ) );
    SPI_FLASH_SendByte( (uint8_t)( address >> 8 ) );
    SPI_FLASH_SendByte( (uint8_t)address );
    PIN_FLASH_CS_HIGH( );
    do
    {
        temp = FLASH_ReadStatusReg( );    
    }while( temp & 0x01 );
}

/*******************************************************************************
* Function Name  : FLASH_RD_Block_Start
* Description    : FLASH块数据读取开始
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/  
void FLASH_RD_Block_Start( uint32_t address )
{
    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_READ );
    SPI_FLASH_SendByte( (uint8_t)( address >> 16 ) );
    SPI_FLASH_SendByte( (uint8_t)( address >> 8 ) );
    SPI_FLASH_SendByte( (uint8_t)address );
}

/*******************************************************************************
* Function Name  : FLASH_RD_Block
* Description    : FLASH块数据读取
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/  
void FLASH_RD_Block( uint8_t *pbuf, uint32_t len )
{
    while( len-- )                                                             
    {
        *pbuf++ = SPI_FLASH_ReadByte( );
    }
}

/*******************************************************************************
* Function Name  : FLASH_RD_Block_End
* Description    : FLASH块数据读取结束
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_RD_Block_End( void )
{
    PIN_FLASH_CS_HIGH( );
}

/*******************************************************************************
* Function Name  : W25XXX_WR_Page
* Description    : 写入一块数据
*                  注：当前程序不支持跨页写数据      
* Input          : address----准备写的首地址
*                  len--------准备写的数据长度
*                  *pbuf------准备写入的缓冲区地址 
* Output         : None
* Return         : None
*******************************************************************************/
void W25XXX_WR_Page( uint8_t *pbuf, uint32_t address, uint32_t len )
{
    uint8_t  temp;
    FLASH_WriteEnable( );
    PIN_FLASH_CS_LOW( );
    SPI_FLASH_SendByte( CMD_FLASH_BYTE_PROG );
    SPI_FLASH_SendByte( (uint8_t)( address >> 16 ) );
    SPI_FLASH_SendByte( (uint8_t)( address >> 8 ) );
    SPI_FLASH_SendByte( (uint8_t)address );
    if( len > SPI_FLASH_PerWritePageSize )
    {
        len = SPI_FLASH_PerWritePageSize;
    }
    while( len-- )
    {
        SPI_FLASH_SendByte( *pbuf++ );
    }
    PIN_FLASH_CS_HIGH( );
    do
    {
        temp = FLASH_ReadStatusReg( );    
    }while( temp & 0x01 );
}

/*******************************************************************************
* Function Name  : W25XXX_WR_Block
* Description    : 写入一块数据
* Input          : address----准备写的首地址
*                  len--------准备写的数据长度
*                  *pbuf------准备写入的缓冲区地址
* Output         : None
* Return         : None
*******************************************************************************/
void W25XXX_WR_Block( uint8_t *pbuf, uint32_t address, uint32_t len )
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = address % SPI_FLASH_PageSize;
    count = SPI_FLASH_PageSize - Addr;
    NumOfPage =  len / SPI_FLASH_PageSize;
    NumOfSingle = len % SPI_FLASH_PageSize;

    if( Addr == 0 )
    {
        if( NumOfPage == 0 )
        {
            W25XXX_WR_Page( pbuf, address, len );
        }
        else
        {
            while( NumOfPage-- )
            {
                W25XXX_WR_Page( pbuf, address, SPI_FLASH_PageSize );
                address +=  SPI_FLASH_PageSize;
                pbuf += SPI_FLASH_PageSize;
            }
            W25XXX_WR_Page( pbuf, address, NumOfSingle );
        }
    }
    else
    {
        if( NumOfPage == 0 )
        {
            if( NumOfSingle > count )
            {
                temp = NumOfSingle - count;
                W25XXX_WR_Page( pbuf, address, count );
                address +=  count;
                pbuf += count;
                W25XXX_WR_Page( pbuf, address, temp );
            }
            else
            {
                W25XXX_WR_Page( pbuf, address, len );
            }
        }
        else
        {
            len -= count;
            NumOfPage =  len / SPI_FLASH_PageSize;
            NumOfSingle = len % SPI_FLASH_PageSize;
            W25XXX_WR_Page( pbuf, address, count );
            address +=  count;
            pbuf += count;
            while( NumOfPage-- )
            {
                W25XXX_WR_Page( pbuf, address, SPI_FLASH_PageSize );
                address += SPI_FLASH_PageSize;
                pbuf += SPI_FLASH_PageSize;
            }
            if( NumOfSingle != 0 )
            {
                W25XXX_WR_Page( pbuf, address, NumOfSingle );
            }
        }
    }
}



/*******************************************************************************
* Function Name  : FLASH_IC_Check
* Description    : FLASH芯片检测
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IC_Check( void )
{
    uint32_t count;

    /* 读取FLASH芯片ID号 */    
    Flash_ID = FLASH_ReadID( );
    printf("Flash_ID: %08x\n",(uint32_t)Flash_ID);

    /* 根据芯片型号,判断容量大小 */
    Flash_Type = 0x00;                                                                
    Flash_Sector_Count = 0x00;
    Flash_Sector_Size = 0x00;

    switch( Flash_ID )
    {
        /* W25XXX系列芯片 */
        case W25X10_FLASH_ID:                                                   /* 0xEF3011-----1M bit */
            count = 1;
            break;

        case W25X20_FLASH_ID:                                                   /* 0xEF3012-----2M bit */
            count = 2;
            break;

        case W25X40_FLASH_ID:                                                   /* 0xEF3013-----4M bit */
            count = 4;
            break;

        case W25X80_FLASH_ID:                                                   /* 0xEF4014-----8M bit */
            count = 8;
            break;

        case W25Q16_FLASH_ID1:                                                  /* 0xEF3015-----16M bit */
        case W25Q16_FLASH_ID2:                                                  /* 0xEF4015-----16M bit */
            count = 16;
            break;

        case W25Q32_FLASH_ID1:                                                  /* 0xEF4016-----32M bit */
        case W25Q32_FLASH_ID2:                                                  /* 0xEF6016-----32M bit */
            count = 32;
            break;

        case W25Q64_FLASH_ID1:                                                  /* 0xEF4017-----64M bit */
        case W25Q64_FLASH_ID2:                                                  /* 0xEF6017-----64M bit */
            count = 64;
            break;

        case W25Q128_FLASH_ID1:                                                 /* 0xEF4018-----128M bit */
        case W25Q128_FLASH_ID2:                                                 /* 0xEF6018-----128M bit */
            count = 128;
            break;

        case W25Q256_FLASH_ID1:                                                 /* 0xEF4019-----256M bit */
        case W25Q256_FLASH_ID2:                                                 /* 0xEF6019-----256M bit */
            count = 256;
            break;
         default:
             if( ( Flash_ID != 0xFFFFFFFF ) || ( Flash_ID != 0x00000000 ) )
             {
                count = 16;
             }
             else
             {
                 count = 0x00;
            }
            break;
    }
    count = ( (uint32_t)count * 1024 ) * ( (uint32_t)1024 / 8 );

    /* 保存FLASH芯片扇区数目,扇区大小等信息 */
    if( count )
    {
        Flash_Sector_Count = count / DEF_UDISK_SECTOR_SIZE;
        Flash_Sector_Size = DEF_UDISK_SECTOR_SIZE;
    }
}

