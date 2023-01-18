/********************************** (C) COPYRIGHT *******************************
* File Name          : SW_CDROM.C
* Author             : WCH
* Version            : V1.0.1
* Date               : 2022/11/24
* Description        : CD-ROM Source File
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/******************************************************************************/
/* Header Files */
#include <SPI_FLASH.h>
#include <SW_CDROM.h>
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_spi.h"
/******************************************************************************/
/* Variable Definition */
__attribute__ ((aligned(4))) uint8_t  CDROM_Pack_Buffer[DEF_CDROM_PACK_512];

/******************************************************************************/
/* INQUITY */
uint8_t CDROM_Inquity_Tab[ ] =
{
    0x05,                                                /* Peripheral Device CD-ROM = 0x05 */
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
/* formatted capacity information */
uint8_t CDROM_Rd_Format_Capacity[ ] =
{
    0x00,
    0x00,
    0x00,
    0x08,
    ( MY_CDROM_SIZE >> 24 ) & 0xFF,
    ( MY_CDROM_SIZE >> 16 ) & 0xFF,
    ( MY_CDROM_SIZE >> 8 ) & 0xFF,
    ( MY_CDROM_SIZE ) & 0xFF,
    0x02,
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,             /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};

/******************************************************************************/
/* capacity information */
uint8_t CDROM_Rd_Capacity[ ] =
{
    ( ( MY_CDROM_SIZE - 1 ) >> 24 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 ) >> 16 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 ) >> 8 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 ) ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 24 ) & 0xFF,             /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};

/******************************************************************************/
/* MODE_SENSE data,For CMD 0X1A */
uint8_t CDROM_Mode_Sense_1A[ ] =
{
    0x0B,
    0x00,
    0x00,                                               /* 0x00:write-unprotected,0x80:write-protected */
    0x08,
    ( ( MY_CDROM_SIZE - 1 ) >> 24 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 )  >> 16 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 )  >> 8 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 )  ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 24 ) & 0xFF,             /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};

/******************************************************************************/
/* MODE_SENSE data,For CMD 0X5A */
uint8_t CDROM_Mode_Senese_5A[ ] =
{
    0x00,
    0x0E,
    0x00,
    0x00,                                              /* 0x00:write-unprotected,0x80:write-protected */
    0x00,
    0x00,
    0x00,
    0x08,
    ( ( MY_CDROM_SIZE - 1 ) >> 24 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 )  >> 16 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 )  >> 8 ) & 0xFF,
    ( ( MY_CDROM_SIZE - 1 )  ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 24 ) & 0xFF,            /* Number of Blocks */
    ( DEF_CFG_DISK_SEC_SIZE >> 16 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE >> 8 ) & 0xFF,
    ( DEF_CFG_DISK_SEC_SIZE ) & 0xFF,
};


/* command response ->0x46 */
uint8_t CDROM_CMD46_Ret[ 136 ] =
{
    0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x03, 0x0C, 0x00, 0x08, 0x00, 0x00,
    0x00, 0x09, 0x00, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x00, 0x01, 0x03, 0x04, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x04, 0x2C, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x01, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x1D, 0x03, 0x00,
    0x00, 0x1E, 0x05, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x21, 0x01, 0x08, 0x00, 0x00, 0x00, 0x01,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x23, 0x01, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x2D, 0x01, 0x04,
    0x17, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x01, 0x04, 0x2F, 0x00, 0x20, 0x00, 0x01, 0x00, 0x03, 0x00,
    0x01, 0x03, 0x00, 0x04, 0x07, 0x00, 0x00, 0xFF, 0x01, 0x08, 0x03, 0x0C, 0x41, 0x41, 0x30, 0x31,
    0x20, 0x30, 0x30, 0x38, 0x31, 0x39, 0x37, 0x20
};

/* command response ->0x4A */
uint8_t CDROM_CMD4A_00_Ret[ 4 ] =
{
    /* Event Header Return Data */
    0x00, 0x02,                                                                 /* Event Data Length */
    0x04,                                                                       /* Returns Media Class events */
    0x10                                                                        /* support Media Status Class Events */
};

uint8_t CDROM_CMD4A_10_Ret[ 8 ] =
{
    0x00, 0x06,                                                                 /* Event Header Return Data */
    0x84,                                                                       /* NEA=1 */
    0x10,
    0x00, 0x00, 0x00, 0x00                                                      /* Media Event Descriptor */
};

/* command response ->0x43 */
uint8_t CDROM_CMD43_40_Ret[ 12 ] =
{
    0x00,
    0x0a,                                                                       /* number of bytes below */
    0x01,                                                                       /*first track */
    0x01,                                                                       /* last track */
    0x00,                                                                       /* reserved */
    0x14,                                                                       /* (adr, control) */
    0x01,                                                                       /* (track being described) */
    0x00,                                                                       /* (reserved) */
    0x00,
    (uint8_t)( ( MY_CDROM_SIZE - 1 ) >>16 ),
    (uint8_t)( ( MY_CDROM_SIZE - 1 ) >> 8 ),
    (uint8_t)( MY_CDROM_SIZE - 1 )                                              /* (start logical block address 0) */
};

uint8_t CDROM_CMD43_00_Ret[ 20 ] =
{
    0x00,
    0x12,                                                                       /* number of bytes below */
    0x01,                                                                       /* first track */
    0x01,                                                                       /* last track */

    0x00,                                                                       /* reserved */
    0x14,                                                                       /* (adr, control) */
    0x01,                                                                       /* (track being described) */
    0x00,                                                                       /* (reserved) */
    0x00,
    0x02,
    0x00,
    0x00,

    0x00,
    0x14,
    0xaa,
    0x00,

    0x00,
    (uint8_t)( ( MY_CDROM_SIZE - 1 ) >> 16 ),
    (uint8_t)( ( MY_CDROM_SIZE - 1 ) >> 8 ),
    (uint8_t)( MY_CDROM_SIZE - 1 )                                           /* (start logical block address 0) */
};


const uint8_t CDROM_CMD43_80_Ret[ 48 ] =
{
    0x00, 0x2E, 0x01, 0x01, 0x01, 0x14, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x00, 0x01,
    0x14, 0x00, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x14, 0x00, 0xA2, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x2F, 0x3D, 0x01, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00
};

/******************************************************************************/
/* command response ->0x51 */
const uint8_t CDROM_CMD51_Ret[ 28 ] =
{
    0x00, 0x20, 0x0e, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
};

/******************************************************************************/
/* command response ->0x52 */
const uint8_t CDROM_CMD52_Ret[ 28 ] =
{
    0x00, 0x1A, 0x01, 0x01, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00
};

const uint8_t CDROM_CMD5A_2A_Ret[ 28 ] =
{
    0x00, 0x1a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x12, 0x07, 0x07, 0x70, 0x00, 0x28, 0x00,
    0x56, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x56, 0xa0, 0x00, 0x00, 0x00, 0x00
};


volatile uint8_t  CDROM_Status = 0x00;
volatile uint8_t  CDROM_Transfer_Status = 0x00; 
volatile uint32_t CDROM_Capability = 0x00; 
volatile uint8_t  CDROM_CBW_Tag_Save[ 4 ];
volatile uint8_t  CDROM_Sense_Key = 0x00;
volatile uint8_t  CDROM_Sense_ASC = 0x00;
volatile uint8_t  CDROM_CSW_Status = 0x00;

volatile uint32_t CDROM_Transfer_DataLen = 0x00;
volatile uint32_t CDROM_Cur_Sec_Lba = 0x00;
volatile uint16_t CDROM_Sec_Pack_Count = 0x00;
volatile uint16_t CDROM_Pack_Size = DEF_CDROM_PACK_512;

BULK_ONLY_CMD mBOC;
uint8_t   *pEndp2_Buf;


/*******************************************************************************
* Function Name  : CDROM_CMD_Deal_Ret_ErrorData
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void CDROM_CMD_Deal_Ret_ErrorData()
{
    uint16_t i;
    /* Upload 60 bytes of all 0 data */
    if( CDROM_Transfer_DataLen )
    {
        if( CDROM_Transfer_DataLen > 60 )
        {
            CDROM_Transfer_DataLen = 60;
        }
        for( i = 0x00; i < CDROM_Transfer_DataLen; i++ )
        {
            mBOC.buf[ i ] = 0x00;
        }
        pEndp2_Buf = mBOC.buf;
    }
}

/*******************************************************************************
* Function Name  : CDROM_CMD_Deal_Status
* Description    : Current command execution status
* Input          : key------Primary key for disk error details
*                  asc------Minor key for disk error details
*                  status---Current command execution result status
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_CMD_Deal_Status( uint8_t key, uint8_t asc, uint8_t status )
{
    CDROM_Sense_Key  = key;
    CDROM_Sense_ASC  = asc;
    CDROM_CSW_Status = status;
}

/*******************************************************************************
* Function Name  : CDROM_CMD_Deal_Fail
* Description    : The current command failed to execute
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_CMD_Deal_Fail( void )
{
    if( CDROM_Transfer_Status & DEF_CDROM_BLUCK_UP_FLAG )
    {
        /* EP2 -> STALL */
        USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_STALL;
        CDROM_Transfer_Status &= ~DEF_CDROM_BLUCK_UP_FLAG;
    }
    if( CDROM_Transfer_Status & DEF_CDROM_BLUCK_DOWN_FLAG )
    {
        /* EP3 -> STALL */
        USBHSD->UEP3_RX_CTRL = ( USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK ) | USBHS_UEP_R_RES_STALL;
        CDROM_Transfer_Status &= ~DEF_CDROM_BLUCK_DOWN_FLAG;
    }
}

/*******************************************************************************
* Function Name  : CMD_RD_WR_Deal_Pre
* Description    : Preparation before read and write sector processing
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMD_RD_WR_Deal_Pre( void )
{
    /* Save the sector number to be operated currently */
    CDROM_Cur_Sec_Lba = (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 2 ] << 24;
    CDROM_Cur_Sec_Lba = CDROM_Cur_Sec_Lba + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 3 ] << 16 );
    CDROM_Cur_Sec_Lba = CDROM_Cur_Sec_Lba + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 4 ] << 8 );
    CDROM_Cur_Sec_Lba = CDROM_Cur_Sec_Lba + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 5 ] );
        
    /* Save the current length of data to be manipulated */                    
    CDROM_Transfer_DataLen = ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 7 ] << 8 );
    CDROM_Transfer_DataLen = CDROM_Transfer_DataLen + ( (uint32_t)mBOC.mCBW.mCBW_CB_Buf[ 8 ] );
    CDROM_Transfer_DataLen = CDROM_Transfer_DataLen * DEF_CDROM_SECTOR_SIZE;         

    /* Clear related variables */
    CDROM_Sec_Pack_Count = 0x00;
    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
}

/*******************************************************************************
* Function Name  : CDROM_SCSI_CMD_Deal
* Description    : dealing SCSI command
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_SCSI_CMD_Deal( void )
{
    uint8_t i;

    if( ( mBOC.mCBW.mCBW_Sig[ 0 ] == 'U' ) && ( mBOC.mCBW.mCBW_Sig[ 1 ] == 'S' ) 
      &&( mBOC.mCBW.mCBW_Sig[ 2 ] == 'B' ) && ( mBOC.mCBW.mCBW_Sig[ 3 ] == 'C' ) )
    {
        CDROM_CBW_Tag_Save[ 0 ] = mBOC.mCBW.mCBW_Tag[ 0 ];
        CDROM_CBW_Tag_Save[ 1 ] = mBOC.mCBW.mCBW_Tag[ 1 ];
        CDROM_CBW_Tag_Save[ 2 ] = mBOC.mCBW.mCBW_Tag[ 2 ];
        CDROM_CBW_Tag_Save[ 3 ] = mBOC.mCBW.mCBW_Tag[ 3 ];

        CDROM_Transfer_DataLen = ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 3 ] << 24;
        CDROM_Transfer_DataLen += ( ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 2 ] << 16 );
        CDROM_Transfer_DataLen += ( ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 1 ] << 8 );
        CDROM_Transfer_DataLen += ( ( uint32_t )mBOC.mCBW.mCBW_DataLen[ 0 ] );
        
        if( CDROM_Transfer_DataLen )                                     
        {
            if( mBOC.mCBW.mCBW_Flag & 0x80 )
            {
                CDROM_Transfer_Status |= DEF_CDROM_BLUCK_UP_FLAG;
            }    
            else
            {    
                CDROM_Transfer_Status |= DEF_CDROM_BLUCK_DOWN_FLAG;
            }
        }
        CDROM_Transfer_Status |= DEF_CDROM_CSW_UP_FLAG;

        /* SCSI command packet processing */ 
        switch( mBOC.mCBW.mCBW_CB_Buf[ 0 ] )
        {
            case  CMD_U_INQUIRY:                                                                    
                /* CMD: 0x12 */
                if( CDROM_Transfer_DataLen > 0x24 )
                {
                    CDROM_Transfer_DataLen = 0x24;
                }    

                /* Add upload FLASH chip ID number */
                CDROM_Inquity_Tab[ 32 ] =  (uint8_t)( Flash_ID >> 24 );
                CDROM_Inquity_Tab[ 33 ] =  (uint8_t)( Flash_ID >> 16 );
                CDROM_Inquity_Tab[ 34 ] =  (uint8_t)( Flash_ID >> 8 );
                CDROM_Inquity_Tab[ 35 ] =  (uint8_t)( Flash_ID );

                /* CD-ROM Mode */
                CDROM_Inquity_Tab[ 0 ] = 0x05;
                pEndp2_Buf = (uint8_t *)CDROM_Inquity_Tab;
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;
    
            case  CMD_U_READ_FORMAT_CAPACITY:                                     
                /* CMD: 0x23 */
                if( ( CDROM_Status & DEF_CDROM_EN_FLAG ) )
                {    
                    if( CDROM_Transfer_DataLen > 0x0C )
                    {
                        CDROM_Transfer_DataLen = 0x0C; 
                    }    
                                    
                    for( i = 0x00; i < CDROM_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = CDROM_Rd_Format_Capacity[ i ];
                    } 
                    mBOC.buf[ 4 ] = ( ( CDROM_Capability >> 24 ) & 0xFF );
                    mBOC.buf[ 5 ] = ( ( CDROM_Capability >> 16 ) & 0xFF );
                    mBOC.buf[ 6 ] = ( ( CDROM_Capability >> 8  ) & 0xFF );
                    mBOC.buf[ 7 ] = ( ( CDROM_Capability       ) & 0xFF );
                    pEndp2_Buf = mBOC.buf;   
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    CDROM_CMD_Deal_Fail( );
                }    
                break;

            case  CMD_U_READ_CAPACITY:                                              
                /* CMD: 0x25 */
                if( ( CDROM_Status & DEF_CDROM_EN_FLAG ) )  
                {    
                    if( CDROM_Transfer_DataLen > 0x08 )
                    {
                        CDROM_Transfer_DataLen = 0x08;
                    }    
                                                
                    for( i = 0x00; i < CDROM_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = CDROM_Rd_Capacity[ i ];    
                    } 
                    mBOC.buf[ 0 ] = ( ( CDROM_Capability - 1 ) >> 24 ) & 0xFF;
                    mBOC.buf[ 1 ] = ( ( CDROM_Capability - 1 ) >> 16 ) & 0xFF;
                    mBOC.buf[ 2 ] = ( ( CDROM_Capability - 1 ) >> 8  ) & 0xFF;
                    mBOC.buf[ 3 ] = ( ( CDROM_Capability - 1 )       ) & 0xFF;

                    pEndp2_Buf = mBOC.buf;     
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x02, 0x3A, 0x01 ); 
                    CDROM_CMD_Deal_Fail( ); 
                }    
                break;

            case  CMD_U_READ10:                                                     
                /* CMD: 0x28 */
                if( ( CDROM_Status & DEF_CDROM_EN_FLAG ) )
                {                    
                    CMD_RD_WR_Deal_Pre( );
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    CDROM_CMD_Deal_Fail( );
                }    
                break;
    
            case  CMD_U_WR_VERIFY10:                                             
                /* CMD: 0x2E */
            case  CMD_U_WRITE10:                                                
                /* CMD: 0x2A */
                if( CDROM_Status & DEF_CDROM_EN_FLAG )
                {        
                    CMD_RD_WR_Deal_Pre( );
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    CDROM_CMD_Deal_Fail( );
                }                
                break;
    
            case  CMD_U_MODE_SENSE:                                                 
                /* CMD: 0x1A */
                if( ( CDROM_Status & DEF_CDROM_EN_FLAG ) )
                {    
                    if( CDROM_Transfer_DataLen > 0x0C )
                    {
                        CDROM_Transfer_DataLen = 0x0C;
                    }
                    for( i = 0x00; i < CDROM_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = CDROM_Mode_Sense_1A[ i ];    
                    } 
                    mBOC.buf[ 4 ] = ( CDROM_Capability >> 24 ) & 0xFF;
                    mBOC.buf[ 5 ] = ( CDROM_Capability >> 16 ) & 0xFF;
                    mBOC.buf[ 6 ] = ( CDROM_Capability >> 8  ) & 0xFF;
                    mBOC.buf[ 7 ] = ( CDROM_Capability       ) & 0xFF;
                    pEndp2_Buf = mBOC.buf;                
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x02, 0x3A, 0x01 );
                    CDROM_CMD_Deal_Fail( );
                }
                break;

            case  CMD_U_MODE_SENSE2:                                             
                /* CMD: 0x5A */
                if( mBOC.mCBW.mCBW_CB_Buf[ 2 ] == 0x3F )
                {    
                    if( CDROM_Transfer_DataLen > 0x10 )
                    {
                        CDROM_Transfer_DataLen = 0x10;
                    }

                    for( i = 0x00; i < CDROM_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = CDROM_Mode_Senese_5A[ i ];    
                    } 
                    mBOC.buf[ 8 ]  = ( CDROM_Capability >> 24 ) & 0xFF;
                    mBOC.buf[ 9 ]  = ( CDROM_Capability >> 16 ) & 0xFF;
                    mBOC.buf[ 10 ] = ( CDROM_Capability >> 8  ) & 0xFF;
                    mBOC.buf[ 11 ] = ( CDROM_Capability       ) & 0xFF;
                    pEndp2_Buf = mBOC.buf;         
                }
                else if (mBOC.mCBW.mCBW_CB_Buf[ 2 ] == 0x2A)
				{
                    if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD5A_2A_Ret ) )
                    {
                        CDROM_Transfer_DataLen = sizeof( CDROM_CMD5A_2A_Ret );
                    }
                    for( i = 0x00; i < CDROM_Transfer_DataLen; i++ )
                    {
                        mBOC.buf[ i ] = CDROM_CMD5A_2A_Ret[ i ];
                    }
                    pEndp2_Buf = mBOC.buf;
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x05, 0x20, 0x01 );
                    CDROM_CMD_Deal_Fail( );
                }
                break;
    
            case  CMD_U_REQUEST_SENSE:                                              
                /* CMD: 0x03 */
                mBOC.ReqSense.ErrorCode = 0x70;
                mBOC.ReqSense.Reserved1 = 0x00;
                mBOC.ReqSense.SenseKey  = CDROM_Sense_Key;
                mBOC.ReqSense.Information[ 0 ] = 0x00;
                mBOC.ReqSense.Information[ 1 ] = 0x00;
                mBOC.ReqSense.Information[ 2 ] = 0x00;
                mBOC.ReqSense.Information[ 3 ] = 0x00;
                mBOC.ReqSense.SenseLength = 0x0A;
                mBOC.ReqSense.Reserved2[ 0 ] = 0x00;
                mBOC.ReqSense.Reserved2[ 1 ] = 0x00;
                mBOC.ReqSense.Reserved2[ 2 ] = 0x00;
                mBOC.ReqSense.Reserved2[ 3 ] = 0x00;
                mBOC.ReqSense.SenseCode = CDROM_Sense_ASC;
                mBOC.ReqSense.SenseCodeQua = 0x00;
                mBOC.ReqSense.Reserved3[ 0 ] = 0x00;
                mBOC.ReqSense.Reserved3[ 1 ] = 0x00;
                mBOC.ReqSense.Reserved3[ 2 ] = 0x00;
                mBOC.ReqSense.Reserved3[ 3 ] = 0x00;
                pEndp2_Buf = mBOC.buf;
                CDROM_CSW_Status = 0x00;
                break;
            case  CMD_U_TEST_READY:       
                if( CDROM_Status & DEF_CDROM_EN_FLAG )
                {    
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                }
                else
                {
                    CDROM_CMD_Deal_Status( 0x02, 0x3A, 0x01 ); 
                    CDROM_Transfer_Status |= DEF_CDROM_BLUCK_UP_FLAG;   
                    CDROM_CMD_Deal_Fail( ); 
                }
                break;
    
            case  CMD_U_PREVT_REMOVE:                                             
                /* CMD: 0x1E */
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;
    
            case  CMD_U_VERIFY10:                                                  
                /* CMD: 0x1F */
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;
                
            case  CMD_U_START_STOP:                                                  
                /* CMD: 0x1B */
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );
                break;

			case 0x46:
                if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD46_Ret ) )
                {
                    CDROM_Transfer_DataLen = sizeof( CDROM_CMD46_Ret );
                }
                pEndp2_Buf = (uint8_t *)CDROM_CMD46_Ret;
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                      /* Disk command executed successfully */
                break;

            case 0x4A:
                if( mBOC.mCBW.mCBW_CB_Buf[ 4 ] == 0x00 )
                {
                    if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD4A_00_Ret ) )
                    {
                        CDROM_Transfer_DataLen = sizeof( CDROM_CMD4A_00_Ret );
                    }
                    pEndp2_Buf = (uint8_t *)CDROM_CMD4A_00_Ret;
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                  /* Disk command executed successfully */
                }
                else if( mBOC.mCBW.mCBW_CB_Buf[ 4 ] == 0x10 )
                {
                    if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD4A_10_Ret ) )
                    {
                        CDROM_Transfer_DataLen = sizeof( CDROM_CMD4A_10_Ret );
                    }
                    pEndp2_Buf = (uint8_t *)CDROM_CMD4A_10_Ret;
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                  /* Disk command executed successfully */
                }
                else
                {
                    CDROM_CMD_Deal_Ret_ErrorData( );                            /* return invalid data */
                    CDROM_CMD_Deal_Status( 0x05, 0x20, 0x01 );                  /* report parameter error */
                    CDROM_CMD_Deal_Fail( );                                     /* Disk command execution failed */
                }
                break;

            case 0x43:
                if( mBOC.mCBW.mCBW_CB_Buf[ 9 ] == 0x40 )
                {
                    if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD43_40_Ret ) )
                    {
                        CDROM_Transfer_DataLen = sizeof( CDROM_CMD43_40_Ret );
                    }

                    CDROM_CMD43_40_Ret[9] = (uint8_t)( ( CDROM_Capability - 1) >>16 );
                    CDROM_CMD43_40_Ret[10] = (uint8_t)( ( CDROM_Capability - 1 ) >>8 );
                    CDROM_CMD43_40_Ret[11] = (uint8_t)( CDROM_Capability - 1);
                    pEndp2_Buf = (uint8_t *)CDROM_CMD43_40_Ret;
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                  /* Disk command executed successfully */
                }
                else if( mBOC.mCBW.mCBW_CB_Buf[ 9 ] == 0x00 )
                {
                    if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD43_00_Ret ) )
                    {
                        CDROM_Transfer_DataLen = sizeof( CDROM_CMD43_00_Ret );
                    }
                    CDROM_CMD43_00_Ret[17] = (uint8_t)( ( CDROM_Capability - 1) >>16 );
                    CDROM_CMD43_00_Ret[18] = (uint8_t)( ( CDROM_Capability - 1) >>8 );
                    CDROM_CMD43_00_Ret[19] = (uint8_t)( CDROM_Capability - 1);
                    pEndp2_Buf = (uint8_t *)CDROM_CMD43_00_Ret;
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                  /* Disk command executed successfully */
                }
                else if( mBOC.mCBW.mCBW_CB_Buf[ 9 ] == 0x80 )
                {
                    if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD43_80_Ret ) )
                    {
                        CDROM_Transfer_DataLen = sizeof( CDROM_CMD43_80_Ret );
                    }
                    pEndp2_Buf = (uint8_t *)CDROM_CMD43_80_Ret;
                    CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                  /* Disk command executed successfully */
                }
                else
                {
                    CDROM_CMD_Deal_Ret_ErrorData( );                            /* return invalid data */
                    CDROM_CMD_Deal_Status( 0x05, 0x20, 0x01 );                  /* report parameter error */
                    CDROM_CMD_Deal_Fail( );                                     /* Disk command execution failed */
                }
                break;

            case 0xBB:
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                      /* Disk command executed successfully */
                break;

            case 0x51:
                if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD51_Ret ) )
                {
                    CDROM_Transfer_DataLen = sizeof( CDROM_CMD51_Ret );
                }
                pEndp2_Buf = (uint8_t *)CDROM_CMD51_Ret;
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                      /* Disk command executed successfully */
                break;

            case 0x52:
                if( CDROM_Transfer_DataLen > sizeof( CDROM_CMD52_Ret ) )
                {
                    CDROM_Transfer_DataLen = sizeof( CDROM_CMD52_Ret );
                }
                pEndp2_Buf = (uint8_t *)CDROM_CMD52_Ret;
                CDROM_CMD_Deal_Status( 0x00, 0x00, 0x00 );                      /* Disk command executed successfully */
                break;

            case 0xAC:
            case 0x85:
                CDROM_CMD_Deal_Ret_ErrorData( );
                CDROM_CMD_Deal_Status( 0x05, 0x20, 0x01 );                      /* report parameter error */
                break;

            default:
                CDROM_CMD_Deal_Status( 0x05, 0x20, 0x01 );
                CDROM_Transfer_Status |= DEF_CDROM_BLUCK_UP_FLAG; 
                CDROM_CMD_Deal_Fail( );
                break;
        }
    }    
    else                                                                         
    {   /* Bad package flag for CBW package */
        CDROM_CMD_Deal_Status( 0x05, 0x20, 0x02 );
        CDROM_Transfer_Status |= DEF_CDROM_BLUCK_UP_FLAG;
        CDROM_Transfer_Status |= DEF_CDROM_BLUCK_DOWN_FLAG;
        CDROM_CMD_Deal_Fail(  );
    }
}

/*******************************************************************************
* Function Name  : CDROM_In_EP_Deal
* Description    : Upload endpoint handling
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_In_EP_Deal( void )
{        
    if( CDROM_Transfer_Status & DEF_CDROM_BLUCK_UP_FLAG ) 
    {
        if( mBOC.mCBW.mCBW_CB_Buf[ 0 ] == CMD_U_READ10 )
        {
            CDROM_Up_OnePack( );
        }
        else
        {
            CDROM_Bulk_UpData( );
        }
    }
    else if( CDROM_Transfer_Status & DEF_CDROM_CSW_UP_FLAG )
    {    
        CDROM_Up_CSW( );
    }
}

/*******************************************************************************
* Function Name  : CDROM_Out_EP_Deal
* Description    : Download endpoint processing
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_Out_EP_Deal( uint8_t *pbuf, uint16_t packlen )
{
    uint32_t i;
    /* Endpoint 2 download data processing */
    if( CDROM_Transfer_Status & DEF_CDROM_BLUCK_DOWN_FLAG )
    {
        CDROM_Down_OnePack( pbuf, packlen );
    }
    else
    {                                
        if( packlen == 0x1F )
        {
            for( i = 0; i < packlen; i++ ) 
            {
                mBOC.buf[ i ] = *pbuf++;
            }
            CDROM_SCSI_CMD_Deal( );
            if( ( CDROM_Transfer_Status & DEF_CDROM_BLUCK_DOWN_FLAG ) == 0x00 )
            {
                if( CDROM_Transfer_Status & DEF_CDROM_BLUCK_UP_FLAG )
                {
                    if( mBOC.mCBW.mCBW_CB_Buf[ 0 ] == CMD_U_READ10 )
                    {
                        CDROM_Up_OnePack( );
                    }
                    else
                    {
                        CDROM_Bulk_UpData( );
                    }
                }
                else if( CDROM_CSW_Status == 0x00 )
                {
                    /* upload CSW */
                    CDROM_Up_CSW(  );                     
                }                        
            }
        }
    }
}

/*******************************************************************************
* Function Name  : CDROM_Bulk_UpData
* Description    : EP2 upload data
* Input          : Transfer_DataLen--- length of data transferred
*                  *pBuf---data address pointer
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_Bulk_UpData( void )
{                                            
    uint32_t  len;

    if( CDROM_Transfer_DataLen > CDROM_Pack_Size )
    {
        len = CDROM_Pack_Size;
        CDROM_Transfer_DataLen -= CDROM_Pack_Size;
    }
    else
    {
        len = CDROM_Transfer_DataLen;
        CDROM_Transfer_DataLen = 0x00;
        CDROM_Transfer_Status &= ~DEF_CDROM_BLUCK_UP_FLAG;        
    }

    /* Load the data into the upload buffer and start the upload */
    USBHS_Endp_DataUp(DEF_UEP2, pEndp2_Buf, len, DEF_UEP_CPY_LOAD );
}

/*******************************************************************************
* Function Name  : CDROM_Up_CSW
* Description    : Bulk endpoint endpoint 2 upload CSW package
* Input          : CBW_Tag_Save---command block label
*                  CSW_Status---Current command execution result status
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_Up_CSW( void )
{
    CDROM_Transfer_Status = 0x00;

    mBOC.mCSW.mCSW_Sig[ 0 ] = 'U';
    mBOC.mCSW.mCSW_Sig[ 1 ] = 'S';
    mBOC.mCSW.mCSW_Sig[ 2 ] = 'B';
    mBOC.mCSW.mCSW_Sig[ 3 ] = 'S';
    mBOC.mCSW.mCSW_Tag[ 0 ] = CDROM_CBW_Tag_Save[ 0 ];
    mBOC.mCSW.mCSW_Tag[ 1 ] = CDROM_CBW_Tag_Save[ 1 ];
    mBOC.mCSW.mCSW_Tag[ 2 ] = CDROM_CBW_Tag_Save[ 2 ];
    mBOC.mCSW.mCSW_Tag[ 3 ] = CDROM_CBW_Tag_Save[ 3 ];
    mBOC.mCSW.mCSW_Residue[ 0 ] = 0x00;
    mBOC.mCSW.mCSW_Residue[ 1 ] = 0x00;
    mBOC.mCSW.mCSW_Residue[ 2 ] = 0x00;
    mBOC.mCSW.mCSW_Residue[ 3 ] = 0x00;
    mBOC.mCSW.mCSW_Status = CDROM_CSW_Status;

    /* Load the data into the upload buffer and start the upload */
    USBHS_Endp_DataUp(DEF_UEP2, (uint8_t *)mBOC.buf, 0x0D, DEF_UEP_CPY_LOAD );

}

/*******************************************************************************
* Function Name  : CDROM_Up_OnePack
* Description    : CD-ROM upload a pack
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_Up_OnePack( void )
{    
    uint8_t *pbuf = NULL;

    if( CDROM_Sec_Pack_Count == 0x00 )     
    {
        FLASH_RD_Block_Start( CDROM_Cur_Sec_Lba * DEF_CDROM_SECTOR_SIZE );
    }
    FLASH_RD_Block( CDROM_Pack_Buffer, CDROM_Pack_Size );
    pbuf = CDROM_Pack_Buffer;

    /* USB upload this package data */
    USBHS_Endp_DataUp(DEF_UEP2, pbuf,CDROM_Pack_Size, DEF_UEP_CPY_LOAD );

    /* Determine whether the current sector data is read and uploaded */
    CDROM_Sec_Pack_Count++;
    CDROM_Transfer_DataLen -= CDROM_Pack_Size;

    if( CDROM_Sec_Pack_Count == ( DEF_CDROM_SECTOR_SIZE / CDROM_Pack_Size ) )
    {
        PIN_FLASH_CS_HIGH( );
        CDROM_Sec_Pack_Count = 0x00;
        CDROM_Cur_Sec_Lba++;
    }
    /* Determine whether the current sector data is read and uploaded */    
     if( CDROM_Transfer_DataLen == 0x00 )
    {    
        CDROM_Transfer_Status &= ~DEF_CDROM_BLUCK_UP_FLAG;
    }
}

/*******************************************************************************
* Function Name  : CDROM_Down_OnePack
* Description    : Do nothing, cause CD-ROM Mode is read-only.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CDROM_Down_OnePack( uint8_t *pbuf, uint16_t packlen )
{
    // CD-ROM Mode is read-only
    return;
}
