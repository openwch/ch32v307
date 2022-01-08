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


static VOID SwtmrF01(VOID)
{
    UINT32 ret;
    CHAR buffer1[] = "MiniOS";

    ret = LOS_QueueWrite(g_testQueueID01, &buffer1, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    g_testCount++;
}

static VOID SwtmrF02(VOID)
{
    UINT32 ret;
    CHAR buffer2[] = "UniDSP";

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 1, g_testCount);

    ret = LOS_QueueRead(g_testQueueID01, &buffer2, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    g_testCount++;
}
static UINT32 Testcase(VOID)
{
    UINT32 ret;
    CHAR buffer2[] = "UniDSP";
    UINT32 swTmrID1;
    UINT32 swTmrID2;

    g_testCount = 0;

    // 4, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(4, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)SwtmrF01, &swTmrID1, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    // 8, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(8, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)SwtmrF02, &swTmrID2, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT3);

    ret = LOS_QueueCreate("Q1", 1, &g_testQueueID01, 0, QUEUE_BASE_MSGSIZE);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    LOS_SwtmrStart(swTmrID1);
    LOS_SwtmrStart(swTmrID2);

    ret = LOS_TaskDelay(5); // 5, set delay time
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_TaskDelay(5); // 5, set delay time
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_QueueRead(g_testQueueID01, &buffer2, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_QUEUE_ISEMPTY, ret, EXIT1);

    LOS_SwtmrDelete(swTmrID1);
    LOS_SwtmrDelete(swTmrID2);

    ret = LOS_QueueDelete(g_testQueueID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    return LOS_OK;

EXIT1:
    LOS_SwtmrDelete(swTmrID1);
    LOS_SwtmrDelete(swTmrID2);
    LOS_QueueDelete(g_testQueueID01);
    return LOS_OK;
EXIT2:
    LOS_SwtmrDelete(swTmrID1);
    return LOS_OK;
EXIT3:
    LOS_SwtmrDelete(swTmrID2);
    return LOS_OK;
}

VOID ItLosQueue073(VOID)
{
    TEST_ADD_CASE("ItLosQueue073", Testcase, TEST_LOS, TEST_QUE, TEST_LEVEL1, TEST_FUNCTION);
}

