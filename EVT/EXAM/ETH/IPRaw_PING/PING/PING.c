/********************************** (C) COPYRIGHT *******************************
* File Name          : PING.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/06/11
* Description        : ping related functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "PING.h"

u8 sendErrCnt;
u8 unreachCnt;
u8 timeoutCnt;
u8 sucRecCnt;
u8 sendBuf[100];
u8 ICMPCnt = 0;
u8 ICMPSeq = 0;
u8 ICMPSuc = 0;
u8 ICMPTmpVal = 0;
u32 timeCnt;
extern u8 SocketId;
/*********************************************************************
 * @fn      InitParameter
 *
 * @brief   Initializes parameters.
 *
 * @return  none
 */
void InitParameter(void)
{
    unreachCnt = 0;
    timeoutCnt = 0;
    sucRecCnt = 0;
    ICMPSuc = ICMP_KEEP_NO;
}

/*********************************************************************
 * @fn      InitPING
 *
 * @brief   Initializes PING.
 *
 * @return  none
 */
void InitPING(void)
{
    IcmpHeader head;
    u32 check_sum = 0;
    u8 i;

    ICMPCnt++;
    ICMPSeq += 1;
    head.i_type = ICMP_HEAD_TYPE;
    head.i_code = ICMP_HEAD_CODE;
    head.i_id = ICMP_HEAD_ID;
    head.i_seq = ICMP_HEAD_SEQ + ICMPSeq;
    memset(head.i_data, 0, sizeof(head.i_data));
    for(i = 0; i < ICMP_DATA_BYTES; i++) {
        if(i < 26)
            head.i_data[i] = i + 'a';
        else
            head.i_data[i] = i + 'a' - 26;
        if(i % 2 == 0)
            check_sum += head.i_data[i] << 8;
        else
            check_sum += head.i_data[i];
    }
    check_sum += head.i_type << 8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    head.i_cksum = check_sum >> 16;
    head.i_cksum += check_sum & 0xffff;
    head.i_cksum = 0xffff - head.i_cksum;
    memset(sendBuf, 0, sizeof(sendBuf));
    memcpy(sendBuf, &head, sizeof(head));
    sendBuf[2] = head.i_cksum >> 8;
    sendBuf[3] = head.i_cksum & 0xff;
    sendBuf[4] = head.i_id >> 8;
    sendBuf[5] = head.i_id & 0xff;
    sendBuf[6] = head.i_seq >> 8;
    sendBuf[7] = head.i_seq & 0xff;
}

/*********************************************************************
 * @fn      Respond_PING
 *
 * @brief   Respond to PING request.
 *
 * @param   pDat - ACK data.
 *
 * @return  none
 */
void Respond_PING(u8 *pDat)
{
    IcmpHeader head;
    u32 check_sum = 0;
    u8 i;

    head.i_type = ICMP_HEAD_REPLY;
    head.i_code = pDat[1];
    head.i_id = (pDat[4] << 8) + pDat[5];
    head.i_seq = (pDat[6] << 8) + pDat[7];

    check_sum += head.i_type << 8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    for (i = 0; i < 32; i++) {
        head.i_data[i] = pDat[i + 8];
        if (i % 2 == 0)
            check_sum += head.i_data[i] << 8;
        else
            check_sum += head.i_data[i];
    }
    head.i_cksum = check_sum >> 16;
    head.i_cksum += check_sum & 0xffff;
    head.i_cksum = 0xffff - head.i_cksum;
    memset(sendBuf, 0, sizeof(sendBuf));
    memcpy(sendBuf, &head, sizeof(head));
    sendBuf[2] = head.i_cksum >> 8;
    sendBuf[3] = head.i_cksum & 0xff;
    sendBuf[4] = head.i_id >> 8;
    sendBuf[5] = head.i_id & 0xff;
    sendBuf[6] = head.i_seq >> 8;
    sendBuf[7] = head.i_seq & 0xff;
}

/*********************************************************************
 * @fn      WCHNET_ICMPRecvData
 *
 * @brief   receive and parse data.
 *
 * @param   len - data length.
 *          pDat - data buff
 *
 * @return  none
 */
void WCHNET_ICMPRecvData(u32 len, u8 *pDat)
{
    u16 tmp = 0;

    ICMPTmpVal = ICMPSuc;
    ICMPSuc = 3;
    if (len == 40) {
        if (pDat[0] == ICMP_HEAD_REPLY) {
            if (pDat[1] == ICMP_HEAD_CODE) {
                tmp = pDat[4];
                tmp = tmp << 8;
                tmp += pDat[5];
                if (tmp == ICMP_HEAD_ID) {
                    tmp = pDat[6];
                    tmp = (tmp << 8);
                    tmp += pDat[7] - ICMPSeq;
                    if (tmp == ICMP_HEAD_SEQ) {
                        ICMPSuc = ICMP_RECV_SUC;
                    }
                }
            }
        }
        if (pDat[0] == ICMP_HEAD_TYPE) {
            if (pDat[1] == ICMP_HEAD_CODE) {
                Respond_PING(pDat);
                ICMPSuc = ICMP_REPLY;
            }
        }
    } else {
        if (pDat[0] == 3) {
            if (pDat[1] == 1) {
                ICMPSuc = ICMP_UNRECH;
            }
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_PINGSendData
 *
 * @brief   send PING command.
 *
 * @param   PSend - data buff.
 *          Len - data length
 *          id - socket id
 *
 * @return  none
 */
void WCHNET_PINGSendData( u8 *PSend, u32 Len,u8 id )
{
    u32 length;
    u8 i,count=0;

    length = Len;
    count = 0;

    while(1){
        Len = length;
        i = WCHNET_SocketSend(id,PSend,&Len);
        mStopIfError(i);
        if(Len == 0){
            count++;
            if(count>2){
                return;
            }
        }
        length -= Len;
        PSend += Len;                             //offset buffer pointer
        if(length)continue;                       //If the data is not sent, continue to send
        break;
    }
    ICMPSuc++;
}

/*********************************************************************
 * @fn      WCHNET_PINGCmd
 *
 * @brief   Query status and execute corresponding commands.
 *
 * @return  none
 */
void WCHNET_PINGCmd(void)
{
    if (ICMPSuc < ICMP_KEEP_NO) {
        switch (ICMPSuc) {
        case ICMP_SOKE_CON:
            ICMPSuc = 1;
            timeCnt = 0;
            WCHNET_PINGSendData(sendBuf, 40, SocketId);
            printf("Ping %d.%d.%d.%d with %d bytes of data.\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3], (u16) ICMP_DATA_BYTES);
            break;
        case ICMP_SEND_ERR:
            if (sendErrCnt > 10) {
                ICMPSuc = ICMP_KEEP_NO;
                sendErrCnt = 0;
            }
            if (timeCnt > 250) {
                printf("send data fail!\n");
                timeCnt = 0;
                sendErrCnt++;
                WCHNET_PINGSendData(sendBuf, 40, SocketId);
            }
            break;
        case ICMP_SEND_SUC:
            if (timeCnt > 100) {
                printf("Request timeout.\n");
                timeoutCnt++;
                if (ICMPCnt < PING_SEND_CNT) {
                    ICMPSuc = 1;
                    InitPING();
                    timeCnt = 0;
                    WCHNET_PINGSendData(sendBuf, 40, SocketId);
                } else {
                    printf("PING end!\r\n");
                    printf("send = %d Received = %d", PING_SEND_CNT, sucRecCnt);
                    ICMPSuc = ICMP_KEEP_NO;
                }
            }
            break;
        case ICMP_RECV_ERR:                                               // error unknown
            printf("receive unknown data.\r\n");
            ICMPSuc = ICMP_KEEP_NO;
            break;
        case ICMP_RECV_SUC:                                               // success
            printf("Reply from %d.%d.%d.%d: bytes=%d time<4ms\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3], (u16) ICMP_DATA_BYTES);
            sucRecCnt++;
            if (ICMPCnt < PING_SEND_CNT) {
                ICMPSuc = 1;
                InitPING();
                timeCnt = 0;
                WCHNET_PINGSendData(sendBuf, 40, SocketId);
            } else {
                printf(
                        "Ping statistics for %d.%d.%d.%d:\r\nPackets: Sent = %d,Received = %d,Lost = %d<%d%% loss>.\r\n",
                        (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                        (u16) DESIP[3], PING_SEND_CNT, (u16) sucRecCnt,
                        (u16) (PING_SEND_CNT - sucRecCnt),
                        (u16) ((PING_SEND_CNT - sucRecCnt) * 100 / PING_SEND_CNT));
                ICMPSuc = ICMP_KEEP_NO;
            }
            break;
        case ICMP_UNRECH:                                                         // unreachable
            printf("Reply from %d.%d.%d.%d: Destination host unreachable.\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3]);
            unreachCnt++;
            if (ICMPCnt < PING_SEND_CNT) {
                ICMPSuc = 1;
                InitPING();
                timeCnt = 0;
                WCHNET_PINGSendData(sendBuf, 40, SocketId);
            } else {
                printf("PING end!\r\n");
                printf("send = %d Received = %d", PING_SEND_CNT, sucRecCnt);
                ICMPSuc = ICMP_KEEP_NO;
            }
            break;
        case ICMP_REPLY:
            WCHNET_PINGSendData(sendBuf, 40, SocketId);
            break;
        case ICMP_REPLY_SUC:
            printf("Reply ping.\r\n");
            ICMPSuc = ICMPTmpVal;
            break;
        case ICMP_KEEP_NO:
            break;
        default:
            break;
        }
    }
}

/*********************************** endfile **********************************/
