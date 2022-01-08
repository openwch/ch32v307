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

#define TEST_CASE 230
#define STACK_PORT_TCP_LONG 2231
#define TCP_LONG_BUF_SIZE (2 * 1024)

static int SampleTcpServer()
{
    int sfd, lsfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    int ret;
    static char serverBuf[TCP_LONG_BUF_SIZE] = {0};

    /* tcp server */
    lsfd = socket(AF_INET, SOCK_STREAM, 0);
    ICUNIT_ASSERT_NOT_EQUAL(lsfd, -1, 1);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT_TCP_LONG);
    ret = bind(lsfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    ICUNIT_ASSERT_EQUAL(ret, 0, 2);

    ret = listen(lsfd, 0);
    ICUNIT_ASSERT_EQUAL(ret, 0, 3);

    sfd = accept(lsfd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 4);

    /* send */
    ret = send(sfd, serverBuf, TCP_LONG_BUF_SIZE, 0);
    ICUNIT_ASSERT_EQUAL(ret, TCP_LONG_BUF_SIZE, 6);

    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 7);
    ret = closesocket(lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 8);

    return 0;
}

static int SampleTcpClient()
{
    int sfd;
    struct sockaddr_in srvAddr = { 0 };
    int ret, i;
    int recvCount = 0;
    static char clientBuf[TCP_LONG_BUF_SIZE] = {0};

    /* tcp client connection */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 9);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    srvAddr.sin_port = htons(STACK_PORT_TCP_LONG);
    ret = connect(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    ICUNIT_ASSERT_EQUAL(ret, 0, 10);

    /* recv */
    for (i = 0; i < TCP_LONG_BUF_SIZE; i++) {
        ret = recv(sfd, clientBuf, TCP_LONG_BUF_SIZE, 0);
        recvCount += ret;
        if (recvCount >= TCP_LONG_BUF_SIZE) {
            LogPrintln("client recv on socket %d: %d, i = %d", sfd, recvCount, i);
            break;
        }
    }
    ICUNIT_ASSERT_NOT_EQUAL(i, TCP_LONG_BUF_SIZE, 12);
    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 13);
    return 0;
}

static void TcpServerRoutine(void *p)
{
    (void)p;
    g_testCase++;
    (void)SampleTcpServer();
}

static void TcpClientRoutine(void *p)
{
    (void)p;
    g_testCase++;
    (void)SampleTcpClient();
}

void TcpTestLong()
{
    LogPrintln("net_socket_test_013.c enter");
    g_testCase = TEST_CASE;
    int ret;
    ret = sys_thread_new("tcp_server_long", TcpServerRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 23);

    ret = sys_thread_new("tcp_client_long", TcpClientRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 24);
}
