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

static char g_buf[BUF_SIZE + 1] = { 0 };
static char g_buf_temp[BUF_SIZE + 1] = { 0 };

#define SEND_UDP_COUNT 10000  // send count
#define TEST_CASE 170

static void UdpTestMoreTask(void *p)
{
    (void)p;
    LogPrintln("net_socket_test_007.c enter");
    if (g_testCase < TEST_CASE) {
        g_testCase = TEST_CASE;
    } else {
        g_testCase++;
    }

    int sfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    int ret;
    int i;

    /* socket creation */
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    LWIP_ASSERT("socket invalid param.", sfd != -1);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT);
    ret = bind(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LWIP_ASSERT("socket invalid param.", ret == 0);

    /* send */
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    clnAddr.sin_port = htons(PEER_PORT);
    if (g_buf_temp[0] == '\0') {
        for (i = 0; i < BUF_SIZE; i++) {
            g_buf_temp[i] = 'U';
        }
        g_buf_temp[BUF_SIZE] = '\0';
    }

    LogPrintln("udp begin send");
    for (i = 0; i < SEND_UDP_COUNT; i++) {
        ret = memcpy_s(g_buf, sizeof(g_buf), g_buf_temp, sizeof(g_buf_temp));
        ICUNIT_ASSERT_EQUAL(ret, EOK, 1);
        ret = sendto(sfd, g_buf, BUF_SIZE, 0, (struct sockaddr*)&clnAddr, (socklen_t)sizeof(clnAddr));
        LWIP_ASSERT("socket invalid param.", ret != -1);

        /* recv */
        (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
        ret = recvfrom(sfd, g_buf, sizeof(g_buf), 0, (struct sockaddr*)&clnAddr,
            &clnAddrLen);
        LWIP_ASSERT("socket invalid param.", ret == BUF_SIZE);
    }
    LogPrintln("udp end send");

    /* close socket */
    ret = closesocket(sfd);
    LWIP_ASSERT("socket invalid param.", ret != -1);
}

void UdpTestMore()
{
    int ret;
    ret = sys_thread_new("udp_more", UdpTestMoreTask, NULL,
        STACK_TEST_SIZE * 2, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 10);
}

