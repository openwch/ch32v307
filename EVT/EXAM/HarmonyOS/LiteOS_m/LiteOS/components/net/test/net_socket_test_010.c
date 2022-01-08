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

#define SRV_MSG "Hi, I am TCP server"
#define CLI_MSG "Hi, I am TCP client"

#define TEST_CASE 200
#define STACK_PORT_10 2230
#define POLL_OPEN_MAX 10
#define TIME_OUT (1000 * 10) // timeout 10s
static char g_buf[BUF_SIZE + 1] = { 0 };

static int SampleTcpServer()
{
#if LWIP_SOCKET_POLL
    g_testCase++;
    int sfd, lsfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    struct pollfd client[POLL_OPEN_MAX] = {0};
    int ret;

    /* tcp server */
    lsfd = socket(AF_INET, SOCK_STREAM, 0);
    LogPrintln("create server socket inet stream: %d", lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(lsfd, -1, 1);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT_10);
    ret = bind(lsfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LogPrintln("bind socket %d to %s:%d: %d", lsfd, inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 2);

    ret = listen(lsfd, 0);
    LogPrintln("listen socket %d: %d", lsfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 3);

    client[0].fd = lsfd;
    client[0].events = POLLIN;
    ret = poll(client, 1, TIME_OUT);
    if (ret < 0) {
        LogPrintln("poll error");
        ICUNIT_ASSERT_EQUAL(-1, 0, 4);
    } else if (ret == 0) {
        LogPrintln("poll timeout");
        ICUNIT_ASSERT_EQUAL(-1, 0, 5);
    }

    sfd = accept(lsfd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    LogPrintln("accept socket %d: %d <%s:%d>", lsfd, sfd, inet_ntoa(clnAddr.sin_addr), ntohs(clnAddr.sin_port));
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 6);

    /* recv */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    ret = recv(sfd, g_buf, sizeof(g_buf), 0);
    LogPrintln("server recv on socket %d: %d", sfd, ret);
    LogPrintln("ser:%s", g_buf);
    ICUNIT_ASSERT_EQUAL(ret, strlen(CLI_MSG), 7);

    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 8);
    ret = closesocket(lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 9);
#endif
    return 0;
}

static int SampleTcpClient()
{
#if LWIP_SOCKET_POLL
    g_testCase++;
    int sfd;
    struct sockaddr_in srvAddr = { 0 };
    int ret;

    /* tcp client connection */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    LogPrintln("create client socket inet stream: %d", sfd);
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 10);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    srvAddr.sin_port = htons(STACK_PORT_10);
    ret = connect(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LogPrintln("connect socket %d to %s:%d: %d", sfd, inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 11);

    /* send */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)strcpy_s(g_buf, sizeof(g_buf), CLI_MSG);
    ret = send(sfd, g_buf, strlen(CLI_MSG), 0);
    LogPrintln("client send on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, strlen(CLI_MSG), 12);

    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 13);
#endif
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

void TcpTestPoll()
{
    LogPrintln("net_socket_test_010.c enter");
    g_testCase = TEST_CASE;
    int ret;
    ret = sys_thread_new("tcp_server_poll", TcpServerRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 23);

    ret = sys_thread_new("tcp_client_poll", TcpClientRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 24);
}
