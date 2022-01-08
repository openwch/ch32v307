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

#ifndef _FS_OPERATIONS_H_
#define _FS_OPERATIONS_H_

#include "fcntl.h"
#include "dirent.h"
#include "unistd.h"
#include "sys/mount.h"
#include "sys/stat.h"
#include "sys/statfs.h"

#define FS_SUCCESS            0
#define FS_FAILURE            (-1)
#define MAX_FILESYSTEM_LEN 2

struct MountOps {
    int (*Mount)(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags,
        const void *data);
    int (*Umount)(const char* target);
    int (*Umount2)(const char* target, int flag);
    int (*Statfs)(const char *path, struct statfs *buf);
};

struct FsMap {
    const char *fileSystemtype;
    const struct MountOps *fsMops;
    const struct FileOps *fsFops;
};

struct FileOps {
    int (*Open)(const char *path, int openFlag, ...);
    int (*Close)(int fd);
    int (*Unlink)(const char *fileName);
    int (*Rmdir)(const char *dirName);
    int (*Mkdir)(const char *dirName, mode_t mode);
    struct dirent *(*Readdir)(DIR *dir);
    DIR *(*Opendir)(const char *dirName);
    int (*Closedir)(DIR *dir);
    int (*Read)(int fd, void *buf, size_t len);
    int (*Write)(int fd, const void *buf, size_t len);
    off_t (*Seek)(int fd, off_t offset, int whence);
    int (*Getattr)(const char *path, struct stat *buf);
    int (*Rename)(const char *oldName, const char *newName);
    int (*Fsync)(int fd);
    int (*Fstat)(int fd, struct stat *buf);
    int (*Stat)(const char *path, struct stat *buf);
    int (*Ftruncate)(int fd, off_t length);
};

#endif /* _FS_OPERATIONS_H_ */
