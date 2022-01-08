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

#include <regex.h>
#include "ohos_types.h"
#include "los_config.h"
#include "Public.h"
#include "PCommon.h"
#include "fuzz_posix.h"

#define CYCLE_TOTAL_TIMES 1000
#define FUZZ_SYS_NAME_LEN 80
#define MAX_STR_BUF_LEN 10

extern S_ElementInit g_element[ELEMENT_LEN];
extern int g_iteration;

static uint32_t RegexFuzz(void)
{
    int fd;
    int ret;
    regex_t preg;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    
    const int initIntValue = 6;
    const int initStrLen = 4;
    const int maxStrLen = 5;
    const int maxNumRange = 6;
    const int maxNum1Range = 3;
    int spid;
    int num;
    int num1;
    char *string1 = NULL;
    char str1[MAX_STR_BUF_LEN] = {0};
    char *string2 = NULL;
    char str2[MAX_STR_BUF_LEN] = {0};

    INIT_FuzzEnvironment();
    CreatPrecondForQueue();

    printf("RegexFuzz starts,count=%d\n", CYCLE_TOTAL_TIMES);
    for (int i = 0; i < CYCLE_TOTAL_TIMES; i++) {
        hi_watchdog_feed();
        heartbeatPrint(i);

        string1 = DT_SetGetString(&g_element[NUM_0_INDEX], initStrLen, maxStrLen, "CHN");
        (void)strncpy_s(str1, MAX_STR_BUF_LEN, string1, maxStrLen);
        str1[MAX_STR_BUF_LEN - 1] = '\0';
        string2 = DT_SetGetString(&g_element[NUM_1_INDEX], initStrLen, maxStrLen, "CHN");
        (void)strncpy_s(str2, MAX_STR_BUF_LEN, string2, maxStrLen);
        str2[MAX_STR_BUF_LEN - 1] = '\0';

        num = *((int *)DT_SetGetS32(&g_element[NUM_2_INDEX], initIntValue));
        num = num % maxNumRange;
        num1 = *((UINT32 *)DT_SetGetU32(&g_element[NUM_3_INDEX], initIntValue));
        num1 = num1 % maxNum1Range;

        ret = regcomp(&preg, str1, num);
        if (ret != 0) {
            regfree(&preg);
            continue;
        }
        regexec(&preg, str2, nmatch, pmatch, num1);
        regfree(&preg);
    }

    CleanPrecondForQueue();
    DT_Clear(g_element);
    CLOSE_Log();
    CLEAR_FuzzEnvironment();

    printf("Fuzz test in line [%d] regcomp/regexec/regfree ok\n", __LINE__);

    return 0;
}

void ItSuiteRegexFuzz(void)
{
    RegexFuzz();
}