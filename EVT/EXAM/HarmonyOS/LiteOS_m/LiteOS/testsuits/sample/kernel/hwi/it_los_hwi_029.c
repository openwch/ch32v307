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


static VOID HwiF03(VOID)
{
    TestHwiClear(HWI_NUM_TEST1);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 4, g_testCount); // Compare wiht the expected value 4.
    g_testCount++;

    return;
}

static VOID HwiF02(VOID)
{
    TestHwiClear(HWI_NUM_TEST3);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 2, g_testCount); // Compare wiht the expected value 2.
    g_testCount++;

    TestHwiTrigger(HWI_NUM_TEST1);
    g_testCount++;

    return;
}

static VOID HwiF01(VOID)
{
    TestHwiClear(HWI_NUM_TEST);
    g_testCount++;

    TestHwiTrigger(HWI_NUM_TEST3);
    g_testCount++;

    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 loop;
    HWI_PRIOR_T hwiPrio = 2;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;

    for (loop = 0; loop < HWI_LOOP_NUM; loop++) {
        g_testCount = 0;
        ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
        ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

        ret = LOS_HwiCreate(HWI_NUM_TEST3, hwiPrio, mode, (HWI_PROC_FUNC)HwiF02, arg);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

        ret = LOS_HwiCreate(HWI_NUM_TEST1, hwiPrio, mode, (HWI_PROC_FUNC)HwiF03, arg);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

        TestHwiTrigger(HWI_NUM_TEST);

        ICUNIT_ASSERT_EQUAL(g_testCount, 5, g_testCount); // Compare wiht the expected value 5.

        TestHwiDelete(HWI_NUM_TEST);
        TestHwiDelete(HWI_NUM_TEST3);
        TestHwiDelete(HWI_NUM_TEST1);
    }

EXIT2:
    TestHwiDelete(HWI_NUM_TEST3);

EXIT1:
    TestHwiDelete(HWI_NUM_TEST);

    return LOS_OK;
}

VOID ItLosHwi029(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi029", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL3, TEST_PRESSURE);
}
