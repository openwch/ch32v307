/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"           /* Obtains integer types */
#include "diskio.h"       /* Declarations of disk functions */
#include "sdio.h"
/* Definitions of physical drive number for each drive */
#define DEV_RAM     0   /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC     1   /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB     2   /* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat= STA_NOINIT;;
    u32 result;

    switch (pdrv) {
    case DEV_MMC :
        result = SD_GetState();
              if (result < SD_CARD_DISCONNECTED) {
                  stat &= ~STA_NOINIT;
              } else {
                  stat = STA_NOINIT;
              }
        return stat;
    }
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    switch (pdrv) {
    case DEV_MMC :
        stat = disk_status(DEV_MMC);
        return stat;
    }
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    DRESULT res= RES_PARERR;
    int result;

    switch (pdrv) {
    case DEV_MMC :
        result=SD_ReadDisk(buff,sector,count);
        if (result == SD_OK) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        return res;
    }

    return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    DRESULT res= RES_PARERR;
    int result;

    switch (pdrv) {
    case DEV_MMC :
        result=SD_WriteDisk((u8*)buff,sector,count);
        if (result == SD_OK) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        return res;
    }

    return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res= RES_PARERR;
    switch (pdrv) {
    case DEV_MMC :
        switch(cmd){
        case CTRL_SYNC:
                   break;
               case GET_SECTOR_COUNT:
                   *(DWORD*)buff = SDCardInfo.CardCapacity/512;
                   break;
               case GET_SECTOR_SIZE:
                   *(WORD*)buff = SDCardInfo.CardBlockSize;
                   break;
               case GET_BLOCK_SIZE:
                   *(WORD*)buff = 1;
                   break;
               case CTRL_TRIM:
                   break;
        }
        res = RES_OK;
        return res;
    }

    return RES_PARERR;
}

//User defined function to give a current time to fatfs module
DWORD get_fattime (void)
{
    return 0;
}



