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


#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
static VOID Case1(UINT32 arg)
{
    ICUNIT_ASSERT_EQUAL_VOID(arg, 0xffff, arg);
    g_uwsTick1 = LOS_TickCountGet();
    g_testCount++;
    return;
}

static VOID Case2(UINT32 arg)
{
    UINT16 align;
    ICUNIT_ASSERT_EQUAL_VOID(arg, 0xffff, arg);
    g_uwsTick2 = LOS_TickCountGet();
    align = g_uwsTick2 - g_uwsTick1;
    ICUNIT_ASSERT_NOT_EQUAL_VOID(align, 0, align);
    g_testCount++;
    return;
}

static VOID Case3(UINT32 arg)
{
    UINT16 align;
    ICUNIT_ASSERT_EQUAL_VOID(arg, 0xffff, arg);
    g_uwsTick3 = LOS_TickCountGet();
    align = g_uwsTick3 - g_uwsTick2;
    ICUNIT_ASSERT_NOT_EQUAL_VOID(align, 0, align);
    g_testCount++;
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 swtmrId1;
    UINT32 swtmrId2;
    UINT32 swtmrId3;

    g_testCount = 0;

    // 7, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(7, LOS_SWTMR_MODE_PERIOD, Case1, &swtmrId1, 0xffff, OS_SWTMR_ROUSES_ALLOW,
        OS_SWTMR_ALIGN_INSENSITIVE);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    // 8, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(8, LOS_SWTMR_MODE_PERIOD, Case2, &swtmrId2, 0xffff, OS_SWTMR_ROUSES_ALLOW,
        OS_SWTMR_ALIGN_INSENSITIVE);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    // 9, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(9, LOS_SWTMR_MODE_PERIOD, Case3, &swtmrId3, 0xffff, OS_SWTMR_ROUSES_ALLOW,
        OS_SWTMR_ALIGN_INSENSITIVE);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrStart(swtmrId1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskDelay(2); // 2, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrStart(swtmrId2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskDelay(2); // 2, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrStart(swtmrId3);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskDelay(3); // 3, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);

    ret = LOS_SwtmrDelete(swtmrId1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrDelete(swtmrId2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_SwtmrDelete(swtmrId3);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_TaskDelay(10); // 10, set delay time.
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    ICUNIT_ASSERT_EQUAL(g_testCount, 1, g_testCount);

    return LOS_OK;

EXIT:
    LOS_SwtmrDelete(swtmrId1);
    LOS_SwtmrDelete(swtmrId2);
    LOS_SwtmrDelete(swtmrId3);

    return LOS_OK;
}

VOID ItLosSwtmrAlign029() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmrAlign029", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL1, TEST_FUNCTION);
}
#endif
