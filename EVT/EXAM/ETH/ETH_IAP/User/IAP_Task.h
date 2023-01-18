/********************************** (C) COPYRIGHT *******************************
* File Name          : IAP_Task.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/25
* Description        : This file contains the headers of the IAP task.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __IAP_TASK_H__
#define __IAP_TASK_H__

#include "debug.h"
#include "ch32v30x_it.h"

/*
flash:256K
BIM area（Image Boot Manager）：40K
USER area（用户代码区）：108K
BACKUP area（代码备份区）：108K
address assignment：
0x0         size:0xA000
0xA000      size:0x1B000
0x25000     size:0x1B000
*/

/*address assignment */
#define BIM_IMAGE_START_ADD             ((uint32_t) 0x08000000)
#define BIM_IMAGE_MAX_SIZE              0xA000

#define USER_IMAGE_START_ADD            ((uint32_t) 0x0800A000)
#define USER_IMAGE_MAX_SIZE             0x1B000

#define BACKUP_IMAGE_START_ADD          ((uint32_t) 0x08025000)
#define BACKUP_IMAGE_MAX_SIZE           0x1B000

#define UPDATA_FLAG_STORAGE_ADD         ((uint32_t) 0x0803FF00)

#define FLASH_BLOCK_SIZE                4096

#define READ_DATA_LEN                   2048

#define FLASH_PAGE_SIZE                 256

/*Flash operation information*/
#define BIN_INF_LEN        512

#define FILE_FLAG          "WCHNET"

#define IMAGE_FLAG_UPDATE   0x57434820   //'W','C','H',' '

typedef struct
{
    u8  iapFileFlag[8];                  //8-byte IAP flag, fixed as "WCHNET", the extra space is 0
    u32 iapFileLen;                      //4 bytes IAP file length, which is bin file length + 512 bytes
    u32 iapFileCheckSum;                 //4-byte checksum
}iapFileHeader;

typedef struct{
    u16 readIndex;
    u16 writeIndex;
    u16 buffUsedLen;
    u8  *dataBuff;
}ethDataDeal;

extern ethDataDeal dataDeal;
extern iapFileHeader iapPara;
extern u32 fileDataLen;
extern u32 fileCheckSum;

extern u8 IAPCopyFlashDeal(void);
extern void IAPParaInit(void);
extern void saveUpdatedFile(void);
extern void receUpdatedFile(u8 id);
extern void IAPFileParaCheck(u8 id);
extern void IAP_EEPROM_ERASE_108k(u32 StartAddr);
extern void IAP_EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length );
extern void IAP_EEPROM_ERASE(uint32_t Page_Address, u32 Length);
extern void IAP_EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

#endif /* __IAP_TASK_H__ */
