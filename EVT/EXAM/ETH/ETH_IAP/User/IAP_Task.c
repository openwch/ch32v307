/********************************** (C) COPYRIGHT *******************************
* File Name          : IAP_Task.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/25
* Description        : IAP related functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "wchnet.h"
#include "IAP_Task.h"

__attribute__((__aligned__(4))) u8 dataDealBuf[RECE_BUF_LEN] = {0};

ethDataDeal dataDeal = {                              //Ring buffer parameter initialization
        .readIndex = 0,
        .writeIndex = 0,
        .buffUsedLen = 0,
        .dataBuff = dataDealBuf,
};

u32 fileDataLen = 0;                                  //IAP file data length
u32 fileCheckSum = 0;                                 //IAP file data checksum
u32 flashProgramLen = 0;                              //length of data written to backup area
iapFileHeader iapPara;                                //IAP file parameters

extern u8 SocketId;

/*********************************************************************
 * @fn      IAP_EEPROM_ERASE
 *
 * @brief   erase Data-Flash block, minimal block is 256B
 *
 * @param   Page_Address - the address of the page being erased.
 *          Length - Erased data length
 *
 * @return  none
 */
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

/*********************************************************************
 * @fn      IAP_EEPROM_WRITE
 *
 * @brief   write Data-Flash data block
 *
 * @param   StartAddr - the address of the page being written.
 *          Buffer - data buff
 *          Length - written data length
 *
 * @return  FLASH_Status
 */
void IAP_EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length )
{
    u32 address = StartAddr;
    u32 *p_buff = (u32 *)Buffer;
    u16 pageNum = Length / FLASH_PAGE_SIZE;
    u16 lastDataNum = Length % FLASH_PAGE_SIZE;
    u16 i;

    if(pageNum){                                                    //write by page(256B)
        FLASH_Unlock_Fast();

        for(i = 0; i < pageNum; i++)
            FLASH_ProgramPage_Fast((address + FLASH_PAGE_SIZE * i),(p_buff + (FLASH_PAGE_SIZE / 4) * i));

        FLASH_Lock_Fast();
    }

    if(lastDataNum){                                                //write by a half word(2B)
        u8 *p_buff1 = Buffer + FLASH_PAGE_SIZE * pageNum;
        address = StartAddr + FLASH_PAGE_SIZE * pageNum;
        FLASH_Unlock();

        for(i = 0; i < lastDataNum; i += 2)
            FLASH_ProgramHalfWord((address + i), *(u16 *)(p_buff1 + i));

        FLASH_Lock();
    }
}

/*********************************************************************
 * @fn      IAP_EEPROM_READ
 *
 * @brief   read Data-Flash data block
 *
 * @param   StartAddr - the address of the page being read.
 *          Buffer - data buff
 *          Length - read data length
 *
 * @return  none
 */
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

/*********************************************************************
 * @fn      IAP_EEPROM_ERASE_108k
 *
 * @brief   erase Data-Flash block, minimal block is 4KB.
 *          This function is used to erase USER and BACKUP area.
 *
 * @param   Page_Address - the address of the page being erased.
 *
 * @return  none
 */
void IAP_EEPROM_ERASE_108k(u32 StartAddr)
{
    u8 i = 0;

    FLASH_Unlock();
    for(i = 0; i < USER_IMAGE_MAX_SIZE / FLASH_BLOCK_SIZE; i++)
        FLASH_ErasePage(StartAddr + i * FLASH_BLOCK_SIZE);
    FLASH_Lock();
}

/*********************************************************************
 * @fn      IAPParaInit
 *
 * @brief   IAP parameters initialize
 *
 * @return  none
 */
void IAPParaInit(void)
{
    iapPara.iapFileCheckSum = 0;
    memset(iapPara.iapFileFlag, 0,sizeof(iapPara.iapFileFlag));
    iapPara.iapFileLen = 0;
    dataDeal.buffUsedLen = 0;
    dataDeal.dataBuff = dataDealBuf;
    dataDeal.readIndex = 0;
    dataDeal.writeIndex = 0;
    fileDataLen = 0;
    fileCheckSum = 0;
    flashProgramLen = 0;
}

/*********************************************************************
 * @fn      IAPFileParaCheck
 *
 * @brief   Verify upgrade file parameters.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void IAPFileParaCheck(u8 id)
{
    u32 len;
    u8 *pBuff = NULL;
    len = WCHNET_SocketRecvLen(id,NULL);
    if(BIN_INF_LEN <= len){                                  //The first 512 bytes are the bin file parameters
        u8 buffIndex;
        u32 recLen = BIN_INF_LEN;

        pBuff = malloc(recLen);

        WCHNET_SocketRecv(id,pBuff,&recLen);
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

/*Ring buffer data distribution*/
//first case
/**start******************readIndex********************writeIndex***************end**
 ******|   writable area  |*******|    readable area   |********| writable area |****/

//second case
/**start******************writeIndex********************readIndex***************end**
 ******|   readable area  |********|   writable area    |*******| readable area |****/

/*********************************************************************
 * @fn      receUpdatedFile
 *
 * @brief   ETH receive data and save it to ring buff.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void receUpdatedFile(u8 id)
{
    u32 len, remainLen, recLen;
    int8_t receive_state = -1;

    len = WCHNET_SocketRecvLen(id,NULL);                                    //query length
    if( dataDeal.buffUsedLen < RECE_BUF_LEN )                               //Ring buffer has free space
    {
        if(dataDeal.readIndex <= dataDeal.writeIndex){
            remainLen = RECE_BUF_LEN - dataDeal.writeIndex;
            if(len <= remainLen){
                recLen = len;
            }
            else{
                recLen = remainLen;
            }
            receive_state = WCHNET_SocketRecv(id,&dataDeal.dataBuff[dataDeal.writeIndex],&recLen);
            if(receive_state == WCHNET_ERR_SUCCESS){
                dataDeal.writeIndex = (dataDeal.writeIndex + recLen)%RECE_BUF_LEN;
                dataDeal.buffUsedLen += recLen;
                fileDataLen += recLen;

                if(len > remainLen){
                    remainLen = RECE_BUF_LEN - dataDeal.buffUsedLen;
                    len -= recLen;
                    if(len <= remainLen){
                        remainLen = len;
                    }
                    receive_state = WCHNET_SocketRecv(id,&dataDeal.dataBuff[dataDeal.writeIndex],&remainLen);
                    if(receive_state == WCHNET_ERR_SUCCESS){
                        dataDeal.writeIndex = (dataDeal.writeIndex + remainLen)%RECE_BUF_LEN;
                        dataDeal.buffUsedLen += remainLen;
                        fileDataLen += remainLen;
                    }
                }
            }
        }
        else{
            remainLen = dataDeal.readIndex - dataDeal.writeIndex;
            if(len <= remainLen){
                remainLen = len;
            }
            receive_state = WCHNET_SocketRecv(id,&dataDeal.dataBuff[dataDeal.writeIndex],&remainLen);
            if(receive_state == WCHNET_ERR_SUCCESS){
                dataDeal.writeIndex = (dataDeal.writeIndex + remainLen)%RECE_BUF_LEN;
                dataDeal.buffUsedLen += remainLen;
                fileDataLen += remainLen;
            }
        }
    }
    else
    {
        printf("ETH receive buff busy\n");
    }
}

/*********************************************************************
 * @fn      saveUpdatedFile
 *
 * @brief   Save the upgrade file to the backup area.
 *
 * @return  none
 */
void saveUpdatedFile(void)
{
    u8 *pBuff = NULL, pageCnt = 0;
    u16 writableDataLen = 0, tempLen = 0;
    u32 remainLen = 0;

    if(dataDeal.buffUsedLen){
        pageCnt = dataDeal.buffUsedLen / FLASH_PAGE_SIZE;       // Calculate the number of data pages
        if(pageCnt){
            writableDataLen = pageCnt * FLASH_PAGE_SIZE;        //at least one page
        }
        else if(iapPara.iapFileLen == fileDataLen){             //Extract data that is less than one page at the end of the file
            writableDataLen = dataDeal.buffUsedLen;
        }
        else return;

        pBuff = malloc(writableDataLen);
        if(dataDeal.writeIndex <= dataDeal.readIndex){
            remainLen = RECE_BUF_LEN - dataDeal.readIndex;
            if(writableDataLen <= remainLen){
                tempLen = writableDataLen;
            }
            else{
                tempLen = remainLen;
            }

            memcpy(pBuff, &dataDeal.dataBuff[dataDeal.readIndex], tempLen);
            dataDeal.readIndex = (dataDeal.readIndex + tempLen)%RECE_BUF_LEN;
            dataDeal.buffUsedLen -= tempLen;

            if(remainLen < writableDataLen){
                remainLen = writableDataLen - tempLen;
                memcpy((pBuff + tempLen), &dataDeal.dataBuff[dataDeal.readIndex], remainLen);
                dataDeal.readIndex = (dataDeal.readIndex + remainLen)%RECE_BUF_LEN;
                dataDeal.buffUsedLen -= remainLen;
            }
            IAP_EEPROM_WRITE((BACKUP_IMAGE_START_ADD + flashProgramLen), pBuff, writableDataLen );
        }
        else{
            memcpy(pBuff, &dataDeal.dataBuff[dataDeal.readIndex], writableDataLen);
            dataDeal.readIndex = (dataDeal.readIndex + writableDataLen)%RECE_BUF_LEN;
            dataDeal.buffUsedLen -= writableDataLen;

            IAP_EEPROM_WRITE((BACKUP_IMAGE_START_ADD + flashProgramLen), pBuff, writableDataLen );
        }
        flashProgramLen += writableDataLen;

        for(u16 i = 0; i < (writableDataLen / 4); i++)          //Calculate checksum
            fileCheckSum += *((u32 *)pBuff + i);

        /*Calculate the checksum of data less than 4 bytes at the end of the file*/
        if((writableDataLen % 4) != 0){
            u8 lastDataLen = writableDataLen % 4;
            u8 temBuff[4] = {0};
            memcpy(temBuff, (pBuff + writableDataLen - lastDataLen), lastDataLen);
            fileCheckSum += *(u32 *)temBuff;
        }

        free(pBuff);

        /*After the data is saved, compare the checksum. If the checksum is correct,
         * update the upgrade flag. Disconnect if checksum error.*/
        if(iapPara.iapFileLen == (flashProgramLen + BIN_INF_LEN)){
            if(fileCheckSum == iapPara.iapFileCheckSum){
                printf("FileCheckSum is right!\r\n");
                u32 updateFlag = IMAGE_FLAG_UPDATE;
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
    }
}

/*********************************************************************
 * @fn      IAPCopyFlashDeal
 *
 * @brief   Move the BACKUP area code to the USER area.
 *          Write in 2KB units.
 *
 * @return  upgrade status
 */
u8 IAPCopyFlashDeal(void)
{
    u16 i,j;
    u32 *pBuf;
    u32 flashAddr;
    u32 updataBuff[READ_DATA_LEN / 4];

    pBuf = updataBuff;

    IAP_EEPROM_ERASE_108k(USER_IMAGE_START_ADD);

    for(i = 0; i < (BACKUP_IMAGE_MAX_SIZE / READ_DATA_LEN); i++)
    {
        flashAddr = USER_IMAGE_START_ADD + i * READ_DATA_LEN;

        IAP_EEPROM_READ((flashAddr + USER_IMAGE_MAX_SIZE), (u8 *)pBuf, READ_DATA_LEN);

        IAP_EEPROM_WRITE(flashAddr, (u8 *)pBuf, READ_DATA_LEN);

        //Check data
        for(j = 0; j < (READ_DATA_LEN / 4); j++){
            if(pBuf[j] != *(u32 *)(flashAddr + 4 * j))
            return NoREADY;
        }
    }
    IAP_EEPROM_ERASE_108k(BACKUP_IMAGE_START_ADD);
    return READY;
}
