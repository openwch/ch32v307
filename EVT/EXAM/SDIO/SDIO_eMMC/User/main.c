/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/11/02
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 * 8 lines SDIO routine to operate eMMC card:
 *   This example demonstrates reading and writing all sectors of the eMMC card--KLM8G1GETF-B041
 *   through the SDIO interface by CH32V307WCU6.
 * DVP--PIN:
 *   D0--PC8
 *   D1--PC9
 *   D2--PC10
 *   D3--PC11
 *   D4--PB8
 *   D5--PB9
 *   D6--PC6
 *   D7--PC7
 *   CMD--PD2
 *   SCK--PC12
 *   Note: Except for SCK, the rest need to pull up 47K resistors
 *
 */

#include "debug.h"
#include "sdio.h"
#include "string.h"

u8 buf[512];
u8 Readbuf[512];
/*********************************************************************
 * @fn      show_eMMCcard_info
 *
 * @brief   eMMC Card information.
 *
 * @return  none
 */
void show_eMMCcard_info(void)
{
    switch(eMMCCardInfo.CardType)
    {
        case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
        case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
        case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
        case SDIO_HIGH_CAPACITY_MMC_CARD:printf("Card Type:eMMC Card\r\n");break;
    }
    printf("Card ManufacturerID:0x%x\r\n",eMMCCardInfo.eMMC_cid.ManufacturerID);
    printf("Card SectorNums:0x%08x\n", eMMCCardInfo.SectorNums);
    printf("Card Capacity:%d MB\r\n",(u32)((eMMCCardInfo.SectorNums>>20)*512));
    printf("Card BlockSize:%dB\r\n",eMMCCardInfo.CardBlockSize);

}




/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    u32 i;
    u32 Sector_Nums;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    while(eMMC_Init())
    {
        printf("eMMC Card Error!\r\n");
        delay_ms(1000);
    }
    show_eMMCcard_info();
    printf("eMMC Card initial OK!\r\n");
    Sector_Nums = ((u32)(eMMCCardInfo.SectorNums));
    eMMC_Change_Tran_Mode();
    for(i=0; i<512; i++)
    {
        buf[i] = i;
    }
    for(i=0; i<Sector_Nums; i++)
    {
        if(SD_WriteDisk(buf,i,1))
        {
            printf("Wr %d sector fail\n", i);
        }
        else
        {
            printf("Wr %d sector success\n", i);
        }
        if(SD_ReadDisk(Readbuf,i,1))
        {
            printf("Rd %d sector fail\n", i);
        }
        else
        {
            printf("Rd %d sector success\n", i);
        }
        if(memcmp(buf, Readbuf, 512))
        {
            printf(" %d sector Verify fail\n", i);
            break;
        }
    }
    printf("end\n");
    while(1);
}







