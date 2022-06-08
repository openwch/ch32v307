/********************************** (C) COPYRIGHT *******************************
* File Name          : IAP_Task.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/25
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "WCHNET.h"
#include "IAP_Task.h"
#include "WCHNET.h"

__attribute__((__aligned__(4))) u8 dataDealBuf[RECE_BUF_LEN] = {0};

ethDataDeal dataDeal = {
        .head = 0,
        .tail = 0,
        .buffUsedLen = 0,
        .dataBuff = dataDealBuf,
};

iapFileHeader iapPara;
u32 fileDataLen = 0;
u32 fileCheckSum = 0;
u32 flashProgramLen = 0;
u16 flashProgramPage = 0;

extern u8 SocketId;

/*erase Data-Flash block, minimal block is 256B, return SUCCESS if success*/
void IAP_EEPROM_ERASE(uint32_t Page_Address, u32 Length)
{
    u32 NbrOfPage, EraseCounter;

    FLASH_Unlock_Fast();
    NbrOfPage = Length / FLASH_PAGE_SIZE;

    for (EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++) {
        FLASH_ErasePage_Fast( Page_Address + (FLASH_PAGE_SIZE * EraseCounter)); //Erase 256B
    }
    FLASH_Lock_Fast();
}

/*write Data-Flash data block, return FLASH_Status*/
void IAP_EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length )
{
    u32 address = StartAddr;
    u32 *p_buff = (u32 *)Buffer;
    u16 pageNum = Length / FLASH_PAGE_SIZE;
    u16 lastDataNum = Length % FLASH_PAGE_SIZE;
    u16 i;

    if(pageNum){
        FLASH_Unlock_Fast();

        for(i = 0; i < pageNum; i++)
            FLASH_ProgramPage_Fast((address + FLASH_PAGE_SIZE * i),(p_buff + (FLASH_PAGE_SIZE / 4) * i));

        FLASH_Lock_Fast();
    }

    if(lastDataNum){
        address = StartAddr + FLASH_PAGE_SIZE * pageNum;
        u8 *p_buff1 = Buffer + FLASH_PAGE_SIZE * pageNum;
        FLASH_Unlock();

        for(u8 i = 0; i < lastDataNum; i += 2)
            FLASH_ProgramHalfWord((address + i), *(u16 *)(p_buff1 + i));

        FLASH_Lock();
    }
}

/*read Data-Flash data block */
void IAP_EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length )
{
    u32 address = StartAddr;
    u32 *p_buff = (u32 *)Buffer;

    while(address < (StartAddr + Length))
    {
        *p_buff = (*(u32 *)address);
        address += 4;
        p_buff++;
    }
}

void IAP_EEPROM_ERASE_108k(u32 StartAddr)
{
    u8 i = 0;

    FLASH_Unlock();
    for(i = 0; i < USER_IMAGE_MAX_SIZE / FLASH_BLOCK_SIZE; i++)
        FLASH_ErasePage(StartAddr + i * FLASH_BLOCK_SIZE);
    FLASH_Lock();
}

void IAPParaInit(void)
{
    iapPara.iapFileCheckSum = 0;
    memset(iapPara.iapFileFlag, 0,sizeof(iapPara.iapFileFlag));
    iapPara.iapFileLen = 0;
    dataDeal.buffUsedLen = 0;
    dataDeal.dataBuff = dataDealBuf;
    dataDeal.head = 0;
    dataDeal.tail = 0;
    fileDataLen = 0;
    fileCheckSum = 0;
    flashProgramLen = 0;
    flashProgramPage = 0;
}

void iapFileParaCheck(u8 socketid)
{
    u32 len;
    u8 *pBuff = NULL;
    len = WCHNET_SocketRecvLen(socketid,NULL);
    if(BIN_INF_LEN <= len){
        u8 buffIndex;
        u32 recLen = BIN_INF_LEN;

        pBuff = malloc(recLen);

        WCHNET_SocketRecv(socketid,pBuff,&recLen);
        memcpy(iapPara.iapFileFlag, pBuff, 8);
        buffIndex = 8;
        iapPara.iapFileLen |= *(pBuff + buffIndex++);
        iapPara.iapFileLen |= *(pBuff + buffIndex++) << 8;
        iapPara.iapFileLen |= *(pBuff + buffIndex++) << 16;
        iapPara.iapFileLen |= *(pBuff + buffIndex++) << 24;

        iapPara.iapFileCheckSum |= *(pBuff + buffIndex++);
        iapPara.iapFileCheckSum |= *(pBuff + buffIndex++) << 8;
        iapPara.iapFileCheckSum |= *(pBuff + buffIndex++) << 16;
        iapPara.iapFileCheckSum |= *(pBuff + buffIndex++) << 24;
        fileDataLen += recLen;

        free(pBuff);

        if(memcmp(iapPara.iapFileFlag, FILE_FLAG, strlen(FILE_FLAG))){
            printf("The file's flag is wrong!\r\n");
            WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
        }
    }
}

/*********************************************************************
 * @fn      ETHRx
 *
 * @brief   ETH receive data and save it to buff
 *
 * @return  none
 */
void ETHRx(u8 socketid)
{
    u32 len, remainLen, recLen;
    int8_t receive_state = -1;

    len = WCHNET_SocketRecvLen(socketid,NULL);                               /* query length */
    if( dataDeal.buffUsedLen < RECE_BUF_LEN )
    {
        if(dataDeal.head <= dataDeal.tail){
            remainLen = RECE_BUF_LEN - dataDeal.tail;                       //到数组结尾剩余字节数
            if(len <= remainLen){                                           //收到的字节数小于remainLen
                recLen = len;
            }
            else{
                recLen = remainLen;
            }
            receive_state = WCHNET_SocketRecv(socketid,&dataDeal.dataBuff[dataDeal.tail],&recLen);
            if(receive_state == WCHNET_ERR_SUCCESS){
                dataDeal.tail = (dataDeal.tail + recLen)%RECE_BUF_LEN;  //防止越界
                dataDeal.buffUsedLen += recLen;
                fileDataLen += recLen;

                if(len > remainLen){
                    remainLen = RECE_BUF_LEN - dataDeal.buffUsedLen;        //缓冲区剩余长度
                    len -= recLen;                                          //待接收数据长度
                    if(len <= remainLen){
                        remainLen = len;
                    }
                    receive_state = WCHNET_SocketRecv(socketid,&dataDeal.dataBuff[dataDeal.tail],&remainLen);
                    if(receive_state == WCHNET_ERR_SUCCESS){
                        dataDeal.tail = (dataDeal.tail + remainLen)%RECE_BUF_LEN;  //防止越界
                        dataDeal.buffUsedLen += remainLen;
                        fileDataLen += remainLen;
                    }
                }
            }
        }
        else{
            remainLen = RECE_BUF_LEN - dataDeal.buffUsedLen;                              //buff剩余空间
            if(len <= remainLen){
                remainLen = len;
            }
            receive_state = WCHNET_SocketRecv(socketid,&dataDeal.dataBuff[dataDeal.tail],&remainLen);
            if(receive_state == WCHNET_ERR_SUCCESS){
                dataDeal.tail = (dataDeal.tail + remainLen)%RECE_BUF_LEN;  //防止越界
                dataDeal.buffUsedLen += remainLen;
                fileDataLen += remainLen;
            }
        }
    }
    else
    {
//        printf("eth receive buff busy\n");
    }
//    printf("***rec\r\n");
//    printf("head: %d\r\n",dataDeal.head);
//    printf("tail: %d\r\n",dataDeal.tail);
//    printf("buffUsedLen: %d\r\n",dataDeal.buffUsedLen);
}

void ETHTx(u8 socketid)
{
    u8 *pBuff = NULL, pageCnt = 0;
    u16 writableDataLen = 0, tempLen = 0;
    u32 remainLen = 0;

    if(dataDeal.buffUsedLen){
        pageCnt = dataDeal.buffUsedLen / FLASH_PAGE_SIZE;    // 计算数据页数
        if(pageCnt){                                   // 至少有256B的数据
            writableDataLen = pageCnt * FLASH_PAGE_SIZE;
        }
        else if(iapPara.iapFileLen == fileDataLen){
            writableDataLen = dataDeal.buffUsedLen;
        }
        else return;

        pBuff = malloc(writableDataLen);       // 申请转存数据的空间
        if(dataDeal.tail <= dataDeal.head){
            remainLen = RECE_BUF_LEN - dataDeal.head;
            if(writableDataLen <= remainLen){
                tempLen = writableDataLen;
            }
            else{
                tempLen = remainLen;
            }

            memcpy(pBuff, &dataDeal.dataBuff[dataDeal.head], tempLen);
            dataDeal.head = (dataDeal.head + tempLen)%RECE_BUF_LEN;  //防止越界
            dataDeal.buffUsedLen -= tempLen;

            if(remainLen < writableDataLen){
                remainLen = writableDataLen - tempLen;              // 转存剩余空间长度
                memcpy((pBuff + tempLen), &dataDeal.dataBuff[dataDeal.head], remainLen);
                dataDeal.head = (dataDeal.head + remainLen)%RECE_BUF_LEN;  //防止越界
                dataDeal.buffUsedLen -= remainLen;
            }
            IAP_EEPROM_WRITE((BACKUP_IMAGE_START_ADD + flashProgramLen), pBuff, writableDataLen );
        }
        else{
            memcpy(pBuff, &dataDeal.dataBuff[dataDeal.head], writableDataLen);
            dataDeal.head = (dataDeal.head + writableDataLen)%RECE_BUF_LEN;  //防止越界
            dataDeal.buffUsedLen -= writableDataLen;

            IAP_EEPROM_WRITE((BACKUP_IMAGE_START_ADD + flashProgramLen), pBuff, writableDataLen );
        }
        flashProgramLen += writableDataLen;

        for(u16 i = 0; i < (writableDataLen / 4); i++)
            fileCheckSum += *((u32 *)pBuff + i);

        if((writableDataLen % 4) != 0){
            u8 lastData = writableDataLen % 4;
            u8 temBuff[4] = {0};
            memcpy(temBuff, (pBuff + writableDataLen - lastData), lastData);
            fileCheckSum += *(u32 *)temBuff;
        }

        free(pBuff);

        if((iapPara.iapFileLen == (flashProgramLen + BIN_INF_LEN))){
            if(fileCheckSum == iapPara.iapFileCheckSum){
                printf("FileCheckSum is right!\r\n");
                u32 updateFlag = IMAGE_FLAG_UPDATA;
                IAP_EEPROM_ERASE(UPDATA_FLAG_STORAGE_ADD, FLASH_PAGE_SIZE);
                IAP_EEPROM_WRITE(UPDATA_FLAG_STORAGE_ADD, (u8 *)&updateFlag, 4);
                WCHNET_SocketClose(SocketId, TCP_CLOSE_RST);
                NVIC_SystemReset();
            }
            else{
                printf("FileCheckSum is wrong!\r\n");
                WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
            }
        }
//        printf("@@@send\r\n");
//        printf("head: %d\r\n",dataDeal.head);
//        printf("tail: %d\r\n",dataDeal.tail);
//        printf("buffUsedLen: %d\r\n",dataDeal.buffUsedLen);
    }
}

/* Flash升级复制 */
u8 IAPCopyFlashDeal(void)
{
    u32 *pBuf;
    u32 i,j;
    u32 flashAddr;
    u32 updataBuff[READ_DATA_LEN / 4];

    pBuf = updataBuff;

    //擦
    IAP_EEPROM_ERASE_108k(USER_IMAGE_START_ADD);

    for(i = 0; i < (BACKUP_IMAGE_MAX_SIZE / READ_DATA_LEN); i++)                           /*code flash  以2K为单位进行擦写*/
    {
        flashAddr = USER_IMAGE_START_ADD + i * READ_DATA_LEN;
        //读
        IAP_EEPROM_READ((flashAddr + USER_IMAGE_MAX_SIZE), (u8 *)pBuf, READ_DATA_LEN);
        //写
        IAP_EEPROM_WRITE(flashAddr, (u8 *)pBuf, READ_DATA_LEN);

        //校验
        for(j = 0; j < (READ_DATA_LEN / 4); j++){
            if(pBuf[j] != *(u32 *)(flashAddr + 4 * j))
            return ERROR;
        }
    }
    IAP_EEPROM_ERASE_108k(BACKUP_IMAGE_START_ADD);
    return SUCCESS;
}
