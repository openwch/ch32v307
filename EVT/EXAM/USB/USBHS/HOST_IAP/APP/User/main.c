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
  *This routine is used in conjunction with HOST_IAP, as the APP demonstration code, and prints  
  *“This is a APP code for a IAP application” after jumping ,
 *PA0 pulls the erase flag low, and jumps to IAP after soft reset.
*/

#include "debug.h"

/*******************************************************************************/
/* Macro Definitions */
#define DEF_FLASH_PAGE_SIZE               0x100


/* Flash page size */
#define DEF_FLASH_PAGE_SIZE               0x100                                  /* Flash Page size, refer to the data-sheet (ch32vf2x_3xRM.pdf) for details */

/* APP CODE ADDR Setting */
#define DEF_APP_CODE_START_ADDR           0x08006000                             /* IAP Flash Operation start address, user code start address */
#define DEF_APP_CODE_END_ADDR             0x08030000                             /* IAP Flash Operation end address, user code end address */
                                                                                 /* Please refer to link.ld file for accuracy flash size, the size here is the smallest available size */
                                                                                 /* CH32V303CB/RB, CH32V305RB/FB  - 0x08020000 (128K),
                                                                                  * CH32V307VC/WC/RC, CH32V303VC/RC - 0x08030000 (192K)( Adjustable, refer to link.ld ) */
#define DEF_APP_CODE_MAXLEN               (DEF_APP_CODE_END_ADDR-DEF_APP_CODE_START_ADDR) /* IAP Flash Operation size, user code max size */

/* Verify CODE ADDR Setting */
#define DEF_VERIFY_CODE_START_ADDR        0x08005F00                             /* IAP Flash verify-code start address */
#define DEF_VERIFY_CODE_END_ADDR          0x08006000                             /* IAP Flash verify-code end address */
#define DEF_VERIFY_CODE_MAXLEN            (DEF_VERIFY_CODE_END_ADDR-DEF_VERIFY_CODE_START_ADDR) /* IAP Flash verify-code max size */
#define DEF_VERIFY_CODE_LEN               0x10                                   /* IAP Flash verify-code actual length, be careful not to exceed the DEF_VERIFY_CODE_MAXLEN */

/* Flash Operation Key Setting */
#define DEF_FLASH_OPERATION_KEY_CODE_0    0x1A86FF00                             /* IAP Flash operation Key-code 0 */
#define DEF_FLASH_OPERATION_KEY_CODE_1    0x55AA55AA                             /* IAP Flash operation Key-code 1 */


/*******************************************************************************/
/* Flash Operation Key */
volatile uint32_t Flash_Operation_Key0;
volatile uint32_t Flash_Operation_Key1;

/*******************************************************************************/
/* Function Statement */
void    GPIO_Cfg_init( void );
uint8_t PA0_Check( void );
uint8_t IAP_VerifyCode_Erase( void );
uint8_t IAP_Flash_Erase( uint32_t address, uint32_t length );

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint8_t ret;
	SystemCoreClockUpdate( );
	Delay_Init( );
    Delay_Ms( 100 );
	USART_Printf_Init(115200);
		
	printf( "SystemClk:%d\r\n", SystemCoreClock );
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf( "This is a APP code for a IAP application\n" );
	GPIO_Cfg_init( );

	Flash_Operation_Key0 = DEF_FLASH_OPERATION_KEY_CODE_0;

	while(1)
	{
	    ret = PA0_Check( );
	    if( ret )
	    {
	        printf( "Reset Chip, prepare to Jump To IAP\r\n" );
	        printf( "Erase Verify-Code\r\n" );
	        IAP_VerifyCode_Erase( );
	        NVIC_SystemReset( );
	    }
	    Delay_Ms( 200 );
	    printf( "USER Code\r\n" );
	}
}

/*********************************************************************
 * @fn      IAP_Flash_Erase
 *
 * @brief   Erase Flash In page(256 bytes),Specify length & address,
 *          Based On Fast Flash Operation,
 *          With address protection and program runaway protection.
 *
 * @return  0: Operation Success
 *          See notes for other errors
 */
uint8_t IAP_Flash_Erase( uint32_t address, uint32_t length )
{
    uint32_t i;
    uint32_t erase_addr;
    uint32_t erase_len;
    volatile uint32_t page_cnts;
    volatile uint32_t page_addr;

    /* Set initial value */
    erase_addr = address;
    page_addr = address;
    erase_len = length;
    if( (erase_len%256) == 0 )
    {
        page_cnts = erase_len/256;
    }
    else
    {
        page_cnts = (erase_len/256) + 1;
    }

    /* Verify Keys, No flash operation if keys are not correct */
    if( (Flash_Operation_Key0 != DEF_FLASH_OPERATION_KEY_CODE_0) || (Flash_Operation_Key1 != DEF_FLASH_OPERATION_KEY_CODE_1) )
    {
        /* ERR: Risk of code running away */
        return 0xFF;
    }
    /* Verify Address, No flash operation if the address is out of range */
    if( ((erase_addr >= DEF_APP_CODE_START_ADDR) && (erase_addr <= DEF_APP_CODE_END_ADDR)) || ((erase_addr >= DEF_VERIFY_CODE_START_ADDR) && (erase_addr <= DEF_VERIFY_CODE_END_ADDR)) )
    {
        for( i=0; i<page_cnts; i++ )
        {
            /* Verify Keys, No flash operation if keys are not correct */
            if( Flash_Operation_Key0 != DEF_FLASH_OPERATION_KEY_CODE_0 )
            {
                /* ERR: Risk of code running away */
                return 0xFF;
            }
            FLASH_Unlock_Fast( );
            FLASH_ErasePage_Fast( page_addr );
            page_addr += DEF_FLASH_PAGE_SIZE;
        }
    }

    return 0;
}

/*********************************************************************
 * @fn      IAP_VerifyCode_Erase
 *
 * @brief   Erase IAP VerifyCode, Based On Fast Flash Operation.
 *          With address protection and program runaway protection.
 *
 * @return  ret : The meaning of 'ret' can be found in the notes of the
 *          corresponding function.
 */
uint8_t  IAP_VerifyCode_Erase( void )
{
    uint8_t ret;

    /* Verify Code Erase */
    Flash_Operation_Key1 = DEF_FLASH_OPERATION_KEY_CODE_1;
    ret = IAP_Flash_Erase( DEF_VERIFY_CODE_START_ADDR, DEF_VERIFY_CODE_MAXLEN );
    Flash_Operation_Key1 = 0;
    if( ret != 0 )
    {
        return ret;
    }

    return 0;
}

/*********************************************************************
 * @fn      GPIO_Cfg_init
 *
 * @brief   GPIO init
 *
 * @return  none
 */
void GPIO_Cfg_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      PA0_Check
 *
 * @brief   Check PA0 state
 *
 * @return  0 - not Press Down
 *          1 - Press Down
 */
uint8_t PA0_Check( void )
{
    u8 i;
    GPIO_Cfg_init( );
    i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    if (i == 0)
    {
        Delay_Ms(200);
        i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        if (i == 0)
        {
            return 1;
        }
    }
    return 0;
}
