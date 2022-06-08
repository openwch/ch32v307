/********************************** (C) COPYRIGHT ******************************
* File Name          : mailcmd.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/05/06
* Description        : 收发邮件命令代码
*******************************************************************************/
#include "mailcmd.h"

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
    "EHLO",                                                                     /* 0 退出 */
    "AUTH LOGIN",                                                               /* 1 登陆 */
    "MAIL FROM:",                                                               /* 2 发件人地址 */
    "RCPT TO:",                                                                 /* 3 收件人地址 */
    "DATA",                                                                     /* 4 开始发送数据命令 */
};

/* POP3 CMD Codes */
const char  POP3_CLIENT_CMD[12][5] =
{
    /* basic POP3 commands */
    "QUIT",                                                                     /* 0 退出 */
    "USER",                                                                     /* 1 用户名 */
    "PASS",                                                                     /* 2 密码 */
    "STAT",                                                                     /* 3 邮箱统计资料 */
    "LIST",                                                                     /* 4 返回指定邮件的大小 */
    "RETR",                                                                     /* 5 邮件的全部文本 */
    "DELE",                                                                     /* 6 标记删除 */
    "RSET",                                                                     /* 7 撤销所有的DELE命令 */
    "NOOP",                                                                     /* 8 返回一个肯定的响应 */
    /* alternative POP3 commands */
    "APOP",                                                                     /* 9  认证一种安全传输口令的办法，执行成功导致状态转换 */
    "TOP" ,                                                                     /* 10 处理返回n号邮件的前m行内容，m必须是自然数 */
    "UIDL"                                                                      /* 11 返回用于该指定邮件的唯一标识 */
};
/******************************************************************************/
char     R_argv[3][32];

POP      m_pop3;
SMTP     m_smtp;
/*******************************************************************************
* Function Name  : Base64Encode
* Description    : base64编码
* Input          : src     -需要编码的字符串
                   src_len -需要编码字符串的长度
                   dst     -编码后的字符串
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : Base64Decode
* Description    : base64解码
* Input          : src     -需要解码的字符串
                   src_len -需要解码字符串的长度
                   dst     -解码后的字符串
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : QuotedPrintableEncode
* Description    : quoted printable编码
* Input          : pSrc    -需要编码的字符串
                   pDst    -编码后的字符串
                   nSrcLen -需要编码字符串的长度
                   MaxLine -最大行数
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : QuotedPrintableEncode
* Description    : quoted printable解码
* Input          : pSrc    -需要解码的字符串
                   nSrcLen -需要编码字符串的长度
                   pDst    -解码后的字符串
* Output         : None
* Return         : None
*******************************************************************************/
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
                sscanf( pSrc, "=%02x",*pDst);
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
/*******************************************************************************
* Function Name  : WCHNET_XToChar
* Description    : 16进制转字符串
* Input          : dat -要转换的十六进制数据
                   p   -转换后对应的字符串
                   len -要转换的长度
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_SMTPInit
* Description    : 发送邮件初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPInit(void)
{
    p_smtp = &m_smtp;
    p_smtp->g_MIME = 0;
    memset( p_smtp, '\0', sizeof(SMTP) );
    strcpy( p_smtp->m_strSMTPServer, m_Server );                                /* 服务器名称 */
    strcpy( p_smtp->m_strUSERID,m_UserName );                                   /* 用户名 */
    strcpy( p_smtp->m_strPASSWD,m_PassWord );                                   /* 密码 */        
    strcpy( p_smtp->m_strSendFrom,m_SendFrom );                                 /* 发件人地址 */
    strcpy( p_smtp->m_strSenderName, m_SendName );                              /* 发送人名字 */
#ifdef receive_over_reply
    strcpy( p_smtp->m_strSendTo,R_argv[0] );                                    /* 收件人地址 */
    strcpy( p_smtp->m_strSubject,R_argv[1] );                                   /* 主题 */
    strcpy( p_smtp->m_strFile,R_argv[2] );                                      /* 附件名字(如果不发送附件，此处不需初始化) */
#else
    strcpy( p_smtp->m_strSendTo,m_SendTo );                                     /* 收件人地址 */
    strcpy( p_smtp->m_strSubject,m_Subject );                                   /* 主题    */
    strcpy( p_smtp->m_strFile,m_FileName );                                     /* 附件名字(如果不发送附件，此处不需初始化) */
#endif // receive_over_reply
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPIsMIME
* Description    : 判断有无附件
* Input          : None
* Output         : None
* Return         : 0 - 无附件
                   1 - 有附件
*******************************************************************************/
void WCHNET_SMTPIsMIME( void )
{
    if( strlen(p_smtp->m_strFile) <= 0 ) p_smtp->g_MIME = 0;
    else p_smtp->g_MIME = 1;
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPAttachHeader
* Description    : 用于组建附件信封 
* Input          : pFileName     -附件名字
                   pAttachHeader -组建好的信封内容
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPAttachHeader(  char *pFileName, char *pAttachHeader )
{
    const char *strContentType = "application/octet-stream";
    sprintf(pAttachHeader, "\r\n\r\n--%s\r\nContent-Type: %s;\r\n name=\"%s\"%sContent-Disposition: \
    attachment;\r\n filename=\"%s\"\r\n\r\n", g_strBoundary, strContentType, pFileName,g_AttachHead, pFileName ); 
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPAttachEnd
* Description    : 组建附件结束内容 
* Input          : EndSize     发送长度
*                  pAttachEnd  发送buff
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPAttachEnd( u16 *EndSize, char *pAttachEnd )
{
    strcat( pAttachEnd, "\r\n--" );
    strcat( pAttachEnd, g_strBoundary );
    strcat( pAttachEnd, "--" );
    *EndSize = strlen(pAttachEnd);
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPMailHeader
* Description    : 邮件信封  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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
//    strcat( send_buff, "" );     /* 时间    */
     strcat( send_buff, "\r\n" );
    /* "X-Mailer: " */
    strcat( send_buff, g_xMailer );
    /* 有附件 */
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
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPSendAttachData
* Description    : 发送附件内容  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPSendAttachData( void )
{
    u16  EndSize;

    memset( send_buff, '\0', sizeof(send_buff) );
    WCHNET_SMTPAttachHeader( p_smtp->m_strFile, send_buff);
#if DIALOG
    printf("Attach Header:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);   /* Send attached file header */
/*****************************************************************************
*发送附件内容 
*****************************************************************************/
//    GetAttachedFileBody( &FileSize, m_smtp->m_strFile, pAttachedFileBody );
    memset( send_buff, '\0', sizeof(send_buff) );
    QuotedPrintableEncode( AttachmentData, send_buff, strlen(AttachmentData),200 );
#if DIALOG
    printf("Attach Data send_buff:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
    memset( send_buff, '\0', sizeof(send_buff) );
    WCHNET_SMTPAttachEnd( &EndSize, send_buff );
#if DIALOG
    printf("Attach End :\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);   /* Send attached file end */
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPSendAttachHeader
* Description    : 发送附件信封  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPSendAttachHeader( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, g_FormatMail );
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck,p_smtp->Socket );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n--%s\r\nContent-Type: %s;\r\n\tcharset=\"%s\"%s\r\n", g_strBoundary, g_AttachHedType, g_strcharset,g_AttachHead );
#if DIALOG
    printf("MIME Header:\n%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPEhlo
* Description    : 进入发送邮件状态  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_SMTPAuth
* Description    : 进入发送邮件状态  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_SMTPUser
* Description    : 认证用户名   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_SMTPPass
* Description    : 登陆密码    
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_SMTPMail
* Description    : 发送发件人名字  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPMail( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[2],p_smtp->m_strSendFrom );
#if DIALOG
    printf("MAIL :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_MAIL,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPRcpt
* Description    : 收件人地址 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPRcpt( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[3],p_smtp->m_strSendTo );    
#if DIALOG
    printf("RCPT :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_RCPT,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPData
* Description    : 发送data命令 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPData( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[4] );
#if DIALOG
    printf("DATA :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : WCHNET_SMTPSendMail
* Description    : 发送邮件内容  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_SMTPSendMail( void )
{
    WCHNET_SMTPIsMIME( );
    WCHNET_SMTPMailHeader(  );                                                /* Send Mail Header */
    if( p_smtp->g_MIME ==  1 ){
        WCHNET_SMTPSendAttachHeader(  );                                      /* Send MIME Header */
    }
    else {
        WCHNET_SendData("\r\n", strlen("\r\n"),uncheck,p_smtp->Socket);
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
    WCHNET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
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
/*******************************************************************************
* Function Name  : WCHNET_POP3Init
* Description    : 接收邮件初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_POP3Init( void )
{
    p_pop3 = &m_pop3;
    memset( p_pop3, '\0', sizeof(POP) );
    strcpy( p_pop3->pPop3Server,   p_Server );                                  /* 服务器名称 */
    strcpy( p_pop3->pPop3UserName, p_UserName );                                /* 用户名 */
    strcpy( p_pop3->pPop3PassWd,   p_PassWord );                                /* 密码    */
}

/*******************************************************************************
* Function Name  : WCHNET_POP3User
* Description    : 认证用户名  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_POP3User( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf(send_buff, "%s %s\r\n",POP3_CLIENT_CMD[1], p_pop3->pPop3UserName);
#if DIALOG
    printf("USER :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_USER ,p_pop3->Socket);
}

/*******************************************************************************
* Function Name  : WCHNET_POP3Pass
* Description    : 密码  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_POP3Pass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", POP3_CLIENT_CMD[2], p_pop3->pPop3PassWd );
#if DIALOG
    printf("PASS :%s\n", send_buff);
#endif
    WCHNET_SendData(  send_buff, strlen(send_buff), POP_CHECK_PASS,p_pop3->Socket );
}

/*******************************************************************************
* Function Name  : WCHNET_POP3Stat
* Description    : 回送邮箱统计资料  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_POP3Stat( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff,"%s\r\n", POP3_CLIENT_CMD[3] );
#if DIALOG
    printf("STAT :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_STAT,p_pop3->Socket );
}

/*******************************************************************************
* Function Name  : WCHNET_POP3List
* Description    : 处理server返回指定邮件的大小 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_POP3List( void )
{
#if    0            /* 如果需指定某封邮件则置1 */
    char num;
    num = '1';    /* 根据需要修改邮件号 */
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

/*******************************************************************************
* Function Name  : WCHNET_POP3Retr
* Description    : 处理server邮件的全部文本 
* Input          : num -邮件号
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_POP3Dele
* Description    : 处理server标记删除 
* Input          : num -邮件号
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_POP3Rset
* Description    : 处理server撤销删除
* Input          : num -邮件号
* Output         : None
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : WCHNET_POP3Top
* Description    : 返回n号邮件的前m行内容
* Input          : num -邮件号
                   m   -行数
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_TOP
void WCHNET_POP3Top( char num ,char m  )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c %c\r\n", POP3_CLIENT_CMD[10],num,m);
#if DIALOG
    printf("TOP :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, strlen(send_buff), POP_CHECK_TOP,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : WCHNET_POP3Uidl
* Description    : 处理server返回用于该指定邮件的唯一标识
* Input          : num -邮件号
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_UIDL
void WCHNET_POP3Uidl( char num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[11], num);
#if DIALOG
    printf("UIDL :%s\n", send_buff);
#endif
    WCHNET_SendData( send_buff, sizeof(send_buff), POP_CHECK_UIDL,p_pop3->Socket );
}
#endif

/******************************************************************************/
#endif    // receive_mail
/*******************************************************************************
* Function Name  : WCHNET_Quit
* Description    : 退出登陆
* Input          : index -需退出的socketid
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_Quit( u8 index )
{
#if  DIALOG
    printf("QUIT(socket=%2d)\n",index);
#endif
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[0]);
    if(index==p_smtp->Socket)    WCHNET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_QUIT,index );
    if(index==p_pop3->Socket)    WCHNET_SendData( send_buff, strlen(send_buff),POP_CHECK_QUIT,index );
}

/*******************************************************************************
* Function Name  : WCHNET_MailCmd
* Description    : 判断命令进入对应子程序
* Input          : choiceorder -命令类型
* Output         : None
* Return         : None
*******************************************************************************/
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
            CheckType = uncheck;
            i = WCHNET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case SMTP_SEND_QUIT:
            WCHNET_Quit( p_smtp->Socket );
            break;
        case SMTP_CLOSE_SOCKET:
            CheckType = uncheck;
            i = WCHNET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
#ifdef    receive_over_reply
            OrderType = POP_RECEIVE_DELE;
#endif
            break;
        case SMTP_SEND_START:
            WCHNET_CreatTcpSmtp( );
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
            CheckType = uncheck;
            i = WCHNET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case POP_RECEIVE_QUIT:
            WCHNET_Quit( p_pop3->Socket );
            break;
        case POP_CLOSE_SOCKET:
            CheckType = uncheck;
            i = WCHNET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
#ifdef    send_over_receive
            OrderType = SMTP_SEND_QUIT;
#endif
            break;
        case POP_RECEIVE_START:
            WCHNET_CreatTcpPop3( );                                           /* TCP连接 */
            break;
#endif    // receive_mail
        default: 
            break;
    }
}

/*********************************** endfile **********************************/
