/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "securec.h"
#include "los_config.h"
#include "los_memory.h"


void *calloc(size_t nitems, size_t size)
{
    size_t real_size;
    void *ptr = NULL;

    if (nitems == 0 || size == 0) {
        return NULL;
    }

    real_size = (size_t)(nitems * size);
    ptr = LOS_MemAlloc(OS_SYS_MEM_ADDR, real_size);
    if (ptr != NULL) {
        (void)memset_s(ptr, real_size, 0, real_size);
    }
    return ptr;
}

void free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    LOS_MemFree(OS_SYS_MEM_ADDR, ptr);
}

void *malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    return LOS_MemAlloc(OS_SYS_MEM_ADDR, size);
}

void *zalloc(size_t size)
{
    void *ptr = NULL;

    if (size == 0) {
        return NULL;
    }

    ptr = LOS_MemAlloc(OS_SYS_MEM_ADDR, size);
    if (ptr != NULL) {
        (void)memset_s(ptr, size, 0, size);
    }
    return ptr;
}

void *memalign(size_t boundary, size_t size)
{
    if (size == 0) {
        return NULL;
    }

    return LOS_MemAllocAlign(OS_SYS_MEM_ADDR, size, boundary);
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    return LOS_MemRealloc(OS_SYS_MEM_ADDR, ptr, size);
}

