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

#include "osTest.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SELF_DELETED 0
#define SYS_EXIST_SWTMR 1
#define TEST_HWI_RUNTIME 0x100000
#define TASK_LOOP_NUM 0x100000
#define TASK_STACK_SIZE_TEST 0x400
#if (LOSCFG_BASE_CORE_SWTMR == 1)
#define TASK_EXISTED_NUM 3
#else
#define TASK_EXISTED_NUM 2
#endif

#define TASK_EXISTED_D_NUM TASK_EXISTED_NUM
#define TASK_NAME_NUM 10
#define IT_TASK_LOOP 20

extern EVENT_CB_S g_pevent;

extern UINT32 g_testTaskID01;
extern UINT32 g_testTaskID02;
extern UINT32 g_testTaskID03;
extern UINT32 g_testQueueID01;
extern UINT32 g_idleTaskID;
extern UINT32 g_testTskHandle;
extern UINT32 g_usSwTmrMaxNum;

extern UINT16 g_usSwTmrID1;
extern UINT16 g_usSwTmrID2;
extern UINT16 g_usSwTmrID3;

extern UINT32 g_swtmrCountA;
extern UINT32 g_swtmrCountB;
extern UINT32 g_swtmrCountC;
extern UINT64 g_cpuTickCountA;
extern UINT64 g_cpuTickCountB;

extern VOID LOS_GetCpuTick(UINT32 *puwCntHi, UINT32 *puwCntLo);

extern VOID ItLosEvent001(VOID);
extern VOID ItLosEvent002(VOID);
extern VOID ItLosEvent003(VOID);
extern VOID ItLosEvent004(VOID);
extern VOID ItLosEvent005(VOID);
extern VOID ItLosEvent006(VOID);
extern VOID ItLosEvent007(VOID);
extern VOID ItLosEvent008(VOID);
extern VOID ItLosEvent009(VOID);
extern VOID ItLosEvent010(VOID);
extern VOID ItLosEvent011(VOID);
extern VOID ItLosEvent012(VOID);
extern VOID ItLosEvent013(VOID);
extern VOID ItLosEvent014(VOID);
extern VOID ItLosEvent015(VOID);
extern VOID ItLosEvent016(VOID);
extern VOID ItLosEvent017(VOID);
extern VOID ItLosEvent018(VOID);
extern VOID ItLosEvent019(VOID);
extern VOID ItLosEvent020(VOID);
extern VOID ItLosEvent021(VOID);
extern VOID ItLosEvent022(VOID);
extern VOID ItLosEvent023(VOID);
extern VOID ItLosEvent024(VOID);
extern VOID ItLosEvent025(VOID);
extern VOID ItLosEvent026(VOID);
extern VOID ItLosEvent027(VOID);
extern VOID ItLosEvent028(VOID);
extern VOID ItLosEvent029(VOID);
extern VOID ItLosEvent030(VOID);
extern VOID ItLosEvent031(VOID);
extern VOID ItLosEvent032(VOID);
extern VOID ItLosEvent033(VOID);
extern VOID ItLosEvent034(VOID);
extern VOID ItLosEvent035(VOID);
extern VOID ItLosEvent036(VOID);
extern VOID ItLosEvent037(VOID);
extern VOID ItLosEvent038(VOID);
extern VOID ItLosEvent039(VOID);
extern VOID ItLosEvent040(VOID);
extern VOID ItLosEvent041(VOID);
extern VOID ItLosEvent042(VOID);
extern VOID ItLosEvent043(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
