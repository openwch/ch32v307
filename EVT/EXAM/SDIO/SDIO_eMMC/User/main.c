/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2026/10/14
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
 *   through the SDIO interface by CH32V307WCU.
 *   PIN:
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
        default:break;
    }
    printf("Card ManufacturerID:0x%x\r\n",eMMCCardInfo.eMMC_cid.ManufacturerID);
    printf("Card SectorNums:0x%08x\n", eMMCCardInfo.SectorNums);
    printf("Card Capacity:%d MB\r\n",(u32)((eMMCCardInfo.SectorNums>>20)*512));
    printf("Card BlockSize:%dB\r\n",eMMCCardInfo.CardBlockSize);

}

/*********************************************************************
 * @fn      eMMCinitClock
 *
 * @brief   eMMC clk for init.
 *
 * @return  none
 */
void eMMCinitClock(void)
{
    RCC->CTLR |= (uint32_t)0x00000001;

  #ifdef CH32V30x_D8C
    RCC->CFGR0 &= (uint32_t)0xF8FF0000;
  #else
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
  #endif

    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFF80FFFF;

  #ifdef CH32V30x_D8C
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;
  #else
    RCC->INTR = 0x009F0000;
  #endif

  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /*  PLL configuration: PLLCLK = HSE * 12 = 96 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE |
                                        RCC_PLLMULL));

#ifdef CH32V30x_D8
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL12);
#else
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL12_EXTEN);
#endif

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  {
        /*
         * If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error
         */
  }
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
    eMMCinitClock();
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
	    Readbuf[i]=0;
    }
    for(i=1; i<Sector_Nums; i++)
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







