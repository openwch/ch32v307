/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_usbhs_host.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the Udisk
*                      LongName
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

#ifndef USER_UDISK_LONGNAME_H_
#define USER_UDISK_LONGNAME_H_

#include "debug.h"
#include "string.h"

/*==================== 长文件名新添的宏定义极其全局变量 ======================*/
// 长文件名缓冲区从(0到20)*26
#define     LONG_NAME_BUF_LEN       (20*26)
#define     UNICODE_ENDIAN          0           // 1为UNICDOE大端编码 0为小端
// 长文件名存放缓冲区(Unicode编码)
extern UINT8 LongNameBuf[ ];
// 长文件名(Unicode编码)
extern UINT8 LongName[ ];

#define     LongName_Len            124

#define     TRUE                    1
#define     FALSE                   0

// 函数返回
#define     ERR_NO_NAME             0X44        // 此短文件名没有长文件名或错误的长文件
#define     ERR_BUF_OVER            0X45        // 长文件缓冲区溢出
#define     ERR_LONG_NAME           0X46        // 错误的长文件名
#define     ERR_NAME_EXIST          0X47        // 此短文件名存在

/*============================== 函数功能外扩 ================================*/
extern UINT8 CHRV3GetLongName( void );
extern UINT8 GetUpSectorData( UINT32 *NowSector );
extern UINT8 CHRV3CreateLongName( void );
extern UINT8 AnalyzeLongName( void );
extern UINT8 CheckNameSum( UINT8 *p );

#endif /* USER_UDISK_LONGNAME_H_ */
