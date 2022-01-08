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

#define _GNU_SOURCE 1
#include "ff.h"
#include "fatfs.h"
#include "errno.h"
#include "stdbool.h"
#include "limits.h"
#include "pthread.h"
#include "time.h"
#include "securec.h"
#include "los_compiler.h"
#include "los_debug.h"
#include "cmsis_os2.h"
#include "fs_operations.h"

/* the max name length of different parts should not bigger than 32 */
#define FS_DRIVE_NAME_MAX_LEN 32

#ifndef FAT_MAX_OPEN_DIRS
#define FAT_MAX_OPEN_DIRS     8
#endif /* FAT_MAX_OPEN_DIRS */

#ifndef FS_LOCK_TIMEOUT_SEC
#define FS_LOCK_TIMEOUT_SEC  15
#endif /* FS_LOCK_TIMEOUT_SEC */

#define PART_NAME   0x0
#define VOLUME_NAME 0x1
#define PATH_NAME   0x2
#define NAME_MASK   0x3

typedef struct {
    UINT8 useFlag;
    FIL fil;
} FatHandleStruct;

static FatHandleStruct g_handle[FAT_MAX_OPEN_FILES] = {0};
static DIR g_dir[FAT_MAX_OPEN_DIRS] = {0};
static FATFS g_fatfs[FF_VOLUMES] = {0};
static UINT8 g_workBuffer[FF_MAX_SS];
static UINT32 g_fileNum = 0;
static UINT32 g_dirNum = 0;
static struct dirent g_retValue;
static pthread_mutex_t g_fsMutex = PTHREAD_MUTEX_INITIALIZER;

static const char * const g_volPath[FF_VOLUMES] = {FF_VOLUME_STRS};
static BOOL g_volWriteEnable[FF_VOLUMES] = {FALSE};

static int FsLock(void)
{
    INT32 ret = 0;
    struct timespec absTimeout = {0};
    if (osKernelGetState() != osKernelRunning) {
        return ret;
    }
    ret = clock_gettime(CLOCK_REALTIME, &absTimeout);
    if (ret != 0) {
        PRINTK("clock gettime err 0x%x!\r\n", errno);
        return errno;
    }
    absTimeout.tv_sec += FS_LOCK_TIMEOUT_SEC;
    ret = pthread_mutex_timedlock(&g_fsMutex, &absTimeout);
    return ret;
}

static void FsUnlock(void)
{
    if (osKernelGetState() != osKernelRunning) {
        return;
    }
    (void)pthread_mutex_unlock(&g_fsMutex);
}

static bool IsValidFd(int fd)
{
    if ((fd < 0) || (fd >= FAT_MAX_OPEN_FILES) || (g_handle[fd].useFlag == 0)) {
        return false;
    }
    return true;
}

static int FsChangeDrive(const char *path)
{
    INT32 res;
    CHAR tmpPath[FS_DRIVE_NAME_MAX_LEN] = { "/" }; /* the max name length of different parts is 16 */
    errno_t retErr;
    UINT16 pathLen;
    pathLen = strlen((char const *)path);
    /* make sure the path begin with "/", the path like /xxx/yyy/... */
    if (pathLen >= (FS_DRIVE_NAME_MAX_LEN - 1)) {
        /* 2: except first flag "/" and last end flag */
        pathLen = FS_DRIVE_NAME_MAX_LEN - 2;
    }

    retErr = strncpy_s(tmpPath + 1, (FS_DRIVE_NAME_MAX_LEN - 1), (char const *)path, pathLen);
    if (retErr != EOK) {
        return FS_FAILURE;
    }

    res = f_chdrive(tmpPath);
    if (res != FR_OK) {
        return FS_FAILURE;
    }

    return FS_SUCCESS;
}

static int FsPartitionMatch(const char *path, int flag)
{
    INT32 ret;
    UINT32 index;
    CHAR tmpName[FF_MAX_LFN] = {0};

    if (path == NULL) {
        return FS_FAILURE;
    }

    switch ((UINT32)flag & NAME_MASK) {
        case VOLUME_NAME:
            ret = sscanf_s(path, "/%[^/]", tmpName, FF_MAX_LFN);
            if (ret <= 0) {
                return FS_FAILURE;
            }
            break;
        case PATH_NAME:
            ret = sscanf_s(path, "%[^/]", tmpName, FF_MAX_LFN);
            if (ret <= 0) {
                return FS_FAILURE;
            }
            break;
        case PART_NAME:
        default:
            ret = strcpy_s(tmpName, FF_MAX_LFN, path);
            if (ret != EOK) {
                return FS_FAILURE;
            }
    }

    for (index = 0; index < FF_VOLUMES; index++) {
        if (strcmp(tmpName, g_volPath[index]) == 0) {
            return index;
        }
    }
    return FS_FAILURE;
}

static int Remount(const char *path, unsigned long mountflags)
{
    INT32 index;

    index = FsPartitionMatch(path, PART_NAME);
    if (index == FS_FAILURE) {
        PRINTK("Wrong volume path!\r\n");
        errno = ENOENT;
        return FS_FAILURE;
    }

    /* remount is not allowed when the device is not mounted. */
    if (g_fatfs[index].fs_type == 0) {
        errno = EINVAL;
        return FS_FAILURE;
    }
    g_volWriteEnable[index] = (mountflags & MS_RDONLY) ? FALSE : TRUE;

    return FS_SUCCESS;
}

static bool FsCheckByPath(const char *path)
{
    INT32 index;

    index = FsPartitionMatch(path, PATH_NAME);
    if (index == FS_FAILURE) {
        return FS_FAILURE;
    }

    return g_volWriteEnable[index];
}

static bool FsCheckByID(int id)
{
    INT32 index;

    for (index = 0; index < FF_VOLUMES; index++) {
        if (g_fatfs[index].id == id) {
            return g_volWriteEnable[index];
        }
    }
    return false;
}

static unsigned int FatFsGetMode(int oflags)
{
    UINT32 fmode = FA_READ;

    if ((UINT32)oflags & O_WRONLY) {
        fmode |= FA_WRITE;
    }

    if (((UINT32)oflags & O_ACCMODE) & O_RDWR) {
        fmode |= FA_WRITE;
    }
    /* Creates a new file if the file is not existing, otherwise, just open it. */
    if ((UINT32)oflags & O_CREAT) {
        fmode |= FA_OPEN_ALWAYS;
        /* Creates a new file. If the file already exists, the function shall fail. */
        if ((UINT32)oflags & O_EXCL) {
            fmode |= FA_CREATE_NEW;
        }
    }
    /* Creates a new file. If the file already exists, its length shall be truncated to 0. */
    if ((UINT32)oflags & O_TRUNC) {
        fmode |= FA_CREATE_ALWAYS;
    }

    return fmode;
}

static int FatfsErrno(int result)
{
    INT32 status = 0;

    if (result < 0) {
        return result;
    }

    /* FatFs errno to Libc errno */
    switch (result) {
        case FR_OK:
            break;

        case FR_NO_FILE:
        case FR_NO_PATH:
        case FR_NO_FILESYSTEM:
            status = ENOENT;
            break;

        case FR_INVALID_NAME:
            status = EINVAL;
            break;

        case FR_EXIST:
        case FR_INVALID_OBJECT:
            status = EEXIST;
            break;

        case FR_DISK_ERR:
        case FR_NOT_READY:
        case FR_INT_ERR:
            status = EIO;
            break;

        case FR_WRITE_PROTECTED:
            status = EROFS;
            break;
        case FR_MKFS_ABORTED:
        case FR_INVALID_PARAMETER:
            status = EINVAL;
            break;

        case FR_NO_SPACE_LEFT:
            status = ENOSPC;
            break;
        case FR_NO_DIRENTRY:
            status = ENFILE;
            break;
        case FR_NO_EMPTY_DIR:
            status = ENOTEMPTY;
            break;
        case FR_IS_DIR:
            status = EISDIR;
            break;
        case FR_NO_DIR:
            status = ENOTDIR;
            break;
        case FR_NO_EPERM:
        case FR_DENIED:
            status = EPERM;
            break;
        case FR_LOCKED:
            status = EBUSY;
            break;
        default:
            status = result;
            break;
    }

    return status;
}

int fatfs_mount(const char *source, const char *target,
                const char *filesystemtype, unsigned long mountflags,
                const void *data)
{
    INT32 index;
    FRESULT res;
    INT32 ret;

    if ((target == NULL) || (filesystemtype == NULL)) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (mountflags & MS_REMOUNT) {
        ret = Remount(target, mountflags);
        goto OUT;
    }

    if (strcmp(filesystemtype, "fat") != 0) {
        errno = ENODEV;
        ret = FS_FAILURE;
        goto OUT;
    }

    index = FsPartitionMatch(target, VOLUME_NAME);
    if (index == FS_FAILURE) {
        errno = ENODEV;
        ret = FS_FAILURE;
        goto OUT;
    }

    /* If the volume has been mounted */
    if (g_fatfs[index].fs_type != 0) {
        errno = EBUSY;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_mount(&g_fatfs[index], target, 1);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    g_volWriteEnable[index] = (mountflags & MS_RDONLY) ? FALSE : TRUE;
    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_umount(const char *target)
{
    FRESULT res;
    INT32 ret;
    INT32 index;

    if (target == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    index = FsPartitionMatch(target, VOLUME_NAME);
    if (index == FS_FAILURE) {
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    /* The volume is not mounted */
    if (g_fatfs[index].fs_type == 0) {
        errno = EINVAL;
        ret = FS_FAILURE;
        goto OUT;
    }

    /* umount is not allowed when a file or diretory is opened. */
    if (f_checkopenlock(index) != FR_OK) {
        errno = EBUSY;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_mount((FATFS *)NULL, target, 0);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    if (g_fatfs[index].win != NULL) {
        ff_memfree(g_fatfs[index].win);
    }

    (void)memset_s(&g_fatfs[index], sizeof(FATFS), 0x0, sizeof(FATFS));

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

static int CloseAll(int index)
{
    INT32 i;
    FRESULT res;

    for (i = 0; i < FAT_MAX_OPEN_FILES; i++) {
        if (g_fileNum <= 0) {
            break;
        }
        if ((g_handle[i].useFlag == 1) && (g_handle[i].fil.obj.fs == &g_fatfs[index])) {
            res = f_close(&g_handle[i].fil);
            if (res != FR_OK) {
                errno = FatfsErrno(res);
                return FS_FAILURE;
            }
            (void)memset_s(&g_handle[i], sizeof(FatHandleStruct), 0x0, sizeof(FatHandleStruct));
            g_fileNum--;
        }
    }

    for (i = 0; i < FAT_MAX_OPEN_DIRS; i++) {
        if (g_dirNum <= 0) {
            break;
        }
        if (g_dir[i].obj.fs == &g_fatfs[index]) {
            res = f_closedir(&g_dir[i]);
            if (res != FR_OK) {
                errno = FatfsErrno(res);
                return FS_FAILURE;
            }
            (void)memset_s(&g_dir[i], sizeof(DIR), 0x0, sizeof(DIR));
            g_dirNum--;
        }
    }

    return FS_SUCCESS;
}

int fatfs_umount2(const char *target, int flag)
{
    INT32 index;
    INT32 ret;
    UINT32 flags;
    FRESULT res;

    if (target == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    flags = MNT_FORCE | MNT_DETACH | MNT_EXPIRE | UMOUNT_NOFOLLOW;
    if ((UINT32)flag & ~flags) {
        errno = EINVAL;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    index = FsPartitionMatch(target, VOLUME_NAME);
    if (index == FS_FAILURE) {
        errno = ENOENT;
        ret =  FS_FAILURE;
        goto OUT;
    }

    /* The volume is not mounted */
    if (g_fatfs[index].fs_type == 0) {
        errno = EINVAL;
        ret = FS_FAILURE;
        goto OUT;
    }

    if ((UINT32)flag & MNT_FORCE) {
        ret = CloseAll(index);
        if (ret != FS_SUCCESS) {
            goto OUT;
        }
    }

    res = f_mount((FATFS *)NULL, target, 0);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    if (g_fatfs[index].win != NULL) {
        ff_memfree(g_fatfs[index].win);
    }

    (void)memset_s(&g_fatfs[index], sizeof(FATFS), 0x0, sizeof(FATFS));
    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_open(const char *path, int oflag, ...)
{
    FRESULT res;
    UINT32 i;
    UINT32 openNum = 0;
    UINT32 fmode;
    INT32 ret;
    FILINFO fileInfo = {0};

    if (path == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    fmode = FatFsGetMode(oflag);

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (g_fileNum >= FAT_MAX_OPEN_FILES) {
        PRINTK("FAT g_fileNum is out of range 0x%x!\r\n", g_fileNum);
        errno = ENFILE;
        ret = FS_FAILURE;
        goto OUT;
    }

    for (i = 0; i < FAT_MAX_OPEN_FILES; i++) {
        if (g_handle[i].useFlag == 0) {
            openNum = i;
            break;
        }
    }

    if (i >= FAT_MAX_OPEN_FILES) {
        PRINTK("FAT opennum is out of range 0x%x!\r\n", openNum);
        errno = ENFILE;
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FsChangeDrive(path);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT open ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    /* cannot creat a new file in the write protected part */
    if ((((UINT32)oflag & O_CREAT) != 0) && (!FsCheckByPath(path))) {
        res = f_stat(path, &fileInfo);
        if ((res == FR_NO_FILE) || (res == FR_NO_PATH)) {
            PRINTK("FAT creat err 0x%x!\r\n", res);
            errno = EACCES;
            ret = FS_FAILURE;
            goto OUT;
        }
    }

    res = f_open(&g_handle[openNum].fil, path, fmode);
    if (res != FR_OK) {
        PRINTK("FAT open err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    g_handle[openNum].useFlag = 1;
    g_fileNum++;

    ret = openNum;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_close(int fd)
{
    FRESULT res;
    INT32 ret;

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        FsUnlock();
        errno = EBADF;
        return FS_FAILURE;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        FsUnlock();
        errno = ENOENT;
        return FS_FAILURE;
    }

    res = f_close(&g_handle[fd].fil);
    if (res != FR_OK) {
        PRINTK("FAT close err 0x%x!\r\n", res);
        FsUnlock();
        errno = FatfsErrno(res);
        return FS_FAILURE;
    }

#if !FF_FS_TINY
    if (g_handle[fd].fil.buf != NULL) {
        (void)ff_memfree(g_handle[fd].fil.buf);
    }
#endif

    (void)memset_s(&g_handle[fd], sizeof(FatHandleStruct), 0x0, sizeof(FatHandleStruct));

    if (g_fileNum > 0) {
        g_fileNum--;
    }

    FsUnlock();

    return FS_SUCCESS;
}

ssize_t fatfs_read(int fd, void *buf, size_t nbyte)
{
    FRESULT res;
    INT32 ret;
    UINT32 lenRead;

    if (buf == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        FsUnlock();
        errno = EBADF;
        return FS_FAILURE;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        FsUnlock();
        errno = ENOENT;
        return FS_FAILURE;
    }

    res = f_read(&g_handle[fd].fil, buf, nbyte, &lenRead);
    if (res != FR_OK) {
        FsUnlock();
        errno = FatfsErrno(res);
        return FS_FAILURE;
    }
    FsUnlock();

    return (ssize_t)lenRead;
}

ssize_t fatfs_write(int fd, const void *buf, size_t nbyte)
{
    FRESULT res;
    INT32 ret;
    UINT32 lenWrite;
    static BOOL overFlow = FALSE;

    if (buf == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        errno = EBADF;
        goto ERROUT;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        errno = ENOENT;
        goto ERROUT;
    }

    if (!FsCheckByID(g_handle[fd].fil.obj.fs->id)) {
        errno = EACCES;
        goto ERROUT;
    }

    res = f_write(&g_handle[fd].fil, buf, nbyte, &lenWrite);
    if ((res == FR_OK) && (lenWrite == 0) && (nbyte != 0) && (overFlow == FALSE)) {
        overFlow = TRUE;
        PRINTK("FAT write err 0x%x!\r\n", fd);
    }

    if ((res != FR_OK) || (nbyte != lenWrite)) {
        errno = FatfsErrno(res);
        goto ERROUT;
    }

    FsUnlock();
    return (ssize_t)lenWrite;

ERROUT:
    FsUnlock();
    return FS_FAILURE;
}

off_t fatfs_lseek(int fd, off_t offset, int whence)
{
    FRESULT res;
    INT32 ret;
    off_t pos;

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        errno = EBADF;
        goto ERROUT;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        errno = ENOENT;
        goto ERROUT;
    }

    if (whence == SEEK_SET) {
        pos = 0;
    } else if (whence == SEEK_CUR) {
        pos = f_tell(&g_handle[fd].fil);
    } else if (whence == SEEK_END) {
        pos = f_size(&g_handle[fd].fil);
    } else {
        errno = EINVAL;
        goto ERROUT;
    }

    res = f_lseek(&g_handle[fd].fil, offset + pos);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        goto ERROUT;
    }

    pos = f_tell(&g_handle[fd].fil);
    FsUnlock();
    return pos;

ERROUT:
    FsUnlock();
    return FS_FAILURE;
}

/* Remove the specified FILE */
int fatfs_unlink(const char *path)
{
    FRESULT res;
    INT32 ret;

    if (path == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!FsCheckByPath(path)) {
        errno = EACCES;
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FsChangeDrive(path);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT ulink ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_unlink(path);
    if (res != FR_OK) {
        PRINTK("FAT ulink err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

/* Return information about a file */
int fatfs_fstat(int fd, struct stat *buf)
{
    INT32 ret;

    if (buf == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        FsUnlock();
        errno = EBADF;
        return FS_FAILURE;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        FsUnlock();
        errno = ENOENT;
        return FS_FAILURE;
    }

    buf->st_size = f_size(&g_handle[fd].fil);
    buf->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                   S_IWUSR | S_IWGRP | S_IWOTH |
                   S_IXUSR | S_IXGRP | S_IXOTH;
    if (g_handle[fd].fil.obj.attr & AM_RDO) {
        buf->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    }

    FsUnlock();
    return FS_SUCCESS;
}

int fatfs_stat(const char *path, struct stat *buf)
{
    FRESULT res;
    FILINFO fileInfo = {0};
    INT32 ret;

    if ((path == NULL) || (buf == NULL)) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    ret = FsChangeDrive(path);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT stat ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_stat(path, &fileInfo);
    if (res != FR_OK) {
        PRINTK("FAT stat err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    buf->st_size = fileInfo.fsize;
    buf->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                   S_IWUSR | S_IWGRP | S_IWOTH |
                   S_IXUSR | S_IXGRP | S_IXOTH;

    if (fileInfo.fattrib & AM_RDO) {
        buf->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    }

    if (fileInfo.fattrib & AM_DIR) {
        buf->st_mode &= ~S_IFREG;
        buf->st_mode |= S_IFDIR;
    }

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

/* Synchronize all changes to Flash */
int fatfs_fsync(int fd)
{
    FRESULT res;
    INT32 ret;

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        errno = EBADF;
        ret = FS_FAILURE;
        goto OUT;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    if (!FsCheckByID(g_handle[fd].fil.obj.fs->id)) {
        errno = EACCES;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_sync(&g_handle[fd].fil);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }
    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_mkdir(const char *path, mode_t mode)
{
    FRESULT res;
    INT32 ret;

    if (path == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!FsCheckByPath(path)) {
        errno = EACCES;
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FsChangeDrive(path);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT mkdir ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_mkdir(path);
    if (res != FR_OK) {
        PRINTK("FAT mkdir err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }
    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

DIR *fatfs_opendir(const char *dirName)
{
    FRESULT res;
    UINT32 openNum = 0;
    UINT32 i;
    INT32 ret;

    if (dirName == NULL) {
        errno = EFAULT;
        return NULL;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return NULL;
    }

    if (g_dirNum >= FAT_MAX_OPEN_DIRS) {
        PRINTK("FAT opendir g_dirNum err 0x%x!\r\n", g_dirNum);
        errno = ENFILE;
        goto ERROUT;
    }

    for (i = 0; i < FAT_MAX_OPEN_DIRS; i++) {
        if (g_dir[i].dir == NULL) {
            openNum = i;
            break;
        }
    }

    if (i >= FAT_MAX_OPEN_DIRS) {
        PRINTK("FAT dir opennum is out of range 0x%x!\r\n", openNum);
        errno = ENFILE;
        goto ERROUT;
    }

    ret = FsChangeDrive(dirName);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT opendir ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        goto ERROUT;
    }

    res = f_opendir(&g_dir[openNum], dirName);
    if (res != FR_OK) {
        g_dir[openNum].dir = NULL;
        PRINTK("FAT opendir err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        goto ERROUT;
    }

    g_dirNum++;

    FsUnlock();
    return &g_dir[openNum];

ERROUT:
    FsUnlock();
    return NULL;
}

struct dirent *fatfs_readdir(DIR *dir)
{
    FRESULT res;
    INT32 ret;
    FILINFO fileInfo = {0};

    if (dir == NULL) {
        errno = EBADF;
        return NULL;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return NULL;
    }

    res = f_readdir(dir, &fileInfo);
    /* if res not ok or fname is NULL , return NULL */
    if ((res != FR_OK) || (fileInfo.fname[0] == 0x0)) {
        PRINTK("FAT readdir err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        FsUnlock();
        return NULL;
    }

    (void)memcpy_s(g_retValue.d_name, sizeof(g_retValue.d_name), fileInfo.fname, sizeof(g_retValue.d_name));
    if (fileInfo.fattrib & AM_DIR) {
        g_retValue.d_type = DT_DIR;
    } else {
        g_retValue.d_type = DT_REG;
    }
    FsUnlock();

    return &g_retValue;
}

int fatfs_closedir(DIR *dir)
{
    FRESULT res;
    INT32 ret;

    if (dir == NULL) {
        errno = EBADF;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    g_dirNum--;

    res = f_closedir(dir);
    if (res != FR_OK) {
        PRINTK("FAT closedir err 0x%x!\r\n", res);
        FsUnlock();
        errno = FatfsErrno(res);
        return FS_FAILURE;
    }
    dir->dir = NULL;
    FsUnlock();

    return FS_SUCCESS;
}

int fatfs_rmdir(const char *path)
{
    FRESULT res;
    INT32 ret;

    if (path == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!FsCheckByPath(path)) {
        errno = EACCES;
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FsChangeDrive(path);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT rmdir ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_rmdir(path);
    if (res != FR_OK) {
        PRINTK("FAT rmdir err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }
    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_rename(const char *oldName, const char *newName)
{
    FRESULT res;
    INT32 ret;

    if ((oldName == NULL) || (newName == NULL)) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!FsCheckByPath(oldName) || !FsCheckByPath(newName)) {
        errno = EACCES;
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FsChangeDrive(oldName);
    if (ret != FS_SUCCESS) {
        PRINTK("FAT f_getfree ChangeDrive err 0x%x!\r\n", ret);
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_rename(oldName, newName);
    if (res != FR_OK) {
        PRINTK("FAT frename err 0x%x!\r\n", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }
    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_statfs(const char *path, struct statfs *buf)
{
    FATFS *fs = NULL;
    UINT32 freeClust;
    FRESULT res;
    INT32 ret;

    if ((path == NULL) || (buf == NULL)) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    ret = FsChangeDrive(path);
    if (ret != FR_OK) {
        PRINTK("FAT f_getfree ChangeDrive err %d.", ret);
        errno = FatfsErrno(FR_INVALID_PARAMETER);
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_getfree(path, &freeClust, &fs);
    if (res != FR_OK) {
        PRINTK("FAT f_getfree err 0x%x.", res);
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }
    buf->f_bfree  = freeClust;
    buf->f_bavail = freeClust;
    /* Cluster #0 and #1 is for VBR, reserve sectors and fat */
    buf->f_blocks = fs->n_fatent - 2;
#if FF_MAX_SS != FF_MIN_SS
    buf->f_bsize  = fs->ssize * fs->csize;
#else
    buf->f_bsize  = FF_MIN_SS * fs->csize;
#endif

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

static int do_truncate(int fd, off_t length, UINT count)
{
    FRESULT res = FR_OK;
    INT32 ret = FS_SUCCESS;
    DWORD csz;

    csz = (DWORD)(g_handle[fd].fil.obj.fs)->csize * SS(g_handle[fd].fil.obj.fs); /* Cluster size */
    if (length > csz * count) {
#if FF_USE_EXPAND
        res = f_expand(&g_handle[fd].fil, 0, (FSIZE_t)(length), FALLOC_FL_KEEP_SIZE);
#else
        errno = ENOSYS;
        ret = FS_FAILURE;
        return ret;
#endif
    } else if (length < csz * count) {
        res = f_truncate(&g_handle[fd].fil, (FSIZE_t)length);
    }

    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        return ret;
    }

    g_handle[fd].fil.obj.objsize = length; /* Set file size to length */
    g_handle[fd].fil.flag |= 0x40; /* Set modified flag */

    return ret;
}

int fatfs_ftruncate(int fd, off_t length)
{
    FRESULT res;
    INT32 ret;
    UINT count;
    DWORD fclust;

    if ((length < 0) || (length > UINT_MAX)) {
        errno = EINVAL;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    if (!IsValidFd(fd)) {
        errno = EBADF;
        ret = FS_FAILURE;
        goto OUT;
    }

    if (g_handle[fd].fil.obj.fs == NULL) {
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    if (!FsCheckByID(g_handle[fd].fil.obj.fs->id)) {
        errno = EACCES;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_getclustinfo(&g_handle[fd].fil, &fclust, &count);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = do_truncate(fd, length, count);
    if (ret != FR_OK) {
        goto OUT;
    }

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_fdisk(int pdrv, const unsigned int *partTbl)
{
    INT32 index;
    FRESULT res;
    INT32 ret;

    if (partTbl == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        ret = FS_FAILURE;
        goto OUT;
    }

    /* fdisk is not allowed when the device has been mounted. */
    for (index = 0; index < FF_VOLUMES; index++) {
        if ((g_fatfs[index].pdrv == pdrv) && (g_fatfs[index].fs_type != 0)) {
            errno = EBUSY;
            ret = FS_FAILURE;
            goto OUT;
        }
    }

    res = f_fdisk(pdrv, (DWORD const *)partTbl, g_workBuffer);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

int fatfs_format(const char *dev, int sectors, int option)
{
    INT32 index;
    FRESULT res;
    INT32 ret;

    if (dev == NULL) {
        errno = EFAULT;
        return FS_FAILURE;
    }

    ret = FsLock();
    if (ret != 0) {
        errno = ret;
        return FS_FAILURE;
    }

    index = FsPartitionMatch(dev, VOLUME_NAME);
    if (index == FS_FAILURE) {
        errno = ENOENT;
        ret = FS_FAILURE;
        goto OUT;
    }

    /* format is not allowed when the device has been mounted. */
    if (g_fatfs[index].fs_type != 0) {
        errno = EBUSY;
        ret = FS_FAILURE;
        goto OUT;
    }

    res = f_mkfs(dev, option, sectors, g_workBuffer, FF_MAX_SS);
    if (res != FR_OK) {
        errno = FatfsErrno(res);
        ret = FS_FAILURE;
        goto OUT;
    }

    ret = FS_SUCCESS;

OUT:
    FsUnlock();
    return ret;
}

struct MountOps g_fatfsMnt = {
    .Mount = fatfs_mount,
    .Umount = fatfs_umount,
    .Umount2 = fatfs_umount2,
    .Statfs = fatfs_statfs,
};

struct FileOps g_fatfsFops = {
    .Mkdir = fatfs_mkdir,
    .Unlink = fatfs_unlink,
    .Rmdir = fatfs_rmdir,
    .Opendir = fatfs_opendir,
    .Readdir = fatfs_readdir,
    .Closedir = fatfs_closedir,
    .Open = fatfs_open,
    .Close = fatfs_close,
    .Write = fatfs_write,
    .Read = fatfs_read,
    .Seek = fatfs_lseek,
    .Rename = fatfs_rename,
    .Getattr = fatfs_stat,
    .Fsync = fatfs_fsync,
    .Fstat = fatfs_fstat,
};