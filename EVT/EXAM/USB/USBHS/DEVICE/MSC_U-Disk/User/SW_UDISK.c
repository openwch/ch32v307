/********************************** (C) COPYRIGHT *******************************
* File Name          : SW_UDISK.C
* Author             : WCH
* Version            : V1.00
* Date               : 2021/08/01
* Description        : Part of the program that simulates a USB drive (FLASH as the storage medium)
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/******************************************************************************/
/* Header file inclusions */
#include <SPI_FLASH.h>
#include <SW_UDISK.h>
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_spi.h"
#include "Internal_Flash.h"
/******************************************************************************/
/* Constants and variable definitions */

__attribute__ ((aligned(4))) uint8_t  UDisk_Down_Buffer[DEF_FLASH_SECTOR_SIZE];
__attribute__ ((aligned(4))) uint8_t  UDisk_Pack_Buffer[DEF_UDISK_PACK_512];

/******************************************************************************/
/* INQUIRYInformation */
uint8_t UDISK_Inquity_Tab[ ] =
{
    /* UDISK */
    0x00,                                                /* Peripheral Device Type:UDISK = 0x00 */
    0x80,                                                /* Removable */
    0x02,                                                /* ISO/ECMA */
    0x02,
    0x1F,                                                /* Additional Length */
    0x00,                                                /* Reserved */
    0x00,                                                /* Reserved */
    0x00,                                                /* Reserved */
    'F',                                                 /* Vendor Information */
    'l',
    'a',
    's',
    'h',
    ' ',
    ' ',
    ' ',
    'U',
    'S',
    'B',
    ' ',
    'S',
    'p',
    'e',
    'c',
    'i',
    'a',
    'l',
    ' ',
    'D',
    'i',
    's',
    'k',
    '2',
    '.',
    'D',
    '0'
};

/******************************************************************************/
/* Formatted capacity information */
uint8_t  const  UDISK_Rd_Format_Capacity[ ] =
{
    0x00,
    0x00,
    0x00,
    0x08,
    ( MY_UDISK_SIZE >> 24 ) & 0xFF,
    ( MY_UDISK_SIZE >> 16 ) & 0xFF,
    ( MY_UDISK_SIZE >> 8 ) & 0xFF,
    ( MY_UDISK_SIZE ) & 0xFF,
    0x00,
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,             /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};

/******************************************************************************/
/* Capacity information */
uint8_t  const  UDISK_Rd_Capacity[ ] =
{
    ( ( MY_UDISK_SIZE - 1 ) >> 24 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 ) >> 16 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 ) >> 8 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 ) ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 24 ) & 0xFF,             /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};

/******************************************************************************/
/* MODE_SENSEinformation, for CMD 0x1A */
uint8_t  const  UDISK_Mode_Sense_1A[ ] =
{
    0x0B,
    0x00,
    0x00,                                               /* 0x00:write-unprotected,0x80:write-protected */
    0x08,
    ( ( MY_UDISK_SIZE - 1 ) >> 24 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 )  >> 16 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 )  >> 8 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 )  ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 24 ) & 0xFF,             /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};

/******************************************************************************/
/* MODE_SENSEinformation, for CMD 0x5A */
uint8_t  const  UDISK_Mode_Senese_5A[ ] =
{
    0x00,
    0x0E,
    0x00,
    0x00,                                              /* 0x00:write-unprotected,0x80:write-protected */
    0x00,
    0x00,
    0x00,
    0x08,
    ( ( MY_UDISK_SIZE - 1 ) >> 24 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 )  >> 16 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 )  >> 8 ) & 0xFF,
    ( ( MY_UDISK_SIZE - 1 )  ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 24 ) & 0xFF,            /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};


volatile uint8_t  Udisk_Status = 0x00;                                                              /* UUSB drive related status */
volatile uint8_t  Udisk_Transfer_Status = 0x00;                                                     /* UUSB drive transfer related status */
volatile uint32_t Udisk_Capability = 0x00;                                                          /* UUSB drive formatted capacity */
volatile uint8_t  Udisk_CBW_Tag_Save[ 4 ];
volatile uint8_t  Udisk_Sense_Key = 0x00;
volatile uint8_t  Udisk_Sense_ASC = 0x00;
volatile uint8_t  Udisk_CSW_Status = 0x00;

volatile uint32_t UDISK_Transfer_DataLen = 0x00;
volatile uint32_t UDISK_Cur_Sec_Lba = 0x00;
volatile uint16_t UDISK_Sec_Pack_Count = 0x00;
volatile uint16_t UDISK_Pack_Size = DEF_UDISK_PACK_512;

BULK_ONLY_CMD mBOC;
uint8_t   *pEndp2_Buf;


/*******************************************************************************
* Function Name  : UDISK_CMD_Deal_Status
* Description    : Current USB drive command execution status
* Input          : key------Key of the disk error detail information
*                  asc------Sub-key of the disk error detail information
*                  status---Current command execution result status
* Output         : None
* Return         : None
*******************************************************************************/
void UDISK_CMD_Deal_Status( uint8_t key, uint8_t asc, uint8_t status )
{
    Udisk_Sense_Key  = key;
    Udisk_Sense_ASC  = asc;
    Udisk_CSW_Status = status;
}

/*******************************************************************************
* Function Name  : UDISK_CMD_Deal_Fail
* Description    : Current USB drive command execution failed
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UDISK_CMD_Deal_Fail( void )
{
    if( Udisk_Transfer_Status & DEF_UDISK_BLUCK_UP_FLAG )
    {
        /* Endpoint 2 returns STALL on upload */
        USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_STALL;
        Udisk_Transfer_Status &= ~DEF_UDISK_BLUCK_UP_FLAG;
    }
    if( Udisk_Transfer_Status & DEF_UDISK_BLUCK_DOWN_FLAG )
    {
        /* Endpoint 3 returns STALL on download */
        USBHSD->UEP3_RX_CTRL = ( USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK ) | USBHS_UEP_R_RES_STALL;
        Udisk_Transfer_Status &= ~DEF_UDISK_BLUCK_DOWN_FLAG;
    }
}

/*******************************************************************************
* Function Name  : CMD_RD_WR_Deal_Pre
* Description    : Preparation before sector read/write processing
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMD_RD_WR_Deal_Pre( void )
{
    /* Save the sector number to operate on */
    UDISK_Cur_Sec_Lba = (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 2 ] << 24;
    UDISK_Cur_Sec_Lba = UDISK_Cur_Sec_Lba + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 3 ] << 16 );
    UDISK_Cur_Sec_Lba = UDISK_Cur_Sec_Lba + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 4 ] << 8 );
    UDISK_Cur_Sec_Lba = UDISK_Cur_Sec_Lba + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 5 ] );
        
    /* Save the length of the data to operate on */                    
    UDISK_Transfer_DataLen = ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 7 ] << 8 );
    UDISK_Transfer_DataLen = UDISK_Transfer_DataLen + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 8 ] );
    UDISK_Transfer_DataLen = UDISK_Transfer_DataLen * DEF_UDISK_SECTOR_SIZE;         

    /* Clear the related variables */
    UDISK_Sec_Pack_Count = 0x00;
    UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
}

/*******************************************************************************
* Function Name  : UDISK_SCSI_CMD_Deal
* Description    : 
* Input          : 
* Output         : 
* Return         : None
*******************************************************************************/
void UDISK_SCSI_CMD_Deal( void )
{
    uint8_t i;

    if( ( mBOC.mCBW.mCBW_Sig[ 0 ] == 'U' ) && ( mBOC.mCBW.mCBW_Sig[ 1 ] == 'S' ) 
      &&( mBOC.mCBW.mCBW_Sig[ 2 ] == 'B' ) && ( mBOC.mCBW.mCBW_Sig[ 3 ] == 'C' ) )
    {
        Udisk_CBW_Tag_Save[ 0 ] = mBOC.mCBW.mCBW_Tag[ 0 ];
        Udisk_CBW_Tag_Save[ 1 ] = mBOC.mCBW.mCBW_Tag[ 1 ];
        Udisk_CBW_Tag_Save[ 2 ] = mBOC.mCBW.mCBW_Tag[ 2 ];
        Udisk_CBW_Tag_Save[ 3 ] = mBOC.mCBW.mCBW_Tag[ 3 ];

        UDISK_Transfer_DataLen = ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 3 ] << 24;
        UDISK_Transfer_DataLen += ( ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 2 ] << 16 );
        UDISK_Transfer_DataLen += ( ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 1 ] << 8 );
        UDISK_Transfer_DataLen += ( ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 0 ] );
        
        if( UDISK_Transfer_DataLen )                                     
        {
            if( mBOC.mCBW.mCBW_Flag & 0x80 )
            {
                Udisk_Transfer_Status |= DEF_UDISK_BLUCK_UP_FLAG;
            }    
            else
            {    
                Udisk_Transfer_Status |= DEF_UDISK_BLUCK_DOWN_FLAG;
            }
        }
        Udisk_Transfer_Status |= DEF_UDISK_CSW_UP_FLAG;

        /* UUSB drive SCSI command packet handling */ 
        switch( mBOC.mCBW.mCBW_CB_Buf[ 0 ] )
        {
            case  CMD_U_INQUIRY:                                                                    
                /* CMD: 0x12 */
                if( UDISK_Transfer_DataLen > 0x24 )
                {
                    UDISK_Transfer_DataLen = 0x24;
                }    

                /* Add upload of the FLASH chip ID */
#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
                UDISK_Inquity_Tab[ 32 ] =  (uint8_t)( Flash_ID >> 24 );
                UDISK_Inquity_Tab[ 33 ] =  (uint8_t)( Flash_ID >> 16 );
                UDISK_Inquity_Tab[ 34 ] =  (uint8_t)( Flash_ID >> 8 );
                UDISK_Inquity_Tab[ 35 ] =  (uint8_t)( Flash_ID );
#endif
                /* UDISKmode */
                UDISK_Inquity_Tab[ 0 ] = 0x00;
                pEndp2_Buf = (uint8_t *)UDISK_Inquity_Tab;
                UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;
    
            case  CMD_U_READ_FORMAT_CAPACITY:                                     
                /* CMD: 0x23 */
                if( ( Udisk_Status & DEF_UDISK_EN_FLAG ) )
                {    
                    if( UDISK_Transfer_DataLen > 0x0C )
                    {
                        UDISK_Transfer_DataLen = 0x0C; 
                    }    
                                    
                    for( i = 0x00; i < UDISK_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = UDISK_Rd_Format_Capacity[ i ];
                    } 
                    mBOC.buf[ 4 ] = ( ( Udisk_Capability >> 24 ) & 0xFF );
                    mBOC.buf[ 5 ] = ( ( Udisk_Capability >> 16 ) & 0xFF );
                    mBOC.buf[ 6 ] = ( ( Udisk_Capability >> 8  ) & 0xFF );
                    mBOC.buf[ 7 ] = ( ( Udisk_Capability       ) & 0xFF );
                    pEndp2_Buf = mBOC.buf;   
                    UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    UDISK_CMD_Deal_Fail( );
                }    
                break;

            case  CMD_U_READ_CAPACITY:                                              
                /* CMD: 0x25 */
                if( ( Udisk_Status & DEF_UDISK_EN_FLAG ) )  
                {    
                    if( UDISK_Transfer_DataLen > 0x08 )
                    {
                        UDISK_Transfer_DataLen = 0x08;
                    }    
                                                
                    for( i = 0x00; i < UDISK_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = UDISK_Rd_Capacity[ i ];    
                    } 
                    mBOC.buf[ 0 ] = ( ( Udisk_Capability - 1 ) >> 24 ) & 0xFF;
                    mBOC.buf[ 1 ] = ( ( Udisk_Capability - 1 ) >> 16 ) & 0xFF;
                    mBOC.buf[ 2 ] = ( ( Udisk_Capability - 1 ) >> 8  ) & 0xFF;
                    mBOC.buf[ 3 ] = ( ( Udisk_Capability - 1 )       ) & 0xFF;

                    pEndp2_Buf = mBOC.buf;     
                    UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x02, 0x3A, 0x01 ); 
                    UDISK_CMD_Deal_Fail( ); 
                }    
                break;

            case  CMD_U_READ10:                                                     
                /* CMD: 0x28 */
                if( ( Udisk_Status & DEF_UDISK_EN_FLAG ) )
                {                    
                    CMD_RD_WR_Deal_Pre( );
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    UDISK_CMD_Deal_Fail( );
                }    
                break;
    
            case  CMD_U_WR_VERIFY10:                                             
                /* CMD: 0x2E */
            case  CMD_U_WRITE10:                                                
                /* CMD: 0x2A */
                if( Udisk_Status & DEF_UDISK_EN_FLAG )
                {        
                    CMD_RD_WR_Deal_Pre( );
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    UDISK_CMD_Deal_Fail( );
                }                
                break;
    
            case  CMD_U_MODE_SENSE:                                                 
                /* CMD: 0x1A */
                if( ( Udisk_Status & DEF_UDISK_EN_FLAG ) )
                {    
                    if( UDISK_Transfer_DataLen > 0x0C )
                    {
                        UDISK_Transfer_DataLen = 0x0C;
                    }
                    for( i = 0x00; i < UDISK_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = UDISK_Mode_Sense_1A[ i ];    
                    } 
                    mBOC.buf[ 4 ] = ( Udisk_Capability >> 24 ) & 0xFF;
                    mBOC.buf[ 5 ] = ( Udisk_Capability >> 16 ) & 0xFF;
                    mBOC.buf[ 6 ] = ( Udisk_Capability >> 8  ) & 0xFF;
                    mBOC.buf[ 7 ] = ( Udisk_Capability       ) & 0xFF;
                    pEndp2_Buf = mBOC.buf;                
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    UDISK_CMD_Deal_Fail( );
                }
                break;

            case  CMD_U_MODE_SENSE2:                                             
                /* CMD: 0x5A */
                if( mBOC.mCBW.mCBW_CB_Buf[ 2 ] == 0x3F )
                {    
                    if( UDISK_Transfer_DataLen > 0x10 )
                    {
                        UDISK_Transfer_DataLen = 0x10;
                    }

                    for( i = 0x00; i < UDISK_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = UDISK_Mode_Senese_5A[ i ];    
                    } 
                    mBOC.buf[ 8 ]  = ( Udisk_Capability >> 24 ) & 0xFF;
                    mBOC.buf[ 9 ]  = ( Udisk_Capability >> 16 ) & 0xFF;
                    mBOC.buf[ 10 ] = ( Udisk_Capability >> 8  ) & 0xFF;
                    mBOC.buf[ 11 ] = ( Udisk_Capability       ) & 0xFF;
                    pEndp2_Buf = mBOC.buf;         
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x05, 0x20, 0x01 );
                    UDISK_CMD_Deal_Fail( );
                }
                break;
    
            case  CMD_U_REQUEST_SENSE:                                              
                /* CMD: 0x03 */
                if( UDISK_Transfer_DataLen > 0x12 )
                {
                    UDISK_Transfer_DataLen = 0x12;
                }
                mBOC.ReqSense.ErrorCode = 0x70;
                mBOC.ReqSense.Reserved1 = 0x00;
                mBOC.ReqSense.SenseKey  = Udisk_Sense_Key;
                mBOC.ReqSense.Information[ 0 ] = 0x00;
                mBOC.ReqSense.Information[ 1 ] = 0x00;
                mBOC.ReqSense.Information[ 2 ] = 0x00;
                mBOC.ReqSense.Information[ 3 ] = 0x00;
                mBOC.ReqSense.SenseLength = 0x0A;
                mBOC.ReqSense.Reserved2[ 0 ] = 0x00;
                mBOC.ReqSense.Reserved2[ 1 ] = 0x00;
                mBOC.ReqSense.Reserved2[ 2 ] = 0x00;
                mBOC.ReqSense.Reserved2[ 3 ] = 0x00;
                mBOC.ReqSense.SenseCode = Udisk_Sense_ASC;
                mBOC.ReqSense.SenseCodeQua = 0x00;
                mBOC.ReqSense.Reserved3[ 0 ] = 0x00;
                mBOC.ReqSense.Reserved3[ 1 ] = 0x00;
                mBOC.ReqSense.Reserved3[ 2 ] = 0x00;
                mBOC.ReqSense.Reserved3[ 3 ] = 0x00;
                pEndp2_Buf = mBOC.buf;
                Udisk_CSW_Status = 0x00;
                break;
            case  CMD_U_TEST_READY:       
                if( Udisk_Status & DEF_UDISK_EN_FLAG )
                {    
                    UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                }
                else
                {
                    UDISK_CMD_Deal_Status( 0x02, 0x3A, 0x01 ); 
                    Udisk_Transfer_Status |= DEF_UDISK_BLUCK_UP_FLAG;   
                    UDISK_CMD_Deal_Fail( ); 
                }
                break;
    
            case  CMD_U_PREVT_REMOVE:                                             
                /* CMD: 0x1E */
                UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;
    
            case  CMD_U_VERIFY10:                                                  
                /* CMD: 0x1F */
                UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;
                
            case  CMD_U_START_STOP:                                                  
                /* CMD: 0x1B */
                UDISK_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;

            default:
                UDISK_CMD_Deal_Status( 0x05, 0x20, 0x01 );
                Udisk_Transfer_Status |= DEF_UDISK_BLUCK_UP_FLAG; 
                UDISK_CMD_Deal_Fail( );
                break;
        }
    }    
    else                                                                         
    {   /* CBWThe packet flag of the packet is invalid */
        UDISK_CMD_Deal_Status( 0x05, 0x20, 0x02 );
        Udisk_Transfer_Status |= DEF_UDISK_BLUCK_UP_FLAG;
        Udisk_Transfer_Status |= DEF_UDISK_BLUCK_DOWN_FLAG;
        UDISK_CMD_Deal_Fail(  );
    }
}

/*******************************************************************************
* Function Name  : UDISK_In_EP_Deal
* Description    : UUSB drive upload endpoint handling
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UDISK_In_EP_Deal( void )
{        
    if( Udisk_Transfer_Status & DEF_UDISK_BLUCK_UP_FLAG ) 
    {
        if( mBOC.mCBW.mCBW_CB_Buf[ 0 ] == CMD_U_READ10 )
        {
            UDISK_Up_OnePack( );
        }
        else
        {
            UDISK_Bulk_UpData( );
        }
    }
    else if( Udisk_Transfer_Status & DEF_UDISK_CSW_UP_FLAG )
    {    
        UDISK_Up_CSW( );
    }
}

/*******************************************************************************
* Function Name  : UDISK_Out_EP_Deal
* Description    : UUSB drive download endpoint handling
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UDISK_Out_EP_Deal( uint8_t *pbuf, uint16_t packlen )
{
    uint32_t i;
    /* Process the data downloaded on endpoint 2 */
    if( Udisk_Transfer_Status & DEF_UDISK_BLUCK_DOWN_FLAG )
    {
        /* Process the downloaded USB data packet */
        UDISK_Down_OnePack( pbuf, packlen );
    }
    else
    {                                
        if( packlen == 0x1F )
        {
            for( i = 0; i < packlen; i++ ) 
            {
                mBOC.buf[ i ] = *pbuf++;
            }

            /* Process the SCSI command packet */
            UDISK_SCSI_CMD_Deal( );
            if( ( Udisk_Transfer_Status & DEF_UDISK_BLUCK_DOWN_FLAG ) == 0x00 )
            {
                if( Udisk_Transfer_Status & DEF_UDISK_BLUCK_UP_FLAG )
                {
                    if( mBOC.mCBW.mCBW_CB_Buf[ 0 ] == CMD_U_READ10 )
                    {
                        UDISK_Up_OnePack( );
                    }
                    else
                    {
                        UDISK_Bulk_UpData( );
                    }
                }
                else if( Udisk_CSW_Status == 0x00 )
                {
                    /* Upload the CSW packet */
                    UDISK_Up_CSW(  );                     
                }                        
            }
        }
    }
}

/*******************************************************************************
* Function Name  : UDISK_Bulk_UpData
* Description    : Upload the data packet on bulk endpoint 2
* Input          : Transfer_DataLen--- Data transfer length
*                  *pBuf---Data address pointer
* Output         : Transfer_DataLen--- Data transfer length
*                  *pBuf---Data address pointer
* Return         : None
*******************************************************************************/
void UDISK_Bulk_UpData( void )
{                                            
    uint32_t  len;

    if( UDISK_Transfer_DataLen > UDISK_Pack_Size )
    {
        len = UDISK_Pack_Size;
        UDISK_Transfer_DataLen -= UDISK_Pack_Size;
    }
    else
    {
        len = UDISK_Transfer_DataLen;
        UDISK_Transfer_DataLen = 0x00;
        Udisk_Transfer_Status &= ~DEF_UDISK_BLUCK_UP_FLAG;        
    }

    /* Load the data into the upload buffer and start the upload */
    USBHS_Endp_DataUp(DEF_UEP2, pEndp2_Buf, len, DEF_UEP_CPY_LOAD );
}

/*******************************************************************************
* Function Name  : UDISK_Up_CSW
* Description    : Upload the CSW packet on bulk endpoint 2
* Input          : CBW_Tag_Save---Command block tag
*                  CSW_Status---Current command execution result status
* Output         : Bit_DISK_CSW---Flag indicating that no CSW packet has been uploaded yet
*                  Bit_DISK_Bluck_Up---Data upload flag
*                  Bit_DISK_Bluck_Down---Data download flag
* Return         : None
*******************************************************************************/
void UDISK_Up_CSW( void )
{
    Udisk_Transfer_Status = 0x00;

    mBOC.mCSW.mCSW_Sig[ 0 ] = 'U';
    mBOC.mCSW.mCSW_Sig[ 1 ] = 'S';
    mBOC.mCSW.mCSW_Sig[ 2 ] = 'B';
    mBOC.mCSW.mCSW_Sig[ 3 ] = 'S';
    mBOC.mCSW.mCSW_Tag[ 0 ] = Udisk_CBW_Tag_Save[ 0 ];
    mBOC.mCSW.mCSW_Tag[ 1 ] = Udisk_CBW_Tag_Save[ 1 ];
    mBOC.mCSW.mCSW_Tag[ 2 ] = Udisk_CBW_Tag_Save[ 2 ];
    mBOC.mCSW.mCSW_Tag[ 3 ] = Udisk_CBW_Tag_Save[ 3 ];
    mBOC.mCSW.mCSW_Residue[ 0 ] = 0x00;
    mBOC.mCSW.mCSW_Residue[ 1 ] = 0x00;
    mBOC.mCSW.mCSW_Residue[ 2 ] = 0x00;
    mBOC.mCSW.mCSW_Residue[ 3 ] = 0x00;
    mBOC.mCSW.mCSW_Status = Udisk_CSW_Status;

    /* Load the data into the upload buffer and start the upload */
    USBHS_Endp_DataUp(DEF_UEP2, (uint8_t *)mBOC.buf, 0x0D, DEF_UEP_CPY_LOAD );

}

/*******************************************************************************
* Function Name  : UDISK_Up_OnePack
* Description    : USBThe storage device uploads one data packet
*                  This routine is used for USB drive sector read commands; after each uploaded data packet, as needed, in the interrupt
*                  Decide whether to continue uploading
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UDISK_Up_OnePack( void )
{    
    /* Check whether a sector read command must be issued */
    uint8_t *pbuf = NULL;

#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
    if( UDISK_Sec_Pack_Count == 0x00 )     
    {
        FLASH_RD_Block_Start( UDISK_Cur_Sec_Lba * DEF_UDISK_SECTOR_SIZE );
    }
    FLASH_RD_Block( UDisk_Pack_Buffer, UDISK_Pack_Size );
    pbuf = UDisk_Pack_Buffer;
#elif (STORAGE_MEDIUM == MEDIUM_INTERAL_FLASH)
    pbuf = (uint8_t*)(IFLASH_UDISK_START_ADDR + UDISK_Cur_Sec_Lba * DEF_UDISK_SECTOR_SIZE + UDISK_Pack_Size * UDISK_Sec_Pack_Count);
#endif

    /* USBUpload this packet's data */
    USBHS_Endp_DataUp(DEF_UEP2, pbuf,UDISK_Pack_Size, DEF_UEP_CPY_LOAD );

    /* Check whether all data of the current sector has been read and uploaded */
    UDISK_Sec_Pack_Count++;
    UDISK_Transfer_DataLen -= UDISK_Pack_Size;

    if( UDISK_Sec_Pack_Count == ( DEF_UDISK_SECTOR_SIZE / UDISK_Pack_Size ) )
    {
#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
        PIN_FLASH_CS_HIGH( );
#endif
        UDISK_Sec_Pack_Count = 0x00;
        UDISK_Cur_Sec_Lba++;
    }
    /* Check whether all data has been uploaded */    
     if( UDISK_Transfer_DataLen == 0x00 )
    {    
        Udisk_Transfer_Status &= ~DEF_UDISK_BLUCK_UP_FLAG;
    }
}

/*******************************************************************************
* Function Name  : UDISK_Down_OnePack
* Description    : USBThe storage device processes one downloaded data packet
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UDISK_Down_OnePack( uint8_t *pbuf, uint16_t packlen )
{
    uint32_t address;
    uint32_t sec_start_addr;
    if( UDISK_Sec_Pack_Count == 0x00 )
    {
        address = (uint32_t)UDISK_Cur_Sec_Lba * DEF_UDISK_SECTOR_SIZE;
        sec_start_addr = ( address / DEF_FLASH_SECTOR_SIZE ) * DEF_FLASH_SECTOR_SIZE;
#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
        FLASH_Erase_Sector(sec_start_addr);
#endif
    }
    memcpy(UDisk_Down_Buffer + (uint32_t)UDISK_Sec_Pack_Count * UDISK_Pack_Size ,pbuf, UDISK_Pack_Size);
    UDISK_Sec_Pack_Count++;
    UDISK_Transfer_DataLen -= UDISK_Pack_Size;

    if( UDISK_Sec_Pack_Count == ( DEF_UDISK_SECTOR_SIZE / UDISK_Pack_Size ) )
    {
        address = (uint32_t)UDISK_Cur_Sec_Lba * DEF_UDISK_SECTOR_SIZE;
        sec_start_addr = ( address / DEF_FLASH_SECTOR_SIZE ) * DEF_FLASH_SECTOR_SIZE;

#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
        W25XXX_WR_Block(UDisk_Down_Buffer, sec_start_addr, DEF_FLASH_SECTOR_SIZE );
#elif (STORAGE_MEDIUM == MEDIUM_INTERAL_FLASH)
        IFlash_Prog_512(IFLASH_UDISK_START_ADDR + sec_start_addr,(uint32_t*)UDisk_Down_Buffer);
#endif
        /* Check whether all data has been downloaded */
        if( UDISK_Transfer_DataLen == 0x00 )
        {
            Udisk_Transfer_Status &= ~DEF_UDISK_BLUCK_DOWN_FLAG;
            UDISK_Up_CSW( );
        }
        UDISK_Sec_Pack_Count = 0x00;
        UDISK_Cur_Sec_Lba++;
    }
}
