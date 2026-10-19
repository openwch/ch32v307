/********************************** (C) COPYRIGHT *******************************
* File Name          : mailcmd.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/18
* Description        : Definition for mailcmd.c.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __MAILCMD__
#define __MAILCMD__

#include "mail.h"

/*Size of the command buffer. Declared here so that every user of send_buff
 *can take its size with sizeof() instead of an unrelated length.*/
#define SEND_BUFF_SIZE      512

extern char send_buff[SEND_BUFF_SIZE];
extern char EncodeHostName[];

extern const char *g_autograph2;

extern char R_argv[3][32];

extern const char *g_autograph1;

void Base64Encode(char *src, u16 src_len, char *dst);

void Base64Decode(char *src, u16 src_len, char *dst, u16 dst_cap);

void QuotedPrintableEncode( char *pSrc, char *pDst, u16 nSrcLen, u16 MaxLine, u16 dst_cap );

void QuotedPrintableDecode( char *pSrc, char *pDst, u16 nSrcLen, u16 dst_cap );

void WCHNET_XToChar( char  *dat,char  *p,char len);

void WCHNET_SMTPInit(void);

void WCHNET_SMTPIsMIME(void);

void WCHNET_SMTPAttachHeader(  char *pFileName, char *pAttachHeader, u16 dst_cap );

void WCHNET_SMTPAttachEnd( u16 *EndSize, char *pAttachEnd );

void WCHNET_SMTPMailHeader( void );

void WCHNET_SMTPSendAttachData( void );

void WCHNET_SMTPSendAttachHeader( void );

void WCHNET_SMTPEhlo( void );

void WCHNET_SMTPAuth( void );

void WCHNET_SMTPUser( void );

void WCHNET_SMTPPass( void );

void WCHNET_SMTPMail( void );

void WCHNET_SMTPRcpt( void );

void WCHNET_SMTPData( void );

void WCHNET_SMTPSendMail( void );

void WCHNET_POP3Init( void );

void WCHNET_POP3User( void );

void WCHNET_POP3Pass( void );

void WCHNET_POP3Stat( void );

void WCHNET_POP3List( void );

void WCHNET_POP3Retr( u8 num );

void WCHNET_POP3Dele( u8 num );

void WCHNET_POP3Rset( void );

void WCHNET_POP3Top( char num ,char m  );

void WCHNET_POP3Uidl( char num );

void WCHNET_Quit( u8 index );

void WCHNET_MailCmd( u8 choiceorder );

#endif
