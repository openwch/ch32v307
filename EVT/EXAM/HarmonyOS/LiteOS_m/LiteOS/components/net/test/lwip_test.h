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

#ifndef LWIP_TEST_H
#define LWIP_TEST_H

#include "lwip/arch.h"
#include "lwip/sockets.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int g_testCase;
extern int g_testError;
extern int g_testTemp;
#define LWIP_TEST_RET_OK    0
#define LWIP_TEST_RET_ERR   1

#define LOCALHOST "127.0.0.1"
#define STACK_IP LOCALHOST
#define STACK_PORT 2277
#define PEER_PORT STACK_PORT
#define SERVER_PORT STACK_PORT
#define CLIENT_PORT 2288
#define PEER_IP LOCALHOST
#define BUF_SIZE (128 * 1)
#define IOV_LENGTH 2
#define STACK_TEST_SIZE (0x800UL)

#define ICUNIT_ASSERT_NOT_EQUAL(a, b, c)  if ((a) == (b)) { \
                                              g_testError = (c); \
                                              LWIP_ASSERT("lwip test", 0); \
                                          }
#define ICUNIT_ASSERT_EQUAL(a, b, c)      if ((a) != (b)) { \
                                              g_testError = (c); \
                                              LWIP_ASSERT("lwip test", 0); \
                                          }

#define LogPrintln(fmt, ...) \
    HILOG_INFO(HILOG_MODULE_APP, fmt "%c", \
         ##__VA_ARGS__, \
        ('\n' == " "fmt[sizeof(" "fmt)-2]) ? '\0' : '\n') // trailing newline is auto appended

#ifdef __cplusplus
}
#endif

#endif /* LWIP_TEST_H */
