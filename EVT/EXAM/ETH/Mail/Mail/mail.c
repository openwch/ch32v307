/********************************** (C) COPYRIGHT *******************************
* File Name          : mail.c
* Author             : WCH
* Version            : V1.0
* Date               : 2022/05/06
* Description        : for sending and receiving mail.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "mail.h"

POP      *p_pop3;
SMTP     *p_smtp;
/*********************************************************************
 * @fn      WCHNET_ReplyMailBody
 *
 * @brief   Reply email body content.
 *
 * @return  none
 */
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

/*********************************************************************
 * @fn      WCHNET_AnalyseMailData
 *
 * @brief   Analyze incoming mails.
 *
 * @param   recv_buff - data buff.
 *
 * @return  none
 */
void WCHNET_AnalyseMailData( char *recv_buff )
{
     u16    i=0,j=0;

    if(p_pop3->ReceFlag){
        i = 0;
        if(p_pop3->EncodeType == 2) goto type2;                    //Jump to attachment receiving, encoding -- quoted-printable
        if(p_pop3->EncodeType == 1) goto type1;                    //Jump to attachment receiving, encoding --- base64/
        if(p_pop3->EncodeType == 0) goto type0;                    //Jump to attachment receiving, encoding -- others
    }
    for(i=0;i<ReceLen;i++){
        /* sender name/address */
        if( strncmp("\nFrom: ", &recv_buff[i], 7) == 0 ){
            i += 7;
#if DIALOG
    printf("1#From \n");
#endif
            if(recv_buff[i] == '"') i++;
            if(recv_buff[i] == '='){
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                i += 3;
                j = 0;
                memset( send_buff, '\0', strlen(send_buff) );
                /* sender name */
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
            else{                                                               //not encoded
                j = 0;
                while((recv_buff[i] != '"')&&(i < ReceLen)){                    //sender name
                    p_pop3->DecodeRName[j] = recv_buff[i];
                    j++;
                    i++;        
                }
            }                       
            while((recv_buff[i] != '<')&&(i < ReceLen)) i++;
            i++;
            j = 0;
            while((recv_buff[i] != '>')&&(i < ReceLen)){                        //sender address
                R_argv[0][j] = recv_buff[i];
                j++;
                i++; 
            }
        }

        if( strncmp("\nCc: ", &recv_buff[i], 5) == 0 ){
            i += 5;
#if DIALOG
    printf("2#Cc \n");
#endif        
            while(recv_buff[i] != '<')    i++;
            i++;
            j = 0;
            while(recv_buff[i] != '>'&&(i < ReceLen)){                          //CC address
                p_pop3->Ccname[j] = recv_buff[i];
                j++;
                i++;        
            }
        }

        if( strncmp("\nDate: ", &recv_buff[i], 7) == 0 ){
            i += 7;
#if DIALOG
    printf("3#date \n");
#endif
            j = 0;
            while((recv_buff[i] != '\r')&&(i < ReceLen)){                       //date sent
                p_pop3->sBufTime[j] = recv_buff[i];
                j++;
                i++;        
            }
        }

        if( strncmp("\nSubject: ", &recv_buff[i], 10) == 0 ){
            i += 10;
#if DIALOG
    printf("4#Subject \n");
#endif
            if(recv_buff[i] == '[') {
                while(recv_buff[i] == ']'&&(i < ReceLen)) i++;
                i += 2;
            }
            if(recv_buff[i] == '='){
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                i += 3;
                j = 0;
                memset( send_buff, '\0', strlen(send_buff) );

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
            else{
                j = 0;
                while(recv_buff[i] != '\r'&&(i < ReceLen)){
                    R_argv[1][j] = recv_buff[i];
                    j++;
                    i++;      
                }
            }
        }
        if( strncmp("name=", &recv_buff[i], 5) == 0 ){
#if DIALOG
    printf("5#\n");
#endif
            i += 5;
            while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)&&strncmp("filename=", &recv_buff[i], 9)&&(i < ReceLen)) i++;
            if(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27) == 0){

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
                while(strncmp("filename=", &recv_buff[i], 9)) i++;
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
#if DIALOG
    printf("6#filename \n");
#endif
                if(recv_buff[i] == '='){
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', strlen(send_buff) );

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
            }
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
                    memset(send_buff,'\0', strlen(send_buff) );
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
            while( strncmp("\n\r\n", &recv_buff[i], 3) != 0 &&(i < ReceLen)) i++;
            i += 3;
            if(p_pop3->EncodeType==1){    /* base64 */
                j = 0;
                memset(send_buff,'\0', strlen(send_buff) );
type1:          while((recv_buff[i] != '=')&&strncmp("\r\n--", &recv_buff[i], 4)&&strncmp("\r\n\r\n", &recv_buff[i], 4)&&(i < ReceLen)&&\
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
                memset(send_buff,'\0', strlen(send_buff));
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
    printf("addr:\n %s\n",R_argv[0]);
    printf("send name:\n %s\n",p_pop3->DecodeRName);
    printf("subject:\n %s\n",R_argv[1]);
    printf("attach name:\n%s\n",R_argv[2]);
    printf("send time:\n %s\n",p_pop3->sBufTime);
    printf("attach text:\n%s\n",AttachmentData);
#endif
        if(strncmp("wch", AttachmentData, 3) == 0) p_pop3->identitycheck = 1;
        else p_pop3->identitycheck = 0;
#ifdef    receive_over_reply
        WCHNET_SMTPInit(  );
#endif
    }
}

/*********************************************************************
 * @fn      strFind
 *
 * @brief   query for a specific string.
 *
 * @param   str  - source string.
 *          substr - String to be queried.
 *
 * @return  The number of data segments contained in the received data
 */
int strFind( char str[], char substr[] )
{
    int i, j, check ,count = 0;
    int len = strlen( str );
    int sublen = strlen( substr );
    for( i = 0; i < len; i++ )
    {
        check = 1;
        for( j = 0; j + i < len && j < sublen; j++ )
        {
            if( str[i + j] != substr[j] )
            {
                check = 0;
                break;
            }
        }
        if( check == 1 )
        {
            count++;
            i = i + sublen;
        }
    }
    return count;
}

/*********************************************************************
 * @fn      WCHNET_CheckResponse
 *
 * @brief   Verify handshake signals.
 *
 * @param   recv_buff  - data buff.
 *          check_type - check type.
 *
 * @return  state
 */
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
        case SMTP_CHECK_AUTH:
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = COMMAND_UNUSEFULL;
                return (CHECK_SUCCESS);
            }
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_USER;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_AUTH;
        case SMTP_CHECK_USER:
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_PASS;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_USER;
        case SMTP_CHECK_PASS:
            if(strncmp("235", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_MAIL;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_PASS;
        case SMTP_CHECK_MAIL:
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_RCPT;
                return (CHECK_SUCCESS);    
            } 
            return SMTP_ERR_MAIL;
        case SMTP_CHECK_RCPT:
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_DATA;
                return (CHECK_SUCCESS);            
            }
            return SMTP_ERR_RCPT;
        case SMTP_CHECK_DATA:
            if(strncmp("354", recv_buff, 3) == 0){
                OrderType = SMTP_DATA_OVER;
                return (CHECK_SUCCESS);
            } 
            return SMTP_ERR_DATA;
        case SMTP_CHECK_DATA_END:
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
        case SMTP_CHECK_QUIT:
            if(strncmp("220", recv_buff, 3) == 0||strncmp("221", recv_buff, 3) == 0){
                OrderType = SMTP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_QUIT;
#endif    // send_mail
#ifdef receive_mail
        case POP_CHECK_CNNT:
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_USER;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_CNNT;
        case POP_CHECK_USER:
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_PASS;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_USER;
        case POP_CHECK_PASS:
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_STAT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_PASS;
        case POP_CHECK_STAT:
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_LIST;
                return (CHECK_SUCCESS);    
            } 
            return POP_ERR_STAT;
        case POP_CHECK_LIST:
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
        case POP_CHECK_QUIT:
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_QUIT;
        case POP_CHECK_RETR:
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
                    p_pop3->AnalyMailDat = 1;
                    WCHNET_AnalyseMailData( recv_buff );
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
        case POP_CHECK_DELE:
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_DELE;
        case POP_CHECK_RSET:
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_RSET;
        case POP_CHECK_TOP:        // 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            }
            if(strncmp("Return", recv_buff, 6) == 0){
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_TOP;
        case POP_CHECK_UIDL:    //
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_UIDL;
#endif    // receive_mail
        default:
            return SMTP_ERR_UNKNOW;
    }
}

/*********************************************************************
 * @fn      WCHNET_MailQuery
 *
 * @brief   Status query.
 *
 * @return  None
 */
void WCHNET_MailQuery( void )
{
    if( p_pop3->DiscnntFlag == 1){
        p_pop3->DiscnntFlag = 0;
#ifdef    POP_REFRESH
        if(p_pop3->RefreshTime){
            p_pop3->RefreshTime = 0;
            Delay_Ms(200);
            WCHNET_CreateTcpPop3( );
        } 
#endif
    }
    if(ReceDatFlag){
#ifdef    POP_RTER                                                                                                               
        if(OrderType == POP_RECEIVE_RTER ){
            ReceDatFlag = 0;    
            WCHNET_POP3Retr( '2' );
        }
#endif
#ifdef    POP_DELE 
        if(OrderType == POP_RECEIVE_DELE ){
            ReceDatFlag = 0;    
            WCHNET_POP3Dele( '1' );
        }
#endif
#ifdef    POP_RSET 
        if(OrderType == POP_RECEIVE_RSET ){ 
            ReceDatFlag = 0;    
            WCHNET_POP3Rset(  );
        }
#endif
#ifdef    POP_TOP 
        if(OrderType == POP_RECEIVE_TOP ){
            ReceDatFlag = 0;    
            WCHNET_POP3Top( '1','3' );
        }
#endif
#ifdef    POP_UIDL 
        if(OrderType == POP_RECEIVE_UIDL ){
            ReceDatFlag = 0;    
            WCHNET_POP3Uidl( '1' );
        }
#endif
    }
}

/*********************************** endfile **********************************/
