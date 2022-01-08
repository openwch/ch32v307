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

#include "iCunit.h"
#include "iCunit.inc"
#include "string.h"
#include "osTest.h"
#include "iCunit_config.h"

#include <stdio.h>


extern UINT32 g_failResult;
extern UINT32 g_passResult;
void ICunitSaveErr(iiUINT32 line, iiUINT32 retCode)
{
    iCunit_errLineNo = (iCunit_errLineNo == 0) ? line : iCunit_errLineNo;
    iCunit_errCode = (iCunit_errCode == 0) ? (iiUINT32)retCode : iCunit_errCode;
}

iUINT32 ICunitAddCase(iCHAR *caseName, CASE_FUNCTION caseFunc, iUINT16 testcaseLayer, iUINT16 testcaseModule,
    iUINT16 testcaseLevel, iUINT16 testcaseType)
{
    iUINT16 idx;

    if (iCunit_Init_Success) {
        return (iUINT32)ICUNIT_UNINIT;
    }

    idx = 0;
    if (idx == ICUNIT_CASE_SIZE) {
        iCunit_ErrLog_AddCase++;
        return (iUINT32)ICUNIT_CASE_FULL;
    }

    iCunit_CaseArray[idx].pcCaseID = caseName;
    iCunit_CaseArray[idx].pstCaseFunc = caseFunc;
    iCunit_CaseArray[idx].testcase_layer = testcaseLayer;
    iCunit_CaseArray[idx].testcase_module = testcaseModule;
    iCunit_CaseArray[idx].testcase_level = testcaseLevel;
    iCunit_CaseArray[idx].testcase_type = testcaseType;

    ICunitRun();
    return (iUINT32)ICUNIT_SUCCESS;
}

iUINT32 ICunitInit()
{
    iCunit_Init_Success = 0x0000;
    iCunit_Case_Cnt = 0x0000;

    iCunit_Case_FailedCnt = 0;

    iCunit_ErrLog_AddCase = 0;

    (void)memset_s(iCunit_CaseArray, sizeof(iCunit_CaseArray), 0, sizeof(iCunit_CaseArray));
    return (iUINT32)ICUNIT_SUCCESS;
}

char *g_strLayer[] = {
    "LOS", "CMSIS", "POSIX", "LIB", "VFS", "EXTEND",
    "PARTITION", "CPP", "SHELL", "LINUX", "USB", "DRIVERFRAME", "CONTEXTHUB"
};
char *g_strModule[] = {
    "TASK", "MEM", "SEM", "MUX", "EVENT", "QUE", "SWTMR", "HWI", "ATO", "CPUP", "SCATTER", "RUNSTOP", "TIMER", "MMU", "TICKLESS",
    "ROBIN", "LIBC", "WAIT", "VFAT", "YAFFS", "JFFS", "RAMFS", "NFS", "PROC", "FS",
    "PTHREAD", "COMP", "HWI_HALFBOTTOM", "WORKQ", "WAKELOCK", "TIMES",
    "LIBM", "SUPPORT", "STL", "MAIL", "MSG", "CP", "SIGNAL", "SCHED", "MTDCHAR", "TIME", "WRITE", "READ", "DYNLOAD", "REGISTER", "SR", "UNAME", "ERR"
};
char *g_strLevel[] = {
    "LEVEL0", "LEVEL1", "LEVEL2", "LEVEL3"
};
char *g_strType[] = {
    "FUNCTITON", "PRESSURE", "PERFORMANCE"
};

iUINT32 ICunitRunF()
{
    iUINT32 idx, idx1;
    ICUNIT_CASE_S *psubCaseArray;
    iUINT32 caseRet;

    psubCaseArray = iCunit_CaseArray;
    idx1 = 1;

    for (idx = 0; idx < idx1; idx++, psubCaseArray++) {
        iCunit_errLineNo = 0;
        iCunit_errCode = 0;

        caseRet = psubCaseArray->pstCaseFunc();
        psubCaseArray->errLine = iCunit_errLineNo;
        psubCaseArray->retCode = (0 == iCunit_errLineNo) ? (caseRet) : (iCunit_errCode);

        if (0 == iCunit_errLineNo && 0 == caseRet) {
            g_passResult++;
            PRINTF("  [Passed]-%s-%s-%s-%s-%s\n", psubCaseArray->pcCaseID, g_strLayer[psubCaseArray->testcase_layer],
                g_strModule[psubCaseArray->testcase_module], g_strLevel[psubCaseArray->testcase_level],
                g_strType[psubCaseArray->testcase_type]);
        } else {
            g_failResult++;
            iCunit_Case_FailedCnt++;
            PRINTF("  [Failed]-%s-%s-%s-%s-%s-[Errline: %d RetCode:0x%x]\n", psubCaseArray->pcCaseID,
                g_strLayer[psubCaseArray->testcase_layer], g_strModule[psubCaseArray->testcase_module],
                g_strLevel[psubCaseArray->testcase_level], g_strType[psubCaseArray->testcase_type],
                psubCaseArray->errLine, psubCaseArray->retCode);
        }
    }

    return (iUINT32)ICUNIT_SUCCESS;
}

iUINT32 ICunitRun()
{
    if (iCunit_Init_Success) {
        return (iUINT32)ICUNIT_UNINIT;
    }

    ICunitRunF();

    return (iUINT32)ICUNIT_SUCCESS;
}

