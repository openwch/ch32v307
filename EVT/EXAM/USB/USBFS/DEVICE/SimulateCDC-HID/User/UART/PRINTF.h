/******************** (C) COPYRIGHT ********************************************
* File Name          : PRINTF.H
* Author             : MJX
* Version            : V1.00
* Date               : 2022/03/31
* Description        : Head-file of Debug printf switch
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef	__PRINTF_H__
#define __PRINTF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************/
/* Defining the serial printout switch */
#define     DEBUG_SW               1				 				            /* General print monitoring switch */

/* If the above macro is defined then the output is printed via the serial port, otherwise it is not processed */
#if( DEBUG_SW == 1 )
#define DUG_PRINTF( format, arg... )    printf( format, ##arg )		 			/* Uart printf output */
#else
#define DUG_PRINTF( format, arg... )    do{ if( 0 )printf( format, ##arg ); }while( 0 );
#endif

#ifdef __cplusplus
}
#endif

#endif

