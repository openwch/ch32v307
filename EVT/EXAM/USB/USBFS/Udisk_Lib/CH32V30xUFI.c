/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2019   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CHRV3 **
**  KEIL423, gcc 8.20          **
*****************************************
*/
/* CHRV3 UHost file system interface for USB drives; supports FAT12/FAT16/FAT32 */

//#define DISK_BASE_BUF_LEN     512 /* Default disk data buffer size is 512 bytes (2048 or even 4096 may be selected to support USB drives with large sectors); 0 disables the buffer definition in this file and lets the application specify it in pDISK_BASE_BUF */
/* To share the disk data buffer and save RAM, define DISK_BASE_BUF_LEN as 0 to prevent the buffer from being defined in this file, and have the application store the start address of the shared buffer into pDISK_BASE_BUF before calling CHRV3LibInit */

//#define NO_DEFAULT_ACCESS_SECTOR      1       /* Disable the default disk sector read/write routine; a user-written routine replaces it below */
//#define NO_DEFAULT_DISK_CONNECT       1       /* Disable the default disk connection check routine; a user-written routine replaces it below */
//#define NO_DEFAULT_FILE_ENUMER        1       /* Disable the default file name enumeration callback; a user-written routine replaces it below */
//#include "CHRV3SFR.H"

#include "debug.h"
#include "ch32v30x.h"
#include "usb_host_config.h"
#include "CHRV3UFI.h"

uint8_t USBHostTransact( uint8_t endp_pid, uint8_t tog, uint32_t timeout )
{
#if DEF_USB_PORT_FS_EN
    uint8_t  r, trans_rerty;
    uint16_t i;
    USBFSH->HOST_TX_CTRL = USBFSH->HOST_RX_CTRL = 0;
    if( tog & 0x80 )
    {
        USBFSH->HOST_RX_CTRL = 1<<2;
    }
    if( tog & 0x40 )
    {
        USBFSH->HOST_TX_CTRL = 1<<2;
    }
    trans_rerty = 0;
    do
    {
        USBFSH->HOST_EP_PID = endp_pid;       // Specify token PID and endpoint number
        USBFSH->INT_FG = USBFS_UIF_TRANSFER;  // Allow transmission
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBFSH->INT_FG & USBFS_UIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }
        USBFSH->HOST_EP_PID = 0x00;  // Stop USB transfer
        if( ( USBFSH->INT_FG & USBFS_UIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }
        else
        {
            /* Complete transfer */
            if( USBFSH->INT_ST & USBFS_UIS_TOG_OK )
            {
                return ERR_SUCCESS;
            }
            r = USBFSH->INT_ST & USBFS_UIS_H_RES_MASK;  // USB device answer status
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }
            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_rerty;
            }
            else switch ( endp_pid >> 4 )
            {
                case USB_PID_SETUP:
                case USB_PID_OUT:
                    if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) || ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        }
        Delay_Us( 15 );
        if( USBFSH->INT_FG & USBFS_UIF_DETECT )
        {
            Delay_Us( 200 );
            if( USBFSH_CheckRootHubPortEnable( ) == 0 )
            {
                return ERR_USB_DISCON;  // USB device disconnect event
            }
        }
    }while( ++trans_rerty < 10 );

    return ERR_USB_TRANSFER; // Reply timeout
#elif DEF_USB_PORT_HS_EN
    uint8_t   r, trans_retry;
    uint16_t  i;
    USBHSH->HOST_TX_CTRL = USBHSH->HOST_RX_CTRL = 0;
    if( tog & 0x80 )
    {
        USBHSH->HOST_RX_CTRL = 1<<3;
    }
    if( tog & 0x40 )
    {
        USBHSH->HOST_TX_CTRL = 1<<3;
    }
    trans_retry = 0;
    do
    {
        USBHSH->HOST_EP_PID = endp_pid; // Set the token for the host to send the packet
        USBHSH->INT_FG = USBHS_UIF_TRANSFER;
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBHSH->INT_FG & USBHS_UIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }
        USBHSH->HOST_EP_PID = 0x00;
        if( ( USBHSH->INT_FG & USBHS_UIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }

        if( USBHSH->INT_FG & USBHS_UIF_DETECT )
        {
            USBHSH->INT_FG = USBHS_UIF_DETECT;
            Delay_Us( 200 );
            if( USBHSH->MIS_ST & USBHS_UIF_TRANSFER )
            {
                if( USBHSH->HOST_CTRL & USBHS_UH_SOF_EN )
                {
                    return ERR_USB_CONNECT;
                }
            }
            else
            {
                return ERR_USB_DISCON;
            }
        }
        else if( USBHSH->INT_FG & USBHS_UIF_TRANSFER ) // The packet transmission was successful
        {
            r = USBHSH->INT_ST & USBHS_UIS_H_RES_MASK;
            if( ( endp_pid >> 4 ) == USB_PID_IN )
            {
                if( USBHSH->INT_ST & USBHS_UIS_TOG_OK )
                {
                    return ERR_SUCCESS; // Packet token match
                }
            }
            else
            {
                if( ( r == USB_PID_ACK ) || ( r == USB_PID_NYET ) )
                {
                    return ERR_SUCCESS;
                }
            }
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }

            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_retry;
            }
            else switch( endp_pid >> 4  )
            {
                case USB_PID_SETUP:

                case USB_PID_OUT:
                    if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) || ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        }
        else
        {
            USBHSH->INT_FG = USBHS_UIF_DETECT | USBHS_UIF_TRANSFER | USBHS_UIF_SUSPEND | USBHS_UIF_HST_SOF | USBHS_UIF_FIFO_OV | USBHS_UIF_SETUP_ACT;
        }
        Delay_Us( 15 );
    } while( ++trans_retry < 10 );

    return ERR_USB_TRANSFER;
#endif
}

uint8_t HostCtrlTransfer( uint8_t *DataBuf, uint8_t *RetLen )
{
    uint8_t  ret;
    uint16_t retlen;
    retlen = (uint16_t)(*RetLen);
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_CtrlTransfer( RootHubDev[ DEF_USB_PORT_FS ].bEp0MaxPks, DataBuf, &retlen );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_CtrlTransfer( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, DataBuf, &retlen );
#endif
    return ret;
}

void CopySetupReqPkg( const char * pReqPkt )
{
    uint8_t i;
    for(i = 0; i != sizeof(USB_SETUP_REQ); i++)
    {
#if DEF_USB_PORT_FS_EN
        ((char *)pUSBFS_SetupRequest)[i] = *pReqPkt;
#elif DEF_USB_PORT_HS_EN
        ((char *)pUSBHS_SetupRequest)[i] = *pReqPkt;
#endif
        pReqPkt++;
    }
}

uint8_t CtrlGetDeviceDescrTB( void )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_GetDeviceDescr( &RootHubDev[ DEF_USB_PORT_FS ].bEp0MaxPks, TxBuffer );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_GetDeviceDescr( &RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer );
#endif
    return ret;
}

uint8_t CtrlGetConfigDescrTB( void )
{
    uint16_t len;
    uint8_t  ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_GetConfigDescr( RootHubDev[ DEF_USB_PORT_FS ].bEp0MaxPks, TxBuffer, 256, &len );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_GetConfigDescr( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer, 256, &len );
#endif
    return ret;
}

uint8_t CtrlSetUsbConfig( uint8_t cfg )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_SetUsbConfig( RootHubDev[ DEF_USB_PORT_FS ].bEp0MaxPks, cfg );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_SetUsbConfig( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, cfg );
#endif
    return ret;
}

uint8_t CtrlSetUsbAddress( uint8_t addr )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_SetUsbAddress( RootHubDev[ DEF_USB_PORT_FS ].bEp0MaxPks, addr );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_SetUsbAddress( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, addr );
#endif
    return ret;
}

uint8_t CtrlClearEndpStall( uint8_t endp )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_ClearEndpStall( RootHubDev[ DEF_USB_PORT_FS ].bEp0MaxPks, endp );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_ClearEndpStall( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, endp );
#endif
    return ret;
}

#ifndef FOR_ROOT_UDISK_ONLY
uint8_t CtrlGetHubDescr( void )
{

}

uint8_t HubGetPortStatus( uint8_t HubPortIndex )
{

}

uint8_t HubSetPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt )
{

}

uint8_t HubClearPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt )
{

}
#endif

CMD_PARAM_I mCmdParam;                      /* Command parameters */
#if     DISK_BASE_BUF_LEN > 0
//uint8_t   DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((at(BA_RAM+SZ_RAM/2)));    /* Disk data buffer in external RAM; its length is one sector */
uint8_t DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));            /* Disk data buffer in external RAM; its length is one sector */
//UINT8 DISK_FAT_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));             /* Disk FAT data buffer in external RAM; its length is one sector */
#endif

/* The routines below can be modified as needed */

#ifndef NO_DEFAULT_ACCESS_SECTOR        /* Defining NO_DEFAULT_ACCESS_SECTOR in the application disables the default disk sector read/write routine so that a user-written routine can replace it */
//if ( use_external_interface ) {  // Replace the low-level USB drive sector read/write routines
//    CHRV3vSectorSize=512;  // Set the actual sector size; it must be a multiple of 512 and is the sector size of the disk
//    CHRV3vSectorSizeB=9;   // Set the shift value of the actual sector size: 9 for 512, 10 for 1024 and 11 for 2048
//    CHRV3DiskStatus=DISK_MOUNTED;  // Force the block device connect to succeed (only the file system analysis is left)
//}

uint8_t CHRV3ReadSector( uint8_t SectCount, uint8_t *DataBuf )  /* Read several sectors from the disk into the buffer */
{
    uint8_t retry;
//  if ( use_external_interface ) return( extReadSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* External interface */
    for( retry = 0; retry < 3; retry ++ ) {  /* Error retry */
        pCBW -> mCBW_DataLen = (uint32_t)SectCount << CHRV3vSectorSizeB;  /* Data transfer length */
        pCBW -> mCBW_Flag = 0x80;
        pCBW -> mCBW_LUN = CHRV3vCurrentLun;
        pCBW -> mCBW_CB_Len = 10;
        pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_READ10;
        pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 2 ] = (uint8_t)( CHRV3vLbaCurrent >> 24 );
        pCBW -> mCBW_CB_Buf[ 3 ] = (uint8_t)( CHRV3vLbaCurrent >> 16 );
        pCBW -> mCBW_CB_Buf[ 4 ] = (uint8_t)( CHRV3vLbaCurrent >> 8 );
        pCBW -> mCBW_CB_Buf[ 5 ] = (uint8_t)( CHRV3vLbaCurrent );
        pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
        pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
        CHRV3BulkOnlyCmd( DataBuf );  /* Execute a command based on the Bulk-Only protocol */
        if ( CHRV3IntStatus == ERR_SUCCESS ) {
            return( ERR_SUCCESS );
        }
        CHRV3IntStatus = CHRV3AnalyzeError( retry );
        if ( CHRV3IntStatus != ERR_SUCCESS ) {
            return( CHRV3IntStatus );
        }
    }
    return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* Disk operation error */
}

#ifdef  EN_DISK_WRITE
uint8_t CHRV3WriteSector( uint8_t SectCount, uint8_t *DataBuf )  /* Write the multi-sector data block in the buffer to the disk */
{
    uint8_t retry;
//  if ( use_external_interface ) return( extWriteSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* External interface */
    for( retry = 0; retry < 3; retry ++ ) {  /* Error retry */
        pCBW -> mCBW_DataLen = (uint32_t)SectCount << CHRV3vSectorSizeB;  /* Data transfer length */
        pCBW -> mCBW_Flag = 0x00;
        pCBW -> mCBW_LUN = CHRV3vCurrentLun;
        pCBW -> mCBW_CB_Len = 10;
        pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_WRITE10;
        pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 2 ] = (uint8_t)( CHRV3vLbaCurrent >> 24 );
        pCBW -> mCBW_CB_Buf[ 3 ] = (uint8_t)( CHRV3vLbaCurrent >> 16 );
        pCBW -> mCBW_CB_Buf[ 4 ] = (uint8_t)( CHRV3vLbaCurrent >> 8 );
        pCBW -> mCBW_CB_Buf[ 5 ] = (uint8_t)( CHRV3vLbaCurrent );
        pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
        pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
        CHRV3BulkOnlyCmd( DataBuf );  /* Execute a command based on the Bulk-Only protocol */
        if ( CHRV3IntStatus == ERR_SUCCESS ) {
            Delay_Us( 200 );  /* Delay after a write operation */
            return( ERR_SUCCESS );
        }
        CHRV3IntStatus = CHRV3AnalyzeError( retry );
        if ( CHRV3IntStatus != ERR_SUCCESS ) {
            return( CHRV3IntStatus );
        }
    }
    return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* Disk operation error */
}
#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR

#ifndef NO_DEFAULT_DISK_CONNECT         /* Defining NO_DEFAULT_DISK_CONNECT in the application disables the default disk connection check routine so that a user-written routine can replace it */

/*
Convention: USB device address assignment rules (see USB_DEVICE_ADDR)
Address value   Device location
0x02    USB device or external HUB under the built-in root HUB0
0x03    USB device or external HUB under the built-in root HUB1
0x1x    USB device on port x of the external HUB under the built-in root HUB0, where x is 1~n
0x2x    USB device on port x of the external HUB under the built-in root HUB1, where x is 1~n
*/
#if DEF_USB_PORT_FS_EN
#define     UHUB_DEV_ADDR   (USBFSH->DEV_ADDR)
#define     UHUB_MIS_STAT   (USBFSH->MIS_ST)
#define     UHUB_HOST_CTRL  (USBFSH->HOST_CTRL)
#define     UHUB_INT_FLAG   (USBFSH->INT_FG)
#define     bUMS_ATTACH     USBFS_UMS_DEV_ATTACH
#define     bUMS_SUSPEND    USBFS_UMS_SUSPEND
#define     DEF_ADR_OFFSET  0
#elif DEF_USB_PORT_HS_EN
#define     UHUB_DEV_ADDR   (USBHSH->DEV_ADDR)
#define     UHUB_MIS_STAT   (USBHSH->MIS_ST)
#define     UHUB_HOST_CTRL  (USBHSH->HOST_CTRL)
#define     UHUB_INT_FLAG   (USBHSH->INT_FG)
#define     bUMS_ATTACH     USBHS_UMS_DEV_ATTACH
#define     bUMS_SUSPEND    USBHS_UMS_SUSPEND
#define     DEF_ADR_OFFSET  1
#endif

/* Check whether the disk is connected */
uint8_t CHRV3DiskConnect( void )
{
    uint8_t ums, devaddr;
    UHUB_DEV_ADDR = UHUB_DEV_ADDR & 0x7F;
    ums = UHUB_MIS_STAT;
    devaddr = UHUB_DEV_ADDR;
    if ( devaddr == (USB_DEVICE_ADDR + DEF_ADR_OFFSET) )
    {
        /* USB device on the built-in root HUB */
        if ( ums & bUMS_ATTACH )
        {
            /* A USB device is present on the built-in root HUB */
            if ( ( ums & bUMS_SUSPEND ) == 0 )
            {
                /* A USB device is present on the built-in root HUB and has not been plugged or unplugged */
                return( ERR_SUCCESS );  /* USBThe device is connected and has not been plugged or unplugged */
            }
            else
            {
                /* A USB device is present on the built-in root HUB */
                CHRV3DiskStatus = DISK_CONNECT;  /* It was disconnected at some point */
                return( ERR_SUCCESS );  /* An external HUB or USB device has been connected, or reconnected after being disconnected */
            }
        }
        else
        {
            /* USBDevice disconnected */
mDiskDisconn:
            CHRV3DiskStatus = DISK_DISCONNECT;
            return( ERR_USB_DISCON );
        }
    }
    else
    {
        goto mDiskDisconn;
    }
}
#endif  // NO_DEFAULT_DISK_CONNECT

#ifndef NO_DEFAULT_FILE_ENUMER          /* Defining NO_DEFAULT_FILE_ENUMER in the application disables the default file name enumeration callback so that a user-written routine can replace it */
void xFileNameEnumer( void )            /* File name enumeration callback */
{
/* If the enumeration index CHRV3vFileSize is set to 0xFFFFFFFF before calling FileOpen, this callback is invoked for every file that is found,
   After the callback xFileNameEnumer returns, FileOpen decrements CHRV3vFileSize and keeps enumerating until no more files or directories are found. The recommended approach is
   Define a global variable as 0 before calling FileOpen. After FileOpen invokes this routine, the routine derives the FAT_DIR_INFO structure from CHRV3vFdtOffset,
   Examine DIR_Attr and DIR_Name in the structure to decide whether this is the wanted file or directory name, record the related information and increment the global counter,
   When FileOpen returns, both ERR_MISS_FILE and ERR_FOUND_NAME count as success; the global variable holds the number of valid files found.
   Setting CHRV3vFileSize to 1 inside this callback xFileNameEnumer tells FileOpen to stop searching early. An example callback is shown below */
#if 0
    uint8_t         i;
    uint16_t        FileCount;
    PX_FAT_DIR_INFO pFileDir;
    uint8_t         *NameBuf;
    pFileDir = (PX_FAT_DIR_INFO)( pDISK_BASE_BUF + CHRV3vFdtOffset );  /* Start address of the current FDT */
    FileCount = (UINT16)( 0xFFFFFFFF - CHRV3vFileSize );  /* Enumeration index of the current file name; CHRV3vFileSize starts at 0xFFFFFFFF and is decremented each time a file name is found */
    if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* Check whether the buffer is large enough, assuming that each file name needs 12 bytes */
        NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* Compute the address of the buffer that holds the current file name */
        for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* Copy the file name; it is 11 characters long and spaces are not handled */
//      if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* Determine whether it is a directory name */
        NameBuf[ i ] = 0;  /* File name terminator */
    }
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER

uint8_t CHRV3LibInit( void )  /* Initialize the CHRV3 library; returns 0 on success */
{
    uint8_t s;
    s = CHRV3GetVer( );
    if( s < CHRV3_LIB_VER )
    {
        return( 0xFF );  /* Get the version number of the current library; an error is returned if the version is too old */
    }
    printf( "lib vision:%02x\r\n",s );
#if     DISK_BASE_BUF_LEN > 0
    pDISK_BASE_BUF = & DISK_BASE_BUF[0]; /* Points to the disk data buffer in external RAM */
    pDISK_FAT_BUF = & DISK_BASE_BUF[0];  /* Points to the disk FAT data buffer in external RAM; it may be shared with pDISK_BASE_BUF to save RAM */
//  pDISK_FAT_BUF = & DISK_FAT_BUF[0];   /* Points to the disk FAT data buffer in external RAM; kept separate from pDISK_BASE_BUF for higher speed */
/* To speed up file access, after calling CHRV3LibInit in main, point pDISK_FAT_BUF to another separately allocated buffer of the same size as pDISK_BASE_BUF */
#endif
    CHRV3DiskStatus = DISK_UNKNOWN;  /* Unknown status */
    CHRV3vSectorSizeB = 9;  /* The default sector size of the physical disk is 512 B */
    CHRV3vSectorSize = 512; /* The default sector size of the physical disk is 512 B; this value is the sector size of the disk */
    CHRV3vStartLba = 0;     /* Defaults to automatic detection of FDD and HDD */
#if DEF_USB_PORT_FS_EN
    CHRV3vPacketSize = 64;  /* USBMaximum packet size of the mass-storage device: 64 at FS, 512 at HS/SS; initialized by the application and updated to 512 as soon as the drive enumerates as high-speed or super-speed */
    pTX_DMA_A_REG = (uint32_t *)&(USBFSH->HOST_TX_DMA);  /* Points to the transmit DMA address register; initialized by the application */
    pRX_DMA_A_REG = (uint32_t *)&(USBFSH->HOST_RX_DMA);  /* Points to the receive DMA address register; initialized by the application */
    pTX_LEN_REG = (uint16_t *)&(USBFSH->HOST_TX_LEN);    /* Points to the transmit length register; initialized by the application */
    pRX_LEN_REG = (uint16_t *)&(USBFSH->RX_LEN);         /* Points to the receive length register; initialized by the application */
#elif DEF_USB_PORT_HS_EN
    CHRV3vPacketSize = 512;  /* USBMaximum packet size of the mass-storage device: 64 at FS, 512 at HS/SS; initialized by the application and updated to 512 as soon as the drive enumerates as high-speed or super-speed */
    pTX_DMA_A_REG = (uint32_t *)&(USBHSH->HOST_TX_DMA);  /* Points to the transmit DMA address register; initialized by the application */
    pRX_DMA_A_REG = (uint32_t *)&(USBHSH->HOST_RX_DMA);  /* Points to the receive DMA address register; initialized by the application */
    pTX_LEN_REG = (uint16_t *)&(USBHSH->HOST_TX_LEN);    /* Points to the transmit length register; initialized by the application */
    pRX_LEN_REG = (uint16_t *)&(USBHSH->RX_LEN);         /* Points to the receive length register; initialized by the application */
#endif

    return( ERR_SUCCESS );
}

void mDelaymS( uint16_t n )
{
    Delay_Ms(n);
}

