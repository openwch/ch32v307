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


#define LOOP_NUM 10

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    void *p0 = NULL;
    void *f0 = NULL;
    UINT32 size;
    int count;
    int i, j;

    MemInit();

    size = 0x8;
    for (count = 0; count < LOOP_NUM; count++) {
        for (p0 = LOS_MemAlloc(g_memPool, size), f0 = p0, i = 0; p0 != NULL; i++) {
            p0 = LOS_MemAlloc(g_memPool, size);
        }

        if (MemGetFreeSize(g_memPool) >= (size + LOS_DLNK_NODE_HEAD_SIZE)) {
            ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
        }

        for (j = 0; j < i; j++) {
            ret = LOS_MemFree(g_memPool, f0);
            f0 = (void *)((char *)f0 + ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));
            if (ret != LOS_OK) {
                ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
            }
        }
    }

    for (count = 0; count < LOOP_NUM; count++) {
        for (p0 = LOS_MemAlloc(g_memPool, size), i = 0; p0 != NULL; i++) {
            f0 = p0;
            p0 = LOS_MemAlloc(g_memPool, size);
        }

        if (MemGetFreeSize(g_memPool) >= (size + LOS_DLNK_NODE_HEAD_SIZE)) {
            ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
        }

        for (j = 0; j < i; j++) {
            ret = LOS_MemFree(g_memPool, f0);
            f0 = (void *)((char *)f0 - ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));
            if (ret != LOS_OK) {
                ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
            }
        }
    }

    for (count = 0; count < LOOP_NUM; count++) {
        for (p0 = LOS_MemAlloc(g_memPool, size), f0 = p0, i = 0; p0 != NULL; i++) {
            p0 = LOS_MemAlloc(g_memPool, size);
        }

        if (MemGetFreeSize(g_memPool) >= (size + LOS_DLNK_NODE_HEAD_SIZE)) {
            ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
        }

        p0 = f0;
        for (j = 0; j < i; j++) {
            if (!(j % 2)) { // 2, judge j is even number or not.
                ret = LOS_MemFree(g_memPool, f0);
                f0 = (void *)((char *)f0 + 2 * ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE)); // 2, set new size.
                if (ret != LOS_OK) {
                    ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
                }
            }
        }

        f0 = (void *)((char *)p0 + ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));
        for (j = 0; j < i; j++) {
            if (j % 2) { // 2, judge j is odd number or not.
                ret = LOS_MemFree(g_memPool, f0);
                f0 = (void *)((char *)f0 + 2 * ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE)); // 2, set new size.
                if (ret != LOS_OK) {
                    ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
                }
            }
        }
    }

EXIT:
    MemFree();
    return LOS_OK;
}

VOID ItLosMem035(void)
{
    TEST_ADD_CASE("ItLosMem035", TestCase, TEST_LOS, TEST_MEM, TEST_LEVEL3, TEST_PRESSURE);
}

