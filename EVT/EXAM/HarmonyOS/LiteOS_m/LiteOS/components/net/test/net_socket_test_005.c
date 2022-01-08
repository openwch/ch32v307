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

#define TEST_CASE 150

int ByteOrderTest(void)
{
    LogPrintln("net_socket_test_005.c enter");
#if BYTE_ORDER == LITTLE_ENDIAN
    g_testCase = TEST_CASE + 1;
    uint32_t hl = ntohl(0x12345678);
    ICUNIT_ASSERT_EQUAL(hl, 0x78563412, 1);

    uint32_t nl = htonl(0x12345678);
    ICUNIT_ASSERT_EQUAL(nl, 0x78563412, 2);

    uint16_t hs = ntohs(0x1234);
    ICUNIT_ASSERT_EQUAL(hs, 0x3412, 3);

    uint16_t ns = htons(0x1234);
    ICUNIT_ASSERT_EQUAL(ns, 0x3412, 4);
#else
    g_testCase = TEST_CASE;
    uint32_t hl = ntohl(0x12345678);
    ICUNIT_ASSERT_EQUAL(hl, 0x12345678, 5);

    uint32_t nl = htonl(0x12345678);
    ICUNIT_ASSERT_EQUAL(nl, 0x12345678, 6);

    uint16_t hs = ntohs(0x1234);
    ICUNIT_ASSERT_EQUAL(hs, 0x1234, 7);

    uint16_t ns = htons(0x1234);
    ICUNIT_ASSERT_EQUAL(ns, 0x1234, 8);
#endif
    return LWIP_TEST_RET_OK;
}
