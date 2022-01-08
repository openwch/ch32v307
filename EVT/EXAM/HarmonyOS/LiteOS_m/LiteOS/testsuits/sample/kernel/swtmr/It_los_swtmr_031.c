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


static VOID Case1(UINT32 arg)
{
    // 20, Here, assert that g_testCount is equal to this .
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 20, g_testCount);
    // 30, Set the number to determine whether the process is as expected.
    g_testCount = 30;
}

static VOID Case2()
{
    // 10, Here, assert that g_testCount is equal to this .
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 10, g_testCount);
    // 20, Set the number to determine whether the process is as expected.
    g_testCount = 20;
}

static VOID Case3()
{
    // 10, Set the number to determine whether the process is as expected.
    g_testCount = 10;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 swtmrId1;
    UINT32 swtmrId2;
    UINT32 swtmrId3;

    g_testCount = 0;

    // 5, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(5, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case3, &swtmrId1, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    // 6, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(6, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case2, &swtmrId2, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 7, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(7, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case1, &swtmrId3, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    
    LOS_SwtmrStart(swtmrId1);
    LOS_SwtmrStart(swtmrId2);
    LOS_SwtmrStart(swtmrId3);

    LOS_TaskDelay(19); // 19, set delay time.

    LOS_SwtmrDelete(swtmrId1);
    LOS_SwtmrDelete(swtmrId2);
    LOS_SwtmrDelete(swtmrId3);

    return LOS_OK;
EXIT:
    LOS_SwtmrDelete(swtmrId1);
    LOS_SwtmrDelete(swtmrId2);
    LOS_SwtmrDelete(swtmrId3);
    return LOS_OK;
}

VOID ItLosSwtmr031() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr031", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL1, TEST_FUNCTION);
}

