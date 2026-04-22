#ifndef __RAMFS_H__
#define __RAMFS_H__

#define MAX_NAME 32
#define MAX_FILES 64
#define MAX_FD    16

struct ramfs_file {
    char name[MAX_NAME];
    void *data;
    size_t size;
    int used;
};

struct ramfs_fd {
    int file_idx;
    size_t offset;
    int flags;
};

int ramfs_open(const char *path, int flags);
int ramfs_read(int fd, void *buf, size_t len);
int ramfs_write(int fd, const void *buf, size_t len);
int ramfs_close(int fd);

#endif /* __RAMFS_H__ */
