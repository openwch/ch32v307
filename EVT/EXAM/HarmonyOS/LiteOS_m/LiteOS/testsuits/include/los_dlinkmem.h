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

#ifndef _LOS_DLINK_MEM_H
#define _LOS_DLINK_MEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (OS_SYS_MEM_CHECK == YES)
#define LOS_DLNK_ENABLE_ALLOC_CHECK
#endif

typedef VOID (*MALLOC_HOOK)(VOID);

extern MALLOC_HOOK g_mallocHook;

/* *
 * @ingroup los_dlinkmem
 * Memory pool information structure
 */
typedef struct {
    void *pPoolAddr;   /* *<Starting address of a memory pool  */
    UINT32 uwPoolSize; /* *<Memory pool size    */
} LOS_DLNK_POOL_INFO;

/* *
 * @ingroup los_dlinkmem
 * Memory linked list node structure
 */
typedef struct tagLOS_DLNK_NODE {
    LOS_DL_LIST stFreeNodeInfo;          /* *<Free memory node  */
    struct tagLOS_DLNK_NODE *pstPreNode; /* *<Pointer to the previous memory node */
    UINT32 uwSizeAndFlag; /* *<Size and flag of the current node (the highest bit represents a flag, and the rest bits
                             specify the size) */
} LOS_DLNK_NODE;

/* *
 * @ingroup los_dlinkmem
 * @brief Initialize dynamic memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to initialize the dynamic memory of a doubly linked list.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>Call this API when dynamic memory needs to be initialized during the startup of HuaweiLite OS.</li>
 * </ul>
 *
 * @param pool           [IN] Starting address of memory.
 * @param size           [IN] Memory size.
 *
 * @retval #OS_ERROR     ¨C1: The dynamic memory fails to be initialized.
 * @retval #LOS_OK       0: The dynamic memory is successfully initialized.
 * @par Dependency:
 * <ul>
 * <li>los_dlinkmem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern UINT32 LOS_DLnkInitMem(VOID *pool, UINT32 size);

/* *
 * @ingroup los_dlinkmem
 * @brief Allocate memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to allocate memory of which the size is specified by size.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>After calling this API, ensure that the returned memory address is not null in case that a null pointer will be
 accessed later.</li>

 * </ul>
 *
 * @param pool    [IN] Starting address of memory.
 * @param size    [IN] Size of the memory to be allocated (unit: byte).
 *
 * @retval Address of the allocated memory. The memory is successfully allocated.
 * @retval NULL. The memory fails to be allocated.
 * @par Dependency:
 * <ul>
 * <li>los_dlinkmem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see LOS_DLnkFreeMem
 */
extern void *LOS_DLnkAllocMem(VOID *pool, UINT32 size);

/* *
 * @ingroup los_dlinkmem
 * @brief Free memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to free the allocated memory.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The memory fails to be freed if it has been already freed.</li>

 * </ul>
 *
 * @param pool       [IN] Starting address of memory.
 * @param mem        [IN] Address of the memory to be freed.
 *
 * @retval #LOS_NOK  1: The memory fails to be freed.
 * @retval #LOS_OK   0: The memory is successfully freed.
 * @par Dependency:
 * <ul>
 * <li>los_dlinkmem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see LOS_DLnkAllocMem
 */
extern UINT32 LOS_DLnkFreeMem(VOID *pool, VOID *mem);

/* *
 * @ingroup los_dlinkmem
 * @brief Re-allocate memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to re-allocate memory if the memory allocated before is insufficient. Data in the original memory
 will be copied to the re-allocated memory, after which the original memory will be freed.</li>

 * </ul>
 * @attention
 * <ul>
 * <li>Before calling this API, check whether the return value of this API is null.</li>
 * <li>If the passed-in address of the original memory is not null, and the passed-in size of the new memory block is 0,
 calling this API frees the original memory.</li>
 * <li>If the passed-in address of the original memory is null, calling this API allocates a new memory block.</li>
 * <li>If the new memory block fails to be allocated, the original one will not be released.</li>
 * </ul>
 *
 * @param pool    [IN] Starting address of memory.
 * @param ptr     [IN] Address of the re-allocated memory.
 * @param size    [IN] Size of the memory to be allocated.
 *
 * @retval Address of the re-allocated memory. The memory is successfully re-allocated.
 * @retval NULL. The memory fails to be re-allocated.
 * @par Dependency:
 * <ul>
 * <li>los_dlinkmem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see LOS_DLnkAllocMem
 */
extern void *LOS_DLnkReAllocMem(VOID *pool, VOID *ptr, UINT32 size);

/* *
 * @ingroup los_dlinkmem
 * @brief Allocate aligned memory.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to allocate memory blocks of specified size and of which the starting addresses are aligned on a
 * specified boundary.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The alignment parameter value must be a power of 2 with the minimum value being 4.</li>
 * </ul>
 *
 * @param pool    [IN] Starting address of memory.
 * @param size    [IN] Size of the memory to be allocated.
 * @param boundary[IN] Boundary on which the memory is aligned.
 *
 * @retval Starting address of the allocated aligned memory. The memory is successfully allocated.
 * @retval The memory fails to be allocated.
 * @par Dependency:
 * <ul>
 * <li>los_dlinkmem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern void *LOS_DlnkAllocMemAlign(void *pool, UINT32 size, UINT32 boundary);

extern UINT32 LOS_DLnkCheckMem(void *pool);
extern UINT32 LOS_DLnkGetTotalMemUsed(void *pool);
extern UINT32 LOS_DLnkGetMemFreeBlks(void *pool);
extern UINT32 LOS_DLnkGetMemUsedBlks(void *pool);
extern UINT32 LOS_DLnkGetMemTskId(void *ptr);

#ifdef OS_MEM_CHECK_DEBUG

/*
 * memcheck error code: the stack have not inited
 * Value: 0x02000100
 * Solution: do memcheck must after stack mem init
 */
#define OS_ERRNO_MEMCHECK_NOT_INIT LOS_ERRNO_OS_ERROR(OS_MOD_MEM, 0x0)
/*
 * memcheck error code: the pPtr is NULL
 * Value: 0x02000101
 * Solution: don't give a NULL parameter
 */
#define OS_ERRNO_MEMCHECK_PARA_NULL LOS_ERRNO_OS_ERROR(OS_MOD_MEM, 0x1)
/*
 * memcheck error code: the pPtr addr not in the suit range
 * Value: 0x02000102
 * Solution: check pPtr and comfirm it included by stack
 */
#define OS_ERRNO_MEMCHECK_OUTSIDE LOS_ERRNO_OS_ERROR(OS_MOD_MEM, 0x2)
/*
 * memcheck error code: can't find the ctrl node
 * Value: 0x02000103
 * Solution: confirm the pPtr if this node has been freed or has not been alloced
 */
#define OS_ERRNO_MEMCHECK_NO_HEAD LOS_ERRNO_OS_ERROR(OS_MOD_MEM, 0x3)
/*
 * memcheck error code: the para level is wrong
 * Value: 0x02000104
 * Solution: checkout the memcheck level by the func mCheck_Level"
 */
#define OS_ERRNO_MEMCHECK_WRONG_LEVEL LOS_ERRNO_OS_ERROR(OS_MOD_MEM, 0x4)
/*
 * memcheck error code: memcheck func not open
 * Value: 0x02000105
 * Solution: enable memcheck by the func "OS_SetMemCheck_Level"
 */
#define OS_ERRNO_MEMCHECK_DISABLED LOS_ERRNO_OS_ERROR(OS_MOD_MEM, 0x5)

#define LOS_MEM_CHECK_LEVEL_LO 0
#define LOS_MEM_CHECK_LEVEL_HI 1
#define LOS_MEM_CHECK_DISABLE 0xff
#define LOS_MEM_CHECK_LEVEL_D OS_ERRNO_MEMCHECK_DISABLED

extern UINT32 LOS_CheckMemSize(VOID *pool, VOID *ptr, UINT32 *totalSize, UINT32 *availSize);
extern UINT32 LOS_SetMemCheck_Level(UINT8 level);
extern UINT8 LOS_GetMemCheck_Level(VOID);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_DLINK_MEM_H */
