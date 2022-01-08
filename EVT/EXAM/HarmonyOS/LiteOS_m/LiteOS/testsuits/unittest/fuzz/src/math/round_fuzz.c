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

#include <math.h>
#include "ohos_types.h"
#include "los_config.h"
#include "Public.h"
#include "PCommon.h"
#include "fuzz_posix.h"

#define CYCLE_TOTAL_TIMES 1000
#define DEFAULT_ROUND_VALUE 0

extern S_ElementInit g_element[ELEMENT_LEN];
extern int g_iteration;

void RoundFuzzTest(void)
{
    double x;

    printf("Fuzz test in line [%d] round start.\n", __LINE__);

    INIT_FuzzEnvironment();
    CreatPrecondForQueue();

    for (int i = 0; i < CYCLE_TOTAL_TIMES; i++) {
        hi_watchdog_feed();
        heartbeatPrint(i);

        x = *(double *)DT_SetGetDouble(&g_element[0], DEFAULT_ROUND_VALUE);
        (void)round(x);
    }

    CleanPrecondForQueue();
    DT_Clear(g_element);
    CLOSE_Log();
    CLEAR_FuzzEnvironment();

    printf("Fuzz test in line [%d] round ok\n", __LINE__);

    return;
}
