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

#include "osTest.h"
#include "it_los_hwi.h"


static VOID HwiF01(VOID)
{
    CHAR *queName = "TEST_QUE";
    UINT32 ret;
    UINT32 len = 10;
    UINT32 flags = 0;
    UINT32 maxMsgSize = 6;
    UINT8 queueRead[6];
    UINT8  queueWrite[6] = {101, 102, 103, 104, 105, 106};

    TestHwiClear(HWI_NUM_TEST);

    g_testCount++;

    ret = LOS_QueueCreate(queName, len, &g_testQueueID01, flags, maxMsgSize); // Los_QueueCreateSyn
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    ret = LOS_QueueRead(g_testQueueID01, queueRead, maxMsgSize, 0); // Los_QueueReadSyn
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_QUEUE_ISEMPTY, ret, EXIT);

    ret = LOS_QueueWrite(g_testQueueID01, queueWrite, maxMsgSize, 0); // Los_QueueWriteSyn
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueRead(g_testQueueID01, queueRead, maxMsgSize, 0); // Los_QueueReadSyn
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

EXIT:
    ret = LOS_QueueDelete(g_testQueueID01); // Los_QueueDelete
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 1;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;

    g_testCount = 0;

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    TestHwiTrigger(HWI_NUM_TEST);

    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);

    TestHwiDelete(HWI_NUM_TEST);
    return LOS_OK;

EXIT:
    TestHwiDelete(HWI_NUM_TEST);

    return LOS_OK;
}

VOID ItLosHwi023(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi023", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL2, TEST_FUNCTION);
}
