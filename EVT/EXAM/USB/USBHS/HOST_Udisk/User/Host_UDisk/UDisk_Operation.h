/********************************** (C) COPYRIGHT  *******************************
* File Name          : UDisk_Operation.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/11/20
* Description        : This file contains all the functions prototypes for the Udisk 
                       host operation.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_UDISK_OPERATION_H_
#define USER_UDISK_OPERATION_H_

#include "debug.h"
#include "string.h"
#include "CHRV3UFI.h"
#include "ch32v30x_usbhs_host.h"
#include "usb_host_config.h"

/*******************************************************************************/
/* Public Extern Variables */
extern volatile uint8_t          UDisk_Opeation_Flag;
extern struct   _ROOT_HUB_DEVICE RootHubDev[ DEF_TOTAL_ROOT_HUB ];
extern struct   __HOST_CTL       HostCtl[ DEF_TOTAL_ROOT_HUB * DEF_ONE_USB_SUP_DEV_TOTAL ];
extern volatile uint8_t          UDisk_Opeation_Flag;
extern uint8_t  *pCodeStr;

extern __attribute__((aligned(4)))  uint8_t  Com_Buffer[ DEF_COM_BUF_LEN ];     // even address , used for host enumcation and udisk operation
extern __attribute__((aligned(4)))  uint8_t  DevDesc_Buf[ 18 ];                 // Device Descriptor Buffer

/*******************************************************************************/
/* 长文件名相关的宏定义极其全局变量 */
// 长文件名缓冲区从(0到20)*26
#define     LONG_NAME_BUF_LEN       (20*26)
#define     UNICODE_ENDIAN          0           // 1为UNICDOE大端编码 0为小端
// 长文件名存放缓冲区(Unicode编码)
extern uint8_t LongNameBuf[ ];
// 长文件名(Unicode编码)
extern uint8_t LongName[ ];
#define     LongName_Len            124
#define     TRUE                    1
#define     FALSE                   0

// 函数返回
#define     ERR_NO_NAME             0X44        // 此短文件名没有长文件名或错误的长文件
#define     ERR_BUF_OVER            0X45        // 长文件缓冲区溢出
#define     ERR_LONG_NAME           0X46        // 错误的长文件名
#define     ERR_NAME_EXIST          0X47        // 此短文件名存在

/*******************************************************************************/
/* Extern UDisk Operation Functions */
extern void mStopIfError( uint8_t iError );
extern void Udisk_USBH_Initialization( void );
extern uint8_t Udisk_USBH_EnumRootDevice( uint8_t usb_port );
extern uint8_t UDisk_USBH_PreDeal( void );
extern uint8_t UDisk_USBH_DiskReady( void );

/* Extern Long-name Operation Functions */
extern void UDisk_USBH_Longname( void );
extern uint8_t CHRV3GetLongName( void );
extern uint8_t GetUpSectorData( uint32_t *NowSector );
extern uint8_t CHRV3CreateLongName( void );
extern uint8_t AnalyzeLongName( void );
extern uint8_t CheckNameSum( uint8_t *p );

/* Extern Creating Directory Functions */
extern void UDisk_USBH_CreatDirectory( void );
extern uint8_t CreateDirectory( void );

/* Extern Byte/Sector Open/Read/Modify/Delete and File-Enumeration Functions */
extern void UDisk_USBH_ByteOperation( void );
extern void UDisk_USBH_SectorOperation( void );
extern void UDisk_USBH_EnumFiles( void );

#endif /* USER_UDISK_OPERATION_H_ */
