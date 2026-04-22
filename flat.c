#include <string.h>
#include <stdlib.h>
#include <flat.h>

static struct ramfs_file files[MAX_FILES];
static struct ramfs_fd fd_table[MAX_FD];

// Simple path → index lookup (no directories, just flat names)
static int ramfs_find_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++)
        if (files[i].used && strcmp(files[i].name, name) == 0)
            return i;
    return -1;
}

int ramfs_open(const char *path, int flags) {
    // Skip leading slash if present
    if (path[0] == '/') path++;

    int idx = ramfs_find_file(path);
    if (idx == -1) {
        // Create new file if O_CREAT is set (simplified)
        if (!(flags & O_CREAT)) return -1;
        for (idx = 0; idx < MAX_FILES; idx++)
            if (!files[idx].used) break;
        if (idx == MAX_FILES) return -1;
        strncpy(files[idx].name, path, MAX_NAME);
        files[idx].data = NULL;
        files[idx].size = 0;
        files[idx].used = 1;
    }

    // Find free file descriptor
    int fd;
    for (fd = 0; fd < MAX_FD; fd++)
        if (fd_table[fd].file_idx == -1) break;
    if (fd == MAX_FD) return -1;

    fd_table[fd].file_idx = idx;
    fd_table[fd].offset = 0;
    fd_table[fd].flags = flags;
    return fd;
}

int ramfs_read(int fd, void *buf, size_t len) {
    if (fd < 0 || fd >= MAX_FD) return -1;
    int idx = fd_table[fd].file_idx;
    if (idx == -1) return -1;
    struct ramfs_file *f = &files[idx];
    size_t avail = f->size - fd_table[fd].offset;
    size_t to_read = len < avail ? len : avail;
    memcpy(buf, (char*)f->data + fd_table[fd].offset, to_read);
    fd_table[fd].offset += to_read;
    return to_read;
}

int ramfs_write(int fd, const void *buf, size_t len) {
    if (fd < 0 || fd >= MAX_FD) return -1;
    int idx = fd_table[fd].file_idx;
    if (idx == -1) return -1;
    struct ramfs_file *f = &files[idx];
    size_t new_size = fd_table[fd].offset + len;
    if (new_size > f->size) {
        void *new_data = realloc(f->data, new_size);
        if (!new_data) return -1;
        f->data = new_data;
        f->size = new_size;
    }
    memcpy((char*)f->data + fd_table[fd].offset, buf, len);
    fd_table[fd].offset += len;
    return len;
}

int ramfs_close(int fd) {
    if (fd < 0 || fd >= MAX_FD) return -1;
    fd_table[fd].file_idx = -1;
    return 0;
}
