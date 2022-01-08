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


#define LOOP_NUM 500

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    void *p0 = NULL;
    void *f0 = NULL;
    UINT32 size, sizeadd;
    int count;
    UINT32 freesize[2] = {0}; // 2, array size.

    MemInit();

    freesize[0] = MemGetFreeSize(g_memPool);

    size = 0x8;
    for (count = 0; count < LOOP_NUM; count++) {
        p0 = LOS_MemAlloc(g_memPool, size);
        ICUNIT_GOTO_NOT_EQUAL(p0, NULL, count, EXIT);

        f0 = LOS_MemRealloc(g_memPool, p0, size / 2); // 2, The reallocated memory is half of its previous size.
        ICUNIT_GOTO_NOT_EQUAL(f0, NULL, count, EXIT);

        f0 = LOS_MemRealloc(g_memPool, p0, size * 2); // 2, The reallocated memory is 2 times of its previous size.
        ICUNIT_GOTO_NOT_EQUAL(f0, NULL, count, EXIT);

        ret = LOS_MemFree(g_memPool, f0);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }

    freesize[1] = MemGetFreeSize(g_memPool);
    ICUNIT_GOTO_EQUAL(freesize[1], freesize[0], freesize[1], EXIT);

    size = 0x800;
    for (count = 0; count < LOOP_NUM; count++) {
        p0 = LOS_MemAlloc(g_memPool, size);
        ICUNIT_GOTO_NOT_EQUAL(p0, NULL, count, EXIT);

        f0 = LOS_MemRealloc(g_memPool, p0, size / 2); // 2, The reallocated memory is half of its previous size.
        ICUNIT_GOTO_NOT_EQUAL(f0, NULL, count, EXIT);

        f0 = LOS_MemRealloc(g_memPool, p0, size * 2); // 2, The reallocated memory is 2 times of its previous size.
        ICUNIT_GOTO_NOT_EQUAL(f0, NULL, count, EXIT);

        ret = LOS_MemFree(g_memPool, f0);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }

    freesize[1] = MemGetFreeSize(g_memPool);
    ICUNIT_GOTO_EQUAL(freesize[1], freesize[0], freesize[1], EXIT);

    sizeadd = 4; // 4, set 4 to sizeadd.
    while (1) {
        size = 1 << sizeadd;
        for (count = 0; count < LOOP_NUM; count++) {
            p0 = LOS_MemAlloc(g_memPool, size);
            ICUNIT_GOTO_NOT_EQUAL(p0, NULL, count, EXIT);

            f0 = LOS_MemRealloc(g_memPool, p0, size / 2); // 2, The reallocated memory is half of its previous size.
            ICUNIT_GOTO_NOT_EQUAL(f0, NULL, count, EXIT);

            f0 = LOS_MemRealloc(g_memPool, p0, size * 2); // 2, The reallocated memory is 2 times of its previous size.
            ICUNIT_GOTO_NOT_EQUAL(f0, NULL, count, EXIT);

            ret = LOS_MemFree(g_memPool, f0);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
        }

        sizeadd++;
        if (sizeadd == 9) { // 9, the limit of sizeadd.
            break;
        }
    }

    freesize[1] = MemGetFreeSize(g_memPool);
    ICUNIT_GOTO_EQUAL(freesize[1], freesize[0], freesize[1], EXIT);

EXIT:
    MemFree();
    return LOS_OK;
}

VOID ItLosMem037(void)
{
    TEST_ADD_CASE("ItLosMem037", TestCase, TEST_LOS, TEST_MEM, TEST_LEVEL3, TEST_PRESSURE);
}

