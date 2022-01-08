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
#define TEST_CASE 130

static char g_buf[BUF_SIZE + 1] = { 0 };

static int SampleTcpServer()
{
    g_testCase++;
    int sfd, lsfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    struct msghdr msg = { 0 };
    struct iovec iov[IOV_LENGTH] = { 0 };
    int ret;

    /* tcp server */
    lsfd = socket(AF_INET, SOCK_STREAM, 0);
    LogPrintln("create server socket inet stream: %d", lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(lsfd, -1, 1);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT);
    ret = bind(lsfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LogPrintln("bind socket %d to %s:%d: %d", lsfd, inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 2);

    ret = listen(lsfd, 0);
    LogPrintln("listen socket %d: %d", lsfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 3);

    sfd = accept(lsfd, (struct sockaddr*)&clnAddr, &clnAddrLen);
    LogPrintln("accept socket %d: %d <%s:%d>", lsfd, sfd, inet_ntoa(clnAddr.sin_addr), ntohs(clnAddr.sin_port));
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 4);

    /* send */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)strcpy_s(g_buf, sizeof(g_buf), SRV_MSG);
    ret = send(sfd, g_buf, strlen(SRV_MSG), 0);
    LogPrintln("server send on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, strlen(SRV_MSG), 5);

    /* recv */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    ret = recv(sfd, g_buf, sizeof(g_buf), 0);
    LogPrintln("server recv on socket %d: %d", sfd, ret);
    LogPrintln("ser:%s", g_buf);
    ICUNIT_ASSERT_EQUAL(ret, strlen(CLI_MSG), 6);

    /* sendmsg */
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)strcpy_s(g_buf, sizeof(g_buf), SRV_MSG);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = IOV_LENGTH;
    iov[0].iov_base = g_buf;
    iov[0].iov_len = strlen(SRV_MSG);
    iov[1].iov_base = g_buf;
    iov[1].iov_len = strlen(SRV_MSG);
    ret = sendmsg(sfd, &msg, 0);
    LogPrintln("sendmsg on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, IOV_LENGTH * strlen(SRV_MSG), 7);

    /* recvmsg */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = g_buf;
    iov[0].iov_len = sizeof(g_buf);
    ret = recvmsg(sfd, &msg, 0);
    LogPrintln("recvmsg on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, IOV_LENGTH * strlen(CLI_MSG), 8);

    ret = shutdown(sfd, SHUT_RDWR);
    LogPrintln("shutdown socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 9);

    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 10);
    ret = closesocket(lsfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 11);

    return 0;
}

static int SampleTcpClient()
{
    g_testCase++;
    int sfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    int ret;
    struct msghdr msg = { 0 };
    struct iovec iov[IOV_LENGTH] = { 0 };
    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);

    /* tcp client connection */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    LogPrintln("create client socket inet stream: %d", sfd);
    ICUNIT_ASSERT_NOT_EQUAL(sfd, -1, 10);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    srvAddr.sin_port = htons(PEER_PORT);
    ret = connect(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LogPrintln("connect socket %d to %s:%d: %d", sfd, inet_ntoa(srvAddr.sin_addr), ntohs(srvAddr.sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 11);

    /* test getpeername */
    ret = getpeername(sfd, &addr, &addrLen);
    LogPrintln("getpeername %d %s:%d: %d", \
        sfd, inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr), ntohs(((struct sockaddr_in*)&addr)->sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 12);
    ICUNIT_ASSERT_EQUAL(addrLen, sizeof(struct sockaddr_in), 13);
    ICUNIT_ASSERT_EQUAL(((struct sockaddr_in*)&addr)->sin_addr.s_addr, \
        inet_addr(PEER_IP), 14);

    /* test getsockname */
    ret = getsockname(sfd, &addr, &addrLen);
    LogPrintln("getsockname %d %s:%d: %d", \
        sfd, inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr), ntohs(((struct sockaddr_in*)&addr)->sin_port), ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 15);
    ICUNIT_ASSERT_EQUAL(addrLen, sizeof(struct sockaddr_in), 16);
    ICUNIT_ASSERT_EQUAL(((struct sockaddr_in*)&addr)->sin_addr.s_addr, \
        inet_addr(STACK_IP), 17);

    /* send */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)strcpy_s(g_buf, sizeof(g_buf), CLI_MSG);
    ret = send(sfd, g_buf, strlen(CLI_MSG), 0);
    LogPrintln("client send on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, strlen(CLI_MSG), 18);

    /* recv */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    ret = recv(sfd, g_buf, sizeof(g_buf), 0);
    LogPrintln("client recv on socket %d: %d", sfd, ret);
    LogPrintln("cli:%s", g_buf);
    ICUNIT_ASSERT_EQUAL(ret, strlen(SRV_MSG), 19);

    /* sendmsg */
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)strcpy_s(g_buf, sizeof(g_buf), CLI_MSG);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = IOV_LENGTH;
    iov[0].iov_base = g_buf;
    iov[0].iov_len = strlen(CLI_MSG);
    iov[1].iov_base = g_buf;
    iov[1].iov_len = strlen(CLI_MSG);
    ret = sendmsg(sfd, &msg, 0);
    LogPrintln("sendmsg on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, 2 * strlen(CLI_MSG), 20);

    /* recvmsg */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = g_buf;
    iov[0].iov_len = sizeof(g_buf);
    ret = recvmsg(sfd, &msg, 0);
    LogPrintln("recvmsg on socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, IOV_LENGTH * strlen(SRV_MSG), 21);

    ret = shutdown(sfd, SHUT_RDWR);
    LogPrintln("shutdown socket %d: %d", sfd, ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 22);

    ret = closesocket(sfd);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 23);
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

void TcpTest()
{
    LogPrintln("net_socket_test_003.c enter");
    g_testCase = TEST_CASE;
    int ret;
    ret = sys_thread_new("tcp_server", TcpServerRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 23);

    ret = sys_thread_new("tcp_client", TcpClientRoutine, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 24);
}
