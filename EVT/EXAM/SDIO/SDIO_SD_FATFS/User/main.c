/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/11/20
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 * This example is used to show how to migrate an SD card to a FATFS file read/write system.
 *   DVP--PIN:
 *   D0--PC8
 *   D1--PC9
 *   D2--PC10
 *   D3--PC11
 *   SCK--PC12
 *   CMD--PD2
 *   Note: Except for SCK, the rest need to pull up 47K resistors
 *
 */
#include "ff.h"
#include "sdio.h"
#include "debug.h"
#include "string.h"
FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res_sd;
BYTE work[FF_MAX_SS];
u8 WriteBuf[]={"Hello WCH!"};
u8 ReadSDBuf[11]={0};
MKFS_PARM opt = {0};


/*********************************************************************
 * @fn      show_sdcard_info
 *
 * @brief   SD Card information.
 *
 * @return  none
 */
void show_sdcard_info(void)
{
    switch(SDCardInfo.CardType)
    {
    case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
    case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
    case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
    case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
    }
    printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);
    printf("Card RCA:%d\r\n",SDCardInfo.RCA);
    printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));
    printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);
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
    opt.fmt = FM_FAT32;
    opt.n_fat = 1;
    opt.align = 0;
    opt.au_size = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	while(SD_Init())
	{
	    printf("SD Card Error!\r\n");
	    delay_ms(100);
	}
	show_sdcard_info();
	printf("SD Card OK\r\n");
	res_sd=f_mount(&fs, "1:", 1);//SD mount
	if(res_sd==FR_OK)
	{
	    printf("Disk mounted successfully\r\n");
	}
	if(res_sd == FR_NO_FILESYSTEM)
	{
	    printf("Disk formatting\r\n");
	    res_sd=f_mkfs("1:", &opt, work, sizeof(work));
	    if (res_sd == FR_OK)
	    {
	        printf("Disk formatted successfully\r\n");
	        res_sd = f_mount(&fs, "1:", 1);
	        if (res_sd == FR_OK)
	        {
	            printf("Disk mounted successfully\r\n");
	        }
	        else
	        {
	            printf("Disk mounting failed\r\n");
	            printf("error code%x\r\n",res_sd);
	        }
	    }
	    else {
	        printf("Disk formatting failed£¡£¨%d£©\r\n", res_sd);
	    }
	}
    res_sd= f_open(&fnew,(const char*)"1:/testWCH.txt",FA_CREATE_ALWAYS|FA_WRITE);
      if(res_sd!=FR_OK)
      {
          printf("Create file error\r\n");
      }
      else
      {
          printf("Create file successfully\r\n");
      }
      printf("Writing......................\r\n");
      res_sd= f_write(&fnew,WriteBuf,11,&fnum);
      for(int j=0;j<sizeof(WriteBuf);j++)
      {
          printf("WriteBuf[%d]=%d\r\n",j,WriteBuf[j]);
      }
      if(res_sd!=FR_OK)
      {
          printf("Write error\r\n");
      }
      f_close(&fnew);
      res_sd= f_open(&fnew,(const char*)"1:/testWCH.txt",FA_OPEN_EXISTING|FA_READ);
      if(res_sd!=FR_OK)
      {
          printf("Open file error\r\n");
      }
      printf("Reading......................\r\n");
      res_sd= f_read(&fnew,ReadSDBuf,11,(UINT*)&fnum);
      if(res_sd!=FR_OK)
      {
          printf("Read error\r\n");
      }
      for(int i=0;i<sizeof(ReadSDBuf);i++)
      {
          printf("ReadBuf[%d]=%d\r\n",i,ReadSDBuf[i]) ;
      }
    while(1);
}







