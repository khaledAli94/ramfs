#ifndef __RAMFS_H__
#define __RAMFS_H__

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_NAME 32
#define MAX_FILES 64
#define MAX_FD    16

/* POSIX std. defs */
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT  64


struct ramfs_file {
    char *name;
    void *data;
    size_t size;
    int used;
};

struct ramfs_fd {
    int file_idx;
    size_t offset;
    int flags;
};

void ramfs_init(void);
int ramfs_open(const char *path, int flags);
int ramfs_read(int fd, void *buf, size_t len);
int ramfs_write(int fd, const void *buf, size_t len);
int ramfs_close(int fd);

#endif /* __RAMFS_H__ */
