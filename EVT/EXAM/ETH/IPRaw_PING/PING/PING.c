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
/* Two distinct roles, two distinct buffers: reqBuf holds the Echo Request of
 * this node (built by InitPING()), rspBuf holds the Echo Reply to a third
 * party's request (built by Respond_PING()). A single shared buffer would let
 * an incoming request overwrite the request that is currently in flight, so
 * the next (re)transmission would carry someone else's sequence number. */
u8 reqBuf[100];
u8 rspBuf[100];
u8 ICMPCnt = 0;
u8 ICMPSeq = 0;
u8 ICMPSuc = 0;
u8 ICMPTmpVal = 0;
volatile u32 timeCnt;
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
    while(check_sum >> 16)
        check_sum = (check_sum & 0xffff) + (check_sum >> 16);
    head.i_cksum = 0xffff - (u16)check_sum;
    memset(reqBuf, 0, sizeof(reqBuf));
    memcpy(reqBuf, &head, sizeof(head));
    reqBuf[2] = head.i_cksum >> 8;
    reqBuf[3] = head.i_cksum & 0xff;
    reqBuf[4] = head.i_id >> 8;
    reqBuf[5] = head.i_id & 0xff;
    reqBuf[6] = head.i_seq >> 8;
    reqBuf[7] = head.i_seq & 0xff;
}

/*********************************************************************
 * @fn      Respond_PING
 *
 * @brief   Respond to PING request.
 *
 * @param   pDat - ACK data.
 *          len - length of pDat.
 *
 * @return  none
 *
 * @note    The Echo Reply is assembled in the global rspBuf, which is
 *          deliberately kept apart from reqBuf (the Echo Request buffer filled
 *          by InitPING()): answering a third party must not disturb the
 *          request of this node that may still be on its way out.
 */
void Respond_PING(u8 *pDat, u32 len)
{
    IcmpHeader head;
    u32 check_sum = 0;
    u8 i;

    /* The identifier, the sequence number and the data area are read from the
     * request below, so a complete message has to be handed over. */
    if((pDat == NULL) || (len < sizeof(IcmpHeader)))
        return;

    head.i_type = ICMP_HEAD_REPLY;
    head.i_code = pDat[1];
    head.i_id = (pDat[4] << 8) + pDat[5];
    head.i_seq = (pDat[6] << 8) + pDat[7];

    check_sum += head.i_type << 8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    for (i = 0; i < ICMP_DATA_BYTES; i++) {
        head.i_data[i] = pDat[i + 8];
        if (i % 2 == 0)
            check_sum += head.i_data[i] << 8;
        else
            check_sum += head.i_data[i];
    }
    /* Fold the carries until the sum fits into 16 bits: a single fold is not
     * enough when the low half is close to 0xffff. */
    while(check_sum >> 16)
        check_sum = (check_sum & 0xffff) + (check_sum >> 16);
    head.i_cksum = 0xffff - (u16)check_sum;
    memset(rspBuf, 0, sizeof(rspBuf));
    memcpy(rspBuf, &head, sizeof(head));
    rspBuf[2] = head.i_cksum >> 8;
    rspBuf[3] = head.i_cksum & 0xff;
    rspBuf[4] = head.i_id >> 8;
    rspBuf[5] = head.i_id & 0xff;
    rspBuf[6] = head.i_seq >> 8;
    rspBuf[7] = head.i_seq & 0xff;
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

    /* The type, the code and, for a full message, the identifier and the
     * sequence number are read below, so the 8 byte ICMP header must be
     * present before anything else is done. */
    if((pDat == NULL) || (len < 8))
        return;

    ICMPTmpVal = ICMPSuc;
    ICMPSuc = ICMP_RECV_ERR;
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
                Respond_PING(pDat, len);
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
 * @return  WCHNET_ERR_SUCCESS if the whole message has been handed over to the
 *          stack, the error code of WCHNET_SocketSend() if the stack rejected
 *          it, WCHNET_ERR_BUF if the stack took no data at all after three
 *          attempts. The caller is responsible for the state transition.
 */
u8 WCHNET_PINGSendData( u8 *PSend, u32 Len,u8 id )
{
    u32 length;
    u32 sendLen;
    u8 i,count=0;

    length = Len;
    count = 0;

    while(1){
        /* Len is an in/out parameter: the library reports back how much of the
         * message it accepted. Whether it also clears Len when it fails is
         * undocumented, so the outcome is taken from the return value. */
        sendLen = length;
        i = WCHNET_SocketSend(id,PSend,&sendLen);
        mStopIfError(i);
        if(i != WCHNET_ERR_SUCCESS){
            return i;
        }
        if(sendLen == 0){
            count++;
            if(count>2){
                /* The stack kept refusing the data without taking any of it;
                 * report the failure instead of spinning here so that the
                 * caller can schedule the retry. */
                return WCHNET_ERR_BUF;
            }
        }
        length -= sendLen;
        PSend += sendLen;                         //offset buffer pointer
        if(length)continue;                       //If the data is not sent, continue to send
        break;
    }
    return WCHNET_ERR_SUCCESS;
}

/*********************************************************************
 * @fn      PING_SendRequest
 *
 * @brief   Hands the pending Echo Request (reqBuf) over to the stack and moves
 *          the state machine to the successor state implied by the result.
 *          Nothing is built here: the message is either the one InitPING()
 *          prepared, or the one still waiting to be retransmitted.
 *
 * @return  none
 */
static void PING_SendRequest(void)
{
    timeCnt = 0;
    if (WCHNET_PINGSendData(reqBuf, sizeof(IcmpHeader), SocketId) == WCHNET_ERR_SUCCESS)
        ICMPSuc = ICMP_SEND_SUC;                  //handed over, the 1 s timeout starts
    else
        ICMPSuc = ICMP_SEND_ERR;                  //retry after 2.5 s, bounded by sendErrCnt
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
            PING_SendRequest();
            printf("Ping %d.%d.%d.%d with %d bytes of data.\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3], (u16) ICMP_DATA_BYTES);
            break;
        case ICMP_SEND_ERR:
            if (sendErrCnt > 10) {
                ICMPSuc = ICMP_KEEP_NO;
                sendErrCnt = 0;
            }
            else if (timeCnt > 250) {
                printf("send data fail!\r\n");
                sendErrCnt++;
                PING_SendRequest();
            }
            break;
        case ICMP_SEND_SUC:
            if (timeCnt > 100) {
                printf("Request timeout.\r\n");
                timeoutCnt++;
                if (ICMPCnt < PING_SEND_CNT) {
                    InitPING();
                    PING_SendRequest();
                } else {
                    printf("PING end!\r\n");
                    printf("send = %d Received = %d\r\n", (u16) PING_SEND_CNT, (u16) sucRecCnt);
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
                InitPING();
                PING_SendRequest();
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
                InitPING();
                PING_SendRequest();
            } else {
                printf("PING end!\r\n");
                printf("send = %d Received = %d\r\n", (u16) PING_SEND_CNT, (u16) sucRecCnt);
                ICMPSuc = ICMP_KEEP_NO;
            }
            break;
        case ICMP_REPLY:
            /* Send the Echo Reply that Respond_PING() left in rspBuf, which is a
             * different buffer than the one PING_SendRequest() sends from. A
             * rejected reply is dropped and the previous state restored right
             * away, so this branch cannot be re-entered on every main loop round. */
            if (WCHNET_PINGSendData(rspBuf, sizeof(IcmpHeader), SocketId) == WCHNET_ERR_SUCCESS)
                ICMPSuc = ICMP_REPLY_SUC;
            else
                ICMPSuc = ICMPTmpVal;
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
