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

#ifndef __NEED_mode_t
#define __NEED_mode_t
#endif

#include <securec.h>
#include <stdio.h>
#include <libgen.h>
#include "ohos_types.h"
#include "hctest.h"
#include "los_config.h"
#include "kernel_test.h"
#include "log.h"
#include <bits/alltypes.h>
#include <fcntl.h>
#include <dirent.h>
#include "sys/stat.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is PosixFsFuncTestSuite
 */
LITE_TEST_SUIT(Posix, PosixFs, PosixFsFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixFsFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixFsFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

#define FILE0 "FILE0"
#define FILE1 "/opt/test/FILE1"
#define DIR1 "/opt/test/DIR1/"

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_001
 * @tc.name     dirname basic function test
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsDirname001, Function | MediumTest | Level1)
{
    char path[] = FILE0;
    char *workDir = dirname((char *)path);
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING(".", workDir);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_002
 * @tc.name     dirname basic function test
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsDirname002, Function | MediumTest | Level1)
{
    char path[] = FILE1;
    char *workDir = dirname((char *)path);
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING("/opt/test", workDir);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_003
 * @tc.name     dirname basic function test
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsDirname003, Function | MediumTest | Level1)
{
    // get dir
    char path[] = DIR1;
    char *workDir = dirname((char *)path);
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING("/opt/test", workDir);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_004
 * @tc.name     dirname basic function test for special input
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsDirname004, Function | MediumTest | Level1)
{
    // get dir
    char *workDir = dirname("");
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING(".", workDir);

    workDir = dirname(NULL);
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING(".", workDir);

    workDir = dirname("/");
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING("/", workDir);

    workDir = dirname("..");
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING(".", workDir);

    workDir = dirname(".");
    TEST_ASSERT_NOT_NULL(workDir);
    TEST_ASSERT_EQUAL_STRING(".", workDir);
}

#define TEST_FILE_PTAH_RIGHT "/temp"
/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_005
 * @tc.name     fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose001, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_006
 * @tc.name     fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose002, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_007
 * @tc.name     fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose003, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    fp = fopen(TEST_FILE_PTAH_RIGHT, "r");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_008
 * @tc.name     fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose004, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    fp = fopen(TEST_FILE_PTAH_RIGHT, "a");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_009
 * @tc.name     fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose005, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    fp = fopen(TEST_FILE_PTAH_RIGHT, "c");
    TEST_ASSERT_NULL(fp);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_010
 * @tc.name     remove the path before fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose006, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;

    remove(TEST_FILE_PTAH_RIGHT);
    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_011
 * @tc.name     remove the path before fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose007, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;

    remove(TEST_FILE_PTAH_RIGHT);
    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_012
 * @tc.name     remove the path before fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose008, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;

    remove(TEST_FILE_PTAH_RIGHT);
    fp = fopen(TEST_FILE_PTAH_RIGHT, "r");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_013
 * @tc.name     remove the path before fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose009, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;

    remove(TEST_FILE_PTAH_RIGHT);
    fp = fopen(TEST_FILE_PTAH_RIGHT, "a");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_014
 * @tc.name     remove the path before fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose010, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;

    remove(TEST_FILE_PTAH_RIGHT);
    fp = fopen(TEST_FILE_PTAH_RIGHT, NULL);
    TEST_ASSERT_NULL(fp);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_015
 * @tc.name     remove the path before fopen and fclose
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFopenFclose011, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    int err = 0;

    ret = fclose(NULL);
    TEST_ASSERT_EQUAL_INT(ret, -1);
    err = errno;
    TEST_ASSERT_EQUAL_INT(err, EINVAL);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_016
 * @tc.name     fdopen
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFdopen001, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    int fd = 0;

    fd = open(TEST_FILE_PTAH_RIGHT, O_CREAT | O_RDWR, 0666);
    TEST_ASSERT_TRUE(fd > 0);

    fp = fdopen(fd, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_017
 * @tc.name     fdopen
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFdopen002, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    int fd = 0;

    fd = open(TEST_FILE_PTAH_RIGHT, O_CREAT | O_RDWR, 0666);
    TEST_ASSERT_TRUE(fd > 0);

    fp = fdopen(fd, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_018
 * @tc.name     fdopen
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFdopen003, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    int fd = 0;

    fd = open(TEST_FILE_PTAH_RIGHT, O_CREAT | O_RDWR, 0666);
    TEST_ASSERT_TRUE(fd > 0);

    fp = fdopen(fd, "a");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_019
 * @tc.name     fdopen
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFdopen004, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    int fd = 0;

    fd = open(TEST_FILE_PTAH_RIGHT, O_CREAT | O_RDWR, 0666);
    TEST_ASSERT_TRUE(fd > 0);

    fp = fdopen(500, "w");
    // in some fs, may return ok, so return null or not is pass.
    if (NULL == fp) {
        close (fd);
        return;
    }

    fp->fd = fd;
    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_020
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek001, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_021
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek002, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 0L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_022
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek003, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 10L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 10);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_023
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek004, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 10L, SEEK_END);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 10);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_024
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek005, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 10L, SEEK_CUR);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 10);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_025
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek006, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, -1L, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(ret, -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_026
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek007, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, -1L, SEEK_CUR);
    TEST_ASSERT_EQUAL_INT(ret, -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_027
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek008, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, -1L, SEEK_END);
    TEST_ASSERT_EQUAL_INT(ret, -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_028
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek009, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 10L, 5);
    TEST_ASSERT_TRUE(ret != 0);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_029
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek010, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;
    int fd;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    fd = fp->fd;

    fp->fd = 500;

    ret = fseek(fp, 10L, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(ret, -1);

    fp->fd = fd;

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_030
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek011, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 100L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fseek(fp, 40L, SEEK_CUR);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 140);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}


/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_031
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek012, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 100L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fseek(fp, 20L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 20);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_032
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek013, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 100L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fseek(fp, 20L, SEEK_END);
    TEST_ASSERT_TRUE(ret != -1);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 20);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_033
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek014, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 100L, SEEK_SET);
    TEST_ASSERT_TRUE(ret != -1);

    rewind(fp);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_034
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek015, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 100L, SEEK_CUR);
    TEST_ASSERT_TRUE(ret != -1);

    rewind(fp);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_035
 * @tc.name     ftell and fseek
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFtellFseek016, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fseek(fp, 100L, SEEK_END);
    TEST_ASSERT_TRUE(ret != -1);

    rewind(fp);

    off = ftell(fp);
    TEST_ASSERT_EQUAL_INT64(off, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_036
 * @tc.name     fputs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFputs001, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    const char chr1[10] = "hello";
    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fputs(chr1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_037
 * @tc.name     fputs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFputs002, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    const char chr1[10] = "hello";
    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "a");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fputs(chr1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_038
 * @tc.name     fputs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFputs003, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    const char chr1[10] = "hello";
    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fputs(chr1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_039
 * @tc.name     fputs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFputs004, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    const char chr1[10] = "hello";
    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fputs(chr1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    (void)fseek (fp, 0L, SEEK_SET);
    ret = fread(str, strlen(chr1) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = strcmp(str, "hello");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_040
 * @tc.name     fputs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFputs005, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    const char chr1[10] = "hello";
    char str[20] = {0};
    int i;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    for (i = 0; i < 200; i++) {
        ret = fputs(chr1, fp);
        TEST_ASSERT_TRUE(ret != -1);
    }

    ret = ftell(fp);
    TEST_ASSERT_EQUAL_INT(ret, 1000);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_041
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite001, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fwrite(chr1, strlen(chr1) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_042
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite002, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fwrite(0, 0, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_043
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite003, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "r");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fread(chr1, strlen(chr1) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_044
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite004, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "r");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fread(0, 0, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_045
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite005, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "a");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fwrite(chr1, strlen(chr1) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_046
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite006, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fwrite(chr1, strlen(chr1) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    (void)fseek(fp, 0L, SEEK_SET);

    ret = fread(str, 20, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fwrite(chr2, strlen(chr2) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    (void)fseek(fp, 0L, SEEK_SET);
    ret = fread(str, 20, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = strcmp(str, "helloworld");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);

}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_047
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite007, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[20] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    ret = fwrite(chr1, strlen(chr1) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = fwrite(chr2, strlen(chr2) + 1, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    (void)fseek(fp, 0L, SEEK_SET);

    ret = fread(str, 20, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = strcmp(str, "helloworld");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_048
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite008, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;
    int i;

    const char chr1[10] = "hello";
    const char chr2[10] = "world";

    char str[30] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    for (i = 0; i < 200; i++) {
        ret = fwrite(chr1, strlen(chr1) + 1, 1, fp);
        TEST_ASSERT_TRUE(ret != -1);
    }

    (void)fseek(fp, 100L, SEEK_SET);

    ret = fread(str, 20, 1, fp);
    TEST_ASSERT_TRUE(ret != -1);

    ret = strcmp(str, "hellohellohellohello");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_049
 * @tc.name     fread and fwrite
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsFreadFwrite009, Function | MediumTest | Level1)
{
    int ret = 0;
    FILE *fp = NULL;
    long off = 0;
    int i;

    const char chr1[10] = "123456789";
    char str[30] = {0};

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w+");
    TEST_ASSERT_NOT_NULL(fp);

    for (i = 0; i < 200; i++) {
        ret = fwrite(chr1, 5, 1, fp);
        TEST_ASSERT_TRUE(ret != -1);
    }

    (void)fseek(fp, 100L, SEEK_SET);

    for (i = 0; i < 10; i++) {

        ret = fread(str, 20, 1, fp);
        TEST_ASSERT_TRUE(ret != -1);
    }
    ret = strcmp(str, "1234512345123451234512345");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = fclose(fp);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_050
 * @tc.name     readdir
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsReaddir001, Function | MediumTest | Level1)
{
    DIR *dirp;
    struct dirent *dResult;

    dirp = opendir(DIR1);
    TEST_ASSERT_NOT_NULL(dirp);

    dResult = readdir(dirp);
    TEST_ASSERT_NOT_NULL(dResult);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_051
 * @tc.name     readdir
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsReaddir002, Function | MediumTest | Level1)
{
    DIR *dirp;
    struct dirent *dResult;
    long tellDir0;
    long tellDir1;
    long tellDir2;

    dirp = opendir(DIR1);
    TEST_ASSERT_NOT_NULL(dirp);

    dResult = readdir(dirp);
    TEST_ASSERT_NOT_NULL(dirp);
    tellDir0 = dResult->d_off;
    TEST_ASSERT_TRUE(tellDir0 == tellDir1);

    dResult = readdir(dirp);
    TEST_ASSERT_NOT_NULL(dirp);
    tellDir1 = dResult->d_off;

    rewinddir(dirp);
    dResult = readdir(dirp);
    TEST_ASSERT_NOT_NULL(dirp);
    tellDir2 = dResult->d_off;

    TEST_ASSERT_TRUE(tellDir0 == tellDir2);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_052
 * @tc.name     remove
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsRemove001, Function | MediumTest | Level1)
{
    FILE *fp = NULL;
    int ret = 0;

    fp = fopen(TEST_FILE_PTAH_RIGHT, "w");
    fclose(fp);
    ret = remove(TEST_FILE_PTAH_RIGHT);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_053
 * @tc.name     remove
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsRemove002, Function | MediumTest | Level1)
{
    int ret = 0;

    ret = mkdir("a", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);
    ret = remove("a");
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_054
 * @tc.name     rmdir
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsRmdir001, Function | MediumTest | Level1)
{
    int ret = 0;

    ret = mkdir("a", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = mkdir("a/b", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a/b");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a");
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_055
 * @tc.name     rmdir
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsRmdir002, Function | MediumTest | Level1)
{
    int ret = 0;

    ret = mkdir("a", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = mkdir("a/b", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = mkdir("a/c", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a/b");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a/c");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a");
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_056
 * @tc.name     rmdir
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsRmdir003, Function | MediumTest | Level1)
{
    int ret = 0;

    ret = mkdir("a", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = mkdir("a/b", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = mkdir("a/c", 0777);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a/b");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a");
    TEST_ASSERT_EQUAL_INT(ret, 0);

    ret = rmdir("a/c");
    TEST_ASSERT_EQUAL_INT(ret, -1);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_057
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsUnlink001, Function | MediumTest | Level1)
{
    int ret = 0;
    int fd = 0;
    char tmpFileName[]= "test";

    fd = open(tmpFileName, O_RDWR);
    TEST_ASSERT_TRUE(ret != -1);

    (void)close(fd);
    ret = unlink(tmpFileName);
    TEST_ASSERT_TRUE(ret != -1);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_058
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsUnlink002, Function | MediumTest | Level1)
{
    int ret = 0;
    int fd = 0;
    char tmpFileName[5]= "test";

    fd = creat(tmpFileName, 0777);
    TEST_ASSERT_TRUE(ret != -1);

    ret = unlink(tmpFileName);
    TEST_ASSERT_TRUE(ret != -1);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_059
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsStat001, Function | MediumTest | Level1)
{
    struct stat buf;
    int fd = 0;
    char tmpFileName[5]= "test";
    int ret = 0;

    fd = open(tmpFileName, O_CREAT | O_RDWR, 0777);
    TEST_ASSERT_TRUE(ret != -1);

    (void)close(fd);

    ret = stat(tmpFileName, &buf);
    TEST_ASSERT_TRUE(ret != -1);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_060
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsStat002, Function | MediumTest | Level1)
{
    struct stat buf;
    int fd = 0;
    char tmpFileName[5]= "test";
    int ret = 0;
    ssize_t size = 0;
    char writeBuf[] = "write test";

    fd = open(tmpFileName, O_CREAT | O_RDWR, 0777);
    TEST_ASSERT_TRUE(ret != -1);

    size = write(fd, writeBuf, sizeof(writeBuf));
    TEST_ASSERT_TRUE(ret != -1);
    (void)close(fd);

    ret = stat(tmpFileName, &buf);
    TEST_ASSERT_TRUE(ret != -1);

    TEST_ASSERT_TRUE(buf.st_size == sizeof(writeBuf));
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_061
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsStat003, Function | MediumTest | Level1)
{
    struct stat buf;
    int fd;
    char tmpFileName[5]= "test";
    int ret = 0;
    ssize_t size;
    char writeBuf[] = "write test";

    fd = open(tmpFileName, O_CREAT | O_RDWR, 0777);
    TEST_ASSERT_TRUE(ret != -1);

    size = write(fd, writeBuf, sizeof(writeBuf));
    TEST_ASSERT_TRUE(ret != -1);
    (void)close(fd);

    ret = stat(tmpFileName, &buf);
    TEST_ASSERT_TRUE(ret != -1);

    TEST_ASSERT_EQUAL_INT(buf.st_rdev, 0);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_062
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsWrite001, Function | MediumTest | Level1)
{
    int reLseek;
    int fd = 0;
    char writeBuf[100];
    int ret = 0;
    char tmpFileName[5]= "test";

    for (int i = 0; i < 100; i++) {
        writeBuf[i] = '1';
    }

    fd = open(tmpFileName, O_CREAT | O_RDWR, 0777);
    TEST_ASSERT_TRUE(ret != -1);

    ret = write(fd, writeBuf, 20);
    TEST_ASSERT_TRUE(ret != -1);

    reLseek = lseek(fd, 0, SEEK_CUR);

    ret = write(fd, writeBuf, 20);
    TEST_ASSERT_TRUE(ret != -1);

    reLseek = lseek(fd, 0, SEEK_CUR);

    TEST_ASSERT_TRUE(40 == reLseek);

    (void)close(fd);
}

/* *
 * @tc.number   SUB_KERNEL_FS_DIRNAME_063
 * @tc.name     unlink
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, testFsWrite002, Function | MediumTest | Level1)
{
    int fd = 0;
    char writeBuf[] = "123456789";
    int ret = 0;
    struct stat statbuf;
    char tmpFileName[5]= "test";

    fd = open(tmpFileName, O_CREAT | O_RDWR, 0777);
    TEST_ASSERT_TRUE(ret != -1);

    ret = write(fd, writeBuf, 20);
    TEST_ASSERT_TRUE(ret != -1);

    ret = stat(tmpFileName, &statbuf);
    TEST_ASSERT_TRUE(ret != -1);

    TEST_ASSERT_TRUE(statbuf.st_size == sizeof(writeBuf));

    (void)close(fd);
}

RUN_TEST_SUITE(PosixFsFuncTestSuite);


void PosixFsFuncTest()
{
    LOG("begin PosixFsFuncTest....");
    RUN_ONE_TESTCASE(testFsDirname001);
    RUN_ONE_TESTCASE(testFsDirname002);
    RUN_ONE_TESTCASE(testFsDirname003);
    RUN_ONE_TESTCASE(testFsDirname004);

    RUN_ONE_TESTCASE(testFsFopenFclose001);
    RUN_ONE_TESTCASE(testFsFopenFclose002);
    RUN_ONE_TESTCASE(testFsFopenFclose003);
    RUN_ONE_TESTCASE(testFsFopenFclose004);
    RUN_ONE_TESTCASE(testFsFopenFclose005);
    RUN_ONE_TESTCASE(testFsFopenFclose006);
    RUN_ONE_TESTCASE(testFsFopenFclose007);
    RUN_ONE_TESTCASE(testFsFopenFclose008);
    RUN_ONE_TESTCASE(testFsFopenFclose009);
    RUN_ONE_TESTCASE(testFsFopenFclose010);
    RUN_ONE_TESTCASE(testFsFopenFclose011);

    RUN_ONE_TESTCASE(testFsFdopen001);
    RUN_ONE_TESTCASE(testFsFdopen002);
    RUN_ONE_TESTCASE(testFsFdopen003);
    RUN_ONE_TESTCASE(testFsFdopen004);


    RUN_ONE_TESTCASE(testFsFtellFseek001);
    RUN_ONE_TESTCASE(testFsFtellFseek002);
    RUN_ONE_TESTCASE(testFsFtellFseek003);
    RUN_ONE_TESTCASE(testFsFtellFseek004);
    RUN_ONE_TESTCASE(testFsFtellFseek005);
    RUN_ONE_TESTCASE(testFsFtellFseek006);
    RUN_ONE_TESTCASE(testFsFtellFseek007);
    RUN_ONE_TESTCASE(testFsFtellFseek008);
    RUN_ONE_TESTCASE(testFsFtellFseek009);
    RUN_ONE_TESTCASE(testFsFtellFseek010);
    RUN_ONE_TESTCASE(testFsFtellFseek011);
    RUN_ONE_TESTCASE(testFsFtellFseek012);
    RUN_ONE_TESTCASE(testFsFtellFseek013);
    RUN_ONE_TESTCASE(testFsFtellFseek014);
    RUN_ONE_TESTCASE(testFsFtellFseek015);
    RUN_ONE_TESTCASE(testFsFtellFseek016);

    RUN_ONE_TESTCASE(testFsFputs001);
    RUN_ONE_TESTCASE(testFsFputs002);
    RUN_ONE_TESTCASE(testFsFputs003);
    RUN_ONE_TESTCASE(testFsFputs004);
    RUN_ONE_TESTCASE(testFsFputs005);

    RUN_ONE_TESTCASE(testFsFreadFwrite001);
    RUN_ONE_TESTCASE(testFsFreadFwrite002);
    RUN_ONE_TESTCASE(testFsFreadFwrite003);
    RUN_ONE_TESTCASE(testFsFreadFwrite004);
    RUN_ONE_TESTCASE(testFsFreadFwrite005);
    RUN_ONE_TESTCASE(testFsFreadFwrite006);
    RUN_ONE_TESTCASE(testFsFreadFwrite007);
    RUN_ONE_TESTCASE(testFsFreadFwrite008);
    RUN_ONE_TESTCASE(testFsFreadFwrite009);

    RUN_ONE_TESTCASE(testFsReaddir001);
    RUN_ONE_TESTCASE(testFsReaddir002);

    RUN_ONE_TESTCASE(testFsRemove001);
    RUN_ONE_TESTCASE(testFsRemove002);

    RUN_ONE_TESTCASE(testFsRmdir001);
    RUN_ONE_TESTCASE(testFsRmdir002);
    RUN_ONE_TESTCASE(testFsRmdir003);

    RUN_ONE_TESTCASE(testFsUnlink001);
    RUN_ONE_TESTCASE(testFsUnlink002);

    RUN_ONE_TESTCASE(testFsStat001);
    RUN_ONE_TESTCASE(testFsStat002);
    RUN_ONE_TESTCASE(testFsStat003);

    RUN_ONE_TESTCASE(testFsWrite001);
    RUN_ONE_TESTCASE(testFsWrite002);

    return;
}
