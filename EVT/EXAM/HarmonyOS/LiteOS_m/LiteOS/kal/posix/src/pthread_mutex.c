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

#include <pthread.h>
#include <time.h>
#include "los_compiler.h"
#include "los_mux.h"
#include "errno.h"


#define OS_SYS_NS_PER_MSECOND 1000000
#define OS_SYS_NS_PER_SECOND  1000000000

static inline int MapError(UINT32 err)
{
    switch (err) {
        case LOS_OK:
            return 0;
        case LOS_ERRNO_MUX_PEND_INTERR:
            return EPERM;
        case LOS_ERRNO_MUX_PEND_IN_LOCK:
            return EDEADLK;
        case LOS_ERRNO_MUX_PENDED:
        case LOS_ERRNO_MUX_UNAVAILABLE:
            return EBUSY;
        case LOS_ERRNO_MUX_TIMEOUT:
            return ETIMEDOUT;
        case LOS_ERRNO_MUX_ALL_BUSY:
            return EAGAIN;
        case LOS_ERRNO_MUX_INVALID:
        default:
            return EINVAL;
    }
}

/* Initialize mutex. If mutexAttr is NULL, use default attributes. */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexAttr)
{
    UINT32 muxHandle;
    UINT32 ret;

    if (mutexAttr != NULL) {
        return EOPNOTSUPP;
    }

    ret = LOS_MuxCreate(&muxHandle);
    if (ret != LOS_OK) {
        return MapError(ret);
    }

    mutex->magic = _MUX_MAGIC;
    mutex->handle = muxHandle;

    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    UINT32 ret;
    if (mutex->magic != _MUX_MAGIC) {
        return EINVAL;
    }
    ret = LOS_MuxDelete(mutex->handle);
    if (ret != LOS_OK) {
        return MapError(ret);
    }
    mutex->handle = _MUX_INVALID_HANDLE;
    return 0;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *absTimeout)
{
    UINT32 ret;
    UINT32 timeout;
    UINT64 timeoutNs;
    struct timespec curTime = {0};
    if ((mutex->magic != _MUX_MAGIC) || (absTimeout->tv_nsec < 0) || (absTimeout->tv_nsec >= OS_SYS_NS_PER_SECOND)) {
        return EINVAL;
    }
    if (mutex->handle == _MUX_INVALID_HANDLE) {
        ret = LOS_MuxCreate(&mutex->handle);
        if (ret != LOS_OK) {
            return MapError(ret);
        }
    }
    ret = clock_gettime(CLOCK_REALTIME, &curTime);
    if (ret != LOS_OK) {
        return EINVAL;
    }
    timeoutNs = (absTimeout->tv_sec - curTime.tv_sec) * OS_SYS_NS_PER_SECOND + (absTimeout->tv_nsec - curTime.tv_nsec);
    if (timeoutNs <= 0) {
        return ETIMEDOUT;
    }
    timeout = (timeoutNs + (OS_SYS_NS_PER_MSECOND - 1)) / OS_SYS_NS_PER_MSECOND;
    ret = LOS_MuxPend(mutex->handle, timeout);
    return MapError(ret);
}

/* Lock mutex, waiting for it if necessary. */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    UINT32 ret;
    if (mutex->magic != _MUX_MAGIC) {
        return EINVAL;
    }
    if (mutex->handle == _MUX_INVALID_HANDLE) {
        ret = LOS_MuxCreate(&mutex->handle);
        if (ret != LOS_OK) {
            return MapError(ret);
        }
    }
    ret = LOS_MuxPend(mutex->handle, LOS_WAIT_FOREVER);
    return MapError(ret);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    UINT32 ret;
    if (mutex->magic != _MUX_MAGIC) {
        return EINVAL;
    }
    if (mutex->handle == _MUX_INVALID_HANDLE) {
        ret = LOS_MuxCreate(&mutex->handle);
        if (ret != LOS_OK) {
            return MapError(ret);
        }
    }
    ret = LOS_MuxPend(mutex->handle, 0);
    return MapError(ret);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    UINT32 ret;
    if (mutex->magic != _MUX_MAGIC) {
        return EINVAL;
    }
    ret = LOS_MuxPost(mutex->handle);
    return MapError(ret);
}

