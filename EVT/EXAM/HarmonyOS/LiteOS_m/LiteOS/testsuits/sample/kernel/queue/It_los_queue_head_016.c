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
    UINT32 queueID[LOSCFG_BASE_IPC_QUEUE_LIMIT + 1];
    CHAR buff1[8] = "UniDSP";
    CHAR buff2[8] = "";

    for (index = 0; index < LOSCFG_BASE_IPC_QUEUE_LIMIT - QUEUE_EXISTED_NUM; index++) {
        ret = LOS_QueueCreate(NULL, QUEUE_BASE_NUM, &queueID[index], 0, QUEUE_BASE_MSGSIZE);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }

    ret = LOS_QueueCreate("Q1", QUEUE_BASE_NUM, &queueID[LOSCFG_BASE_IPC_QUEUE_LIMIT], 0, QUEUE_BASE_MSGSIZE);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_QUEUE_CB_UNAVAILABLE, ret, EXIT);

    ret = LOS_QueueWriteHead(LOSCFG_BASE_IPC_QUEUE_LIMIT + 1, &buff1, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_QUEUE_INVALID, ret, EXIT);

    ret = LOS_QueueRead(LOSCFG_BASE_IPC_QUEUE_LIMIT + 1, &buff2, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_QUEUE_INVALID, ret, EXIT);

EXIT:
    for (index = 0; index < LOSCFG_BASE_IPC_QUEUE_LIMIT - QUEUE_EXISTED_NUM; index++) {
        ret = LOS_QueueDelete(queueID[index]);
        ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    }

    return LOS_OK;
}

VOID ItLosQueueHead016(VOID)
{
    TEST_ADD_CASE("ItLosQueueHead016", Testcase, TEST_LOS, TEST_QUE, TEST_LEVEL1, TEST_FUNCTION);
}

