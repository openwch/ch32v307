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
    UINT32 size;
    int count;
    int i, j, k;
    int sizeadd;

    MemInit();

    for (count = 0; count < LOOP_NUM; count++) {
        i = 0;
        sizeadd = 4; // 4, init sizeadd.
        size = 1 << sizeadd;
        for (p0 = LOS_MemAlloc(g_memPool, size), f0 = p0; p0 != NULL; i++) {
            if ((sizeadd += 1) == 9) { // 9, the limit of sizeadd.
                sizeadd = 4; // 4, set 4 to sizeadd.
            }
            size = 1 << sizeadd;
            p0 = LOS_MemAlloc(g_memPool, size);
        }

        if (MemGetFreeSize(g_memPool) >= (size + LOS_DLNK_NODE_HEAD_SIZE)) {
            ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
        }

        sizeadd = 4; // 4, set 4 to sizeadd.
        size = 1 << sizeadd;
        for (j = 0; j < i; j++) {
            ret = LOS_MemFree(g_memPool, f0);
            f0 = (void *)((char *)f0 + ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));

            if ((sizeadd += 1) == 9) { // 9, the limit of sizeadd.
                sizeadd = 4; // 4, set 4 to sizeadd.
            }

            size = 1 << sizeadd;

            if (ret != LOS_OK) {
                ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
            }
        }
    }

    for (count = 0; count < LOOP_NUM; count++) {
        i = 0;
        sizeadd = 4; // 4, set 4 to sizeadd.
        size = 1 << sizeadd;
        for (p0 = LOS_MemAlloc(g_memPool, size); p0 != NULL; i++) {
            f0 = p0;
            if ((sizeadd += 1) == 9) { // 9, the limit of sizeadd.
                sizeadd = 4; // 4, set 4 to sizeadd.
            }
            size = 1 << sizeadd;
            p0 = LOS_MemAlloc(g_memPool, size);
        }

        if (MemGetFreeSize(g_memPool) >= (size + LOS_DLNK_NODE_HEAD_SIZE)) {
            ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
        }

        if (sizeadd == 4) { // 4, when sizeadd == 4, set 8 to sizeadd.
            sizeadd = 8; // 8, set 8 to sizeadd.
            size = 1 << sizeadd;
        } else {
            sizeadd--;
            size = 1 << sizeadd;
        }

        for (j = 0; j < i; j++) {
            ret = LOS_MemFree(g_memPool, f0);

            if ((sizeadd -= 1) == 3) {  // 3, when (sizeadd -= 1) == 3, set 8 to sizeadd.
                sizeadd = 8; // 8, set 8 to sizeadd.
            }

            size = 1 << sizeadd;
            f0 = (void *)((char *)f0 - ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));

            if (ret != LOS_OK) {
                ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
            }
        }
    }

    for (count = 0; count < LOOP_NUM; count++) {
        i = 0;
        sizeadd = 4; // 4, set 4 to sizeadd.
        size = 1 << sizeadd;
        for (p0 = LOS_MemAlloc(g_memPool, size), f0 = p0; p0 != NULL; i++) {
            if ((sizeadd += 1) == 9) { // 9, the limit of sizeadd.
                sizeadd = 4; // 4, set 4 to sizeadd.
            }
            size = 1 << sizeadd;
            p0 = LOS_MemAlloc(g_memPool, size);
        }

        if (MemGetFreeSize(g_memPool) >= (size + LOS_DLNK_NODE_HEAD_SIZE)) {
            ICUNIT_GOTO_EQUAL(1, 0, i, EXIT);
        }

        p0 = f0;
        sizeadd = 4; // 4, set 4 to sizeadd.
        size = 1 << sizeadd;
        k = 0;
        for (j = 0; j < i; j++) {
            if (!(k % 2)) { // 2, judge k is even number or not.
                ret = LOS_MemFree(g_memPool, f0);
            }

            f0 = (void *)((char *)f0 + ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));
            if ((sizeadd += 1) == 9) { // 9, the limit of sizeadd.
                sizeadd = 4; // 4, set 4 to sizeadd.
            }

            size = 1 << sizeadd;

            if ((k += 1) == 5) { // 5, judge k
                k = 0;
            }

            if (ret != LOS_OK) {
                ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
            }
        }

        f0 = p0;
        sizeadd = 4; // 4, set 4 to sizeadd.
        size = 1 << sizeadd;
        k = 0;
        for (j = 0; j < i; j++) {
            if ((k % 2)) { // 2, judge k is odd number or not.
                ret = LOS_MemFree(g_memPool, f0);
            }

            f0 = (void *)((char *)f0 + ((UINT32)size + LOS_DLNK_NODE_HEAD_SIZE));
            if ((sizeadd += 1) == 9) { // 9, the limit of sizeadd.
                sizeadd = 4; // 4, set 4 to sizeadd.
            }

            size = 1 << sizeadd;

            if ((k += 1) == 5) { // 5, judge k
                k = 0;
            }

            if (ret != LOS_OK) {
                ICUNIT_GOTO_EQUAL(1, 0, j, EXIT);
            }
        }
    }

EXIT:
    MemFree();
    return LOS_OK;
}

VOID ItLosMem036(void)
{
    TEST_ADD_CASE("ItLosMem036", TestCase, TEST_LOS, TEST_MEM, TEST_LEVEL3, TEST_PRESSURE);
}

