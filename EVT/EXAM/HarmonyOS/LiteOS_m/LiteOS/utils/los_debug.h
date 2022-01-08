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

/**@defgroup los_debug
 * @ingroup kernel
 */

#ifndef _LOS_DEBUG_H
#define _LOS_DEBUG_H

#include "los_config.h"
#include "los_compiler.h"
#include "stdio.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_PLATFORM_EXC == 1)
enum MemMangType {
    MEM_MANG_MEMBOX,
    MEM_MANG_MEMORY,
    MEM_MANG_EMPTY
};

typedef struct {
    UINT32 type;
    UINT32 startAddr;
    UINT32 size;
    VOID *blkAddrArray;
} MemInfo;

typedef struct {
    enum MemMangType type;
    UINT32 startAddr;
    UINT32 size;
    UINT32 free;
    UINT32 blockSize;
    UINT32 errorAddr;
    UINT32 errorLen;
    UINT32 errorOwner;
} MemInfoCB;
#endif

typedef enum {
    EXC_REBOOT,
    EXC_ASSERT,
    EXC_STACKOVERFLOW,
    EXC_INTERRUPT,
    EXC_TYPE_END
} EXC_TYPE;

typedef VOID (*ExcHookFn)(EXC_TYPE excType);

VOID OsExcHookRegister(ExcHookFn excHookFn);

VOID OsDoExcHook(EXC_TYPE excType);

#define LOG_EMG_LEVEL       0

#define LOG_COMMON_LEVEL    (LOG_EMG_LEVEL + 1)

#define LOG_ERR_LEVEL       (LOG_COMMON_LEVEL + 1)

#define LOG_WARN_LEVEL      (LOG_ERR_LEVEL + 1)

#define LOG_INFO_LEVEL      (LOG_WARN_LEVEL + 1)

#define LOG_DEBUG_LEVEL     (LOG_INFO_LEVEL + 1)

#ifndef PRINT_LEVEL
#define PRINT_LEVEL         LOG_ERR_LEVEL
#endif

typedef enum {
    LOG_MODULE_KERNEL,
    LOG_MODULE_FS,
    LOS_MODULE_OTHERS
} LogModuleType;

/**
 * @ingroup los_printf
 * @brief Format and print data.
 *
 * @par Description:
 * Print argument(s) according to fmt.
 *
 * @attention
 * <ul>
 * <li>None</li>
 * </ul>
 *
 * @param type  [IN] Type LogModuleType indicates the log type.
 * @param level [IN] Type LogLevel indicates the log level.
 * @param fmt   [IN] Type char* controls the ouput as in C printf.
 *
 * @retval None
 * @par Dependency:
 * <ul><li>los_printf.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_Printf
 * @since none
 */
#if (LOSCFG_KERNEL_PRINTF == 1)
extern INT32 printf(const CHAR *fmt, ...);
extern INT32 OsLogLevelCheck(INT32 level);
#define LOS_Printf(type, level, fmt, args...)   do { \
    if (!OsLogLevelCheck(level)) {                   \
        printf(fmt, ##args);                         \
    }                                                \
} while (0)
#elif (LOSCFG_KERNEL_PRINTF == 0)
#define LOS_Printf(type, level, fmt, args...)
#else
extern VOID HalConsoleOutput(LogModuleType type, INT32 level, const CHAR *fmt, ...);
#define LOS_Printf HalConsoleOutput
#endif

#define PRINT_DEBUG(fmt, args...)    LOS_Printf(LOG_MODULE_KERNEL, LOG_DEBUG_LEVEL, fmt, ##args)
#define PRINT_INFO(fmt, args...)     LOS_Printf(LOG_MODULE_KERNEL, LOG_INFO_LEVEL, fmt, ##args)
#define PRINT_WARN(fmt, args...)     LOS_Printf(LOG_MODULE_KERNEL, LOG_WARN_LEVEL, fmt, ##args)
#define PRINT_ERR(fmt, args...)      LOS_Printf(LOG_MODULE_KERNEL, LOG_ERR_LEVEL, fmt, ##args)
#define PRINTK(fmt, args...)         LOS_Printf(LOG_MODULE_KERNEL, LOG_COMMON_LEVEL, fmt, ##args)
#define PRINT_EMG(fmt, args...)      LOS_Printf(LOG_MODULE_KERNEL, LOG_EMG_LEVEL, fmt, ##args)

#if PRINT_LEVEL < LOG_ERR_LEVEL
#define LOS_ASSERT(judge)
#else
#define LOS_ASSERT(judge)                                                          \
    do {                                                                           \
        if ((judge) == 0) {                                                        \
            OsDoExcHook(EXC_ASSERT);                                               \
            (VOID)LOS_IntLock();                                                   \
            PRINT_ERR("ASSERT ERROR! %s, %d, %s\n", __FILE__, __LINE__, __func__); \
            while (1) { }                                                          \
        }                                                                          \
    } while (0)
#endif

typedef VOID (*BACK_TRACE_HOOK)(UINTPTR *LR, UINT32 LRSize, UINT32 jumpCount, UINTPTR SP);
extern VOID OsBackTraceHookSet(BACK_TRACE_HOOK hook);
extern VOID OsBackTraceHookCall(UINTPTR *LR, UINT32 LRSize, UINT32 jumpCount, UINTPTR SP);

/**
 * @ingroup los_trace
 * Task error code: Insufficient memory for trace struct.
 *
 * Value: 0x02001400
 *
 * Solution: Decrease the maximum number of tasks.
 */
#define  LOS_ERRNO_TRACE_NO_MEMORY              LOS_ERRNO_OS_ERROR(LOS_MOD_TRACE, 0x00)

/**
 * @ingroup los_trace
 * Task error code: User type is invalid when register new trace.
 *
 * Value: 0x02001401
 *
 * Solution: Use valid type to register the new trace.
 */
#define  LOS_ERRNO_TRACE_TYPE_INVALID           LOS_ERRNO_OS_ERROR(LOS_MOD_TRACE, 0x01)


/**
 * @ingroup los_trace
 * Task error code: The callback function is null when register new trace.
 *
 * Value: 0x02001402
 *
 * Solution: Use valid callback function to register the new trace.
 */
#define  LOS_ERRNO_TRACE_FUNCTION_NULL          LOS_ERRNO_OS_ERROR(LOS_MOD_TRACE, 0x02)

/**
 * @ingroup los_trace
 * Task error code: The filled size is 0 when register new trace.
 *
 * Value: 0x02001403
 *
 * Solution: Use valid filled size to register the new trace.
 */
#define  LOS_ERRNO_TRACE_MAX_SIZE_INVALID       LOS_ERRNO_OS_ERROR(LOS_MOD_TRACE, 0x03)

/**
 * @ingroup los_trace
 * It's the total size of trace buffer. It's in the unit of char
 */
#define LOS_TRACE_BUFFER_SIZE 1024

/**
 * @ingroup los_trace
 * It's the length of tag, filled by los_trace system
 */
#define LOS_TRACE_TAG_LENGTH  sizeof(UINT32)

/**
 * @ingroup los_trace
 * Stands for the trace type can be registered.
 */
typedef enum enTraceType {
    LOS_TRACE_SWITCH = 0,         /**< trace for task switch, 0 is reserved for taskswitch   */
    LOS_TRACE_INTERRUPT = 1,      /**< trace for Interrupt, 1 is reserved for interrupt      */
    LOS_TRACE_TYPE_NUM =  10,     /**< num for the register type, user can use 2~ LOS_TRACE_TYPE_NUM-1 */
} TRACE_TYPE_E;


/**
 * @ingroup los_trace
 * struct to store the task switch infomation
 */
typedef struct tagTraceTaskSwitch {
    UINT8  ucSrcTaskId;           /**< source taskId    */
    UINT8  ucDestTaskId;          /**< destination taskId    */
    UINT32 uwSwitchTick;          /**< Time at which the task switch happens    */
} TRACE_TASKSWITCH_S;

/**
 * @ingroup los_trace
 * struct to store the interrupt infomation
 */
typedef struct tagTraceInterrupt {
    UINT8  ucIRQEntryExit;        /**< 1 stands for that the trace is happend before interrupt function */
                                  /**< 0 stands for that the trace is after before interrupt function */
    UINT16 usIRQNo;               /**< IRQ number which trigger the interrupt     */
    UINT32 uwTick;                /**< Time at which the the trace is called    */
} TRACE_INTERRUPT_S;

/**
 * @ingroup los_trace
 * union struct to store the interrupt and task switch infomation
 */
typedef struct tagTrace {
    union {
        TRACE_TASKSWITCH_S  stTraceTask;       /**< It used for trace the task    */
        TRACE_INTERRUPT_S   stTraceInterrupt;  /**< It used for trace the interrupt    */
    };
} TRACE_S;

/**
 * @ingroup los_trace
 * Main struct to store the interrupt and task switch infomation
 */
typedef struct tagTraceBuffer {
    UINT16            usTracePos;
    UINT16            usTraceWrapPos;
    UINT8             ucBuffer[LOS_TRACE_BUFFER_SIZE];
} TRACE_BUFFER_S;


/**
 * @ingroup los_trace
 * Struct to store the trace information for each trace type
 */
typedef struct tagTraceInfo {
    TRACE_TYPE_E  eType;                             /**< trace type, selected from TRACE_TYPE_E */
    UINT16 (*pfnHook)(UINT8 *outBuffer, VOID *pInfo); /**< callback function for the specific trace type.
                                                       * This function is used to store the infomation
                                                       * which want to be traced */
} TRACE_INFO_S;

/**
 * @ingroup los_trace
 * @brief  Initialize the trace when the system startup.
 *
 * @par Description:
 * This API is used to initialize the trace for system level.
 * @attention
 * <ul>
 * <li>This API can be called only after the memory is initialized. Otherwise, the CPU usage fails to be obtained.</li>
 * </ul>
 *
 * @param None.
 *
 * @retval #LOS_ERRNO_TRACE_NO_MEMORY         0x02001400: The memory is not enough for initialization.
 * @retval #LOS_OK                            0x00000000: The initialization is successful.
 * @par Dependency:
 * <ul><li>los_trace.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TraceInit
 */
extern UINT32 LOS_TraceInit(VOID);


/**
 * @ingroup los_trace
 * @brief main trace function is called by user to logger the information.
 *
 * @par Description:
 * This API is used to trace the infomation.
 * @attention
 * <ul>
 * <li>This API can be called only after trace type is intialized. Otherwise, the trace will be failed.</li>
 * </ul>
 *
 * @param  traceType     [IN] TRACE_TYPE_E. Type of trace information.
 * @param  traceInfo     [IN] VOID*. It's a input buffer to store trace infomation
 *
 * @retval NONE.
 *
 * @par Dependency:
 * <ul><li>los_trace.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_Trace
 */
extern VOID LOS_Trace(TRACE_TYPE_E traceType, VOID *traceInfo);



/**
 * @ingroup los_trace
 * @brief register the hook for specific trace type.
 *
 * @par Description:
 * This API is used to register the hook for specific trace type.
 * @attention
 * <ul>
 * <li>This API can be called only after trace type is called. therwise, the trace will be failed.</li>
 * </ul>
 *
 * @param  traceType     [IN] TRACE_TYPE_E. Type of trace information.
 * @param  hook          [IN] UINT16 (*)(UINT8*, VOID*). It's an callback function to store the useful trace
 *                            information
 * @param  size          [IN] UINT16. The maximum size the trace will use for the specific trace type.
 *
 * @retval #LOS_ERRNO_TRACE_NO_MEMORY          0x02001400: The memory is not enough for initialization.
 * @retval #LOS_ERRNO_TRACE_TYPE_INVALID       0x02001401: The trace type is invalid. Valid type is from
 *                                                         LOS_TRACE_TYPE_NUM-1
 * @retval #LOS_ERRNO_TRACE_FUNCTION_NULL      0x02001402: The input callback function is NULL
 * @retval #LOS_ERRNO_TRACE_MAX_SIZE_INVALID   0x02001403: The information maxmum size is 0 to store.
 * @retval #LOS_OK                             0x00000000: The register is successful.
 *
 * @par Dependency:
 * <ul><li>los_trace.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TraceUserReg
 */
extern UINT32 LOS_TraceUserReg(TRACE_TYPE_E traceType, UINT16 (*hook)(UINT8 *, VOID *), UINT16 size);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_PRINTF_H */
