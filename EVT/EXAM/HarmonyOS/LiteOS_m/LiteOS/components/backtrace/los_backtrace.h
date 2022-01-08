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

#ifndef _LOS_BACKTRACE_H
#define _LOS_BACKTRACE_H

#include "los_config.h"

#include "../../kernel/arch/risc-v/V4A/gcc/los_arch_interrupt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define BACKTRACE_MAX_DEPTH    LOSCFG_BACKTRACE_DEPTH

#if (LOSCFG_BACKTRACE_TYPE != 0)
#if (LOSCFG_BACKTRACE_TYPE == 1)
/* The default name of the code section and CSTACK section are given below,
   and the user can adjust it according to the linker script file. */
#if defined(__ICCARM__)
/* The default code section name is .text */
#define CODE_SECTION_NAME    ".text"
/* The default C stack section name is CSTACK */
#define CSTACK_SECTION_NAME  "CSTACK"
#pragma section=CODE_SECTION_NAME
#pragma section=CSTACK_SECTION_NAME

/* Default only one code section. In fact, there may be more than one.
   You can define more than one and modify the OsStackDataIsCodeAddr function
   to support searching in multiple code sections */
#define CODE_START_ADDR     ((UINTPTR)__section_begin(CODE_SECTION_NAME))
#define CODE_END_ADDR       ((UINTPTR)__section_end(CODE_SECTION_NAME))
#define CSTACK_START_ADDR   ((UINTPTR)__section_begin(CSTACK_SECTION_NAME))
#define CSTACK_END_ADDR     ((UINTPTR)__section_end(CSTACK_SECTION_NAME))
#elif defined(__CC_ARM) || defined(__CLANG_ARM)
/* The default code section name is ER_IROM1 */
#define CODE_SECTION_NAME    ER_IROM1
/* The default C stack section name is STACK */
#define CSTACK_SECTION_NAME  STACK

#define SECTION_START(_name_)           _name_##$$Base
#define SECTION_END(_name_)             _name_##$$Limit
#define CSTACK_SECTION_START(_name_)    SECTION_START(_name_)
#define CSTACK_SECTION_END(_name_)      SECTION_END(_name_)

#define IMAGE_SECTION_START(_name_)     Image$$##_name_##$$Base
#define IMAGE_SECTION_END(_name_)       Image$$##_name_##$$Limit
#define CODE_SECTION_START(_name_)      IMAGE_SECTION_START(_name_)
#define CODE_SECTION_END(_name_)        IMAGE_SECTION_END(_name_)

extern CHAR *CSTACK_SECTION_START(CSTACK_SECTION_NAME);
extern CHAR *CSTACK_SECTION_END(CSTACK_SECTION_NAME);
extern CHAR *CODE_SECTION_START(CODE_SECTION_NAME);
extern CHAR *CODE_SECTION_END(CODE_SECTION_NAME);

/* Default only one code section. In fact, there may be more than one.
   You can define more than one and modify the OsStackDataIsCodeAddr function
   to support searching in multiple code sections */
#define CODE_START_ADDR     ((UINTPTR)&CODE_SECTION_START(CODE_SECTION_NAME))
#define CODE_END_ADDR       ((UINTPTR)&CODE_SECTION_END(CODE_SECTION_NAME))
#define CSTACK_START_ADDR   ((UINTPTR)&CSTACK_SECTION_START(CSTACK_SECTION_NAME))
#define CSTACK_END_ADDR     ((UINTPTR)&CSTACK_SECTION_END(CSTACK_SECTION_NAME))
#elif defined(__GNUC__)
/* The default code section start address */
#define CODE_SECTION_START      _stext
/* The default code section end address */
#define CODE_SECTION_END        _etext
/* The default C stack section start address */
#define CSTACK_SECTION_START    _sstack
/* The default C stack section end address */
#define CSTACK_SECTION_END      _estack

extern CHAR *CODE_SECTION_START;
extern CHAR *CODE_SECTION_END;
extern CHAR *CSTACK_SECTION_START;
extern CHAR *CSTACK_SECTION_END;

/* Default only one code section. In fact, there may be more than one.
   You can define more than one and modify the OsStackDataIsCodeAddr function
   to support searching in multiple code sections */
#define CODE_START_ADDR     ((UINTPTR)&CODE_SECTION_START)
#define CODE_END_ADDR       ((UINTPTR)&CODE_SECTION_END)
#define CSTACK_START_ADDR   ((UINTPTR)&CSTACK_SECTION_START)
#define CSTACK_END_ADDR     ((UINTPTR)&CSTACK_SECTION_END)
#else
#error Unknown compiler.
#endif
#elif (LOSCFG_BACKTRACE_TYPE == 2) || (LOSCFG_BACKTRACE_TYPE == 3)
#if defined(__GNUC__)
/* The default code section start address */
#define CODE_SECTION_START      __text_start
/* The default code section end address */
#define CODE_SECTION_END        __text_end
/* The default C stack section start address */
#define CSTACK_SECTION_START    __except_stack_top
/* The default C stack section end address */
#define CSTACK_SECTION_END      __start_and_irq_stack_top

extern CHAR *CODE_SECTION_START;
extern CHAR *CODE_SECTION_END;
extern CHAR *CSTACK_SECTION_START;
extern CHAR *CSTACK_SECTION_END;

#define CODE_START_ADDR     ((UINTPTR)&CODE_SECTION_START)
#define CODE_END_ADDR       ((UINTPTR)&CODE_SECTION_END)
#define CSTACK_START_ADDR   ((UINTPTR)&CSTACK_SECTION_START)
#define CSTACK_END_ADDR     ((UINTPTR)&CSTACK_SECTION_END)
#else
#error Unknown compiler.
#endif
#endif

/* This function is used to judge whether the data in the stack is a code section address.
   The default code section is only one, but there may be more than one. Modify the
   judgment condition to support multiple code sections. */
STATIC INLINE BOOL OsStackDataIsCodeAddr(UINTPTR value)
{
    if ((value >= CODE_START_ADDR) && (value < CODE_END_ADDR)) {
        return TRUE;
    }
    return FALSE;
}

/* This function is currently used to register the memory leak check hook,
   other uses do not need to be called temporarily. */
VOID OSBackTraceInit(VOID);

/* This function is used to print the function call stack. */
VOID LOS_BackTrace(VOID);

/* This function is used to record the function call stack. */
VOID LOS_RecordLR(UINTPTR *LR, UINT32 LRSize, UINT32 jumpCount, UINTPTR SP);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif