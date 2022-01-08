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

    ICUNIT_ASSERT_EQUAL_VOID(arg, 0xffff, arg);

    g_testCount++;

    ret = LOS_EventWrite(&g_eventCB0, 0xFFFF);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    return;
}

static VOID SwtmrF02(UINT32 arg)
{
    UINT32 ret;

    ICUNIT_ASSERT_EQUAL_VOID(arg, 0xffff, arg);

    g_testCount++;

    ret = LOS_EventWrite(&g_eventCB0, 0xFFFF);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;

    g_testCount = 0;
    g_eventCB0.uwEventID = 0;
    LOS_EventInit(&g_eventCB0);
    // 1, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)SwtmrF02, &g_swtmrId1, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);
    // 20, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(20, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)SwtmrF01, &g_swtmrId2, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_SwtmrStart(g_swtmrId1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    ret = LOS_SwtmrStart(g_swtmrId2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    while (1) {
        ret = LOS_EventRead(&g_eventCB0, 0xFFFF, LOS_WAITMODE_AND, LOS_WAIT_FOREVER);
        ICUNIT_GOTO_EQUAL(ret, 0xFFFF, ret, EXIT1);
        // 200, Here, check that g_testCount is equal to this .
        if (g_testCount == 200) {
            ret = LOS_SwtmrDelete(g_swtmrId1);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

            ret = LOS_SwtmrDelete(g_swtmrId2);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);
            ret = LOS_EventDestroy(&g_eventCB0);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);
            break;
        }
    }

    return LOS_OK;

EXIT1:
    LOS_SwtmrDelete(g_swtmrId1);
    LOS_SwtmrDelete(g_swtmrId2);
    return LOS_OK;

EXIT2:
    ret = LOS_SwtmrDelete(g_swtmrId1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_SwtmrDelete(g_swtmrId2);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID ItLosSwtmr038() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr038", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL3, TEST_FUNCTION);
}

