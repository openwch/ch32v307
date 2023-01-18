/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   FSMC routine to operate NANDFLASH:
      This routine demonstrates the operation of FS33ND01GS108TF model NANFLASH erase-read-write-read through the FSMC interface, and in the process of reading and writing
  ECC check.
  PIN:
    FSMC_NOE--PD4  (RE)
    FSMC_NWE--PD5  (WE)
    FSMC_NE1_NCE2--PD7  (CE)
    FSMC_NWAIT--PD6  (R/B)
    FSMC_D0 --PD14
    FSMC_D1 -- PD15
    FSMC_D2 -- PD0
    FSMC_D3 -- PD1
    FSMC_D4 -- PE7
    FSMC_D5 -- PE8
    FSMC_D6 -- PE9
    FSMC_D7 -- PE10
    FSMC_A16 -- PD11 (CLE)
    FSMC_A17 -- PD12 (ALE)
*/

#include "debug.h"
#include "NAND_Flash.h"
#include "string.h"


u8 buf[NAND_ECC_SECTOR_SIZE];
u8 pbuf[NAND_ECC_SECTOR_SIZE];

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    u32 i=0;
    u8 t=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	memset(buf, 0, NAND_ECC_SECTOR_SIZE);
	printf("clear buf \n");
    for(i=0; i<NAND_ECC_SECTOR_SIZE; i++){
      printf("%02x ", buf[i]);

    }printf("\n");

    while(NAND_Init())
    {
        printf("NAND Error!\n");
        Delay_Ms(500);

        printf("Please Check\n");
        Delay_Ms(500);
    }
    printf("NAND Size:%dMB\n", (nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);

    /* Erase */
    t = NAND_EraseBlock(0);

    if(t) printf("Erase fail\n");
    else printf("Erase suc\n");

    /* Read */
    t = NAND_ReadPage( 0, 0,buf,NAND_ECC_SECTOR_SIZE);

    if(t){
        printf("read fail\n");
        printf("Err %02x\n", t);
    }
    else{
        printf("1***read data:\n");

        for(i=0; i<NAND_ECC_SECTOR_SIZE; i++){
          printf("%02x ", buf[i]);

        }printf("\n");
    }

    /* Write */
    printf("Write data:\n");
    pbuf[0] = 0X10;
    printf("%02x ", pbuf[0]);
    for(i=1;  i<NAND_ECC_SECTOR_SIZE; i++){
        pbuf[i] = 0X11;
        printf("%02x ", pbuf[i]);
    }

    printf("\r\n");

    t = NAND_WritePage(0,0,pbuf,NAND_ECC_SECTOR_SIZE);

    if(t){
        printf("write fail\n");
    }
    else{
        printf("write suc\n");
    }

    /* Read */
    t = NAND_ReadPage( 0, 0,buf,NAND_ECC_SECTOR_SIZE);

    if(t){
        printf("read fail\n");
        printf("Err %02x\n", t);
    }
    else{
        printf("2***read data:\n");
        for(i=0; i<NAND_ECC_SECTOR_SIZE; i++){
          printf("%02x ", buf[i]);
        }printf("\r\n");
    }


    while(1);

}
