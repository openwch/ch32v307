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
    UINT32 ret;
    UINT32 index;

    // 100, set lookup times
    for (index = 0; index < 100; index++) {
        ret = LOS_EventRead(&g_eventCB0, 0x1111, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        ICUNIT_GOTO_EQUAL(ret, 0x1111, ret, EXIT);
    }

EXIT:
    // 10, Set the number to determine whether the process is as expected.
    g_testCount = 10;

    LOS_TaskDelete(g_testTaskID01);
}

static VOID Case2()
{
    UINT32 ret;

    ret = LOS_EventWrite(&g_eventCB0, 0x1111);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);
}


static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 swTmrID;

    TSK_INIT_PARAM_S task1;
    (void)memset_s(&task1, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)Case1;
    task1.pcName = "TskName";
    task1.usTaskPrio = TASK_PRIO_TEST_NORMAL;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;

    g_testCount = 0;
    LOS_EventInit(&g_eventCB0);

    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case2, &swTmrID, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskLock();

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    LOS_SwtmrStart(swTmrID);

    LOS_TaskUnlock();

    while (g_testCount == 0) {
    }

    LOS_SwtmrDelete(swTmrID);

    return LOS_OK;
EXIT:
    LOS_SwtmrDelete(swTmrID);
    LOS_TaskDelete(g_testTaskID01);
    return LOS_OK;
}

VOID ItLosSwtmr032() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr032", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL3, TEST_FUNCTION);
}

