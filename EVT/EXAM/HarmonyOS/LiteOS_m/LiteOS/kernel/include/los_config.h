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
 * @defgroup los_config System configuration items
 * @ingroup kernel
 */

#ifndef _LOS_CONFIG_H
#define _LOS_CONFIG_H

#include "target_config.h"
#include "los_compiler.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* =============================================================================
                                        System clock module configuration
============================================================================= */
/**
 * @ingroup los_config
 * System clock (unit: HZ)
 */
#ifndef OS_SYS_CLOCK
    #error "OS_SYS_CLOCK is system clock rate which should be defined in target_config.h"
#endif

/**
 * @ingroup los_config
 * Number of Ticks in one second
 */
#ifndef LOSCFG_BASE_CORE_TICK_PER_SECOND
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (100UL)
#endif

#if defined(LOSCFG_BASE_CORE_TICK_PER_SECOND) && \
    ((LOSCFG_BASE_CORE_TICK_PER_SECOND < 1UL) || (LOSCFG_BASE_CORE_TICK_PER_SECOND > 1000000000UL))
    #error "LOSCFG_BASE_CORE_TICK_PER_SECOND SHOULD big than 0, and less than 1000000000UL"
#endif


#if (LOSCFG_BASE_CORE_TICK_PER_SECOND <= 1000UL)
/**
 * @ingroup los_config
 * How much time one tick spent (unit:ms)
 */
#ifndef LOSCFG_BASE_CORE_TICK_PERIOD_MS
#define LOSCFG_BASE_CORE_TICK_PERIOD_MS                     (1000UL / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#endif

#elif (LOSCFG_BASE_CORE_TICK_PER_SECOND <= 1000000UL)
/**
 * @ingroup los_config
 * How much time one tick spent (unit:us)
 */
#ifndef LOSCFG_BASE_CORE_TICK_PERIOD_US
#define LOSCFG_BASE_CORE_TICK_PERIOD_US                     (1000000UL / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#endif

#else
/**
 * @ingroup los_config
 * How much time one tick spent (unit:ns)
 */
#ifndef LOSCFG_BASE_CORE_TICK_PERIOD_NS
#define LOSCFG_BASE_CORE_TICK_PERIOD_NS                     (1000000000UL / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#endif
#endif

#ifndef LOSCFG_BASE_CORE_TICK_HW_TIME
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0
#endif

/**
 * @ingroup los_config
 * System timer is a 64/128 bit timer
 */
#ifndef LOSCFG_BASE_CORE_TICK_WTIMER
#define LOSCFG_BASE_CORE_TICK_WTIMER                        0
#endif

/**
 * @ingroup los_config
 * System timer count maximum
 */
#ifndef LOSCFG_BASE_CORE_TICK_RESPONSE_MAX
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                       0
#endif

/* =============================================================================
                                        Hardware interrupt module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for hardware interrupt tailoring
 */
#ifndef LOSCFG_PLATFORM_HWI
#define LOSCFG_PLATFORM_HWI                                 1
#endif

/**
 * @ingroup los_config
 * Configuration item for using system defined vector base address and interrupt handlers.
 * If LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT is set to 0, vector base address will not be
 * modified by system. In arm, it should be noted that PendSV_Handler and SysTick_Handler should
 * be redefined to HalPendSV and OsTickHandler respectly in this case, because system depends on
 * these interrupt handlers to run normally. What's more, LOS_HwiCreate will not register handler.
 */
#ifndef LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 0
#endif

#if (LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT == 1)
    #if (LOSCFG_PLATFORM_HWI == 0)
        #error "if LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT is set to 1, then LOSCFG_PLATFORM_HWI must also be set to 1"
    #endif
#endif

/**
 * @ingroup los_config
 * Maximum number of used hardware interrupts, including Tick timer interrupts.
 */
#ifndef LOSCFG_PLATFORM_HWI_LIMIT
#define LOSCFG_PLATFORM_HWI_LIMIT                           32
#endif

/* =============================================================================
                                       Task module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Minimum stack size.
 *
 * 0x80 bytes, aligned on a boundary of 8.
 */
#ifndef LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (ALIGN(0x80, 4))
#endif

/**
 * @ingroup los_config
 * Default task priority
 */
#ifndef LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO
#define LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO                   10
#endif

/**
 * @ingroup los_config
 * Maximum supported number of tasks except the idle task rather than the number of usable tasks
 */
#ifndef LOSCFG_BASE_CORE_TSK_LIMIT
#define LOSCFG_BASE_CORE_TSK_LIMIT                          5
#endif

/**
 * @ingroup los_config
 * Size of the idle task stack
 */
#ifndef LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                0x180UL
#endif

/**
 * @ingroup los_config
 * Default task stack size
 */
#ifndef LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             0x400UL
#endif

/**
 * @ingroup los_config
 * Configuration item for task Robin tailoring
 */
#ifndef LOSCFG_BASE_CORE_TIMESLICE
#define LOSCFG_BASE_CORE_TIMESLICE                         1
#endif

/**
 * @ingroup los_config
 * Longest execution time of tasks with the same priorities
 */
#ifndef LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  20000 /* 20ms */
#endif

/**
 * @ingroup los_config
 * Configuration item for task (stack) monitoring module tailoring
 */
#ifndef LOSCFG_BASE_CORE_TSK_MONITOR
#define LOSCFG_BASE_CORE_TSK_MONITOR                        0
#endif

/**
 * @ingroup los_config
 * Configuration item for task perf task filter hook
 */
#ifndef LOSCFG_BASE_CORE_EXC_TSK_SWITCH
#define LOSCFG_BASE_CORE_EXC_TSK_SWITCH                     0
#endif

/**
 * @ingroup los_config
 * Define a usable task priority.Highest task priority.
 */
#ifndef LOS_TASK_PRIORITY_HIGHEST
#define LOS_TASK_PRIORITY_HIGHEST                           0
#endif

/**
 * @ingroup los_config
 * Define a usable task priority.Lowest task priority.
 */
#ifndef LOS_TASK_PRIORITY_LOWEST
#define LOS_TASK_PRIORITY_LOWEST                            31
#endif

/**
 * @ingroup los_config
 * Configuration item for task stack independent
 */
#ifndef LOSCFG_BASE_CORE_TASKSTACK_INDEPENDENT
#define LOSCFG_BASE_CORE_TASKSTACK_INDEPENDENT              0
#endif

/**
 * @ingroup los_config
 * SP align size
 */
#ifndef LOSCFG_STACK_POINT_ALIGN_SIZE
#define LOSCFG_STACK_POINT_ALIGN_SIZE                       8
#endif

/* =============================================================================
                                       Semaphore module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for semaphore module tailoring
 */
#ifndef LOSCFG_BASE_IPC_SEM
#define LOSCFG_BASE_IPC_SEM                                 1
#endif

/**
 * @ingroup los_config
 * Maximum supported number of semaphores
 */
#ifndef LOSCFG_BASE_IPC_SEM_LIMIT
#define LOSCFG_BASE_IPC_SEM_LIMIT                           6
#endif

/**
 * @ingroup los_config
 * Maximum number of semaphores.
 */
#ifndef OS_SEM_COUNTING_MAX_COUNT
#define OS_SEM_COUNTING_MAX_COUNT                           0xFFFF
#endif

/* =============================================================================
                                       Mutex module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for mutex module tailoring
 */
#ifndef LOSCFG_BASE_IPC_MUX
#define LOSCFG_BASE_IPC_MUX                                 1
#endif

/**
 * @ingroup los_config
 * Maximum supported number of mutexes
 */
#ifndef LOSCFG_BASE_IPC_MUX_LIMIT
#define LOSCFG_BASE_IPC_MUX_LIMIT                           6
#endif

/* =============================================================================
                                       Queue module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for queue module tailoring
 */
#ifndef LOSCFG_BASE_IPC_QUEUE
#define LOSCFG_BASE_IPC_QUEUE                               1
#endif

/**
 * @ingroup los_config
 * Maximum supported number of queues rather than the number of usable queues
 */
#ifndef LOSCFG_BASE_IPC_QUEUE_LIMIT
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         6
#endif


/* =============================================================================
                                       Software timer module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for software timer module tailoring
 */
#ifndef LOSCFG_BASE_CORE_SWTMR
#define LOSCFG_BASE_CORE_SWTMR                              1
#endif

/**
 * @ingroup los_config
 * Maximum supported number of software timers rather than the number of usable software timers
 */
#ifndef LOSCFG_BASE_CORE_SWTMR_LIMIT
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        5
#endif

/**
 * @ingroup los_config
 * Software timer task stack size
 */
#ifndef LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE
#define LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE               LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
#endif

/**
 * @ingroup los_config
 * Configurate item for software timer align tailoring
 */
#ifndef LOSCFG_BASE_CORE_SWTMR_ALIGN
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        0
#endif

#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    #if (LOSCFG_BASE_CORE_SWTMR == 0)
        #error "if LOSCFG_BASE_CORE_SWTMR_ALIGN is set to 1, then LOSCFG_BASE_CORE_SWTMR must also be set to 1"
    #endif
#endif

/**
 * @ingroup los_config
 * Maximum size of a software timer queue
 */
#ifndef OS_SWTMR_HANDLE_QUEUE_SIZE
#define OS_SWTMR_HANDLE_QUEUE_SIZE                          (LOSCFG_BASE_CORE_SWTMR_LIMIT + 0)
#endif

/**
 * @ingroup los_config
 * Minimum divisor of software timer multiple alignment
 */
#ifndef LOS_COMMON_DIVISOR
#define LOS_COMMON_DIVISOR                                  10
#endif

#if (LOSCFG_BASE_CORE_SWTMR == 1)
    #if (LOSCFG_BASE_IPC_QUEUE == 0)
        #error "if LOSCFG_BASE_CORE_SWTMR is set to 1, then LOSCFG_BASE_IPC_QUEUE must also be set to 1"
    #endif
#endif
/* =============================================================================
                                       Memory module configuration ---- to be refactored
============================================================================= */
extern UINT8 *m_aucSysMem0;

/**
 * @ingroup los_config
 * Configure whether the kernel uses external heap memory
 */
#ifndef LOSCFG_SYS_EXTERNAL_HEAP
#define LOSCFG_SYS_EXTERNAL_HEAP                            0
#endif

/**
 * @ingroup los_config
 * Starting address of the memory
 */
#ifndef LOSCFG_SYS_HEAP_ADDR
#define LOSCFG_SYS_HEAP_ADDR                                (&m_aucSysMem0[0])
#endif

/**
 * @ingroup los_config
 * Starting address of the task stack
 */
#ifndef OS_TASK_STACK_ADDR
#define OS_TASK_STACK_ADDR                                  LOSCFG_SYS_HEAP_ADDR
#endif

/**
 * @ingroup los_config
 * Memory size
 */
#ifndef LOSCFG_SYS_HEAP_SIZE
#define LOSCFG_SYS_HEAP_SIZE                                0x00013000
#endif

/**
 * @ingroup los_config
 * Configuration module tailoring of more mempry pool checking
 */
#ifndef LOSCFG_MEM_MUL_POOL
#define LOSCFG_MEM_MUL_POOL                                 1
#endif

/**
 * @ingroup los_config
 * Configuration module tailoring of memory released by task id
 */
#ifndef LOSCFG_MEM_FREE_BY_TASKID
#define LOSCFG_MEM_FREE_BY_TASKID                           0
#endif

/**
 * @ingroup los_config
 * Configuration module tailoring of mem node integrity checking
 */
#ifndef LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK
#define LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK                0
#endif

/**
 * @ingroup los_config
 * Configuration memory leak detection
 * @attention
 * Need to enable backtrace module synchronously by configuration LOSCFG_BACKTRACE_TYPE,
 * and call OSBackTraceInit to complete initialization before the memory pool is initialized.
 */
#ifndef LOSCFG_MEM_LEAKCHECK
#define LOSCFG_MEM_LEAKCHECK                                0
#endif

#if (LOSCFG_MEM_LEAKCHECK == 1) && (LOSCFG_BACKTRACE_TYPE == 0)
    #error "if LOSCFG_MEM_LEAKCHECK is set to 1, then LOSCFG_BACKTRACE_TYPE must be set to 1, 2 or 3."
#endif

/**
 * @ingroup los_config
 * The default is 4, which means that the function call stack is recorded from the kernel interface,
 * such as LOS_MemAlloc/LOS_MemAllocAlign/LOS_MemRealloc/LOS_MemFree. If you want to further ignore
 * the number of function call layers, you can increase this value appropriately.
 * @attention
 * The default is in the IAR tool. Under different compilation environments, this value needs to be adjusted.
 */
#ifndef LOSCFG_MEM_OMIT_LR_CNT
#define LOSCFG_MEM_OMIT_LR_CNT                              4
#endif

/**
 * @ingroup los_config
 * The record number of layers of the function call relationship starting from the number of
 * ignored layers(LOSCFG_MEM_OMIT_LR_CNT).
 */
#ifndef LOSCFG_MEM_RECORD_LR_CNT
#define LOSCFG_MEM_RECORD_LR_CNT                            3
#endif

/**
 * @ingroup los_config
 * Configuration memory leak recorded num
 */
#ifndef LOSCFG_MEM_LEAKCHECK_RECORD_MAX_NUM
#define LOSCFG_MEM_LEAKCHECK_RECORD_MAX_NUM                 1024
#endif

/**
 * @ingroup los_config
 * Configuration of memory pool record memory consumption waterline
 */
#ifndef LOSCFG_MEM_WATERLINE
#define LOSCFG_MEM_WATERLINE                                1
#endif

/**
 * @ingroup los_config
 * Number of memory checking blocks
 */
#ifndef OS_SYS_MEM_NUM
#define OS_SYS_MEM_NUM                                      20
#endif

/**
 * @ingroup los_config
 * Size of unaligned memory
 */
#ifndef OS_SYS_NOCACHEMEM_SIZE
#define OS_SYS_NOCACHEMEM_SIZE                              0x0UL
#endif

/**
 * @ingroup los_config
 * Starting address of the unaligned memory
 */
#if (OS_SYS_NOCACHEMEM_SIZE > 0)
#define OS_SYS_NOCACHEMEM_ADDR                              (&g_sysNoCacheMem0[0])
#endif

/* =============================================================================
                                        Exception module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for exception tailoring
 */
#ifndef LOSCFG_PLATFORM_EXC
#define LOSCFG_PLATFORM_EXC                                 0
#endif

/**
 * @ingroup los_config
 * Configuration of hardware stack protection
 */
#ifndef LOSCFG_EXC_HARDWARE_STACK_PROTECTION
#define LOSCFG_EXC_HARDWARE_STACK_PROTECTION                0
#endif

/* =============================================================================
                                        CPUP configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration item for CPU usage tailoring
 */
#ifndef LOSCFG_BASE_CORE_CPUP
#define LOSCFG_BASE_CORE_CPUP                               0
#endif

/* =============================================================================
                                       Test module configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration test case to open
 */

/**
 * @ingroup los_config
 * Configuration CMSIS_OS_VER
 */
#ifndef CMSIS_OS_VER
#define CMSIS_OS_VER                                        2
#endif

/* =============================================================================
                                       trace configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration liteos trace
 */
#ifndef LOSCFG_KERNEL_TRACE
#define LOSCFG_KERNEL_TRACE                                 0
#endif

#ifndef LOSCFG_SUPPORT_LITTLEFS
#define LOSCFG_SUPPORT_LITTLEFS                             1
#endif

#ifndef LOSCFG_LFS_MAX_MOUNT_SIZE
#define LOSCFG_LFS_MAX_MOUNT_SIZE                           3
#endif


/**
 * @ingroup los_config
 * Configuration trace tool
 */
#ifndef LOSCFG_DEBUG_HOOK
#define LOSCFG_DEBUG_HOOK                                   0
#endif

/* =============================================================================
                                       printf configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration liteos printf
 */
#ifndef LOSCFG_KERNEL_PRINTF
#define LOSCFG_KERNEL_PRINTF                                1
#endif

/* =============================================================================
                                       backtrace configuration
============================================================================= */
/**
 * @ingroup los_config
 * Configuration backtrace type
 * 0: Close stack analysis module.
 * 1: Call stack analysis for cortex-m series by scanning the stack.
 * 2: Call stack analysis for risc-v by using frame pointer.
 * 3: Call stack analysis for risc-v by scanning the stack.
 * others: Not currently supported.
 */
#ifndef LOSCFG_BACKTRACE_TYPE
#define LOSCFG_BACKTRACE_TYPE                                0
#endif

/**
 * @ingroup los_config
 * Configuration backtrace depth.
 */
#ifndef LOSCFG_BACKTRACE_DEPTH
#define LOSCFG_BACKTRACE_DEPTH                               15
#endif

/**
 * @ingroup los_config
 * Configuration item for low power frame tailoring
 */
#ifndef LOSCFG_KERNEL_PM
#define LOSCFG_KERNEL_PM                                     1
#endif

/**
 * @ingroup los_config
 * Configuration item for priority of low-power task.
 */
#ifndef LOSCFG_KERNEL_PM_TASK_PTIORITY
#define LOSCFG_KERNEL_PM_TASK_PTIORITY                       1
#endif

/**
 * @ingroup los_config
 * Configuration item for stack size of low-power task.
 */
#ifndef LOSCFG_KERNEL_PM_TASK_STACKSIZE
#define LOSCFG_KERNEL_PM_TASK_STACKSIZE                      0x800
#endif

/**
 * @ingroup los_config
 * Configuration item for low power frame debug tailoring
 */
#ifndef LOSCFG_KERNEL_PM_DEBUG
#define LOSCFG_KERNEL_PM_DEBUG                               0
#endif

/**
 * @ingroup los_config
 * Configuration trustzone secure heap size.
 */
#ifndef LOSCFG_SECURE_HEAP_SIZE
#define LOSCFG_SECURE_HEAP_SIZE                              2048
#endif

/**
 * @ingroup los_config
 * Configuration trustzone secure stack default size.
 * The secure stack must be allocated before the task calls non-secure callble functions.
 */
#ifndef LOSCFG_SECURE_STACK_DEFAULT_SIZE
#define LOSCFG_SECURE_STACK_DEFAULT_SIZE                     512
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif /* _LOS_CONFIG_H */
