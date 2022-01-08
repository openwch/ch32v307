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
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 1, g_testCount);
    g_testCount++;

    return;
}

static VOID HwiF02(VOID)
{
    TestHwiClear(HWI_NUM_TEST3);
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 2, g_testCount); // Compare wiht the expected value 2.
    g_testCount++;

    return;
}

static VOID HwiF01(VOID)
{
    TestHwiClear(HWI_NUM_TEST);
    g_testCount++;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 1;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;
    UINT32 intSave;

    g_testCount = 0;

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_HwiCreate(HWI_NUM_TEST3, hwiPrio, mode, (HWI_PROC_FUNC)HwiF02, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_HwiCreate(HWI_NUM_TEST1, hwiPrio, mode, (HWI_PROC_FUNC)HwiF03, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    intSave = LOS_IntLock();

    TestHwiTrigger(HWI_NUM_TEST1);
    TestHwiTrigger(HWI_NUM_TEST3);
    TestHwiTrigger(HWI_NUM_TEST);

    LOS_IntRestore(intSave);

    ICUNIT_GOTO_EQUAL(g_testCount, 3, g_testCount, EXIT3); // Compare wiht the expected value 3.

EXIT3:
    TestHwiDelete(HWI_NUM_TEST);
    TestHwiDelete(HWI_NUM_TEST3);
    TestHwiDelete(HWI_NUM_TEST1);
    return LOS_OK;

EXIT2:
    TestHwiDelete(HWI_NUM_TEST3);
    TestHwiDelete(HWI_NUM_TEST);
    return LOS_OK;

EXIT1:
    TestHwiDelete(HWI_NUM_TEST);
    return LOS_OK;
}

VOID ItLosHwi014(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi014", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL2, TEST_FUNCTION);
}
