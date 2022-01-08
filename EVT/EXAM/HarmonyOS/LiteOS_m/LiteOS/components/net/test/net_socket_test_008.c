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

#include "lwip_test.h"
#include "lwipopts.h"
#include <arch/sys_arch.h>
#include <lwip/sys.h>

#define SEND_TCP_COUNT 1000  // send count
#define STACK_PORT_8 2288
#define TEST_CASE 180

static char g_serverSendBuf[BUF_SIZE + 1] = { 0 };
static char g_clientRecvBuf[BUF_SIZE + 1] = { 0 };

static void InitMsgBuf()
{
    int i;
    if (g_serverSendBuf[0] != '\0') {
        return;
    }
    for (i = 0; i < BUF_SIZE; i++) {
        g_serverSendBuf[i] = 'S';
    }
    g_serverSendBuf[BUF_SIZE] = '\0';
}

static int SampleTcpServer()
{
    g_testCase++;
    int sfd, lsfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    int ret;
    int i;

    /* tcp server */
    lsfd = socket(AF_INET, SOCK_STREAM, 0);
    LogPrintln("create server socket inet stream: %d", lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(lsfd, -1, 1);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT_8);
    ret = bind(lsfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LogPrintln("bind socket %d to %s:%d: %d", lsfd, inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 2);

    ret = listen(lsfd, 0);
    LogPrintln("listen socket %d: %d", lsfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 3);

    sfd = accept(lsfd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    LogPrintln("accept socket %d: %d <%s:%d>", lsfd, sfd, inet_ntoa(clnAddr.sin_addr), ntohs(clnAddr.sin_port));
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 4);

    for (i = 0; i < SEND_TCP_COUNT; i++) {
        /* send */
        ret = send(sfd, g_serverSendBuf, BUF_SIZE, 0);
        ICUNIT_ASSERT_EQUAL(ret, BUF_SIZE, 5);
    }
    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 7);
    ret = closesocket(lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 8);
    return 0;
}

static int SampleTcpClient()
{
    g_testCase++;
    int sfd;
    struct sockaddr_in srvAddr = { 0 };
    int ret;
    int i;
    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);
    int recvCount = 0;

    /* tcp client connection */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    LogPrintln("create client socket inet stream: %d", sfd);
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 7);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    srvAddr.sin_port = htons(STACK_PORT_8);
    ret = connect(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LogPrintln("connect socket %d to %s:%d: %d", sfd, inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 8);

    /* test getpeername */
    ret = getpeername(sfd, &addr, &addrLen);
    LogPrintln("getpeername %d %s:%d: %d",
        sfd, inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr), ntohs(((struct sockaddr_in*)&addr)->sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 9);
    ICUNIT_ASSERT_EQUAL(addrLen, sizeof(struct sockaddr_in), 10);
    ICUNIT_ASSERT_EQUAL(((struct sockaddr_in*)&addr)->sin_addr.s_addr, \
        inet_addr(PEER_IP), 11);

    /* test getsockname */
    ret = getsockname(sfd, &addr, &addrLen);
    LogPrintln("getsockname %d %s:%d: %d",
        sfd, inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr), ntohs(((struct sockaddr_in*)&addr)->sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 12);
    ICUNIT_ASSERT_EQUAL(addrLen, sizeof(struct sockaddr_in), 13);
    ICUNIT_ASSERT_EQUAL(((struct sockaddr_in*)&addr)->sin_addr.s_addr, \
        inet_addr(STACK_IP), 14);

    LogPrintln("tcp begin recv");
    for (i = 0; i < 2 * SEND_TCP_COUNT; i++) {
        /* recv */
        (void)memset_s(g_clientRecvBuf, sizeof(g_clientRecvBuf), 0, sizeof(g_clientRecvBuf));
        ret = recv(sfd, g_clientRecvBuf, BUF_SIZE, 0);
        recvCount += ret;
        if (recvCount >= BUF_SIZE * SEND_TCP_COUNT) {
            LogPrintln("client recv on socket %d: %d, i = %d", sfd, recvCount, i);
            break;
        }
    }
    ICUNIT_ASSERT_EQUAL(recvCount, BUF_SIZE * SEND_TCP_COUNT, recvCount);
    LogPrintln("tcp end recv");
    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 17);
    return 0;
}

static void TcpServerRoutine(void *p)
{
    (void)p;
    (void)SampleTcpServer();
}

static void TcpClientRoutine(void *p)
{
    (void)p;
    (void)SampleTcpClient();
}

void TcpTestMore()
{
    LogPrintln("net_socket_test_008.c enter");
    if (g_testCase < TEST_CASE) {
        g_testCase = TEST_CASE;
    }

    int ret;
    InitMsgBuf();
    ret = sys_thread_new("tcp_server_more", TcpServerRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 17);

    ret = sys_thread_new("tcp_client_more", TcpClientRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 18);
}
