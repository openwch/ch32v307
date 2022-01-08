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

#ifndef _FATFS_H
#define _FATFS_H

#include "fcntl.h"
#include "dirent.h"
#include "unistd.h"
#include "sys/mount.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "fs_config.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef FAT_MAX_OPEN_FILES
#define FAT_MAX_OPEN_FILES    50
#endif /* FAT_MAX_OPEN_FILES */

/* Format options */
#define FMT_FAT      0x01
#define FMT_FAT32    0x02
#define FMT_ANY      0x07

int fatfs_mount(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data);
int fatfs_umount(const char *target);
int fatfs_umount2(const char *target, int flag);
int fatfs_open(const char *path, int oflag, ...);
int fatfs_close(int fd);
ssize_t fatfs_read(int fd, void *buf, size_t nbyte);
ssize_t fatfs_write(int fd, const void *buf, size_t nbyte);
off_t fatfs_lseek(int fd, off_t offset, int whence);
int fatfs_unlink(const char *path);
int fatfs_fstat(int fd, struct stat *buf);
int fatfs_stat(const char *path, struct stat *buf);
int fatfs_fsync(int fd);
int fatfs_mkdir(const char *path, mode_t mode);
DIR *fatfs_opendir(const char *dirName);
struct dirent *fatfs_readdir(DIR *dir);
int fatfs_closedir(DIR *dir);
int fatfs_rmdir(const char *path);
int fatfs_rename(const char *oldName, const char *newName);
int fatfs_statfs(const char *path, struct statfs *buf);
int fatfs_ftruncate(int fd, off_t length);

/**
  * @brief divide a physical drive (SD card, U disk, and MMC card), this function is OHOS-specific
  * @param pdrv physical drive number.
  * @param partTbl list of partition size to create on the drive.
  *   -- item is <= 100: specifies the partition size in percentage of the entire drive space.
  *   -- item is > 100: specifies number of sectors.
  * @return fdisk result
  * @retval -1 fdisk error
  * @retval 0 fdisk successful
  */
int fatfs_fdisk(int pdrv, const unsigned int *partTbl);

/**
  * @brief format FAT device (SD card, U disk, and MMC card), this function is OHOS-specific
  * @param dev device name.
  * @param sectors sectors per cluster, can be 0 OR power of 2. The sector size for standard FAT volumes is 512 bytes.
  *    -- sector number is 0 OR >128: automatically choose the appropriate cluster size.
  *    -- sector number is 1 ~ 128: cluster size = sectors per cluster * 512B.
  * @param option file system type.
  *    -- FMT_FAT
  *    -- FMT_FAT32
  *    -- FMT_ANY
  * @return format result
  * @retval -1 format error
  * @retval 0 format successful
  */
int fatfs_format(const char *dev, int sectors, int option);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _FATFS_H */
