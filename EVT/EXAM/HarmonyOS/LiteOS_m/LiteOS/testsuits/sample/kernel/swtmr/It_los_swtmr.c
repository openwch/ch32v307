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

#include "It_los_swtmr.h"


EVENT_CB_S g_eventCB0;
EVENT_CB_S g_eventCB1;
EVENT_CB_S g_eventCB2;
EVENT_CB_S g_eventCB3;

VOID ItSuiteLosSwtmr(void)
{
    ItLosSwtmr001();
    ItLosSwtmr002();
    ItLosSwtmr003();
    ItLosSwtmr004();
    ItLosSwtmr005();
    ItLosSwtmr006();
    ItLosSwtmr007();
    ItLosSwtmr008();
    ItLosSwtmr009();
    ItLosSwtmr010();
    ItLosSwtmr011();
    ItLosSwtmr012();
    ItLosSwtmr013();
    ItLosSwtmr014();
    ItLosSwtmr015();
    ItLosSwtmr016();
    ItLosSwtmr017();
    ItLosSwtmr018();
    ItLosSwtmr019();
    ItLosSwtmr020();
    ItLosSwtmr021();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSwtmr022();
    ItLosSwtmr023();
#endif
    ItLosSwtmr024();
    ItLosSwtmr025();
#if (LOS_KERNEL_TEST_NOT_SMOKE == 1)
    ItLosSwtmr026();
#endif
    ItLosSwtmr027();
    ItLosSwtmr029();
    ItLosSwtmr030();
    ItLosSwtmr031();
    ItLosSwtmr032();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSwtmr033();
    ItLosSwtmr034();
#endif
    ItLosSwtmr035();
    ItLosSwtmr036();
    ItLosSwtmr037();
    ItLosSwtmr038();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSwtmr039();
    ItLosSwtmr040();
    ItLosSwtmr041();
#endif
    ItLosSwtmr042();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSwtmr043();
#endif
    ItLosSwtmr044();
    ItLosSwtmr045();
    ItLosSwtmr046();
    ItLosSwtmr047();
    ItLosSwtmr048();
    ItLosSwtmr049();
    ItLosSwtmr050();
    ItLosSwtmr051();
    ItLosSwtmr052();
    ItLosSwtmr053();
    ItLosSwtmr054();
    ItLosSwtmr055();
    ItLosSwtmr056();
    ItLosSwtmr057();
    ItLosSwtmr058();
    ItLosSwtmr059();
    ItLosSwtmr060();
    ItLosSwtmr061();
    ItLosSwtmr062();
    ItLosSwtmr063();
#if (LOS_KERNEL_HWI_TEST == 1)
    ItLosSwtmr064();
    ItLosSwtmr065();
#endif
    ItLosSwtmr066();
    ItLosSwtmr067();
#if (LOS_KERNEL_TEST_NOT_SMOKE == 1)
    ItLosSwtmr068();
    ItLosSwtmr069();
#endif
    ItLosSwtmr071();
    ItLosSwtmr072();
    ItLosSwtmr073();
#if (LOS_KERNEL_TEST_NOT_SMOKE == 1)
    ItLosSwtmr074();
#endif
    ItLosSwtmr075();
    ItLosSwtmr076();
    ItLosSwtmr077();
    ItLosSwtmr078();
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    ItLosSwtmrAlign001();
    ItLosSwtmrAlign002();
    ItLosSwtmrAlign003();
    ItLosSwtmrAlign004();
#if (LOS_KERNEL_TEST_NOT_SMOKE == 1)
    ItLosSwtmrAlign005();
#endif
    ItLosSwtmrAlign006();
    ItLosSwtmrAlign007();
    ItLosSwtmrAlign008();
    ItLosSwtmrAlign009();
    ItLosSwtmrAlign010();
    ItLosSwtmrAlign011();
    ItLosSwtmrAlign012();
    ItLosSwtmrAlign013();
    ItLosSwtmrAlign014();
    ItLosSwtmrAlign015();
    ItLosSwtmrAlign016();
    ItLosSwtmrAlign017();
    ItLosSwtmrAlign018();
    ItLosSwtmrAlign019();
    ItLosSwtmrAlign020();
    ItLosSwtmrAlign021();
    ItLosSwtmrAlign022();
    ItLosSwtmrAlign023();
    ItLosSwtmrAlign024();
    ItLosSwtmrAlign025();
    ItLosSwtmrAlign026();
    ItLosSwtmrAlign027();
    ItLosSwtmrAlign028();
    ItLosSwtmrAlign029();
    ItLosSwtmrAlign030();
    ItLosSwtmrAlign031();
#endif
    ItLosSwtmrDelay001();
    ItLosSwtmrDelay003();
    ItLosSwtmrDelay004();
    ItLosSwtmrDelay005();
}

