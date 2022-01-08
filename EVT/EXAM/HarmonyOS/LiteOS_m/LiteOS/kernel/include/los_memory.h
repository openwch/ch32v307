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

/**
 * @defgroup los_memory  Dynamic memory
 * @ingroup kernel
 */

#ifndef _LOS_MEMORY_H
#define _LOS_MEMORY_H

#include "los_config.h"
#include "los_list.h"
#include "los_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_PLATFORM_EXC == 1)
UINT32 OsMemExcInfoGet(UINT32 memNumMax, MemInfoCB *memExcInfo);
#endif

/**
 * @ingroup los_memory
 * Starting address of the memory.
 */
#define OS_SYS_MEM_ADDR     LOSCFG_SYS_HEAP_ADDR

#if (LOSCFG_MEM_LEAKCHECK == 1)
/**
 * @ingroup los_memory
 * @brief Print function call stack information of all used nodes.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to print function call stack information of all used nodes.</li>
 * </ul>
 *
 * @param pool          [IN] Starting address of memory.
 *
 * @retval none.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern VOID LOS_MemUsedNodeShow(VOID *pool);
#endif

#if (LOSCFG_MEM_MUL_POOL == 1)
/**
 * @ingroup los_memory
 * @brief Deinitialize dynamic memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to deinitialize the dynamic memory of a doubly linked list.</li>
 * </ul>
 *
 * @param pool          [IN] Starting address of memory.
 *
 * @retval #OS_ERROR   The dynamic memory fails to be deinitialized.
 * @retval #LOS_OK     The dynamic memory is successfully deinitialized.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern UINT32 LOS_MemDeInit(VOID *pool);

/**
 * @ingroup los_memory
 * @brief Print infomation about all pools.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to print infomation about all pools.</li>
 * </ul>
 *
 * @retval #UINT32   The pool number.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern UINT32 LOS_MemPoolList(VOID);
#endif

/**
 * @ingroup los_memory
 * Memory pool extern information structure
 */
typedef struct {
    UINT32 totalUsedSize;
    UINT32 totalFreeSize;
    UINT32 maxFreeNodeSize;
    UINT32 usedNodeNum;
    UINT32 freeNodeNum;
#if (LOSCFG_MEM_WATERLINE == 1)
    UINT32 usageWaterLine;
#endif
} LOS_MEM_POOL_STATUS;

/**
 * @ingroup los_memory
 * @brief Initialize dynamic memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to initialize the dynamic memory of a doubly linked list.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The size parameter value should match the following two conditions :
 * 1) Be less than or equal to the Memory pool size;
 * 2) Be greater than the size of OS_MEM_MIN_POOL_SIZE.</li>
 * <li>Call this API when dynamic memory needs to be initialized during the startup of Huawei LiteOS.</li>
 * <li>The parameter input must be four byte-aligned.</li>
 * <li>The init area [pool, pool + size] should not conflict with other pools.</li>
 * </ul>
 *
 * @param pool         [IN] Starting address of memory.
 * @param size         [IN] Memory size.
 *
 * @retval #OS_ERROR   The dynamic memory fails to be initialized.
 * @retval #LOS_OK     The dynamic memory is successfully initialized.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern UINT32 LOS_MemInit(VOID *pool, UINT32 size);

/**
 * @ingroup los_memory
 * @brief Allocate dynamic memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to allocate a memory block of which the size is specified.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of LOS_MemInit.</li>
 * <li>The size of the input parameter size must be four byte-aligned.</li>
 * </ul>
 *
 * @param  pool    [IN] Pointer to the memory pool that contains the memory block to be allocated.
 * @param  size    [IN] Size of the memory block to be allocated (unit: byte).
 *
 * @retval #NULL          The memory fails to be allocated.
 * @retval #VOID*         The memory is successfully allocated with the starting address of the allocated memory block
 *                        returned.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemRealloc | LOS_MemAllocAlign | LOS_MemFree
 */
extern VOID *LOS_MemAlloc(VOID *pool, UINT32 size);

/**
 * @ingroup los_memory
 * @brief Free dynamic memory.
 *
 * @par Description:
 * <li>This API is used to free specified dynamic memory that has been allocated.</li>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The input ptr parameter must be allocated by LOS_MemAlloc or LOS_MemAllocAlign or LOS_MemRealloc.</li>
 * </ul>
 *
 * @param  pool  [IN] Pointer to the memory pool that contains the dynamic memory block to be freed.
 * @param  ptr   [IN] Starting address of the memory block to be freed.
 *
 * @retval #LOS_NOK          The memory block fails to be freed because the starting address of the memory block is
 *                           invalid, or the memory overwriting occurs.
 * @retval #LOS_OK           The memory block is successfully freed.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemAlloc | LOS_MemRealloc | LOS_MemAllocAlign
 */
extern UINT32 LOS_MemFree(VOID *pool, VOID *ptr);

/**
 * @ingroup los_memory
 * @brief Re-allocate a memory block.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to allocate a new memory block of which the size is specified by size if the original memory
 * block size is insufficient. The new memory block will copy the data in the original memory block of which the
 * address is specified by ptr. The size of the new memory block determines the maximum size of data to be copied.
 * After the new memory block is created, the original one is freed.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The input ptr parameter must be allocated by LOS_MemAlloc or LOS_MemAllocAlign.</li>
 * <li>The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of LOS_MemInit.</li>
 * <li>The size of the input parameter size must be aligned as follows: 1) if the ptr is allocated by LOS_MemAlloc,
 * it must be four byte-aligned; 2) if the ptr is allocated by LOS_MemAllocAlign, it must be aligned with the size of
 * the input parameter boundary of LOS_MemAllocAlign.</li>
 * </ul>
 *
 * @param  pool     [IN] Pointer to the memory pool that contains the original and new memory blocks.
 * @param  ptr      [IN] Address of the original memory block.
 * @param  size     [IN] Size of the new memory block.
 *
 * @retval #NULL    The memory fails to be re-allocated.
 * @retval #VOID*   The memory is successfully re-allocated with the starting address of the new memory block returned.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemAlloc | LOS_MemAllocAlign | LOS_MemFree
 */
extern VOID *LOS_MemRealloc(VOID *pool, VOID *ptr, UINT32 size);

/**
 * @ingroup los_memory
 * @brief Allocate aligned memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to allocate memory blocks of specified size and of which the starting addresses are aligned on
 * a specified boundary.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of LOS_MemInit.</li>
 * <li>The alignment parameter value must be a power of 2 with the minimum value being 4.</li>
 * </ul>
 *
 * @param  pool      [IN] Pointer to the memory pool that contains the memory blocks to be allocated.
 * @param  size      [IN] Size of the memory to be allocated.
 * @param  boundary  [IN] Boundary on which the memory is aligned.
 *
 * @retval #NULL    The memory fails to be allocated.
 * @retval #VOID*   The memory is successfully allocated with the starting address of the allocated memory returned.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MemAlloc | LOS_MemRealloc | LOS_MemFree
 */
extern VOID *LOS_MemAllocAlign(VOID *pool, UINT32 size, UINT32 boundary);

/**
 * @ingroup los_memory
 * @brief Get the size of memory pool's size.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get the size of memory pool' total size.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * </ul>
 *
 * @param  pool           [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK        The incoming parameter pool is NULL.
 * @retval #UINT32         The size of the memory pool.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 LOS_MemPoolSizeGet(const VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the size of memory totally used.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get the size of memory totally used in memory pool.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * </ul>
 *
 * @param  pool           [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK        The incoming parameter pool is NULL.
 * @retval #UINT32         The size of the memory pool used.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 LOS_MemTotalUsedGet(VOID *pool);

/**
 * @ingroup los_memory
 * @brief Get the infomation of memory pool.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get the infomation of memory pool.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * </ul>
 *
 * @param  pool                 [IN] A pointer pointed to the memory pool.
 * @param  poolStatus           [IN] A pointer for storage the pool status
 *
 * @retval #LOS_NOK           The incoming parameter pool is NULL or invalid.
 * @retval #LOS_OK            Success to get memory infomation.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 LOS_MemInfoGet(VOID *pool, LOS_MEM_POOL_STATUS *poolStatus);

/**
 * @ingroup los_memory
 * @brief Get the number of free node in every size.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get the number of free node in every size.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * </ul>
 *
 * @param  pool               [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK           The incoming parameter pool is NULL.
 * @retval #UINT32            The address of the last used node that casts to UINT32.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 LOS_MemFreeNodeShow(VOID *pool);

/**
 * @ingroup los_memory
 * @brief Check the memory pool integrity.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to check the memory pool integrity.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input pool parameter must be initialized via func LOS_MemInit.</li>
 * <li>LOS_MemIntegrityCheck will be called by malloc function when the macro of LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK
 * is defined in LiteOS.</li>
 * <li>LOS_MemIntegrityCheck function can be called by user anytime.</li>
 * </ul>
 *
 * @param  pool              [IN] A pointer pointed to the memory pool.
 *
 * @retval #LOS_NOK           The memory pool (pool) is impaired.
 * @retval #LOS_OK            The memory pool (pool) is integrated.
 * @par Dependency:
 * <ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 LOS_MemIntegrityCheck(const VOID *pool);

/**
 * @ingroup los_memory
 * @brief Enable memory pool to support no internal lock during using interfaces.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to enable memory pool to support no internal lock during using interfaces,</li>
 * <li>such as LOS_MemAlloc/LOS_MemAllocAlign/LOS_MemRealloc/LOS_MemFree and so on.
 * </ul>
 * @attention
 * <ul>
 * <li>The memory pool does not support multi-threaded concurrent application scenarios.
 * <li>If you want to use this function, you need to call this interface before the memory
 * pool is used, it cannot be called during the trial period.</li>
 * </ul>
 *
 * @param pool         [IN] Starting address of memory.
 *
 * @retval node.
 * @par Dependency:
 * <ul>
 * <li>los_memory.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern VOID LOS_MemUnlockEnable(VOID *pool);

extern UINT32 OsMemSystemInit(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_MEMORY_H */
