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

#include "It_los_mux.h"


UINT32 g_mutexTest1;
UINT32 g_mutexTest2;

VOID ItSuiteLosMux(void)
{
    ItLosMux001();
    ItLosMux002();
    ItLosMux003();
    ItLosMux004();
    ItLosMux005();
    ItLosMux006();
    ItLosMux007();
    ItLosMux008();
    ItLosMux009();
    ItLosMux010();
    ItLosMux011();
    ItLosMux012();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosMux013();
#endif
    ItLosMux014();
    ItLosMux015();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosMux016();
    ItLosMux017();
#endif
    ItLosMux018();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosMux019();
#endif
#ifndef __RISC_V__
    ItLosMux020();
#endif
    ItLosMux021();
    ItLosMux022();
    ItLosMux023();
    ItLosMux024();
    ItLosMux025();
    ItLosMux026();
    ItLosMux027();
    ItLosMux029();
    ItLosMux030();
    ItLosMux031();
    ItLosMux033();
    ItLosMux034();
}

