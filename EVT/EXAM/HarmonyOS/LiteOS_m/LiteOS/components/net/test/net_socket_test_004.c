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

#define TEST_CASE 140
#define OPT_TIMEOUT 1000

int SockOptTest()
{
    LogPrintln("net_socket_test_004.c enter");
    g_testCase = TEST_CASE;
    int ret, error, flag;
    struct timeval timeout;
    socklen_t len;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ICUNIT_ASSERT_NOT_EQUAL(fd, -1, 1);

    error = -1;
    len = sizeof(error);
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    LogPrintln("getsockopt(%d, SOL_SOCKET, SO_ERROR, &error, &len)=%d, error=%d, len=%d, errno=%d", \
        fd, ret, error, len, errno);
    ICUNIT_ASSERT_EQUAL(ret, 0, 2);
    ICUNIT_ASSERT_EQUAL(error, 0, 3);

    len = sizeof(timeout);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &len);
    ICUNIT_ASSERT_EQUAL(ret, 0, 4);

    timeout.tv_sec = OPT_TIMEOUT;
    len = sizeof(timeout);
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, len);
    ICUNIT_ASSERT_EQUAL(ret, 0, 5);

    (void)memset_s(&timeout, len, 0, len);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &len);
    ICUNIT_ASSERT_EQUAL(ret, 0, 6);
    ICUNIT_ASSERT_EQUAL(timeout.tv_sec, OPT_TIMEOUT, 7);

    error = -1;
    len = sizeof(error);
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    LogPrintln("getsockopt(%d, SOL_SOCKET, SO_ERROR, &error, &len)=%d, error=%d, len=%d, errno=%d", \
        fd, ret, error, len, errno);
    ICUNIT_ASSERT_EQUAL(ret, 0, 8);
    ICUNIT_ASSERT_EQUAL(error, 0, 9);

    flag = 1;
    ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    LogPrintln("setsockopt(TCP_NODELAY) ret=%d", ret);
    ICUNIT_ASSERT_EQUAL(ret, 0, 10);

    ret = closesocket(fd);
    ICUNIT_ASSERT_EQUAL(ret, 0, 11);

    return LWIP_TEST_RET_OK;
}
