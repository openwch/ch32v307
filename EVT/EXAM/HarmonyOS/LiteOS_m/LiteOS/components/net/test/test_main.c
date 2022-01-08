/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sys/socket.h"
#include "hos_cmsis_adp.h"

#define LWIP_TEST_TIMEOUT 1000 // timeout 1s
#define LWIP_TEST_COUNT_FLAG 100

#define SOCKET_TEST     310
#define UDP_TEST        320
#define TCP_TEST        330
#define OPT_TEST        340
#define ORDER_TEST      350
#define INET_TEST       360
#define UDP_MORE_TEST   370
#define TCP_MORE_TEST   380
#define TCP_SELECT_TEST 390
#define TCP_POLL_TEST   400
#define NETIF_TEST      410
#define TCP_DUP_TEST    420
#define TCP_LONG_TEST   430

int g_lwipTimerCount = 0;
int g_testCase = LWIP_TEST_COUNT_FLAG;
int g_testError = 0;
int g_testTemp = LWIP_TEST_COUNT_FLAG;
extern void abort(void);
extern int SocketTest(void);
extern int UdpTest(void);
extern void TcpTest();
extern int SockOptTest();
extern int ByteOrderTest(void);
extern int InetTest();
extern int UdpTestMore(void);
extern void TcpTestMore();
extern void TcpTestSelect();
extern void TcpTestPoll();
extern int UdpTestNetif(void);
extern void TcpTestDup();
extern void TcpTestLong();

// time out 1s
static void LwipTestTimeoutCallback(void const *argument)
{
    g_lwipTimerCount++;
    switch (g_lwipTimerCount) {
        case SOCKET_TEST:
            SocketTest();
            break;
        case UDP_TEST:
            UdpTest();
            break;
        case TCP_TEST:
            TcpTest();
            break;
        case OPT_TEST:
            SockOptTest();
            break;
        case ORDER_TEST:
            ByteOrderTest();
            break;
        case INET_TEST:
            InetTest();
            break;
        case UDP_MORE_TEST:
            UdpTestMore();
            break;
        case TCP_MORE_TEST:
            TcpTestMore();
            break;
        case TCP_SELECT_TEST:
            TcpTestSelect();
            break;
        case TCP_POLL_TEST:
            TcpTestPoll();
            break;
        case NETIF_TEST:
            UdpTestNetif();
            break;
        case TCP_DUP_TEST:
            TcpTestDup();
            break;
        case TCP_LONG_TEST:
            TcpTestLong();
            break;
        default:
          break;
    }
}

osTimerId_t g_lwipTestTimerId = NULL;

void LwipTestStartTimer(uint32_t timeout)
{
    osStatus_t status;
    if (g_lwipTestTimerId != NULL) {
        status = osTimerStart(g_lwipTestTimerId, timeout);
        if (status != osOK) {
            return;
        }
    } else {
        g_lwipTestTimerId = osTimerNew((osTimerFunc_t)LwipTestTimeoutCallback, osTimerPeriodic, NULL, NULL);
        if (g_lwipTestTimerId == NULL) {
            return;
        }
        status = osTimerStart(g_lwipTestTimerId, timeout);
        if (status != osOK) {
            return;
        }
    }
}

void LwipTestEnter()
{
    LwipTestStartTimer(LWIP_TEST_TIMEOUT);
}
