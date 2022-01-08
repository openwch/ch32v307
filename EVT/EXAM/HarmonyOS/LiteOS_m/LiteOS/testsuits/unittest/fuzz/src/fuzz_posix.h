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

#ifndef _FUZZ_POSIX_H
#define _FUZZ_POSIX_H


#define RET_FALSE (-1)
#define RET_OK 0
#define RET_TRUE 1

#define NUM_0_INDEX 0
#define NUM_1_INDEX 1
#define NUM_2_INDEX 2
#define NUM_3_INDEX 3
#define NUM_4_INDEX 4
#define NUM_5_INDEX 5

#define ELEMENT_LEN 10

void ItSuiteFuzz(void);

void IsdigitFuzzTest(void);
extern void IslowerFuzzTest(void);
extern void IsxdigitFuzzTest(void);
extern void TolowerFuzzTest(void);
extern void ToupperFuzzTest(void);
extern void AtoiFuzzTest(void);
extern void AtolFuzzTest(void);
extern void AtollFuzzTest(void);
extern void StrtolFuzzTest(void);
extern void StrtoulFuzzTest(void);
extern void StrtoullFuzzTest(void);
extern void StrchrFuzzTest(void);
extern void StrstrFuzzTest(void);
extern void NetFuzzTest(void);

extern void ItSuiteRegexFuzz(void);
extern void ItSuiteStringsFuzz(void);

extern int IsalnumFuzz(void);
extern int IsasciiFuzz(void);
extern int IsprintFuzz(void);
extern int IsspaceFuzz(void);
extern int IsupperFuzz(void);
extern int StdargFuzz(void);
extern void ReallocFuzzTest(void);
extern int MemcmpFuzzTest(void);
extern int MemcpyFuzzTest(void);
extern int MemsetFuzzTest(void);
extern int StrcmpFuzzTest(void);
extern int StrcspnFuzzTest(void);
extern int StrdupFuzzTest(void);
extern int StrerrorFuzzTest(void);
extern int StrlenFuzz(void);
extern int StrncmpFuzz(void);
extern int StrrchrFuzz(void);
extern int StrncasecmpFuzz(void);
extern int StrptimeFuzzTest(void);

extern void DirnameFuzzTest(void);
extern void LogFuzzTest(void);
extern void PowFuzzTest(void);
extern void RoundFuzzTest(void);
extern void SqrtFuzzTest(void);
extern void SemTimedWaitFuzzTest(void);
extern void ClearerrFuzzTest(void);
extern void FeofFuzzTest(void);
extern void PerrorFuzzTest(void);
extern void AbsFuzzTest(void);
extern void GettimeofdayFuzzTest(void);
extern void TimesFuzzTest(void);
extern void GmtimeFuzzTest(void);
extern void LocaltimeFuzzTest(void);
extern void LocaltimerFuzzTest(void);
extern void MktimeFuzzTest(void);
extern void StrftimeFuzzTest(void);
extern void TimeFuzzTest(void);
extern void UsleepFuzzTest(void);

#endif