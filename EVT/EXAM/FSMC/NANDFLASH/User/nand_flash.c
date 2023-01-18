/********************************** (C) COPYRIGHT *******************************
* File Name          : nand_flash.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the NANDFLASH.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include"NAND_Flash.h"
#include "string.h"

nand_attriute nand_dev;


/*********************************************************************
 * @fn      NAND_Init
 *
 * @brief   Init NANDFLASH
 *
 * @return  none
 */
u8 NAND_Init(void)
{
    FSMC_NANDInitTypeDef  FSMC_NANDInitStructure;
    FSMC_NAND_PCCARDTimingInitTypeDef  ComSpaceTiming;
    FSMC_NAND_PCCARDTimingInitTypeDef  AttSpaceTiming;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);

    /*FSMC_NWAIT PD6 R/B */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FSMC_NADV */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    ComSpaceTiming.FSMC_SetupTime=2;
    ComSpaceTiming.FSMC_WaitSetupTime=3;
    ComSpaceTiming.FSMC_HoldSetupTime=2;
    ComSpaceTiming.FSMC_HiZSetupTime=1;

    AttSpaceTiming.FSMC_SetupTime=2;
    AttSpaceTiming.FSMC_WaitSetupTime=3;
    AttSpaceTiming.FSMC_HoldSetupTime=2;
    AttSpaceTiming.FSMC_HiZSetupTime=1;

    FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
    FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
    FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
    FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_512Bytes;
    FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0;
    FSMC_NANDInitStructure.FSMC_TARSetupTime = 1;
    FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &ComSpaceTiming;
    FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &AttSpaceTiming;
    FSMC_NANDInit(&FSMC_NANDInitStructure);

    FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);

    NAND_Reset();
    Delay_Ms(100);
    nand_dev.id=NAND_ReadID();

    printf("id:%08x\n", nand_dev.id);

    if(nand_dev.id==FS33ND01GS108TF){
        nand_dev.page_totalsize=2112;
        nand_dev.page_mainsize=2048;
        nand_dev.page_sparesize=64;
        nand_dev.block_pagenum=64;
        nand_dev.plane_blocknum=1024;
        nand_dev.block_totalnum=1024;

    }
    else return 1;

    return 0;
}

/*********************************************************************
 * @fn      NAND_ReadID
 *
 * @brief   Read NANDFLASH ID
 *
 * @return  id - NANDFLASH ID
 */
u32 NAND_ReadID(void)
{
    u8 deviceid[5];
    u32 id;
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_READID;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=0X00;

    /* First 5 bytes: 0xEC ,0xF1, 0x00 ,0x95, 0x42 */
    deviceid[0]=*(vu8*)NAND_ADDRESS;
    deviceid[1]=*(vu8*)NAND_ADDRESS;
    deviceid[2]=*(vu8*)NAND_ADDRESS;
    deviceid[3]=*(vu8*)NAND_ADDRESS;
    deviceid[4]=*(vu8*)NAND_ADDRESS;

    id=((u32)deviceid[1])<<24|((u32)deviceid[2])<<16|((u32)deviceid[3])<<8|deviceid[4];
    return id;
}

/*********************************************************************
 * @fn      NAND_ReadStatus
 *
 * @brief   Read NANDFLASH status
 *
 * @return  data -
 *            bit - 0-Pass 1-Fail
 *            bit6 - 0-Busy 1-Ready
 *            bit7 - 0-Protected 1-Not Protected
 */
u8 NAND_ReadStatus(void)
{
    vu8 data=0;
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_READSTA;
    data++;data++;data++;data++;data++;
    data=*(vu8*)NAND_ADDRESS;
    return data;
}

/*********************************************************************
 * @fn      NAND_WaitForReady
 *
 * @brief   Wait NANDFLASH ready
 *
 * @return  NSTA_READY - OK
 *          NSTA_TIMEOUT - timeout
 */
u8 NAND_WaitForReady(void)
{
    u8 status=0;
    vu32 time=0;
    while(1)
    {
        status=NAND_ReadStatus();
        if(status&NSTA_READY)break;
        time++;
        if(time>=0X1FFFF)return NSTA_TIMEOUT;
    }
    return NSTA_READY;
}

/*********************************************************************
 * @fn      NAND_Reset
 *
 * @brief   Reset NANDFLASH
 *
 * @return  0 - success
 *          1 - ERR
 */
u8 NAND_Reset(void)
{
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_RESET;
    if(NAND_WaitForReady()==NSTA_READY)return 0;
    else return 1;
}

/*********************************************************************
 * @fn      NAND_WaitRB
 *
 * @brief   Wait RB signal valid
 *
 * @param   rb -
 *            0- Low level active
 *            1- High level active
 *
 * @return  0 - success
 *          1 - ERR
 */
u8 NAND_WaitRB(vu8 rb)
{
    vu16 time=0;
    while(time<10000)
    {
        time++;
        GPIO_ReadInputDataBit(GPIOD,  GPIO_Pin_6);
    }
    return 1;
}

/*********************************************************************
 * @fn      NAND_Delay
 *
 * @brief   Delay function
 *
 * @param   i - delay time
 *
 * @return  none
 */
void NAND_Delay(vu32 i)
{
    while(i>0)i--;
}

/*********************************************************************
 * @fn      NAND_ReadPage
 *
 * @brief   Read the data of the specified page and column of NAND flash
 *
 * @param   PageNum - page number
 *          ColNum - column number
 *          pBuffer - data
 *          NumByteToRead - data number
 *
 * @return  0 - success
 *          1 - ERR
 */
u8 NAND_ReadPage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead)
{
    vu16 i=0;
    u8 errsta=0;

    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_AREA_A;
    //addr
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)ColNum;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(ColNum>>8);
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)PageNum;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(PageNum>>8);

    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_AREA_TRUE1;

    Delay_Ms(10);
    FSMC_NANDECCCmd(FSMC_Bank2_NAND, DISABLE);
    FSMC_NANDECCCmd(FSMC_Bank2_NAND, ENABLE);

    for(i=0; i<NumByteToRead; i++){
        *(vu8*)pBuffer++ = *(vu8*)NAND_ADDRESS;
    }

    while(FSMC_GetFlagStatus(FSMC_Bank2_NAND, FSMC_FLAG_FEMPT)==RESET);
    printf("FMC_Bank2_3->ECCR3=%08x\r\n",FSMC_GetECC(FSMC_Bank2_NAND));
    FSMC_NANDECCCmd(FSMC_Bank2_NAND, DISABLE);
    if(NAND_WaitForReady()!=NSTA_READY)errsta=NSTA_ERROR;

    return errsta;
}

/*********************************************************************
 * @fn      NAND_WritePage
 *
 * @brief   Write the data of the specified page and column of NAND flash
 *
 * @param   PageNum - page number
 *          ColNum - column number
 *          pBuffer - data
 *          NumByteToRead - data number
 *
 * @return  0 - success
 *          1 - ERR
 */
u8 NAND_WritePage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToWrite)
{
    vu16 i=0;

    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE0;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)ColNum;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(ColNum>>8);
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)PageNum;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(PageNum>>8);

    NAND_Delay(30);
    FSMC_NANDECCCmd(FSMC_Bank2_NAND, ENABLE);

    for(i=0;i<NumByteToWrite;i++)
    {
        *(vu8*)NAND_ADDRESS=*(vu8*)pBuffer++;
    }

    while(FSMC_GetFlagStatus(FSMC_Bank2_NAND, FSMC_FLAG_FEMPT)==RESET);
    printf("FMC_Bank2_3->ECCR3=%08x\r\n",FSMC_GetECC(FSMC_Bank2_NAND));
    FSMC_NANDECCCmd(FSMC_Bank2_NAND, DISABLE);

    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE_TURE1;
    if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;

    return 0;
}

/*********************************************************************
 * @fn      NAND_EraseBlock
 *
 * @brief   Erase NANDFLASH block
 *
 * @param   0 - success
 *          1 - ERR
 *
 * @return  none
 */
u8  NAND_EraseBlock(u32 BlockNum)
{
    if(nand_dev.id==FS33ND01GS108TF)BlockNum<<=6;

    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE0;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)BlockNum;
    *(vu8*)(NAND_ADDRESS|NAND_ADDR)=(u8)(BlockNum>>8);
    *(vu8*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE1;
    if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;

    return 0;
}




