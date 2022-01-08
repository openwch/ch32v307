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

#include "los_backtrace.h"
#include "los_task.h"
#include "los_debug.h"


#if (LOSCFG_BACKTRACE_TYPE != 0)
#if (LOSCFG_BACKTRACE_TYPE == 1)
#define OS_BACKTRACE_START     2
/* Thumb instruction, so the pc must be an odd number */
#define OS_IS_THUMB_INSTRUCTION(pc)    ((pc & 0x1) == 1)

/* BL or BLX instruction flag. */
#define OS_BL_INS_MASK     0xF800
#define OS_BL_INS_HIGH     0xF800
#define OS_BL_INS_LOW      0xF000
#define OS_BLX_INX_MASK    0xFF00
#define OS_BLX_INX         0x4700

#if defined(__ICCARM__) || defined(__CC_ARM)
STATIC INLINE UINTPTR HalSpGet(VOID)
{
    UINTPTR sp;
    __asm("mov %0, sp" : "=r" (sp));
    return sp;
}

STATIC INLINE UINTPTR HalPspGet(VOID)
{
    UINTPTR psp;
    __asm("mrs %0, psp" : "=r" (psp));
    return psp;
}

STATIC INLINE UINTPTR HalMspGet(VOID)
{
    UINTPTR msp;
    __asm("mrs %0, msp" : "=r" (msp));
    return msp;
}
#elif defined(__CLANG_ARM) || defined(__GNUC__)
STATIC INLINE UINTPTR HalSpGet(VOID)
{
    UINTPTR sp;
    __asm volatile("mov %0, sp" : "=r" (sp));
    return sp;
}

STATIC INLINE UINTPTR HalPspGet(VOID)
{
    UINTPTR psp;
    __asm volatile("mrs %0, psp" : "=r" (psp));
    return psp;
}

STATIC INLINE UINTPTR HalMspGet(VOID)
{
    UINTPTR msp;
    __asm volatile("mrs %0, msp" : "=r" (msp));
    return msp;
}
#else
#error Unknown compiler.
#endif

STATIC INLINE BOOL OsInsIsBlOrBlx(UINTPTR addr)
{
    UINT16 ins1 = *((UINT16 *)addr);
    UINT16 ins2 = *((UINT16 *)(addr + 2)); /* 2: Thumb instruction is two bytes. */

    if (((ins2 & OS_BL_INS_MASK) == OS_BL_INS_HIGH) &&
        ((ins1 & OS_BL_INS_MASK) == OS_BL_INS_LOW)) {
        return TRUE;
    } else if ((ins2 & OS_BLX_INX_MASK) == OS_BLX_INX) {
        return TRUE;
    } else {
        return FALSE;
    }
}

STATIC INLINE UINT32 OsStackAddrGet(UINTPTR *stackStart, UINTPTR *stackEnd, UINTPTR SP)
{
    if (SP != 0) {
        *stackStart = SP;
        if ((SP >= CSTACK_START_ADDR) && (SP < CSTACK_END_ADDR)) {
            *stackEnd = CSTACK_END_ADDR;
        } else {
            UINT32 taskID = LOS_CurTaskIDGet();
            LosTaskCB *taskCB = OS_TCB_FROM_TID(taskID);
            *stackEnd = (UINTPTR)taskCB->topOfStack + taskCB->stackSize;
            if ((SP < (UINTPTR)taskCB->topOfStack) || (SP >= *stackEnd)) {
                PRINT_ERR("msp statck [0x%x, 0x%x], cur task stack [0x%x, 0x%x], cur sp(0x%x) is overflow!\n",
                          CSTACK_START_ADDR, CSTACK_END_ADDR, (UINTPTR)taskCB->topOfStack, *stackEnd, SP);
                return LOS_NOK;
            }
        }
    } else {
        if (HalSpGet() != HalPspGet()) {
            *stackStart = HalMspGet();
            *stackEnd = CSTACK_END_ADDR;
            if ((*stackStart < CSTACK_START_ADDR) || (*stackStart >= CSTACK_END_ADDR)) {
                PRINT_ERR("msp stack [0x%x, 0x%x], cur sp(0x%x) is overflow!\n",
                          CSTACK_START_ADDR, CSTACK_END_ADDR, *stackStart);
                return LOS_NOK;
            }
            PRINTK("msp, start = %x, end = %x\n", *stackStart, *stackEnd);
        } else {
            *stackStart = HalPspGet();
            UINT32 taskID = LOS_CurTaskIDGet();
            LosTaskCB *taskCB = OS_TCB_FROM_TID(taskID);
            *stackEnd = (UINTPTR)taskCB->topOfStack + taskCB->stackSize;
            if ((*stackStart < (UINTPTR)taskCB->topOfStack) || (*stackStart >= *stackEnd)) {
                PRINT_ERR("psp stack [0x%x, 0x%x], cur sp(0x%x) is overflow, cur task id is %d!\n",
                          taskCB->topOfStack, *stackEnd, *stackStart, taskID);
                return LOS_NOK;
            }
            PRINTK("psp, start = %x, end = %x\n", *stackStart, *stackEnd);
        }
    }

    return LOS_OK;
}

STATIC INLINE UINTPTR OsAddrIsValid(UINTPTR sp)
{
    UINTPTR pc;
    BOOL ret;

    /* The stack space pointed to by the current SP may store the LR,
       so need decrease a word to PC. */
    pc = *((UINTPTR *)sp) - sizeof(UINTPTR);

    if (!OS_IS_THUMB_INSTRUCTION(pc)) {
        return 0;
    }

    /* PC in thumb mode is an odd number, fix the PC address by decreasing one byte. */
    pc = *((UINTPTR *)sp) - 1;

    ret = OsStackDataIsCodeAddr(pc);
    if (ret == FALSE) {
        return 0;
    }

    ret = OsInsIsBlOrBlx(pc - sizeof(UINTPTR));
    if (ret == FALSE) {
        return 0;
    }

    return pc;
}
#elif (LOSCFG_BACKTRACE_TYPE == 2)
STATIC INLINE BOOL OsBackTraceFpCheck(UINT32 value);
#define OS_BACKTRACE_START     1
#define OS_RA_OFFSET           4
#define OS_FP_OFFSET           8
#define OS_FP_ALIGN(value)     (((UINT32)(value) & (UINT32)(LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) == 0)
#define OS_FP_CHECK(value)     (((UINT32)(value) != FP_INIT_VALUE) && OS_FP_ALIGN(value))

STATIC INLINE UINTPTR OsFpGet(VOID)
{
    UINTPTR fp = 0;
    __asm volatile("mv %0, s0" : "=r"(fp));
    dsb();
    return fp;
}

VOID LOS_RecordLR(UINTPTR *LR, UINT32 LRSize, UINT32 jumpCount, UINTPTR SP)
{
    UNUSED(SP);
    UINT32 backFp = OsFpGet();
    UINT32 tmpFp;
    UINT32 backRa;
    UINT32 count = 0;
    UINT32 index = 0;

    if (LR == NULL) {
        return;
    }

    while (OS_FP_CHECK(backFp)) {
        tmpFp = backFp;
        backRa = *((UINT32 *)(UINTPTR)(tmpFp - OS_RA_OFFSET));
        backFp = *((UINT32 *)(UINTPTR)(tmpFp - OS_FP_OFFSET));
        if (index++ < jumpCount) {
            continue;
        }

        LR[count] = backRa;
        count++;
        if ((count == LRSize) || (backFp == tmpFp) ||
            (!OsStackDataIsCodeAddr(backRa))) {
            break;
        }
    }

    if (count < LRSize) {
        LR[count] = 0;
    }
}
#elif (LOSCFG_BACKTRACE_TYPE == 3)
#define OS_BACKTRACE_START  1
#define OS_JALX_INS_MASK    0x7F
#define OS_JAL_INS_LOW      0x6F
#define OS_JAL_16_INS_MASK  0x2001
#define OS_JALR_INS_LOW     0x67
#define OS_JALR_16_INS_MASK 0x9002
#define OS_JR_16_INS_MASK   0x8002
#define OS_J_16_INS_MASK    0xA001

STATIC INLINE BOOL OsInsIsJump(UINTPTR addr)
{
    UINT16 ins1 = *((UINT16 *)addr);
    UINT16 ins2 = *((UINT16 *)(addr + 2));

    /* Jal ins */
    if (((ins1 & OS_JALX_INS_MASK) == OS_JAL_INS_LOW) ||
        ((ins1 & OS_JAL_16_INS_MASK) == OS_JAL_16_INS_MASK) ||
        ((ins2 & OS_JAL_16_INS_MASK) == OS_JAL_16_INS_MASK)) {
        return TRUE;
    }

    /* Jalr ins */
    if (((ins1 & OS_JALX_INS_MASK) == OS_JALR_INS_LOW) ||
        ((ins1 & OS_JALR_16_INS_MASK) == OS_JALR_16_INS_MASK) ||
        ((ins2 & OS_JALR_16_INS_MASK) == OS_JALR_16_INS_MASK)) {
        return TRUE;
    }

    /* Jr ins */
    if (((ins1 & OS_JR_16_INS_MASK) == OS_JR_16_INS_MASK) ||
        ((ins2 & OS_JR_16_INS_MASK) == OS_JR_16_INS_MASK)) {
        return TRUE;
    }

    /* J ins */
    if (((ins1 & OS_J_16_INS_MASK) == OS_J_16_INS_MASK) ||
        ((ins2 & OS_J_16_INS_MASK) == OS_J_16_INS_MASK)) {
        return TRUE;
    }

    return FALSE;
}

STATIC INLINE UINTPTR OsSpGet(VOID)
{
    UINTPTR sp = 0;
    __asm volatile("mv %0, sp" : "=r"(sp));
    dsb();
    return sp;
}

STATIC INLINE UINT32 OsStackAddrGet(UINTPTR *stackStart, UINTPTR *stackEnd, UINTPTR SP)
{
    if (SP != 0) {
        *stackStart = SP;
        if ((SP >= CSTACK_START_ADDR) && (SP < CSTACK_END_ADDR)) {
            *stackEnd = CSTACK_END_ADDR;
        } else {
            UINT32 taskID = LOS_CurTaskIDGet();
            LosTaskCB *taskCB = OS_TCB_FROM_TID(taskID);
            *stackEnd = (UINTPTR)taskCB->topOfStack + taskCB->stackSize;
            if ((SP < (UINTPTR)taskCB->topOfStack) || (SP >= *stackEnd)) {
                PRINT_ERR("msp statck [0x%x, 0x%x], cur task stack [0x%x, 0x%x], cur sp(0x%x) is overflow!\n",
                          CSTACK_START_ADDR, CSTACK_END_ADDR, (UINTPTR)taskCB->topOfStack, *stackEnd, SP);
                return LOS_NOK;
            }
        }
    } else {
        if (!LOS_TaskIsRunning()) {
            *stackStart = OsSpGet();
            *stackEnd = CSTACK_END_ADDR;
            if ((*stackStart < CSTACK_START_ADDR) || (*stackStart >= CSTACK_END_ADDR)) {
                PRINT_ERR("msp stack [0x%x, 0x%x], cur sp(0x%x) is overflow!\n",
                          CSTACK_START_ADDR, CSTACK_END_ADDR, *stackStart);
                return LOS_NOK;
            }
        } else {
            *stackStart = OsSpGet();
            UINT32 taskID = LOS_CurTaskIDGet();
            LosTaskCB *taskCB = OS_TCB_FROM_TID(taskID);
            *stackEnd = (UINTPTR)taskCB->topOfStack + taskCB->stackSize;
            if ((*stackStart < (UINTPTR)taskCB->topOfStack) || (*stackStart >= *stackEnd)) {
                PRINT_ERR("psp stack [0x%x, 0x%x], cur sp(0x%x) is overflow, cur task id is %d!\n",
                          taskCB->topOfStack, *stackEnd, *stackStart, taskID);
                return LOS_NOK;
            }
        }
    }

    return LOS_OK;
}

STATIC INLINE UINTPTR OsAddrIsValid(UINTPTR sp)
{
    UINTPTR pc;
    BOOL ret;

    pc = *((UINTPTR *)sp);

    ret = OsStackDataIsCodeAddr(pc);
    if (ret == FALSE) {
        return 0;
    }

    ret = OsInsIsJump(pc - sizeof(UINTPTR));
    if (ret == FALSE) {
        return 0;
    }

    return pc;
}
#else
#error Unknown backtrace type.
#endif

#if (LOSCFG_BACKTRACE_TYPE == 1) || (LOSCFG_BACKTRACE_TYPE == 3)
VOID LOS_RecordLR(UINTPTR *LR, UINT32 LRSize, UINT32 jumpCount, UINTPTR SP)
{
    if (LR == NULL) {
        return;
    }

    UINTPTR stackStart;
    UINTPTR stackEnd;
    UINT32 count = 0;
    UINT32 index = 0;
    UINTPTR sp;
    UINTPTR pc;
    UINT32 ret;

    ret = OsStackAddrGet(&stackStart, &stackEnd, SP);
    if (ret != LOS_OK) {
        return;
    }

    /* Traverse the stack space and find the LR address. */
    for (sp = stackStart; sp < stackEnd; sp += sizeof(UINTPTR)) {
        pc = OsAddrIsValid(sp);
        if ((pc != 0) && (count < LRSize)) {
            if (index++ < jumpCount) {
                continue;
            }
            LR[count] = pc;
            count++;
            if (count == LRSize) {
                break;
            }
        }
    }

    if (count < LRSize) {
        LR[count] = 0;
    }
}
#endif

VOID LOS_BackTrace(VOID)
{
    UINTPTR LR[BACKTRACE_MAX_DEPTH] = {0};
    UINT32 index;

    LOS_RecordLR(LR, BACKTRACE_MAX_DEPTH, OS_BACKTRACE_START, 0);

    if (LOS_TaskIsRunning()) {
        PRINTK("taskName = %s\n", g_losTask.runTask->taskName);
        PRINTK("taskID   = %u\n", g_losTask.runTask->taskID);
    }

    PRINTK("----- traceback start -----\r\n");
    for (index = 0; index < BACKTRACE_MAX_DEPTH; index++) {
        if (LR[index] == 0) {
            break;
        }
        PRINTK("traceback %d -- lr = 0x%x\r\n", index, LR[index]);
    }
    PRINTK("----- traceback end -----\r\n");
}

VOID OSBackTraceInit(VOID)
{
    OsBackTraceHookSet(LOS_RecordLR);
}
#endif


