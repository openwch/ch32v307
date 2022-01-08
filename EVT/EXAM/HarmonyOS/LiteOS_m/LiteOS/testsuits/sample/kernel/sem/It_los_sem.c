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

#include "It_los_sem.h"


VOID ItSuiteLosSem(void)
{
    ItLosSem001();
    ItLosSem002();
    ItLosSem003();
    ItLosSem004();
    ItLosSem005();
    ItLosSem006();
    ItLosSem007();
    ItLosSem008();
    ItLosSem009();
    ItLosSem012();
    ItLosSem013();
    ItLosSem014();
    ItLosSem015();
    ItLosSem016();
    ItLosSem017();
    ItLosSem018();
    ItLosSem019();
    ItLosSem020();
    ItLosSem021();
    ItLosSem022();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSem023();
#endif
    ItLosSem024();
    ItLosSem025();
    ItLosSem026();
    ItLosSem027();
    ItLosSem028();
    ItLosSem029();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSem030();
#endif
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == YES)
    ItLosSem031();
#endif
    ItLosSem032();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSem033();
#endif
    ItLosSem034();
    ItLosSem035();
    ItLosSem036();
    ItLosSem037();
    ItLosSem038();
    ItLosSem039();
    ItLosSem040();
    ItLosSem041();
    ItLosSem042();
    ItLosSem043();
}
