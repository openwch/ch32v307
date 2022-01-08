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


static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 muxHandle;

    ret = LOS_MuxCreate(&muxHandle);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxPend(muxHandle, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxPend(muxHandle, 1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxDelete(muxHandle);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_MUX_PENDED, ret);

    ret = LOS_MuxPost(muxHandle);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxDelete(muxHandle);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_MUX_PENDED, ret);

    ret = LOS_MuxPost(muxHandle);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxDelete(muxHandle);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxPend(muxHandle, 0);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_MUX_INVALID, ret);

    ret = LOS_MuxPend(LOSCFG_BASE_IPC_MUX_LIMIT - 1, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_MUX_INVALID, ret);

    ret = LOS_MuxPend(LOSCFG_BASE_IPC_MUX_LIMIT, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_MUX_INVALID, ret);

    return LOS_OK;
}


VOID ItLosMux004(void)
{
    TEST_ADD_CASE("ItLosMux004", Testcase, TEST_LOS, TEST_MUX, TEST_LEVEL0, TEST_FUNCTION);
}

