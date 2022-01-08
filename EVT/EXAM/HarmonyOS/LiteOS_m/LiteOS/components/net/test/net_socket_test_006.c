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

#define TEST_CASE 160
#define TEMP_BUF_SIZE 32
int InetTest()
{
    LogPrintln("net_socket_test_006.c enter");
    g_testCase = TEST_CASE;
    struct in_addr in;
    int ret = inet_pton(AF_INET, "300.10.10.10", &in);
    ICUNIT_ASSERT_EQUAL(ret, 0, 1);

    ret = inet_pton(AF_INET, "10.11.12.13", &in);
    ICUNIT_ASSERT_EQUAL(ret, 1, 2);
#if BYTE_ORDER == LITTLE_ENDIAN
    g_testCase++;
    ICUNIT_ASSERT_EQUAL(in.s_addr, 0x0d0c0b0a, 3);
#else
    ICUNIT_ASSERT_EQUAL(in.s_addr, 0x0a0b0c0d, 4);
#endif

    const char *p = inet_ntoa(in);
    ICUNIT_ASSERT_EQUAL(strcmp(p, "10.11.12.13"), 0, 5);

    char buf[TEMP_BUF_SIZE];
    p = inet_ntop(AF_INET, &in, buf, sizeof(buf));
    ICUNIT_ASSERT_EQUAL(p, buf, 6);
    ICUNIT_ASSERT_EQUAL(strcmp(p, "10.11.12.13"), 0, 7);

    return LWIP_TEST_RET_OK;
}
