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
    if (arg != 0xffff) {
        return;
    }

    g_testCount++;
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 swTmrID;
#ifdef __RISC_V__
    UINT16 swtmrId2;
    UINT32 tick;
#endif
    g_testCount = 0;
    // 4, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(4, LOS_SWTMR_MODE_ONCE, Case1, &swTmrID, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_SwtmrStart(swTmrID);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskDelay(10); // 10, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 1, Here, assert that g_testCount is equal to this .
    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT); 

    ret = LOS_TaskDelay(5); // 5, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 1, Here, assert that g_testCount is equal to this .
    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);

#if SELF_DELETED
    ret = LOS_SwtmrDelete(swTmrID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
#endif

#ifdef __RISC_V__
    // 4, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(4, LOS_SWTMR_MODE_NO_SELFDELETE, Case1, &swtmrId2, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ret = LOS_SwtmrStart(swtmrId2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskDelay(10); // 10, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 2, Here, assert that g_testCount is equal to this .
    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT);

    ret = LOS_TaskDelay(5); // 5, set delay time.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 2, Here, assert that g_testCount is equal to this .
    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT);

    ret = LOS_SwtmrStop(swtmrId2);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_SWTMR_NOT_STARTED, ret, EXIT);

    ret = LOS_SwtmrTimeGet(swtmrId2, &tick);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_SWTMR_NOT_STARTED, ret, EXIT);
#endif

EXIT:
#ifdef __RISC_V__
    ret = LOS_SwtmrDelete(swtmrId2);
#endif
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID ItLosSwtmr001() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr001", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL1, TEST_FUNCTION);
}

