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
#include "It_los_mux.h"


static VOID Func01(void)
{
    UINT32 ret;

    g_testCount++;

    ret = LOS_MuxPend(g_mutexTest, LOS_WAIT_FOREVER);
    if (g_testCount == 1) { // 1, Here, The current possible value of the variable.
        ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);
        TestHwiTrigger(HWI_NUM_TEST);

        TestHwiDelete(HWI_NUM_TEST);
    } else if (g_testCount == 3) { // 3, Here, The current possible value of the variable.
        ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_ERRNO_MUX_PEND_INTERR, ret);
    }

    ret = LOS_MuxPost(g_mutexTest);

    if (g_testCount == 3) { // 3, Here, The current possible value of the variable.
        ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);
    } else if (g_testCount == 4) { // 4, Here, The current possible value of the variable.
        ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_ERRNO_MUX_INVALID, ret);
    }

    g_testCount++;
}

static VOID HwiF01(void)
{
    TestHwiClear(HWI_NUM_TEST);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 1, g_testCount); 

    g_testCount++;

    Func01();
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;

    g_testCount = 0;

    ret = LOS_MuxCreate(&g_mutexTest);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_HwiCreate(HWI_NUM_TEST, 1, 0, (HWI_PROC_FUNC)HwiF01, 0);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    Func01();

    ICUNIT_ASSERT_EQUAL(g_testCount, 5, g_testCount); // 5, Here, assert that g_testCount is equal to 5.

    ret = LOS_MuxDelete(g_mutexTest);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID ItLosMux019(void)
{
    TEST_ADD_CASE("ItLosMux019", Testcase, TEST_LOS, TEST_MUX, TEST_LEVEL2, TEST_FUNCTION);
}

