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
#include "It_los_mem.h"


static UINT32 TestCase(VOID)
{
    UINT32 ret;
    UINT32 size = 0x500;

    void *p[(TEST_MEM_SIZE) / 0x500] = {NULL};
    void *p0 = NULL;
    int i = 0;

    MemInit();

    for (p0 = LOS_MemAlloc(g_memPool, size); p0 != NULL; i++) {
        p[i] = p0;
        p0 = LOS_MemAlloc(g_memPool, size);
    }

    if (MemGetFreeSize(g_memPool) >= size + LOS_MEM_NODE_HEAD_SIZE + LOS_MEM_POOL_SIZE) {
        ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
    }

    ret = LOS_MemFree(g_memPool, p[0]);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_MemFree(g_memPool, p[2]); // 2, release the secend node.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    size = size * 3; // 3, Set new size to 3 times the previous one.
    p0 = LOS_MemAlloc(g_memPool, size);
    ICUNIT_GOTO_EQUAL(p0, NULL, p0, EXIT);

    ret = LOS_MemFree(g_memPool, p[1]);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    p0 = LOS_MemAlloc(g_memPool, size);
    ICUNIT_GOTO_EQUAL(p0, p[0], p0, EXIT);

    ret = LOS_MemFree(g_memPool, p0);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

EXIT:
    MemFree();
    return LOS_OK;
}

VOID ItLosMem015(void)
{
    TEST_ADD_CASE("ItLosMem015", TestCase, TEST_LOS, TEST_MEM, TEST_LEVEL1, TEST_FUNCTION);
}

