/********************************** (C) COPYRIGHT *******************************
* File Name          : system_ch32v30x.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : CH32V30x Device Peripheral Access Layer System Source File.
*                      For HSE = 8Mhz
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x.h" 

/* 
* Uncomment the line corresponding to the desired System clock (SYSCLK) frequency (after 
* reset the HSI is used as SYSCLK source).
* If none of the define below is enabled, the HSI is used as System clock source. 
*/
// #define SYSCLK_FREQ_HSE    HSE_VALUE
//#define SYSCLK_FREQ_48MHz  48000000
#define SYSCLK_FREQ_96MHz  96000000
//#define SYSCLK_FREQ_144MHz  144000000

/* Clock Definitions */
#ifdef SYSCLK_FREQ_HSE
  uint32_t SystemCoreClock         = SYSCLK_FREQ_HSE;          /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_96MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_96MHz;        /* System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_144MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_144MHz;        /* System Clock Frequency (Core Clock) */

#else /* HSI Selected as System Clock source */
  uint32_t SystemCoreClock         = HSI_VALUE;                /* System Clock Frequency (Core Clock) */
#endif

__I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};


/* system_private_function_proto_types */
static void SetSysClock(void);

#ifdef SYSCLK_FREQ_HSE
  static void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_48MHz
  static void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_96MHz
  static void SetSysClockTo96(void);
#elif defined SYSCLK_FREQ_144MHz
  static void SetSysClockTo144(void);

#endif


/*********************************************************************
 * @fn      SystemInit
 *
 * @brief   Setup the microcontroller system Initialize the Embedded Flash Interface,
 *        the PLL and update the SystemCoreClock variable.
 *
 * @return  none
 */
void SystemInit (void)
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
  SetSysClock();
}

/*********************************************************************
 * @fn      SystemCoreClockUpdate
 *
 * @brief   Update SystemCoreClock variable according to Clock Register Values.
 *
 * @return  none
 */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmull = 0, pllsource = 0;
  uint8_t Pll_6_5 = 0;

#ifdef CH32V30x_D8C
  uint8_t Pll2mull = 0;

#endif

  tmp = RCC->CFGR0 & RCC_SWS;
  
  switch (tmp)
  {
    case 0x00:
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08: 
      pllmull = RCC->CFGR0 & RCC_PLLMULL;
      pllsource = RCC->CFGR0 & RCC_PLLSRC; 
      pllmull = ( pllmull >> 18) + 2;

#ifdef CH32V30x_D8
      if(pllmull == 17) pllmull = 18;
#else
      if(pllmull == 2) pllmull = 18;
      if(pllmull == 15){
          pllmull = 13;  /* *6.5 */
          Pll_6_5 = 1;
      }
      if(pllmull == 16) pllmull = 15;
      if(pllmull == 17) pllmull = 16;
#endif

      if (pllsource == 0x00)
      {
          if(EXTEN->EXTEN_CTR & EXTEN_PLL_HSI_PRE) SystemCoreClock = HSI_VALUE * pllmull;
          else SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {

#ifdef CH32V30x_D8
          if ((RCC->CFGR0 & RCC_PLLXTPRE) != (uint32_t)RESET)
          {
            SystemCoreClock = (HSE_VALUE >> 1) * pllmull;
          }
          else
          {
            SystemCoreClock = HSE_VALUE * pllmull;
          }

#else
          if(RCC->CFGR2 & (1<<16)){ /* PLL2 */
              SystemCoreClock = HSE_VALUE/(((RCC->CFGR2 & 0xF0)>>4) + 1);  /* PREDIV2 */

              Pll2mull = (uint8_t)((RCC->CFGR2 & 0xF00)>>8);

              if(Pll2mull == 0) SystemCoreClock = (SystemCoreClock * 5)>>1;
              else if(Pll2mull == 1) SystemCoreClock = (SystemCoreClock * 25)>>1;
              else if(Pll2mull == 15) SystemCoreClock = SystemCoreClock * 20;
              else  SystemCoreClock = SystemCoreClock * (Pll2mull + 2);

              SystemCoreClock = SystemCoreClock/((RCC->CFGR2 & 0xF) + 1);  /* PREDIV1 */
          }
          else{/* HSE */
              SystemCoreClock = HSE_VALUE/((RCC->CFGR2 & 0xF) + 1);  /* PREDIV1 */
          }

          SystemCoreClock = SystemCoreClock * pllmull;
#endif
      }


      if(Pll_6_5 == 1) SystemCoreClock = (SystemCoreClock / 2);

      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
 
  tmp = AHBPrescTable[((RCC->CFGR0 & RCC_HPRE) >> 4)];
  SystemCoreClock >>= tmp;  
}

/*********************************************************************
 * @fn      SetSysClock
 *
 * @brief   Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
  SetSysClockToHSE();
#elif defined SYSCLK_FREQ_48MHz
  SetSysClockTo48();
#elif defined SYSCLK_FREQ_96MHz
  SetSysClockTo96();
#elif defined SYSCLK_FREQ_144MHz
  SetSysClockTo144();

#endif
 
 /* If none of the define above is enabled, the HSI is used as System clock
  * source (default after reset) 
    */
}


#ifdef SYSCLK_FREQ_HSE

/*********************************************************************
 * @fn      SetSysClockToHSE
 *
 * @brief   Sets HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockToHSE(void)
{
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
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV1;
    
    /* Select HSE as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_HSE;    

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  { 
        /* If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  }  
}



#elif defined SYSCLK_FREQ_48MHz

/*********************************************************************
 * @fn      SetSysClockTo48
 *
 * @brief   Sets System clock frequency to 48MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo48(void)
{
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

#ifdef CH32V30x_D8C
    RCC_PREDIV2Config(RCC_PREDIV2_Div1);

    RCC_PLL2Config(RCC_PLL2Mul_4);

    RCC_PLL2Cmd(ENABLE);
    while((RCC->CTLR & (1<<27)) == 0)
    {
    }

#endif

    /*  PLL configuration: PLLCLK = HSE * 6 = 48 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));

#ifdef CH32V30x_D8
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL6);
#else
        /* PLL configuration: PLLCLK = HSE/1*4/2*3 = 48 MHz */
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL3_EXTEN);
        RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div2);
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

#elif defined SYSCLK_FREQ_96MHz

/*********************************************************************
 * @fn      SetSysClockTo96
 *
 * @brief   Sets System clock frequency to 96MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo96(void)
{
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

#ifdef CH32V30x_D8C
    RCC_PREDIV2Config(RCC_PREDIV2_Div1);

    RCC_PLL2Config(RCC_PLL2Mul_4);

    RCC_PLL2Cmd(ENABLE);
    while((RCC->CTLR & (1<<27)) == 0)
    {
    }

#endif


    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE |
                                        RCC_PLLMULL));

#ifdef CH32V30x_D8
        /*  PLL configuration: PLLCLK = HSE * 12 = 96 MHz */
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL12);
#else
        /* PLL configuration: PLLCLK = HSE/1*4/2*6 = 96 MHz */
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL6_EXTEN);
        RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div2);
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

#elif defined SYSCLK_FREQ_144MHz

/*********************************************************************
 * @fn      SetSysClockTo144
 *
 * @brief   Sets System clock frequency to 144MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo144(void)
{
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

#ifdef CH32V30x_D8C
    RCC_PREDIV2Config(RCC_PREDIV2_Div1);

    RCC_PLL2Config(RCC_PLL2Mul_4);

    RCC_PLL2Cmd(ENABLE);
    while((RCC->CTLR & (1<<27)) == 0)
    {
    }

#endif

    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE |
                                        RCC_PLLMULL));

#ifdef CH32V30x_D8
       /*  PLL configuration: PLLCLK = HSE * 18 = 144 MHz */
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL18);
#else
        /* PLL configuration: PLLCLK = HSE/1*4/2*9 = 144 MHz */
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL9_EXTEN);
        RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div2);

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


#endif
