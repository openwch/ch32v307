/********************************** (C) COPYRIGHT *******************************
* File Name          : mailcmd.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/05/06
* Description        : Send and receive mail function.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "mailcmd.h"

char send_buff[512];
char EncodeHostName[32];

#ifdef receive_mail
const     char base64_decode_map[256] =
{
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255,  62, 255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
     255,   0, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255, 255,  26,  27,  28,
      29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
      49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};
#endif

/******************************************************************************/
const char base64_map[64] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* SMTP CMD Codes */
const  char  SMTP_CLIENT_CMD[5][11] =
{
    "EHLO",
    "AUTH LOGIN",
    "MAIL FROM:",
    "RCPT TO:",
    "DATA",
};

/* POP3 CMD Codes */
const char  POP3_CLIENT_CMD[12][5] =
{
    /* basic POP3 commands */
    "QUIT",
    "USER",
    "PASS",
    "STAT",
    "LIST",
    "RETR",
    "DELE",
    "RSET",
    "NOOP",
    /* alternative POP3 commands */
    "APOP",
    "TOP" ,
    "UIDL"
};
/******************************************************************************/
char     R_argv[3][32];

POP      m_pop3;
SMTP     m_smtp;

/*********************************************************************
 * @fn      Base64Encode
 *
 * @brief   base64 encoding.
 *
 * @param   src - String to be encoded.
 *          src_len - the length of the encoded string required
 *          dst - encoded string
 *
 * @return  none
 */
#ifdef send_mail
void Base64Encode(char *src, u16 src_len, char *dst)
{
        u16 i = 0,j = 0;
        
        for (; i < src_len - src_len % 3; i += 3) {
                dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                dst[j++] = base64_map[((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f];
                dst[j++] = base64_map[((src[i + 1] << 2) | (src[i + 2] >> 6 )) & 0x3f];
                dst[j++] = base64_map[src[i + 2] & 0x3f];
        }
        
        if (src_len % 3 == 1) {
                 dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                 dst[j++] = base64_map[(src[i] << 4) & 0x3f];
                 dst[j++] = '=';
                 dst[j++] = '=';
        }
        else if (src_len % 3 == 2) {
                dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                dst[j++] = base64_map[((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f];
                dst[j++] = base64_map[(src[i + 1] << 2) & 0x3f];
                dst[j++] = '=';
        }
        dst[j] = '\0';
}
#endif

/*********************************************************************
 * @fn      Base64Decode
 *
 * @brief   base64 decoding.
 *
 * @param   pSrc - String to decode.
 *          src_len - the length of the string that needs to be decoded
 *          dst - decoded string
 *
 * @return  none
 */
#ifdef receive_mail
void Base64Decode(char *src, u16 src_len, char *dst)
{
        u32 i = 0, j = 0;
        
        for (; i < src_len; i += 4) {
            if(strncmp( &src[i], "\r\n", 2 ) == 0) i += 2;
            dst[j++] = base64_decode_map[src[i]] << 2 |
                    base64_decode_map[src[i + 1]] >> 4;
            dst[j++] = base64_decode_map[src[i + 1]] << 4 |
                    base64_decode_map[src[i + 2]] >> 2;
            dst[j++] = base64_decode_map[src[i + 2]] << 6 |
                    base64_decode_map[src[i + 3]];
        }
        if(src_len%4 == 3) {
            dst[strlen(dst)-1] = '\0'; 
        }
        else if(src_len%4 == 2) {
            dst[strlen((char *)dst)-1] = '\0'; 
            dst[strlen((char *)dst)-1] = '\0'; 
        }
        else dst[j] = '\0';
}
#endif

/*********************************************************************
 * @fn      QuotedPrintableEncode
 *
 * @brief   quoted printable encoding.
 *
 * @param   pSrc - String to be encoded.
 *          pDst - encoded string
 *          nSrcLen -  data length
 *          MaxLine - maximum number of lines
 *
 * @return  none
 */
#ifdef send_mail
void QuotedPrintableEncode( char *pSrc, char *pDst, u16 nSrcLen, u16 MaxLine )
{
    u16 nDstLen  = 0;
    u16 nLineLen = 0;
    u16 i = 0;

    for(i = 0; i < nSrcLen; i++, pSrc++ ){        
        if( (*pSrc >= '!') && (*pSrc <= '~') && (*pSrc != '=') ){
            *pDst++ = *pSrc;
            nDstLen++;
            nLineLen++;
        }
        else{
            sprintf(pDst, "=%02x", *pSrc);
            pDst += 3; 
            nDstLen += 3;
            nLineLen += 3;
        }
        if( nLineLen >= MaxLine - 3 ){    
            sprintf(pDst,"=\r\n");
            pDst += 3;
            nDstLen += 3;
            nLineLen = 0;
        }
    }
    *pDst = '\0';
}
#endif

/*********************************************************************
 * @fn      QuotedPrintableDecode
 *
 * @brief   quoted printable decoding.
 *
 * @param   pSrc - String to decode.
 *          pDst - decoded string
 *          nSrcLen -  data length
 *
 * @return  none
 */
#ifdef receive_mail
void QuotedPrintableDecode( char *pSrc, char *pDst, u16 nSrcLen )
{
    u16 i = 0;
    u16 nDstLen = 0;

    while( i < nSrcLen ){
        if( strncmp( pSrc, "=\r\n", 3 ) == 0 ){
            pSrc += 3;
            i += 3;
        }
        else{
            if( *pSrc == '=' ){
                sscanf( pSrc, "=%02x",pDst);
                pDst++;
                pSrc += 3;
                i += 3;
            }
            else{
                *pDst++ = *pSrc++;
                i++;
            }
            nDstLen++;
        }
    }
    *pDst = '\0';
}
#endif

/*******************************************************************************/
#ifdef send_mail
/*********************************************************************
 * @fn      WCHNET_XToChar
 *
 * @brief   hex to string.
 *
 * @param   dat - hex data to convert.
 *          p - The corresponding string after conversion
 *          len -  length to convert
 *
 * @return  none
 */
#ifdef receive_over_reply
void WCHNET_XToChar( char  *dat,char  *p,char len)
{
    char k;
    for(k=0;k<len;k++){
        *p = (((dat[k]&0xf0)>>4)/10)?(((dat[k]&0xf0)>>4)+'A'-10):(((dat[k]&0xf0)>>4)+'0');
        p++;
        *p = ((dat[k]&0x0f)/10)?((dat[k]&0x0f)+'A'-10):((dat[k]&0x0f)+'0');
        p++;
        if(k<len-1){
            *p = '.';
            p++;        
        }
    }
}
#endif //receive_over_reply

/*********************************************************************
 * @fn      WCHNET_SMTPInit
 *
 * @brief   Email sending initialization.
 *
 * @return  none
 */
void WCHNET_SMTPInit(void)
{
    p_smtp = &m_smtp;
    p_smtp->g_MIME = 0;
    memset( p_smtp, '\0', sizeof(SMTP) );
    strcpy( p_smtp->m_strSMTPServer, m_Server );
    strcpy( p_smtp->m_strUSERID,m_UserName );
    strcpy( p_smtp->m_strPASSWD,m_PassWord );
    strcpy( p_smtp->m_strSendFrom,m_SendFrom );
    strcpy( p_smtp->m_strSenderName, m_SendName );
#ifdef receive_over_reply
    strcpy( p_smtp->m_strSendTo,R_argv[0] );
    strcpy( p_smtp->m_strSubject,R_argv[1] );
    strcpy( p_smtp->m_strFile,R_argv[2] );
#else
    strcpy( p_smtp->m_strSendTo,m_SendTo );
    strcpy( p_smtp->m_strSubject,m_Subject );
    strcpy( p_smtp->m_strFile,m_FileName );
#endif // receive_over_reply
}

/*********************************************************************
 * @fn      WCHNET_SMTPIsMIME
 *
 * @brief   Check for attachments.
 *
 * @return  none
 */
void WCHNET_SMTPIsMIME( void )
{
    if( strlen(p_smtp->m_strFile) <= 0 ) p_smtp->g_MIME = 0;
    else p_smtp->g_MIME = 1;
}

/*********************************************************************
 * @fn      WCHNET_SMTPAttachHeader
 *
 * @brief   Attachment envelope.
 *
 * @param   pFileName - Attachment name.
 *          pAttachHeader - Envelope Contents
 *
 * @return  none
 */
void WCHNET_SMTPAttachHeader(  char *pFileName, char *pAttachHeader )
{
    const char *strContentType = "application/octet-stream";
    sprintf(pAttachHeader, "\r\n\r\n--%s\r\nContent-Type: %s;\r\n name=\"%s\"%sContent-Disposition: \
    attachment;\r\n filename=\"%s\"\r\n\r\n", g_strBoundary, strContentType, pFileName,g_AttachHead, pFileName ); 
}

/*********************************************************************
 * @fn      WCHNET_SMTPAttachEnd
 *
 * @brief   Form attachment end content.
 *
 * @param   EndSize - data length.
 *          pAttachEnd - data buff
 *
 * @return  none
 */
void WCHNET_SMTPAttachEnd( u16 *EndSize, char *pAttachEnd )
{
    strcat( pAttachEnd, "\r\n--" );
    strcat( pAttachEnd, g_strBoundary );
    strcat( pAttachEnd, "--" );
    *EndSize = strlen(pAttachEnd);
}

/*********************************************************************
 * @fn      WCHNET_SMTPMailHeader
 *
 * @brief   mail envelope.
 *
 * @return  none
 */
void WCHNET_SMTPMailHeader( void )
{
    // "FROM: "
    memset( send_buff, '\0', sizeof(send_buff) );
    strcat( send_buff, "From: \"" );
    strcat( send_buff, p_smtp->m_strSenderName );
    strcat( send_buff, "\" <" );
    strcat( send_buff, p_smtp->m_strSendFrom );
    strcat( send_buff, ">\r\n" );
    // "TO: " 
    strcat( send_buff, "To: <" );
    strcat( send_buff, p_smtp->m_strSendTo );
    strcat( send_buff, ">\r\n" );
    // "Subject: " 
    strcat( send_buff, "Subject: ");
#ifdef receive_over_reply
    strcat( send_buff, "Re: ");
#endif
    strcat( send_buff, p_smtp->m_strSubject );
    strcat( send_buff, "\r\n" );
    //"Date: " 
    strcat( send_buff, "Date: ");
//    strcat( send_buff, "" );     /* ʱ��    */
     strcat( send_buff, "\r\n" );
    /* "X-Mailer: " */
    strcat( send_buff, g_xMailer );
    /* �и��� */
    if( p_smtp->g_MIME == 1 ){
        strcat( send_buff, "MIME-Version: 1.0\r\nContent-Type: " );
        strcat( send_buff, g_MailHedType );
        strcat( send_buff, ";\r\n\tboundary=\"" );
        strcat( send_buff, g_strBoundary );
        strcat( send_buff, "\"\r\n" );
    }
    /* Encoding information    */
    strcat( send_buff, g_Encoding );
    strcat( send_buff, p_smtp->m_strSenderName );
    strcat( send_buff, " <" );
    strcat( send_buff, p_smtp->m_strSendFrom );
    strcat( send_buff, ">\r\n" );
    /* add custom-tailor */
    strcat( send_buff, g_Custom );
    /* end of mail header */
    strcat( send_buff, "\r\n\r\n" );
#if DIALOG
    printf("Mail Header:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK ,p_smtp->Socket);
}

/*********************************************************************
 * @fn      WCHNET_SMTPSendAttachData
 *
 * @brief   Send attachment content.
 *
 * @return  none
 */
void WCHNET_SMTPSendAttachData( void )
{
    u16  EndSize;

    memset( send_buff, '\0', sizeof(send_buff) );
    WCHNET_SMTPAttachHeader( p_smtp->m_strFile, send_buff);
#if DIALOG
    printf("Attach Header:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK ,p_smtp->Socket);   /* Send attached file header */
/*****************************************************************************
*Attachment content
*****************************************************************************/
//    GetAttachedFileBody( &FileSize, m_smtp->m_strFile, pAttachedFileBody );
    memset( send_buff, '\0', sizeof(send_buff) );
    QuotedPrintableEncode( AttachmentData, send_buff, strlen(AttachmentData),200 );
#if DIALOG
    printf("Attach Data send_buff:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK ,p_smtp->Socket);
    memset( send_buff, '\0', sizeof(send_buff) );
    WCHNET_SMTPAttachEnd( &EndSize, send_buff );
#if DIALOG
    printf("Attach End :\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK ,p_smtp->Socket);   /* Send attached file end */
}

/*********************************************************************
 * @fn      WCHNET_SMTPSendAttachHeader
 *
 * @brief   Send attachment envelope.
 *
 * @return  none
 */
void WCHNET_SMTPSendAttachHeader( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, g_FormatMail );
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK,p_smtp->Socket );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n--%s\r\nContent-Type: %s;\r\n\tcharset=\"%s\"%s\r\n",
            g_strBoundary, g_AttachHedType, g_strcharset,g_AttachHead );
#if DIALOG
    printf("MIME Header:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK,p_smtp->Socket);
}

/*********************************************************************
 * @fn      WCHNET_SMTPEhlo
 *
 * @brief   Send HELLO command.
 *
 * @return  none
 */
void WCHNET_SMTPEhlo( void )
{
    memset( EncodeHostName, '\0', sizeof(EncodeHostName) );
    QuotedPrintableEncode( (char *)p_smtp->m_strSenderName, EncodeHostName, strlen(p_smtp->m_strSenderName),76 );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", SMTP_CLIENT_CMD[0],EncodeHostName );
#if DIALOG
    printf("EHLO :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_HELO ,p_smtp->Socket);
}

/*********************************************************************
 * @fn      WCHNET_SMTPAuth
 *
 * @brief   send login command.
 *
 * @return  none
 */
void WCHNET_SMTPAuth( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[1]);
#if DIALOG
    printf("AUTH :%s\n", send_buff);
#endif
    /* send "AUTH LOGIN" command */
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_AUTH ,p_smtp->Socket);
}

/*********************************************************************
 * @fn      WCHNET_SMTPUser
 *
 * @brief   Authentication user name.
 *
 * @return  none
 */
void WCHNET_SMTPUser( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    Base64Encode( (char *)p_smtp->m_strUSERID, strlen(p_smtp->m_strUSERID), send_buff );
    sprintf( send_buff, "%s\r\n", send_buff);
#if DIALOG
    printf("USER :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_USER,p_smtp->Socket );
}

/*********************************************************************
 * @fn      WCHNET_SMTPPass
 *
 * @brief   Authentication login password.
 *
 * @return  none
 */
void WCHNET_SMTPPass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    Base64Encode( (char *)p_smtp->m_strPASSWD, strlen(p_smtp->m_strPASSWD), send_buff);
    sprintf( send_buff, "%s\r\n", send_buff);
#if DIALOG
    printf("PASS :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_PASS,p_smtp->Socket );
}

/*********************************************************************
 * @fn      WCHNET_SMTPMail
 *
 * @brief   Send sender name.
 *
 * @return  none
 */
void WCHNET_SMTPMail( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[2],p_smtp->m_strSendFrom );
#if DIALOG
    printf("MAIL :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_MAIL,p_smtp->Socket );
}

/*********************************************************************
 * @fn      WCHNET_SMTPRcpt
 *
 * @brief   receiver's address.
 *
 * @return  none
 */
void WCHNET_SMTPRcpt( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[3],p_smtp->m_strSendTo );    
#if DIALOG
    printf("RCPT :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_RCPT,p_smtp->Socket );
}

/*********************************************************************
 * @fn      WCHNET_SMTPData
 *
 * @brief   send DATA command.
 *
 * @return  none
 */
void WCHNET_SMTPData( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[4] );
#if DIALOG
    printf("DATA :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA,p_smtp->Socket );
}

/*********************************************************************
 * @fn      WCHNET_SMTPSendMail
 *
 * @brief   Send email content.
 *
 * @return  none
 */
void WCHNET_SMTPSendMail( void )
{
    WCHNET_SMTPIsMIME( );
    WCHNET_SMTPMailHeader(  );                                                /* Send Mail Header */
    if( p_smtp->g_MIME ==  1 ){
        WCHNET_SMTPSendAttachHeader(  );                                      /* Send MIME Header */
    }
    else {
        WCHNET_SendData("\r\n", strlen("\r\n"),UNCHECK,p_smtp->Socket);
    }
    memset( send_buff, '\0', sizeof(send_buff) );
#ifdef    receive_over_reply
    WCHNET_ReplyMailBody( );
#else     // receive_over_reply 
     QuotedPrintableEncode( (char *)MailBodyData, send_buff, strlen(MailBodyData),76 );
#endif    // receive_over_reply 
#if DIALOG
    printf("text data:\n%s\n",send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),UNCHECK ,p_smtp->Socket);
    if( 1 == p_smtp->g_MIME ) WCHNET_SMTPSendAttachData( );                   /* Send Attached file */
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n.\r\n" );
#if DIALOG
    printf("OVER :%s\n", send_buff);
#endif
    /* Send end flag of Mail */
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA_END ,p_smtp->Socket);
}

/******************************************************************************/
#endif // send_mail
/******************************************************************************/
#ifdef receive_mail
/*********************************************************************
 * @fn      WCHNET_POP3Init
 *
 * @brief   Receive mail initialization.
 *
 * @return  none
 */
void WCHNET_POP3Init( void )
{
    p_pop3 = &m_pop3;
    memset( p_pop3, '\0', sizeof(POP) );
    strcpy( p_pop3->pPop3Server,   p_Server );
    strcpy( p_pop3->pPop3UserName, p_UserName );
    strcpy( p_pop3->pPop3PassWd,   p_PassWord );
}

/*********************************************************************
 * @fn      WCHNET_POP3User
 *
 * @brief   Authentication user name.
 *
 * @return  none
 */
void WCHNET_POP3User( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf(send_buff, "%s %s\r\n",POP3_CLIENT_CMD[1], p_pop3->pPop3UserName);
#if DIALOG
    printf("USER :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_USER ,p_pop3->Socket);
}

/*********************************************************************
 * @fn      WCHNET_POP3Pass
 *
 * @brief   Authentication password.
 *
 * @return  none
 */
void WCHNET_POP3Pass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", POP3_CLIENT_CMD[2], p_pop3->pPop3PassWd );
#if DIALOG
    printf("PASS :%s\n", send_buff);
#endif
    WCHNET_SendData(  send_buff, strlen(send_buff), POP_CHECK_PASS,p_pop3->Socket );
}

/*********************************************************************
 * @fn      WCHNET_POP3Stat
 *
 * @brief   Get mailbox statistics.
 *
 * @return  none
 */
void WCHNET_POP3Stat( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff,"%s\r\n", POP3_CLIENT_CMD[3] );
#if DIALOG
    printf("STAT :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_STAT,p_pop3->Socket );
}

/*********************************************************************
 * @fn      WCHNET_POP3List
 *
 * @brief   Get email list information.
 *
 * @return  none
 */
void WCHNET_POP3List( void )
{
#if    0            /* �����ָ��ĳ���ʼ�����1 */
    char num;
    num = '1';    /* ������Ҫ�޸��ʼ��� */
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[4],num );
#else
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[4] );
#endif
#if DIALOG
    printf("LIST :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_LIST,p_pop3->Socket);
}

/*********************************************************************
 * @fn      WCHNET_POP3Retr
 *
 * @brief   deal the full text of an email
 *
 * @param   num - mail number.
 *
 * @return  none
 */
#ifdef    POP_RTER 
void WCHNET_POP3Retr( u8 num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[5], num );
#if DIALOG
    printf("RTER :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_RETR,p_pop3->Socket );
}
#endif

/*********************************************************************
 * @fn      WCHNET_POP3Dele
 *
 * @brief   Mark to delete, only when the QUIT
 *          command is executed is actually deleted.
 *
 * @param   num - mail number.
 *
 * @return  none
 */
#ifdef    POP_DELE
void WCHNET_POP3Dele( u8 num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[6], num );
#if DIALOG
    printf("DELE :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_DELE,p_pop3->Socket );
}
#endif

/*********************************************************************
 * @fn      WCHNET_POP3Rset
 *
 * @brief   Undo delete.
 *
 * @return  none
 */
#ifdef    POP_RSET
void WCHNET_POP3Rset( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s \r\n", POP3_CLIENT_CMD[7]);
#if DIALOG
    printf("RSET :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_RSET,p_pop3->Socket );
}
#endif

/*********************************************************************
 * @fn      WCHNET_POP3Top
 *
 * @brief   Returns the first 'm' lines of mail number 'num'.
 *
 * @param   num - mail number.
 *          m - rows.
 *
 * @return  none
 */
#ifdef    POP_TOP
void WCHNET_POP3Top( char num ,char m  )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c %c\r\n", POP3_CLIENT_CMD[10], num, m);
#if DIALOG
    printf("TOP :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_TOP, p_pop3->Socket );
}
#endif

/*********************************************************************
 * @fn      WCHNET_POP3Uidl
 *
 * @brief   Returns the unique identifier for the specified message.
 *
 * @param   num - mail number.
 *
 * @return  none
 */
#ifdef    POP_UIDL
void WCHNET_POP3Uidl( char num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[11], num);
#if DIALOG
    printf("UIDL :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, sizeof(send_buff), POP_CHECK_UIDL, p_pop3->Socket );
}
#endif

/******************************************************************************/
#endif    // receive_mail

/*********************************************************************
 * @fn      WCHNET_Quit
 *
 * @brief   sign out.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_Quit( u8 id )
{
#if  DIALOG
    printf("QUIT(socket=%2d)\n", id);
#endif
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[0]);
    if(id == p_smtp->Socket)    WCHNET_SendData( send_buff, strlen(send_buff), SMTP_CHECK_QUIT, id );
    if(id == p_pop3->Socket)    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_QUIT, id );
}

/*********************************************************************
 * @fn      WCHNET_MailCmd
 *
 * @brief   Analytical data.
 *
 * @param   choiceorder - Command type.
 *
 * @return  none
 */
void WCHNET_MailCmd( u8 choiceorder )
{
    u8 i;

    switch( choiceorder ){
#ifdef send_mail
        case SMTP_SEND_HELO: 
            WCHNET_SMTPEhlo( );
            break;
        case SMTP_SEND_AUTH: 
            WCHNET_SMTPAuth( );
            break;
        case SMTP_SEND_USER: 
            WCHNET_SMTPUser( );
            break;
        case SMTP_SEND_PASS: 
            WCHNET_SMTPPass( );
            break;
        case SMTP_SEND_MAIL: 
            WCHNET_SMTPMail( );
            break;
        case SMTP_SEND_RCPT: 
            WCHNET_SMTPRcpt( );
            break;
        case SMTP_SEND_DATA: 
            WCHNET_SMTPData( );
            break;
        case SMTP_DATA_OVER:
            WCHNET_SMTPSendMail( );
            break;
        case SMTP_ERR_CHECK:
            WCHNET_Quit( p_smtp->Socket );
            CheckType = UNCHECK;
            i = WCHNET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case SMTP_SEND_QUIT:
            WCHNET_Quit( p_smtp->Socket );
            break;
        case SMTP_CLOSE_SOCKET:
            CheckType = UNCHECK;
            i = WCHNET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
#ifdef    receive_over_reply
            OrderType = POP_RECEIVE_DELE;
#endif
            break;
        case SMTP_SEND_START:
            WCHNET_CreateTcpSmtp( );
            break;
#endif    // send_mail 
#ifdef receive_mail
        case POP_RECEIVE_USER: 
            WCHNET_POP3User(  );
            break;
        case POP_RECEIVE_PASS: 
            WCHNET_POP3Pass(  );
            break;
        case POP_RECEIVE_STAT: 
            WCHNET_POP3Stat(  );
            break;
        case POP_RECEIVE_LIST: 
            WCHNET_POP3List(  );
            break;
        case POP_ERR_CHECK:
            WCHNET_Quit( p_pop3->Socket );
            CheckType = UNCHECK;
            i = WCHNET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case POP_RECEIVE_QUIT:
            WCHNET_Quit( p_pop3->Socket );
            break;
        case POP_CLOSE_SOCKET:
            CheckType = UNCHECK;
            i = WCHNET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
#ifdef    send_over_receive
            OrderType = SMTP_SEND_QUIT;
#endif
            break;
        case POP_RECEIVE_START:
            WCHNET_CreateTcpPop3( );
            break;
#endif    // receive_mail
        default: 
            break;
    }
}

/*********************************** endfile **********************************/
