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


#ifdef __RISC_V__
#define HWI_NUM_INT11 HWI_NUM_TEST
#endif

static VOID Case1(UINT32 arg)
{
    UINT32 index;

    TestHwiClear(HWI_NUM_INT11);

    for (index = 0; index < TEST_HWI_RUNTIME; index++) {
    }

    // 10, Set the number to determine whether the process is as expected.
    g_testCount = 10;
}

static VOID Case2()
{
    // 20, Here, assert that g_testCount is equal to this .
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 20, g_testCount);
    g_testCount++;
}

static VOID Case3()
{
    UINT32 index;

    // 10, Here, assert that g_testCount is equal to this .
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 10, g_testCount);

    for (index = 0; index < TEST_HWI_RUNTIME; index++) {
    }
    // 20, Set the number to determine whether the process is as expected.
    g_testCount = 20;
}


static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 swtmrId1;
    UINT32 swtmrId2;

    g_testCount = 0;

    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)Case3, &swtmrId1, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    // 2, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(2, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)Case2, &swtmrId2, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_HwiCreate(HWI_NUM_INT11, 1, 0, (HWI_PROC_FUNC)Case1, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    LOS_SwtmrStart(swtmrId1);
    LOS_SwtmrStart(swtmrId2);

    TestHwiTrigger(HWI_NUM_INT11);

    // 21, Here, check that g_testCount is equal to this .
    while (g_testCount != 21) {
    }

    TestHwiDelete(HWI_NUM_INT11);

    LOS_SwtmrDelete(swtmrId1);
    LOS_SwtmrDelete(swtmrId2);
    return LOS_OK;
EXIT:
    LOS_SwtmrDelete(swtmrId1);
    LOS_SwtmrDelete(swtmrId2);
    TestHwiDelete(HWI_NUM_INT11);
    return LOS_OK;
}

VOID ItLosSwtmr033() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr033", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL1, TEST_FUNCTION);
}

