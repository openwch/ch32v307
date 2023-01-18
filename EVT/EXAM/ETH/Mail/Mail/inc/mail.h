/********************************** (C) COPYRIGHT *******************************
* File Name          : mail.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/18
* Description        : Definition for mail.c.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __MAIL_H__
#define __MAIL_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "wchnet.h"
#include "eth_driver.h"
#include "mailcmd.h"

/* ********************************************************************************************************************
* work mode:
*
*  1  Only send mail, log out after sending and close the socket connection;
*  2  Only receive mail, log out and close the socket connection after receiving
*     [Get mail list];
*  3  Only receive email, log out after receiving [delete after reading emails]
*     and close the socket connection;
*  4  Send email and receive email, start receiving email after sending, and
*     log out after receiving them;
*  5  Send email and receive email, reply email after receiving them, exit
*     SMTP after sending them, and exit POP after deleting email.
*  Description: This code receives and sends two independent socket connections;
***********************************************************************************************************************/
#ifndef    mail_work_mode
    #define  mail_work_mode             1
#endif
#if   (mail_work_mode == 1)
    #define send_mail                   1         // send email
    #define    send_over_quit           1         // send completed and sign out
#elif (mail_work_mode == 2)
    #define receive_mail                1         // receive email
    #define    receive_over_quit        1         // Exit after reading mail
#elif (mail_work_mode == 3)
    #define receive_mail                1         // receive email
    #define    receive_dele_quit        1         // Delete and log out after reading the message
#elif (mail_work_mode == 4)
    #define send_mail                   1         // send email
    #define send_over_receive           1         // send completed and reading mail
    #define receive_mail                0         // receive email,According to the program flow, here should be set to 0
    #define receive_over_quit           1         // Exit after reading mail
#elif (mail_work_mode == 5)
    #define receive_mail                1         // receive email
    #define receive_over_reply          1         // Reply to mails after reading them
    #define send_mail                   0         // send email,According to the program flow, here should be set to 0
    #define send_over_quit              1         // send completed and sign out
#endif

//================================================================================
extern  u8 OrderType;                             //Command type
extern  u8 CheckType;
extern  u8 ReceDatFlag;                           //received data flag
extern  u16 ReceLen;
//=================================================================================
#define attach_max_len           512              //The maximum length of the attachment to send, the default is 512
#define POP3_SERVER_PORT         110              //The destination port  for receiving mails
#define SMTP_SERVER_PORT         25               //SMTP port
#define DIALOG                    0               //1 print debug information 0 close debug information
#define POP3_LOG_EN               1               //Output the obtained file on the serial port

typedef struct
{
    u8 Socket;
    u8 g_MIME;                                    //Attachment flag
    char  m_strFile[48];                          //attachment name
    char  m_strSendFrom[48];                      //sender address
    char  m_strSendTo[48];                        //receiver's address
    char  m_strSMTPServer[32];                    //server name
    char  m_strUSERID[32];                        //user name
    char  m_strPASSWD[32];                        //password
    char  m_strSubject[32];                       //subject
    char  m_strSenderName[32];                    //sender name
}SMTP;

typedef struct
{
    u8 Socket;
    u8 EncodeType;                                //The encoding used by the mail
    u8 AnalyMailDat;                              //Analyze the end-of-mail flag
    u8 identitycheck;                             //Verify Attachment Content Flags
    u8 RefreshTime;                               //No mail found, continue search flag
    u8 DiscnntFlag;
    u8 ReceFlag;
    char  sBufTime[40];                           //Save the time of receiving emails
    char  Ccname[48];                             //CC name
    char  DecodeRName[32];                        //Save the sender's name after the received email is decoded
    char  pPop3Server[32];                        //POP server
    char  pPop3UserName[32];                      //POP login user name
    char  pPop3PassWd[32];                        //POP login password
}POP;

/* SMTP send email related parameters */
extern const char *m_Server;
extern const char *m_UserName;
extern const char *m_PassWord;
extern const char *m_SendFrom;
extern const char *m_SendName;
extern const char *m_SendTo;
extern const char *m_Subject;
extern const char *m_FileName;
/* pop receive mail related*/
extern const char *p_Server;
extern const char *p_UserName;
extern const char *p_PassWord;

extern u8 MACAddr[6];
extern char     AttachmentData[attach_max_len];
extern POP      *p_pop3;
extern SMTP     *p_smtp;
extern char     MailBodyData[128];

extern const u8  SocketSMTPIP[4];
extern const u16  SmtpSourPrt;

extern const u8  SocketPOP3IP[4];
extern const u16  Pop3SourPrt;

/************************************************************************************************************************
* POP optional command
************************************************************************************************************************/
#ifdef receive_mail 
    #define POP_RTER        1            // 1 Process the full text of the server's mail
    #define POP_DELE        1            // 2 Mark for deletion
//    #define POP_RSET      1            // 3 Cancel all DELE commands
//    #define POP_TOP       1            // 4 Returns the first m lines of mail number n
//    #define POP_UIDL      1            // 5 Returns the specified mail, if not specified, returns all.
    #define POP_REFRESH     1            // Email not received continue query sign
#endif

#define g_strBoundary    "18ac0781-9ae4-4a2a-b5f7-5479635efb6b"                      //boundary
#define g_strEncode      "base64"                                                    //Encoding
#define g_strcharset     "gb2312"                                                    //windows format (linux format "utf-8")
#define g_xMailer        "X-Mailer: X-WCH-Mail Client Sender\r\n"                    //X-Mailer Content
#define g_Encoding       "Content-Transfer-Encoding: quoted-printable\r\nReply-To: " //Encoding content
#define g_Custom         "X-Program: CSMTPMessageTester"                             //X-Program Content
#define g_FormatMail     "This is a multi-part message in MIME format."
#define g_AttachHead     "\r\nContent-Transfer-Encoding: quoted-printable\r\n" 
#define g_AttachHedType  "text/plain"
#define g_MailHedType    "multipart/mixed"

#ifdef    receive_over_reply
extern const    char *g_mailbody;
extern const    char *g_autograph1;
extern const    char *g_autograph2;
extern char     MacAddr[6];
extern char     MacAddrC[18];
#endif
/************************************************************************************************************************
* command code
* 0x01-0x0f Command code for receiving mail
****************** 0x01-0x07 Common commands
****************** 0x09-0x0e optional command
****************** 0x01-0x02-0x03-0x06 confirm status
****************** 0x04-0x05 0x09-0x0e operating state
* 0x10-0x1f send mail command code
****************** 0x10-0x17 need to be executed in order
****************** The latter is optional
* 0x00         do nothing
************************************************************************************************************************/
#define COMMAND_UNUSEFULL       0x00
//POP RECEIVE CODE
#define POP_RECEIVE_USER        0x01    //Authenticated user
#define POP_RECEIVE_PASS        0x02    //Authenticated password
#define POP_RECEIVE_STAT        0x03    //Mailbox Statistics
#define POP_RECEIVE_LIST        0x04    //Returns the size of the specified message
#define POP_RECEIVE_RTER        0x05    //Read the entire text of an email
#define POP_RECEIVE_QUIT        0x06    //sign out
#define POP_CLOSE_SOCKET        0x07    //close pop socket
#define POP_RECEIVE_START       0x08    //Start receiving mail
#define POP_RECEIVE_DELE        0x09    //Mark for deletion
#define POP_RECEIVE_RSET        0x0a    //Cancel all DELE commands
#define POP_RECEIVE_TOP         0x0b    //Returns the content of the first m lines of mail number n
#define POP_RECEIVE_UIDL        0x0e    //Returns the unique identifier for the specified message
#define POP_ERR_CHECK           0x0F    //POP handshake error, log out and close socket
//SMTP SEND CODE
#define SMTP_SEND_HELO          0x10    //send hello command
#define SMTP_SEND_AUTH          0x11    //Send login command
#define SMTP_SEND_USER          0x12    //send user name
#define SMTP_SEND_PASS          0x13    //send password
#define SMTP_SEND_MAIL          0x14    //Send sender address
#define SMTP_SEND_RCPT          0x15    //Send receiver address
#define SMTP_SEND_DATA          0x16    //send DATA command
#define SMTP_DATA_OVER          0x17    //Send email content
#define SMTP_SEND_QUIT          0x18    //sign out
#define SMTP_CLOSE_SOCKET       0x19    //close SMTP socket
#define SMTP_SEND_START         0x1E    //Start sending mail
#define SMTP_ERR_CHECK          0x1F    //SMTP handshake error, log out and close socket
/* *********************************************************************************************************************
* �����źź˶Դ������
************************************************************************************************************************/
//POP ERR CHECK CODE
#define POP_ERR_CNNT            0x20     //pop connection error
#define POP_ERR_USER            0x21     //Authentication user name error
#define POP_ERR_PASS            0x22     //Login failed error
#define POP_ERR_STAT            0x23     //Getting email information error
#define POP_ERR_LIST            0x24     //Getting email list information error
#define POP_ERR_RETR            0x25     //Read the specified mail number information error
#define POP_ERR_DELE            0x26     //Deleting a message failed error
#define POP_ERR_QUIT            0x59     //Logout failed
#define POP_ERR_RSET            0x28     //recover deleted messages error
#define POP_ERR_TOP             0x2B     //Failed to read the first m lines of an email
#define POP_ERR_UIDL            0x2C     //Failed to get an email ID
#define POP_ERR_UNKW            0x2F     //pop error unknown
//SMTP ERR CHECK CODE
#define SMTP_ERR_CNNT           0x31     //SMTP connection error
#define SMTP_ERR_HELO           0x32     //Failed to send HELO command
#define SMTP_ERR_AUTH           0x33     //Failed to request login command
#define SMTP_ERR_USER           0x34     //Sending user name not recognized
#define SMTP_ERR_PASS           0x35     //failed to login
#define SMTP_ERR_MAIL           0x36     //Sender address sending failed
#define SMTP_ERR_RCPT           0x37     //receiving address sending failed
#define SMTP_ERR_DATA           0x38     //request to send data
#define SMTP_ERR_DATA_END       0x39     //sending data error
#define SMTP_ERR_QUIT           0x3A     //Logout failed
#define SMTP_ERR_UNKNOW         0x3F     //SMTP unknown error
//
#define send_data_timeout       0x10     //send data timeout
#define send_data_success       0x14     //Send data successfully

#define CHECK_SUCCESS             0x00
#define UNCHECK                   0x00
//POP CHECK CODE
// Check the response information, return "+OK" correctly, return "-ERR" if wrong.
#define POP_CHECK_CNNT            0x50    
#define POP_CHECK_USER            0x51    
#define POP_CHECK_PASS            0x52    
#define POP_CHECK_STAT            0x53
#define POP_CHECK_LIST            0x54    
#define POP_CHECK_RETR            0x55
#define POP_CHECK_DELE            0x56
#define POP_CHECK_SEAR            0x57
#define POP_CHECK_QUIT            0x58
#define POP_CHECK_RSET            0x59
#define POP_CHECK_APOP            0x5A
#define POP_CHECK_TOP             0x5B
#define POP_CHECK_UIDL            0x5C
//SMTP CHECK CODE
#define SMTP_CHECK_CNNT           0x60
#define SMTP_CHECK_HELO           0x61
#define SMTP_CHECK_AUTH           0x62
#define SMTP_CHECK_USER           0x63
#define SMTP_CHECK_PASS           0x64
#define SMTP_CHECK_MAIL           0x65
#define SMTP_CHECK_RCPT           0x66
#define SMTP_CHECK_DATA           0x67
#define SMTP_CHECK_DATA_END       0x68
#define SMTP_CHECK_QUIT           0x69
/******************************************************************************/
void WCHNET_SendData( char *PSend, u32 Len,u8 type,u8 index  );

u8 WCHNET_CheckResponse( char *recv_buff,u8 check_type );

void WCHNET_CreateTcpPop3( void );

void WCHNET_CreateTcpSmtp( void );

void WCHNET_ReplyMailBody( void );

void WCHNET_MailQuery( void );

void mStopIfError(u8 iError);

void WCHNET_HandleGlobalInt( void );

//================================================================================
#endif
