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

#include "It_los_mem.h"

void *g_memPool = NULL;
void *g_testPool = NULL;

void MemStart(void)
{
    g_memPool = LOS_MemAlloc(LOSCFG_SYS_HEAP_ADDR, TEST_MEM_SIZE);
    g_testPool = LOS_MemAlloc(LOSCFG_SYS_HEAP_ADDR, TEST_POOL_SIZE);

    if ((g_memPool == NULL) || (g_testPool == NULL)) {
        PRINT_ERR("alloc failed , mem TestCase would be failed!!!!%x !!! %x", g_memPool, g_testPool);
    }
}

void MemEnd(void)
{
    LOS_MemFree(LOSCFG_SYS_HEAP_ADDR, g_memPool);
    LOS_MemFree(LOSCFG_SYS_HEAP_ADDR, g_testPool);
}

void MemInit(void)
{
    (void)LOS_MemInit(g_memPool, TEST_MEM_SIZE);
}

void MemFree(void)
{
    (void)memset_s(g_memPool, TEST_MEM_SIZE, 0, TEST_MEM_SIZE);
    (void)LOS_MemDeInit(g_memPool);
}

UINT32 MemGetFreeSize(void *pool)
{
    return LOS_MemPoolSizeGet(pool) - LOS_MemTotalUsedGet(pool);
}

UINT32 CalPow(UINT32 exp)
{
    UINT32 pw = 1;
    pw <<= exp;
    return pw;
}

VOID ItSuiteLosMem(void)
{
    MemStart();

    ItLosMem001();
    ItLosMem002();
    ItLosMem003();
    ItLosMem004();
    ItLosMem006();
    ItLosMem007();
    ItLosMem008();
    ItLosMem009();
    ItLosMem010();
    ItLosMem011();
    ItLosMem012();
    ItLosMem013();
    ItLosMem014();
    ItLosMem015();
    ItLosMem016();
    ItLosMem017();
    ItLosMem018();
    ItLosMem019();
    ItLosMem020();
#if (LOSCFG_TEST_MUCH_LOG == 1) // when open this， Too many logs will printed
    ItLosMem035();
    ItLosMem036();
    ItLosMem037();
    ItLosMem038();
#endif
    ItLosMem040();
    ItLosMem045();
    ItLosTick001();

    MemEnd();
}
