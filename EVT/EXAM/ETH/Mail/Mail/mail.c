/********************************** (C) COPYRIGHT ******************************
* File Name          : mail.c
* Author             : WCH
* Version            : V1.0
* Date               : 2022/05/06
* Description        : 用于收发邮件
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "mail.h"

POP      *p_pop3;
SMTP     *p_smtp;
/*******************************************************************************
* Function Name  : WCHNET_ReplyMailBody
* Description    : 回复邮件正文内容，内容可修改
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef receive_over_reply
void WCHNET_ReplyMailBody( void )
{
    u8 i;

    memset( MailBodyData,'\0',sizeof(MailBodyData));
    QuotedPrintableEncode( "hello" ,MailBodyData, strlen("hello"),76 );
    strcat( send_buff, MailBodyData );
    strcat( send_buff, "\r\n    " );
    strcat( send_buff, p_pop3->DecodeRName );
    strcat( send_buff, "!" );
    memset( MailBodyData,'\0',sizeof(MailBodyData));
    QuotedPrintableEncode( (char *)g_mailbody ,MailBodyData, strlen(g_mailbody),76 );
    strcat( send_buff, MailBodyData );
    if(p_pop3->identitycheck == 1){
        for(i=0;i<6;i++) MacAddr[i] = MACAddr[i];
        WCHNET_XToChar(MacAddr,&MacAddrC[0],strlen(MacAddr));
#if DIALOG
    printf("B len= %02X\n",(u16)strlen(MacAddrC));
#endif
        if(strlen(MacAddrC)>17) MacAddrC[17] = '\0';
        strcat( send_buff, "\r\n" );
        memset( MailBodyData,'\0',sizeof(MailBodyData));
        QuotedPrintableEncode( "I am ", MailBodyData, strlen("I am "),76 );
        strcat( send_buff, MailBodyData );
        memset( MailBodyData,'\0',sizeof(MailBodyData));
        QuotedPrintableEncode( (char *)MacAddrC, MailBodyData, strlen(MacAddrC),76 );
        strcat( send_buff, MailBodyData );
        strcat( send_buff, "\r\n" );
    }
    strcat( send_buff, g_autograph1 );
    strcat( send_buff, "From: \"" );
    strcat( send_buff, p_pop3->DecodeRName );
    strcat( send_buff, "\" <" );
    strcat( send_buff, R_argv[0] );
    strcat( send_buff, ">\r\n" );
    strcat( send_buff, "To: ");
    strcat( send_buff, p_smtp->m_strSendFrom );
     strcat( send_buff, "\r\n" );
    if(strlen(p_pop3->Ccname)){
        strcat( send_buff, "Cc: ");
        strcat( send_buff, p_pop3->Ccname);
        strcat( send_buff, "\r\n" );
    }
    strcat( send_buff, "Sent: ");
    strcat( send_buff, p_pop3->sBufTime );
    strcat( send_buff, "\r\nSubject: ");
    strcat( send_buff, p_smtp->m_strSubject );
    strcat( send_buff, g_autograph2 );
}
#endif // receive_over_reply

/*******************************************************************************
* Function Name  : WCHNET_AnalyseMailData
* Description    : 分析接收到的邮件
* Input          : recv_buff-邮件内容
* Output         : None
* Return         : None
*******************************************************************************/

void WCHNET_AnalyseMailData( char *recv_buff )
{
     u16    i=0,j=0;

    if(p_pop3->ReceFlag){
        i = 0;
        if(p_pop3->EncodeType == 2) goto type2;                                 /*  跳转到附件接收 编码方式--quoted-printable */
        if(p_pop3->EncodeType == 1) goto type1;                                 /*  跳转到附件接收 编码方式--base64 */
        if(p_pop3->EncodeType == 0) goto type0;                                 /*  跳转到附件接收 编码方式--others    */
    }
    for(i=0;i<ReceLen;i++){
        /* 发件人名字/地址 */                 
        if( strncmp("\nFrom: ", &recv_buff[i], 7) == 0 ){
            i += 7;
#if DIALOG
    printf("1#From \n");
#endif
            if(recv_buff[i] == '"') i++;
            if(recv_buff[i] == '='){
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;     /* 发件人经过编码 */
                i += 3;
                j = 0;
                memset( send_buff, '\0', sizeof(send_buff) );
                /* 发件人名字 */
                while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                memset( p_pop3->DecodeRName, '\0', sizeof(p_pop3->DecodeRName) );
                Base64Decode(send_buff,  strlen(send_buff),p_pop3->DecodeRName);
#if DIALOG
    printf("DecName:%s\n", p_pop3->DecodeRName);
#endif
            }
            else{                                                               /* 未编码 */
                j = 0;
                while((recv_buff[i] != '"')&&(i < ReceLen)){                    /* 发件人名字 */
                    p_pop3->DecodeRName[j] = recv_buff[i];
                    j++;
                    i++;        
                }
            }                       
            while((recv_buff[i] != '<')&&(i < ReceLen)) i++;
            i++;
            j = 0;
            while((recv_buff[i] != '>')&&(i < ReceLen)){                        /* 发件人地址 */
                R_argv[0][j] = recv_buff[i];
                j++;
                i++; 
            }
        }
        /* 抄送 */
        if( strncmp("\nCc: ", &recv_buff[i], 5) == 0 ){
            i += 5;
#if DIALOG
    printf("2#Cc \n");
#endif        
            while(recv_buff[i] != '<')    i++;
            i++;
            j = 0;
            while(recv_buff[i] != '>'&&(i < ReceLen)){                          /* 抄送人地址 */
                p_pop3->Ccname[j] = recv_buff[i];
                j++;
                i++;        
            }
        }
        /* 日期 */
        if( strncmp("\nDate: ", &recv_buff[i], 7) == 0 ){
            i += 7;
#if DIALOG
    printf("3#date \n");
#endif
            j = 0;
            while((recv_buff[i] != '\r')&&(i < ReceLen)){                       /* 发送邮件日期 */
                p_pop3->sBufTime[j] = recv_buff[i];
                j++;
                i++;        
            }
        }
        /* 主题 */
        if( strncmp("\nSubject: ", &recv_buff[i], 10) == 0 ){
            i += 10;
#if DIALOG
    printf("4#Subject \n");
#endif
            if(recv_buff[i] == '[') {
                while(recv_buff[i] == ']'&&(i < ReceLen)) i++;
                i += 2;
            }
            if(recv_buff[i] == '='){                                            /* 主题经过编码 */
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                i += 3;
                j = 0;
                memset( send_buff, '\0', sizeof(send_buff) );
                /* 主题 */
                while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;      
                }
                Base64Decode(send_buff,strlen(send_buff),R_argv[1]);
#if DIALOG
    printf("Decode subject:%s\n", R_argv[1]);
#endif
            }
            else{                                                               /* 未编码 */
                j = 0;
                while(recv_buff[i] != '\r'&&(i < ReceLen)){                     /* 主题 */
                    R_argv[1][j] = recv_buff[i];
                    j++;
                    i++;      
                }
            }
        }
        /* 附件的编码方式、名字、内容 */
        if( strncmp("name=", &recv_buff[i], 5) == 0 ){
#if DIALOG
    printf("5#\n");
#endif
            i += 5;
            while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)&&strncmp("filename=", &recv_buff[i], 9)&&(i < ReceLen)) i++;
            if(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27) == 0){ /* 查询附件编码方式 */
/* 编码方式 */
                i += 27;
                if(strncmp("base64", &recv_buff[i], 6) == 0 ){
#if DIALOG
    printf("5#base64 \n");
#endif
                    i += 6;
                    p_pop3->EncodeType = 1;
                }
                else if(strncmp("quoted-printable", &recv_buff[i], 16) == 0 ) {
#if DIALOG
    printf("5#quoted-printable \n");
#endif
                    i += 16;
                    p_pop3->EncodeType = 2;
                }
                else p_pop3->EncodeType = 0;  
/* 附件名字 */
                while(strncmp("filename=", &recv_buff[i], 9)) i++;
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
#if DIALOG
    printf("6#filename \n");
#endif
                if(recv_buff[i] == '='){                                        /* 附件名字经过编码 */
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', sizeof(send_buff) );
                    /* 附件名字 */
                    while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){             
                        send_buff[j] = recv_buff[i];
                        j++;
                        i++;      
                    }
                    Base64Decode(send_buff,  strlen(send_buff),R_argv[2]);
                }
                else{                                                           /* 未编码 */
                    j = 0;
                    while(recv_buff[i] != '"'&&(i < ReceLen)){                  /* 附件名字 */
                        R_argv[2][j] = recv_buff[i];
                        j++;
                        i++;        
                    }
                }
            }
/* 附件名字 */
            else if( strncmp("filename=", &recv_buff[i], 9) == 0 ){
#if DIALOG
    printf("6#filename \n");
#endif
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
                if(recv_buff[i] == '='){
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', sizeof(send_buff) );
                    while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                        send_buff[j] = recv_buff[i];
                        j++;
                        i++;      
                    }
                    Base64Decode(send_buff,  strlen(send_buff),R_argv[2]);
                }
                else{
                    j = 0;
                    while(recv_buff[i] != '"'&&(i < ReceLen)){
                        R_argv[2][j] = recv_buff[i];
                        j++;
                        i++;        
                    }
                }
/* 附件编码方式 */
                while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)) i++;
                i += 27;
                if(strncmp("base64", &recv_buff[i], 6) == 0 ){
#if DIALOG
    printf("5#base64 \n");
#endif
                    i += 6;
                    p_pop3->EncodeType = 1;
                }
                else if(strncmp("quoted-printable", &recv_buff[i], 16) == 0 ) {
#if DIALOG
    printf("5#quoted-printable \n");
#endif
                    i += 16;
                    p_pop3->EncodeType = 2;
                }
                else p_pop3->EncodeType = 0;  
            }
/* 附件内容 */
            while( strncmp("\n\r\n", &recv_buff[i], 3) != 0 &&(i < ReceLen)) i++;
            i += 3;
            if(p_pop3->EncodeType==1){    /* base64 */
                j = 0;
                memset(send_buff,'\0', sizeof(send_buff) );
type1:            while((recv_buff[i] != '=')&&strncmp("\r\n--", &recv_buff[i], 4)&&strncmp("\r\n\r\n", &recv_buff[i], 4)&&(i < ReceLen)&&\
                    (j<attach_max_len)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else {
                    p_pop3->ReceFlag = 0;
                    Base64Decode(send_buff,  strlen(send_buff),AttachmentData);
                }
            }
            else if(p_pop3->EncodeType==2){     /* quoted-printable */
                j = 0;
                memset(send_buff,'\0', sizeof(send_buff));
type2:            while(strncmp("\r\n.\r\n", &recv_buff[i], 5)&&strncmp("\r\n--", &recv_buff[i], 4)&&(i < ReceLen)&&(j<attach_max_len)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else {
                    p_pop3->ReceFlag = 0;
                    QuotedPrintableDecode(send_buff,(char *)AttachmentData, strlen(send_buff));
                }
            }
            else{    /* 7bit or others */ 
                j = 0;
type0:            while(recv_buff[i] != '\r'&&(i < ReceLen)&&(j<attach_max_len)){
                    AttachmentData[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else p_pop3->ReceFlag = 0; 
            }
        }
    }
    if(p_pop3->AnalyMailDat){
#if DIALOG
    printf("addr:\n %s\n",R_argv[0]);                                           /* 发送人的地址    */ 
    printf("send name:\n %s\n",p_pop3->DecodeRName);                            /* 发送人名字 */
    printf("subject:\n %s\n",R_argv[1]);                                        /* 邮件主题 */
    printf("attach name:\n%s\n",R_argv[2]);                                     /* 附件名字 */
    printf("send time:\n %s\n",p_pop3->sBufTime);                               /* 发送时间    */
    printf("attach text:\n%s\n",AttachmentData);                                /* 附件内容    */
#endif
        if(strncmp("wch", AttachmentData, 3) == 0) p_pop3->identitycheck = 1;
        else p_pop3->identitycheck = 0;
#ifdef    receive_over_reply
        WCHNET_SMTPInit(  );
#endif
    }
}


/*******************************************************************************
* Function Name  : strFind
* Description    : 验证握手信号
* Input          : str    - 源字符串
                   substr - 待查询字符串
* Output         : None
* Return         : 收到的数据中包含的数据段的数量
*******************************************************************************/
int strFind( char str[], char substr[] )
{
    int i, j, check ,count = 0;
    int len = strlen( str );                                 /*取得字符串长度，不包括'\0'*/
    int sublen = strlen( substr );
    for( i = 0; i < len; i++ )
    {
        check = 1;                                           /*检测标记*/
        for( j = 0; j + i < len && j < sublen; j++ )         /*逐个字符进行检测，在sublen长度内，一旦出现不同字符便将check置为0*/
        {
            if( str[i + j] != substr[j] )
            {
                check = 0;
                break;
            }
        }
        if( check == 1 )                                     /*在sublen长度内的字符都相等*/
        {
            count++;
            i = i + sublen;                                  /*调整检测起始位置*/
        }
    }
    return count;
}

/*******************************************************************************
* Function Name  : WCHNET_CheckResponse
* Description    : 验证握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 返回状态
*******************************************************************************/
u8 WCHNET_CheckResponse( char *recv_buff,u8 checktype )
{
#ifdef receive_mail
    int segCnt;
#endif
    switch(checktype){
#ifdef send_mail
        case SMTP_CHECK_CNNT:    // 
            if( strncmp("220", recv_buff, 3) == 0 ){
                OrderType = SMTP_SEND_HELO;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_CNNT;
        case SMTP_CHECK_HELO:    // 
            if( strncmp("250", recv_buff, 3) == 0 ){
                OrderType = SMTP_SEND_AUTH;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_HELO;
        case SMTP_CHECK_AUTH:                                                   /* 登陆命令 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = COMMAND_UNUSEFULL;
                return (CHECK_SUCCESS);
            }
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_USER;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_AUTH;
        case SMTP_CHECK_USER:                                                   /* 用户名 */
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_PASS;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_USER;
        case SMTP_CHECK_PASS:                                                   /* 密码 */
            if(strncmp("235", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_MAIL;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_PASS;
        case SMTP_CHECK_MAIL:                                                   /* 发送者 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_RCPT;
                return (CHECK_SUCCESS);    
            } 
            return SMTP_ERR_MAIL;
        case SMTP_CHECK_RCPT:                                                   /* 接收 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_DATA;
                return (CHECK_SUCCESS);            
            }
            return SMTP_ERR_RCPT;
        case SMTP_CHECK_DATA:                                                   /* data 命令 */
            if(strncmp("354", recv_buff, 3) == 0){
                OrderType = SMTP_DATA_OVER;
                return (CHECK_SUCCESS);
            } 
            return SMTP_ERR_DATA;
        case SMTP_CHECK_DATA_END:                                               /* 数据发送完成 */
            if(strncmp("250", recv_buff, 3) == 0){
#ifdef    send_over_receive
                OrderType = POP_RECEIVE_START;
#endif
#ifdef    send_over_quit
                OrderType = SMTP_SEND_QUIT;
#endif
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_DATA_END;
        case SMTP_CHECK_QUIT:                                                   /* 退出登陆 */
            if(strncmp("220", recv_buff, 3) == 0||strncmp("221", recv_buff, 3) == 0){
                OrderType = SMTP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_QUIT;
#endif    // send_mail
#ifdef receive_mail
        case POP_CHECK_CNNT:                                                    /* 连接 */
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_USER;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_CNNT;
        case POP_CHECK_USER:                                                    /* 用户名 */
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_PASS;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_USER;
        case POP_CHECK_PASS:                                                    /* 密码 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_STAT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_PASS;
        case POP_CHECK_STAT:                                                    /* 总邮件信息 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_LIST;
                return (CHECK_SUCCESS);    
            } 
            return POP_ERR_STAT;
        case POP_CHECK_LIST:                                                    /* 邮件列表 */
            segCnt = strFind(recv_buff,"\r\n");
            if(segCnt != 1){
                if((strncmp("+OK", recv_buff, 3) == 0)){
                    p_pop3->RefreshTime = 0;
                    OrderType = POP_RECEIVE_RTER;
                    return (CHECK_SUCCESS);
                }
            }
            else{
                if((strncmp("+OK", recv_buff, 3) == 0)){
                    OrderType = COMMAND_UNUSEFULL;
                    return (CHECK_SUCCESS);
                }
                if((strncmp(".", recv_buff, 1) == 0)){
                    p_pop3->RefreshTime = 1;
                    OrderType = POP_RECEIVE_QUIT;
                    return (CHECK_SUCCESS);
                }
                if((strncmp("1", recv_buff, 1) == 0)){
                    p_pop3->RefreshTime = 0;
                    OrderType = POP_RECEIVE_RTER;
                    return (CHECK_SUCCESS);
                }
            }
            return POP_ERR_LIST;
        case POP_CHECK_QUIT:                                                    /* 退出登陆 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_QUIT;
        case POP_CHECK_RETR:                                                    /* 读取邮件内容 */ 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = COMMAND_UNUSEFULL;    
#ifdef    receive_over_reply
                memset(AttachmentData,'\0',sizeof(AttachmentData));
                memset(R_argv,'\0',sizeof(R_argv));
                memset( p_pop3, '\0', sizeof(POP) );
#endif
#if POP3_LOG_EN
                printf("%s\r\n",recv_buff);
#endif
                p_pop3->AnalyMailDat = 0;
                WCHNET_AnalyseMailData( recv_buff );
                return (CHECK_SUCCESS);
            } 
            else if(strncmp("-ERROR", recv_buff, 6) != 0){
                if( strncmp("\r\n.\r\n", &recv_buff[ReceLen-5], 5) == 0 ){
#if POP3_LOG_EN
                    printf("%s\r\n",recv_buff);
#endif
                    p_pop3->AnalyMailDat = 1;                                   /* 接收邮件完成标志 */
                    WCHNET_AnalyseMailData( recv_buff );                        /* 分析邮件内容 */
#ifdef    receive_dele_quit
                OrderType = POP_RECEIVE_DELE;
#endif
#ifdef    receive_over_quit
                OrderType = POP_RECEIVE_QUIT;    
#endif

#ifdef    receive_over_reply
                    OrderType = SMTP_SEND_START;
#endif    // receive_over_reply
                }
                else{
#if POP3_LOG_EN
                    printf("%s\r\n",recv_buff);
#endif
                    p_pop3->AnalyMailDat = 0;
                    WCHNET_AnalyseMailData( recv_buff );
                }
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_RETR;
        case POP_CHECK_DELE:                                                    /* 删除 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_DELE;
        case POP_CHECK_RSET:                                                    /* 撤销删除 */ 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_RSET;
        case POP_CHECK_TOP:        // 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            if(strncmp("Return", recv_buff, 6) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_TOP;
        case POP_CHECK_UIDL:    //
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_UIDL;
#endif    // receive_mail
        default:
            return SMTP_ERR_UNKNOW;
    }
}
/*******************************************************************************
* Function Name  : WCHNET_MailQuery
* Description    : 状态查询
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_MailQuery( void )
{
    if( p_pop3->DiscnntFlag == 1){
        p_pop3->DiscnntFlag = 0;
#ifdef    POP_REFRESH
        if(p_pop3->RefreshTime){
            p_pop3->RefreshTime = 0;
            Delay_Ms(200);
            WCHNET_CreatTcpPop3( );
        } 
#endif
    }
    if(ReceDatFlag){
#ifdef    POP_RTER                                                                                                               
        if(OrderType == POP_RECEIVE_RTER ){
            ReceDatFlag = 0;    
            WCHNET_POP3Retr( '1' );                                           /* 处理server邮件的全部文本内容    （输入邮件号） */
        }
#endif
#ifdef    POP_DELE 
        if(OrderType == POP_RECEIVE_DELE ){
            ReceDatFlag = 0;    
            WCHNET_POP3Dele( '1' );                                           /* 处理server标记删除，QUIT命令执行时才真正删除（输入邮件号） */
        }
#endif
#ifdef    POP_RSET 
        if(OrderType == POP_RECEIVE_RSET ){ 
            ReceDatFlag = 0;    
            WCHNET_POP3Rset(  );                                              /* 处理撤销所有的DELE命令 */
        }
#endif
#ifdef    POP_TOP 
        if(OrderType == POP_RECEIVE_TOP ){
            ReceDatFlag = 0;    
            WCHNET_POP3Top( '1','3' );                                        /* 返回n号邮件的前m行内容(输入邮件号,行号(必须为自然数)) */
        }
#endif
#ifdef    POP_UIDL 
        if(OrderType == POP_RECEIVE_UIDL ){
            ReceDatFlag = 0;    
            WCHNET_POP3Uidl( '1' );                                           /* 处理server返回用于该指定邮件的唯一标识，如果没有指定，返回所有的。（输入邮件号）*/
        }
#endif
    }

}

/*********************************** endfile **********************************/
