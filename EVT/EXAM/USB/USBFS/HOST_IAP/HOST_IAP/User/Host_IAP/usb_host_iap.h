/********************************** (C) COPYRIGHT  *******************************
 * File Name          : iap.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2020/12/16
 * Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __USB_HOST_IAP_H
#define __USB_HOST_IAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"
#include "stdio.h"
#include "string.h"
#include "ch32v30x.h"
#include "ch32v30x_usbfs_host.h"
#include "usb_host_config.h"
#include "CHRV3UFI.h"

/*******************************************************************************/
/* File Descripton */
/*
 *@Note
  适用于CH32系列的单片机，包括risc-v内核的CH32V系列和cortex-m3内核的CH32F系列
  iap_flash.c/iap_flash.h文件包括iap和app需要的iap区域代码起止定义，app区域代码起止定义，
  芯片flash的操作函数。
  ***2022年6月13日***
     1，版本0.1，
  完整的代码包括flash字，半字，字节读，iap特化的flash擦除，写入，编程，读取，校验。
  所有涉及flash读写，擦除的操作均使用DEF_FLASH_OPERATION_KEY_CODE保护，DEF_FLASH_OPERATION_KEY_CODE_0
  在主函数开始时写入，DEF_FLASH_OPERATION_KEY_CODE_1在每次操作flash前写入。
  使用宏定义区分iap代码和app代码，将iap和app对flash操作的代码统一到一个文件内
 iap代码开放所有函数调用，app代码仅开放IAP_VerifyCode_Erase(); 函数。
*/

/*******************************************************************************/
/* Macro Definitions */
#define DEF_CORE_RV                       0x01
#define DEF_CORE_CM3                      0x10
#define DEF_CORE_TYPE                     DEF_CORE_RV

/* IAP binary File */
#define DEF_IAP_FILE_NAME                 "/APP.BIN"

/* IAP Status Definitions */
#define DEF_IAP_SUCCESS                   0x00                                   /* IAP Operation Success */
#define DEF_IAP_DEFAULT                   0xFF                                   /* IAP Operation Default Status */
#define DEF_IAP_ERR_DETECT                0xF1                                   /* IAP Operation, USB device not detected */
#define DEF_IAP_ERR_ENUM                  0xF2                                   /* IAP Operation, Host enumeration failure */
#define DEF_IAP_ERR_FILE                  0xF3                                   /* IAP Operation, File name incorrect or no such file */
#define DEF_IAP_ERR_FLASH                 0xF4                                   /* IAP Operation, Flash operation failure */
#define DEF_IAP_ERR_VERIFY                0xF5                                   /* IAP Operation, Flash data verify error */
#define DEF_IAP_ERR_LENGTH                0xF6                                   /* IAP Operation, Flash data length verify error */

/* IAP Load buffer Definitions */
#define DEF_MAX_IAP_BUFFER_LEN            1024                                   /* IAP Load buffer size */

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
/* Variable Extrapolation */
/* Flash Operation Key Variables, Operation with DEF_FLASH_OPERATION_KEY_CODE_x to ensure the correctness of flash operation*/
extern volatile uint32_t Flash_Operation_Key0;                                   /* IAP Flash operation Key-code Variables 0 */
extern volatile uint32_t Flash_Operation_Key1;                                   /* IAP Flash operation Key-code Variables 1 */
extern __attribute__((aligned(4))) uint8_t  USBHD_TX_Buf[ MAX_PACKET_SIZE ] ;  // IN, must even address
extern __attribute__((aligned(4))) uint8_t  USBHD_RX_Buf[ MAX_PACKET_SIZE ] ;  // OUT, must even address

/*******************************************************************************/
/* Data Structures */
#if DEF_CORE_TYPE == DEF_CORE_CM3
typedef  void (*iapfun)(void);              //Define a function type parameter.
#endif

/*******************************************************************************/
/* Function Extrapolation */
/* Lower operation */
extern uint8_t  FLASH_ReadByte( uint32_t address );
extern uint16_t FLASH_ReadHalfWord( uint32_t address );
extern uint32_t FLASH_ReadWord( uint32_t address );
extern uint8_t  IAP_Flash_Erase( uint32_t address, uint32_t length );
extern uint8_t  IAP_Flash_Read( uint32_t address, uint8_t *buff, uint32_t length );
extern uint8_t  IAP_Flash_Write( uint32_t address, uint8_t *buff, uint32_t length );
extern uint32_t IAP_Flash_Verify( uint32_t address, uint8_t *buff, uint32_t length );
extern uint32_t IAP_Flash_Program( uint32_t address, uint8_t *buff, uint32_t length );
extern uint8_t  IAP_VerifyCode_Erase( void );
extern uint32_t IAP_VerifyCode_Write( void );
extern uint32_t IAP_VerifyCode_Check( void );
extern void     IAP_Jump_APP( void );
extern void     FLASH_ReadWordAdd(uint32_t address, u32 *buff, uint16_t length);

/* upper operation */
extern void IAP_Main_Deal( void );
extern void IAP_Initialization( void );

#ifdef __cplusplus
}
#endif

#endif


