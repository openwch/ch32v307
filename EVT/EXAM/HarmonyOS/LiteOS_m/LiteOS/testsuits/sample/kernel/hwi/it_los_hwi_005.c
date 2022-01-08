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


#ifdef __RISC_V__
#define HWI_NUM_INT0 HWI_NUM_TEST2
#define HWI_NUM_INT1 HWI_NUM_TEST3
#endif

static VOID HwiF01(VOID)
{
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 3;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;

    ret = LOS_HwiCreate(HWI_NUM_INT0, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_HwiCreate(HWI_NUM_INT1, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    ret = LOS_HwiCreate(HWI_NUM_TEST1, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT3);

    TestHwiDelete(HWI_NUM_TEST1);

EXIT3:
    TestHwiDelete(HWI_NUM_TEST);
    TestHwiDelete(HWI_NUM_INT1);
    TestHwiDelete(HWI_NUM_INT0);
    return LOS_OK;

EXIT2:
    TestHwiDelete(HWI_NUM_INT1);
    TestHwiDelete(HWI_NUM_INT0);
    return LOS_OK;

EXIT1:
    TestHwiDelete(HWI_NUM_INT0);
    return LOS_OK;
}

VOID ItLosHwi005(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi005", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL0, TEST_FUNCTION);
}
