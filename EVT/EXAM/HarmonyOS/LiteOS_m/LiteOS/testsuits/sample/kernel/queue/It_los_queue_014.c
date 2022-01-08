/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
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


#include "It_los_queue.h"


static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 index;
    CHAR buff1[] = "UniDSPOS";
    CHAR buff2[9] = ""; // 9, QUEUE_BASE_MSGSIZE + 1

    ret = LOS_QueueCreate("Q1", QUEUE_BASE_NUM, &g_testQueueID01, 0, QUEUE_BASE_MSGSIZE);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueWrite(g_testQueueID01, &buff1, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueRead(g_testQueueID01, &buff2, 0, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_QUEUE_READSIZE_ISZERO, ret, EXIT);
    for (index = 0; index < 9; index++) { // 9, QUEUE_BASE_MSGSIZE + 1
        ICUNIT_GOTO_EQUAL(buff2[index], '\0', buff2[index], EXIT);
    }

EXIT:
    ret = LOS_QueueDelete(g_testQueueID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID ItLosQueue014(VOID)
{
    TEST_ADD_CASE("ItLosQueue014", Testcase, TEST_LOS, TEST_QUE, TEST_LEVEL1, TEST_FUNCTION);
}

