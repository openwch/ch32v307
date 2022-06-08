/********************************** (C) COPYRIGHT *******************************
* File Name          : IAP_Task.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/25
* Description        : This file contains the headers of the IAP task.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __IAP_TASK_H__
#define __IAP_TASK_H__

#include "debug.h"
#include "ch32v30x_it.h"

/*
flash:256K
BIM区（Image Boot Manager）：40K
USER区（用户代码区）：108K
BACKUP区（代码备份区）：108K
地址分配：
0x0         size:0xA000
0xA000      size:0x1B000
0x25000     size:0x1B000
*/

/* flash 定义 */
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

#define IMAGE_FLAG_UPDATA   0x57434820   //'W','C','H',' '

typedef struct
{
    u8  iapFileFlag[8];                  /* 8字节iap标志，固定为 "WCHNET",多余补0 */
    u32 iapFileLen;                      /* 4字节iap文件长度，为bin文件长度+512字节 */
    u32 iapFileCheckSum;                 /* 4字节检验和 */
}iapFileHeader;

typedef struct{
    u16 head;
    u16 tail;
    u16 buffUsedLen;
    u8  *dataBuff;
}ethDataDeal;

extern ethDataDeal dataDeal;
extern iapFileHeader iapPara;
extern u32 fileDataLen;
extern u32 fileCheckSum;

extern void IAPParaInit(void);
extern u8 IAPCopyFlashDeal(void);
extern void ETHTx(u8 socketid);
extern void ETHRx(u8 socketid);
extern void iapFileParaCheck(u8 socketid);
extern void IAP_EEPROM_ERASE_108k(u32 StartAddr);
extern void IAP_EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length );
extern void IAP_EEPROM_ERASE(uint32_t Page_Address, u32 Length);
extern void IAP_EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

#endif /* __IAP_TASK_H__ */
