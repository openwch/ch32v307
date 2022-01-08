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
#include "It_los_swtmr.h"


static VOID SwtmrF01(UINT32 arg)
{
    UINT32 ret;
    UINT32 tick = 0;

    if (arg != TIMER_LOS_HANDLER_PARAMETER) {
        return;
    }

    ret = LOS_SwtmrTimeGet(g_swtmrId1, &tick);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    ICUNIT_GOTO_EQUAL(tick, TIMER_LOS_EXPIRATION1 - 1, tick, EXIT);

    g_testCount++;
    return;
EXIT:
    LOS_SwtmrDelete(g_swtmrId1);
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 tick = 0;

    g_testCount = 0;
    g_swtmrId1 = 0;

    ret = LOS_SwtmrCreate(TIMER_LOS_EXPIRATION1, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)SwtmrF01, &g_swtmrId1,
        TIMER_LOS_HANDLER_PARAMETER
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );

    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrStart(g_swtmrId1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskDelay(10); // 10, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrTimeGet(g_swtmrId1, &tick);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    // 2, assert that uwTick is equal to this.
    ICUNIT_GOTO_EQUAL(tick, 2, tick, EXIT);

    // 2, Here, assert that g_testCount is equal to this .
    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT);

    ret = LOS_SwtmrDelete(g_swtmrId1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrTimeGet(g_swtmrId1, &tick);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_SWTMR_NOT_CREATED, ret, EXIT);

    ret = LOS_TaskDelay(10); // 10, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    // 2, Here, assert that g_testCount is equal to this .
    ICUNIT_ASSERT_EQUAL(g_testCount, 2, g_testCount);

    return LOS_OK;
EXIT:
    LOS_SwtmrDelete(g_swtmrId1);
    return LOS_OK;
}

VOID ItLosSwtmr069(VOID)
{
    TEST_ADD_CASE("ItLosSwtmr069", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL1, TEST_FUNCTION);
}

