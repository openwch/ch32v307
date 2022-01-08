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

#include <securec.h>
#include <time.h>
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "socket.h"
#include "in.h"
#include "los_task.h"
#include "ohos_types.h"
#include "los_config.h"
#include "Public.h"
#include "PCommon.h"
#include "fuzz_posix.h"

#ifndef TASK_PRIO_TEST
#define TASK_PRIO_TEST 2
#endif

#ifndef TASK_STACK_SIZE_TEST
#define TASK_STACK_SIZE_TEST 0x400
#endif
#define LOS_TASK_STATUS_DETACHED 0x0100

extern S_ElementInit g_element[ELEMENT_LEN];
extern int g_iteration;

static UINT32 g_testTaskID01;

#define BUF_SIZE 40
static char g_udpIp[16];
static UINT16 g_udpPort;
#define UDPMSG "ABCDEEFG"
#define SLEEP_SECONDS 2

static void *SampleUdpServer()
{
    int fd;
    struct sockaddr_in srvAddr = { 0 };

    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    char buf[BUF_SIZE] = { 0 };
    int flag;
    struct msghdr msg = { 0 };
    struct iovec iov[2] = { };

    printf("s0\r\n");
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    printf("s1\r\n");
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_udpIp);
    srvAddr.sin_port = htons(g_udpPort);
    int ret = bind(fd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));

    printf("s1.1, ret = %d\r\n", ret);
    (void)recvfrom(fd, buf, sizeof(buf), 0x40, (struct sockaddr *)&clnAddr, &clnAddrLen);

    printf("s2, buf = %s\r\n", buf);
    usleep(SLEEP_SECONDS);
    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = buf;
    iov[0].iov_len = strlen(buf);
    (void)sendmsg(fd, &msg, 0);

    printf("s3\r\n");
    close(fd);
    return NULL;
}

static void *SampleUdpClient()
{
    int fd = 0;
    const int sleepSec = 5;
    struct sockaddr_in clnAddr = { 0 };

    char buf[BUF_SIZE] = { 0 };
    int flag;

    struct msghdr msg;
    struct iovec iov[2];

    printf("c1\r\n");
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    printf("c2\r\n");
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(g_udpIp);
    clnAddr.sin_port = htons(g_udpPort);

    printf("c3\r\n");
    strcpy_s(buf, sizeof(buf), UDPMSG);

    usleep(sleepSec);
    printf("c4, %s, %d\r\n", buf, strlen(buf));
    (void)sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&clnAddr, (socklen_t)sizeof(clnAddr));

    msg.msg_name = &clnAddr;
    msg.msg_namelen = sizeof(clnAddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);
    (void)recvmsg(fd, &msg, 0x40);
    printf("c6\r\n");

    close(fd);
    return NULL;
}

static UINT32 UdpFun(VOID)
{
    int i;

    UINT8 num1;
    UINT8 num2;
    UINT8 num3;
    UINT8 num4;
    UINT8 num5;

    INIT_FuzzEnvironment();
    printf("UDPFun starts\n");

    for (i = 0; i < g_iteration; i++) {
        num1 = *(UINT8 *)DT_SetGetU8(&g_element[NUM_0_INDEX], 1);
        num2 = *(UINT8 *)DT_SetGetU8(&g_element[NUM_1_INDEX], 1);
        num3 = *(UINT8 *)DT_SetGetU8(&g_element[NUM_2_INDEX], 1);
        num4 = *(UINT8 *)DT_SetGetU8(&g_element[NUM_3_INDEX], 1);
        num5 = *(UINT8 *)DT_SetGetU8(&g_element[NUM_4_INDEX], 1);

        (void)snprintf_s(g_udpIp, sizeof(g_udpIp), sizeof(g_udpIp) - 1, "%d.%d.%d.%d", num1, num2, num3, num4);
        g_udpPort = *(UINT16 *)DT_SetGetU16(&g_element[NUM_5_INDEX], 0);

        {
            TSK_INIT_PARAM_S stTask1 = { 0 };
            stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)SampleUdpServer;
            stTask1.uwStackSize = TASK_STACK_SIZE_TEST;
            stTask1.pcName = "Tsk001A";
            stTask1.usTaskPrio = TASK_PRIO_TEST;
            stTask1.uwResved = LOS_TASK_STATUS_DETACHED;
            (void)LOS_TaskCreate(&g_testTaskID01, &stTask1);
        }

        {
            TSK_INIT_PARAM_S stTask1 = { 0 };
            stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)SampleUdpClient;
            stTask1.uwStackSize = TASK_STACK_SIZE_TEST;
            stTask1.pcName = "Tsk001A";
            stTask1.usTaskPrio = TASK_PRIO_TEST;
            stTask1.uwResved = LOS_TASK_STATUS_DETACHED;
            (void)LOS_TaskCreate(&g_testTaskID01, &stTask1);
        }
    }

    DT_Clear(g_element);
    CLOSE_Log();
    CLEAR_FuzzEnvironment();
    printf("UDPFun end ....\n");

    return 0;
}

#define LOCAL_HOST "127.0.0.1"
#define STACK_IP LOCAL_HOST
#define STACK_PORT 2277
#define PEER_PORT STACK_PORT
#define PEER_IP LOCAL_HOST
#define SRV_MSG "Hi, I am TCP server"
#define CLI_MSG "Hi, I am TCP client"

static void *SampleTcpServer()
{
    char buf[BUF_SIZE + 1] = { 0 };
    int sfd;
    int lsfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    int ret;

    // socket had fuzz test in udpfun. now must create a useful fd, wwx520273
    lsfd = socket(AF_INET, SOCK_STREAM, 0);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP);
    srvAddr.sin_port = htons(STACK_PORT);
    (void)bind(lsfd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));

    int num1 = *(int *)DT_SetGetS32(&g_element[NUM_0_INDEX], 0);

    (void)listen(lsfd, num1);

    int num2 = *(int *)DT_SetGetS32(&g_element[NUM_1_INDEX], 0);
    sfd = accept(lsfd, (struct sockaddr *)&clnAddr, &num2);

    int num3 = *(int *)DT_SetGetS32(&g_element[NUM_2_INDEX], 0);
    ret = recv(lsfd, buf, sizeof(buf), num3);

    int num4 = *(int *)DT_SetGetS32(&g_element[NUM_3_INDEX], 0);
    ret = shutdown(sfd, num4);
    if (ret != 0) {
        shutdown(sfd, SHUT_RDWR);
    }

    close(sfd);
    close(lsfd);
    return (void *)(intptr_t)ret;
}

static void *SampleTcpClient()
{
    char buf[BUF_SIZE + 1] = { 0 };
    int sfd = -1;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    int ret;

    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);
    /* tcp client connection */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(PEER_IP);
    srvAddr.sin_port = htons(PEER_PORT);
    connect(sfd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));

    int num1 = *(int *)DT_SetGetS32(&g_element[0], 0);
    (void)getpeername(num1, &addr, &addrLen);
    (void)getsockname(num1, &addr, &addrLen);

    /* send */
    ret = memset_s(buf, sizeof(buf), 0, BUF_SIZE);
    if (err != EOK) {
        return (void *)(intptr_t)ret;
    }
    strcpy_s(buf, sizeof(buf), CLI_MSG);

    int num2 = *(int *)DT_SetGetS32(&g_element[1], 0);
    send(num1, buf, strlen(CLI_MSG), num2);

    ret = shutdown(sfd, SHUT_RDWR);

    close(sfd);
    return (void *)(intptr_t)ret;
}

static UINT32 TcpFun(VOID)
{
    int i;

    INIT_FuzzEnvironment();
    printf("TcpFun starts\n");

    for (i = 0; i < g_iteration; i++) {
        {
            TSK_INIT_PARAM_S stTask1 = { 0 };
            stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)SampleTcpServer;
            stTask1.uwStackSize = TASK_STACK_SIZE_TEST;
            stTask1.pcName = "Tsk001A";
            stTask1.usTaskPrio = TASK_PRIO_TEST;
            stTask1.uwResved = LOS_TASK_STATUS_DETACHED;
            (void)LOS_TaskCreate(&g_testTaskID01, &stTask1);
        }

        {
            TSK_INIT_PARAM_S stTask1 = { 0 };
            stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)SampleTcpClient;
            stTask1.uwStackSize = TASK_STACK_SIZE_TEST;
            stTask1.pcName = "Tsk001A";
            stTask1.usTaskPrio = TASK_PRIO_TEST;
            stTask1.uwResved = LOS_TASK_STATUS_DETACHED;
            (void)LOS_TaskCreate(&g_testTaskID01, &stTask1);
        }
    };

    DT_Clear(g_element);
    CLOSE_Log();
    CLEAR_FuzzEnvironment();
    printf("TcpFun end ....\n");

    return 0;
}

void NetFuzzTest(void)
{
    DnCompFun();

    UdpFun();
    TcpFun();

    InetFun();

    ProtoentFun();
    ConvertFun();

    SocketFun();
}
