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
#include "It_los_event.h"

static UINT32 Testcase(VOID)
{
    UINT32 ret;

    ret = LOS_EventWrite(NULL, 0x1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_EVENT_PTR_NULL, ret);

    g_pevent.uwEventID = 0;
    LOS_EventInit(&g_pevent);

    LOS_EventWrite(&g_pevent, 0x1);
    ICUNIT_GOTO_EQUAL(g_pevent.uwEventID, 1, g_pevent.uwEventID, EXIT);

    LOS_EventWrite(&g_pevent, 0);
    ICUNIT_GOTO_EQUAL(g_pevent.uwEventID, 1, g_pevent.uwEventID, EXIT);

    LOS_EventWrite(&g_pevent, 0x10);
    ICUNIT_GOTO_EQUAL(g_pevent.uwEventID, 0x11, g_pevent.uwEventID, EXIT);

    ret = LOS_EventWrite(&g_pevent, 0xFFFFFFFF);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_EVENT_SETBIT_INVALID, ret, EXIT);
    ICUNIT_GOTO_EQUAL(g_pevent.uwEventID, 0x11, g_pevent.uwEventID, EXIT);
EXIT:

    ret = LOS_EventClear(&g_pevent, 0);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_EventDestroy(&g_pevent);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID ItLosEvent005(VOID)
{
    TEST_ADD_CASE("ItLosEvent005", Testcase, TEST_LOS, TEST_EVENT, TEST_LEVEL0, TEST_FUNCTION);
}
