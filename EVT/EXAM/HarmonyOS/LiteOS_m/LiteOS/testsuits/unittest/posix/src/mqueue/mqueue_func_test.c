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

#include <securec.h>
#include "hctest.h"
#include <mqueue.h>
#include <fcntl.h>
#include "common_test.h"

#define MQUEUE_STANDARD_NAME_LENGTH 50
#define MQUEUE_NO_ERROR 0
#define MQUEUE_SEND_STRING_TEST "mq_test"
#define MQUEUE_SHORT_ARRAY_LENGTH strlen(MQUEUE_SEND_STRING_TEST)

const int MQ_NAME_LEN = 64;  // mqueue name len
const int MQ_TX_LEN   = 64;  // mqueue send buffer len
const int MQ_RX_LEN   = 64;  // mqueue receive buffer len
const int MQ_MSG_SIZE = 64;	 // mqueue message size
const int MQ_MSG_PRIO = 0;   // mqueue message priority
const int MQ_MAX_MSG  = 16;	 // mqueue message number
const char MQ_MSG[] = "MessageToSend";  // mqueue message to send
const int MQ_MSG_LEN = sizeof(MQ_MSG);  // mqueue message len to send

const int MAX_MQ_NUMBER   = 1024;   // max mqueue number
const int MAX_MQ_NAME_LEN = 256;    // max mqueue name length
const int MAX_MQ_MSG_SIZE = 65530;  // max mqueue message size

// return n: 0 < n <= max
unsigned int GetRandom(unsigned int max)
{
    if (max == 0 || max == 1) {
        return 1;
    }
    return (rand() % max) + 1;
}

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is mqueue
 * @param        : module name is mqueue
 * @param        : test suit name is MqueueFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Mqueue, MqueueFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL MqueueFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL MqueueFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_KERNEL_PTHREAD_OPERATION_001
 * @tc.name      : event operation for creat
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqueue001, Function | MediumTest | Level1)
{
    unsigned int ret;
    char msgrcd[MQUEUE_STANDARD_NAME_LENGTH] = {0};
    char mqname[MQUEUE_STANDARD_NAME_LENGTH] = "";
    const char *msgptr = MQUEUE_SEND_STRING_TEST;
    struct mq_attr attr = { 0 };
    mqd_t mqueue;

    attr.mq_msgsize = MQUEUE_STANDARD_NAME_LENGTH;
    attr.mq_maxmsg = 1;

    snprintf_s(mqname, MQUEUE_STANDARD_NAME_LENGTH, MQUEUE_STANDARD_NAME_LENGTH - 1, "/mq002_%d", 1);

    mqueue = mq_open(mqname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_GOTO_NOT_EQUAL(mqueue, (mqd_t)-1, mqueue, EXIT1);

    ret = mq_send(mqueue, msgptr, strlen(msgptr), 0);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT1);

    ret = mq_receive(mqueue, msgrcd, MQUEUE_STANDARD_NAME_LENGTH, NULL);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_SHORT_ARRAY_LENGTH, ret, EXIT1);
    ICUNIT_GOTO_STRING_EQUAL(msgrcd, MQUEUE_SEND_STRING_TEST, msgrcd, EXIT1);

    ret = mq_close(mqueue);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT1);

    ret = mq_unlink(mqname);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    return;

EXIT1:
    mq_close(mqueue);
EXIT:
    mq_unlink(mqname);
    return;
};

/**
 * @tc.number SUB_KERNEL_IPC_MQ_OPEN_0100
 * @tc.name   mq_open function errno for EEXIST test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqOpenEEXIST, Function | MediumTest | Level2)
{
    char qName[MQ_NAME_LEN];
    mqd_t queue, queueOther;
    int ret;

    sprintf_s(qName, MQ_NAME_LEN, "testMqOpenEEXIST_%d", GetRandom(10000));
    queue = mq_open(qName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, NULL);
    ICUNIT_GOTO_NOT_EQUAL(queue, (mqd_t)-1, queue, EXIT1);

    queueOther = mq_open(qName, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    ICUNIT_GOTO_EQUAL(queueOther, (mqd_t)-1, queueOther, EXIT1);
    ICUNIT_GOTO_EQUAL(errno, EEXIST, errno, EXIT1);

EXIT1:
    ret = mq_close(queue);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);
EXIT:
    ret = mq_unlink(qName);
    ICUNIT_ASSERT_EQUAL_VOID(ret, 0, ret);
    return;
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_OPEN_0200
 * @tc.name   mq_open function errno for EINVAL test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqOpenEINVAL, Function | MediumTest | Level2)
{
    int i;
    mqd_t queue;
    struct mq_attr attr = {0};
    char qName[MQ_NAME_LEN];
    const int max = 65535;

    sprintf_s(qName, MQ_NAME_LEN, "testMqOpenEINVAL_%d", GetRandom(10000));

    for (i = 0; i<6; i++) {
        switch (i) {
            case 0:
                attr.mq_msgsize = -1;
                attr.mq_maxmsg = max;
                break;
            case 1:
                /* attr.mq_msgsize > USHRT_MAX - 4 */
                attr.mq_msgsize = max;
                attr.mq_maxmsg = max;
                break;
            case 2:
                attr.mq_msgsize = 10;
                attr.mq_maxmsg = -1;
                break;
            case 3:
                attr.mq_msgsize = 10;
                attr.mq_maxmsg = max + 1;
                break;

            case 4:
                attr.mq_msgsize = 0;
                attr.mq_maxmsg = 16;
                break;

            case 5:
                attr.mq_msgsize = 64;
                attr.mq_maxmsg = 0;
                break;
        }

        queue = mq_open(qName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
        ICUNIT_TRACK_EQUAL(queue, (mqd_t)-1, queue);

        if (queue != (mqd_t)-1) {
            mq_close(queue);
            mq_unlink(qName);
        }

        /* if NOT call mq_close & mq_unlink then errno == ENOENT */
        ICUNIT_TRACK_EQUAL(errno, EINVAL, errno);
    }

    for (i=0; i<MQ_NAME_LEN; i++) {
        qName[i] = 0;
    }
    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_maxmsg = MQ_MAX_MSG;
    queue = mq_open(qName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_TRACK_EQUAL(errno, EINVAL, errno);
}


/**
 * @tc.number SUB_KERNEL_IPC_MQ_OPEN_0300
 * @tc.name   mq_open function errno for ENAMETOOLONG test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqOpenENAMETOOLONG, Function | MediumTest | Level2)
{
    char qName[MAX_MQ_NAME_LEN + 10];
    mqd_t queue;
    int i;

    for (i=0; i<MAX_MQ_NAME_LEN + 5; i++) {
        qName[i] = '8';
    }
    qName[i] = '\0';

    queue = mq_open(qName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, NULL);
    ICUNIT_TRACK_EQUAL(queue, (mqd_t)-1, queue);

    if (queue != (mqd_t)-1) {
        mq_close(queue);
        mq_unlink(qName);
    }

    ICUNIT_TRACK_EQUAL(errno, ENAMETOOLONG, errno);
}


/**
 * @tc.number SUB_KERNEL_IPC_MQ_OPEN_0400
 * @tc.name   mq_open function errno for ENOENT test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqOpenENOENT, Function | MediumTest | Level3)
{
    mqd_t queue;
    char qName[MQ_NAME_LEN];

    sprintf_s(qName, MQ_NAME_LEN, "testMqOpenENOENT_%d", GetRandom(10000));
    queue = mq_open(qName, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    ICUNIT_TRACK_EQUAL(queue, (mqd_t)-1, queue);

    if (queue != (mqd_t)-1) {
        mq_close(queue);
        mq_unlink(qName);
    }
    ICUNIT_TRACK_EQUAL(errno, ENOENT, errno);
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_OPEN_0500
 * @tc.name   mq_open function errno for ENFILE test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqOpenENFILE, Function | MediumTest | Level3)
{
    char qName[MAX_MQ_NUMBER + 1][30];
    mqd_t queue[MAX_MQ_NUMBER + 1];
    int flag = 0;
    int i;
    for (i=0; i<MAX_MQ_NUMBER + 1; i++) {
        sprintf_s(qName[i], MQ_NAME_LEN, "testMqOpenENFILE_%d", i);
    }

    for (i=0; i<MAX_MQ_NUMBER; i++) {
        queue[i] = mq_open(qName[i], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, NULL);

        if (queue[i] == (mqd_t)-1) {
            flag = 1;
            printf("break: i = %d", i);
            break;
        }
        ICUNIT_TRACK_EQUAL(queue[i], (mqd_t)-1, queue[i]);
    }

    printf("func: i = %d", i);
    if (flag == 0) {
        queue[i] = mq_open(qName[i], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, NULL);
    }
    ICUNIT_TRACK_EQUAL(queue[i], (mqd_t)-1, queue[i]);
    ICUNIT_TRACK_EQUAL(errno, ENFILE, errno);

    for (i=0; i<MAX_MQ_NUMBER+1; i++) {
        mq_close(queue[i]);
        mq_unlink(qName[i]);
    }
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_OPEN_0600
 * @tc.name   mq_open function errno for ENOSPC test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqOpenENOSPC, Function | MediumTest | Level3)
{
    mqd_t queue;
    struct mq_attr setAttr = {0};
    char qName[MQ_NAME_LEN];

    sprintf_s(qName, MQ_NAME_LEN, "testMqOpenENOSPC_%d", GetRandom(10000));
    setAttr.mq_msgsize = MAX_MQ_MSG_SIZE + 1;
    setAttr.mq_maxmsg = MAX_MQ_NAME_LEN;
    queue = mq_open(qName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &setAttr);
    ICUNIT_TRACK_EQUAL(queue, (mqd_t)-1, queue);

    if (queue != (mqd_t)-1) {
        mq_close(queue);
        mq_unlink(qName);
    }
    ICUNIT_TRACK_EQUAL(errno, ENOSPC, errno);
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_CLOSE_0100
 * @tc.name   mq_close function errno for EBADF test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqCloseEBADF, Function | MediumTest | Level2)
{
    ICUNIT_TRACK_EQUAL(mq_close(NULL), -1, -1);
    ICUNIT_TRACK_EQUAL(errno, EBADF, errno);
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_SEND_0100
 * @tc.name   mq_send function errno for EAGAIN test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqSendEAGAIN, Function | MediumTest | Level2)
{
    mqd_t queue;
    struct mq_attr attr = {0};
    char qName[MQ_NAME_LEN];
    int ret;

    sprintf_s(qName, MQ_NAME_LEN, "testMqSendEAGAIN_%d", GetRandom(10000));
    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_maxmsg = 1;
    queue = mq_open(qName, O_CREAT | O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_ASSERT_EQUAL_VOID(queue, (mqd_t)-1, queue);

    ret = mq_send(queue, MQ_MSG, MQ_MSG_LEN, 0);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);

    ret = mq_send(queue, MQ_MSG, MQ_MSG_LEN, 0);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EAGAIN, errno);

    ret = mq_close(queue);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);

    ret = mq_unlink(qName);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_SEND_0200
 * @tc.name   mq_send function errno for EBADF and EMSGSIZE test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqSendEBADFEMSGSIZE, Function | MediumTest | Level2)
{
    mqd_t queue;
    struct mq_attr attr = {0};
    char qName[MQ_NAME_LEN];
    int ret;

    sprintf_s(qName, MQ_NAME_LEN, "testMqSendEAGAIN_%d", GetRandom(10000));
    attr.mq_msgsize = 1;
    attr.mq_maxmsg = MQ_MAX_MSG;
    queue = mq_open(qName, O_CREAT | O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_ASSERT_EQUAL_VOID(queue, (mqd_t)-1, queue);

    ret = mq_send(NULL, MQ_MSG, 1, MQ_MSG_PRIO);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EBADF, errno);


    ret = mq_send(queue, MQ_MSG, MQ_MSG_LEN, MQ_MSG_PRIO);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EMSGSIZE, errno);

    ret = mq_close(queue);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
    ret = mq_unlink(qName);
    ICUNIT_ASSERT_EQUAL_VOID(ret, 0, ret);

    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_maxmsg = MQ_MAX_MSG;
    queue = mq_open(qName, O_CREAT | O_RDONLY | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_ASSERT_EQUAL_VOID(queue, (mqd_t)-1, queue);

    ret = mq_send(queue, MQ_MSG, MQ_MSG_LEN, MQ_MSG_PRIO);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EBADF, errno);

    attr.mq_flags |= O_NONBLOCK;
    ret = mq_setattr(queue, &attr, NULL);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
    
    ret = mq_send(queue, MQ_MSG, MQ_MSG_LEN, MQ_MSG_PRIO);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EBADF, errno);

    ret = mq_close(queue);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);

    ret = mq_unlink(qName);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_SEND_0300
 * @tc.name   mq_send function errno for EINVAL  test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqSendEINVAL, Function | MediumTest | Level3)
{
    mqd_t queue;
    struct mq_attr attr = {0};
    char qName[MQ_NAME_LEN];
    int ret;

    sprintf_s(qName, MQ_NAME_LEN, "testMqSendEINVAL_%d", GetRandom(10000));
    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_maxmsg = MQ_MAX_MSG;
    queue = mq_open(qName, O_CREAT | O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_ASSERT_EQUAL_VOID(queue, (mqd_t)-1, queue);

    ret = mq_send(queue, MQ_MSG, 0, MQ_MSG_PRIO);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EINVAL, errno);

    ret = mq_close(queue);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);

    ret = mq_unlink(qName);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
}

/**
 * @tc.number SUB_KERNEL_IPC_MQ_RECEIVE_0100
 * @tc.name   mq_receive function errno for EAGAIN test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(MqueueFuncTestSuite, testMqReceiveEAGAIN, Function | MediumTest | Level2)
{
    mqd_t queue;
    unsigned int prio;
    struct mq_attr attr = {0};
    struct mq_attr getAttr = {0};
    char qName[MQ_NAME_LEN], rMsg[MQ_RX_LEN];
    int ret;

    sprintf_s(qName, MQ_NAME_LEN, "testMqReceiveEAGAIN_%d", GetRandom(10000));
    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_maxmsg = MQ_MAX_MSG;
    queue = mq_open(qName, O_CREAT | O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_ASSERT_EQUAL_VOID(queue, (mqd_t)-1, queue);

    ret = mq_send(queue, MQ_MSG, MQ_MSG_LEN, MQ_MSG_PRIO);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
    ret = mq_getattr(queue, &getAttr);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
    ret = mq_receive(queue, rMsg, getAttr.mq_msgsize, &prio);
    ICUNIT_TRACK_NOT_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(prio, MQ_MSG_PRIO, prio);
    ICUNIT_TRACK_EQUAL(strncmp(MQ_MSG, rMsg, MQ_MSG_LEN), 0, -1);
    ret = mq_receive(queue, rMsg, getAttr.mq_msgsize, &prio);
    ICUNIT_TRACK_EQUAL(ret, -1, ret);
    ICUNIT_TRACK_EQUAL(errno, EAGAIN, errno);

    ret = mq_close(queue);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);

    ret = mq_unlink(qName);
    ICUNIT_TRACK_EQUAL(ret, 0, ret);
}

RUN_TEST_SUITE(MqueueFuncTestSuite);
