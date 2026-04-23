#include <flat.h>

static struct ramfs_file files[MAX_FILES];
static struct ramfs_fd fd_table[MAX_FD];

void ramfs_init(void) {
    memset(files, 0, sizeof(files));
    for (int i = 0; i < MAX_FD; i++) {
        fd_table[i].file_idx = -1;
    }
}

// Simple path → index lookup (no directories, just flat names)
static int ramfs_find_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++)
        if (files[i].used && strcmp(files[i].name, name) == 0)
            return i;
    return -1;
}

int ramfs_open(const char *path, int flags) {
    // 1. trim Path
    if (path[0] == '/') path++;

    // 2. Search for existing file
    int idx = ramfs_find_file(path);

    // 3. Handle creation if not found
    if (idx == -1) {
        if (!(flags & O_CREAT)) return -1;

        for (idx = 0; idx < MAX_FILES; idx++) {
            if (!files[idx].used) break;
        }

        if (idx == MAX_FILES) return -1; // No free slots

        // Allocate memory for the name using your working malloc
        size_t name_len = strlen(path) + 1;
        files[idx].name = (char *)malloc(name_len);
        if (!files[idx].name) return -1;

        memcpy(files[idx].name, path, name_len);
        files[idx].data = NULL;
        files[idx].size = 0;
        files[idx].used = 1;
    }

    // 4. Find free File Descriptor
    int fd;
    for (fd = 0; fd < MAX_FD; fd++) {
        if (fd_table[fd].file_idx == -1) break;
    }
    
    if (fd == MAX_FD) return -1;

    fd_table[fd].file_idx = idx;
    fd_table[fd].offset = 0;
    fd_table[fd].flags = flags;
    
    return fd;
}

int ramfs_read(int fd, void *buf, size_t len) {
    if (fd < 0 || fd >= MAX_FD || fd_table[fd].file_idx == -1) return -1;

    struct ramfs_file *f = &files[fd_table[fd].file_idx];
    
    if (fd_table[fd].offset >= f->size) return 0; // EOF

    size_t avail = f->size - fd_table[fd].offset;
    size_t to_read = (len < avail) ? len : avail;

    memcpy(buf, (uint8_t *)f->data + fd_table[fd].offset, to_read);
    fd_table[fd].offset += to_read;
    
    return (int)to_read;
}

int ramfs_write(int fd, const void *buf, size_t len) {
    if (fd < 0 || fd >= MAX_FD || fd_table[fd].file_idx == -1) return -1;

    struct ramfs_file *f = &files[fd_table[fd].file_idx];
    size_t new_pos = fd_table[fd].offset + len;

    if (new_pos > f->size) {
        // Use your working realloc to expand the file buffer
        void *new_data = realloc(f->data, new_pos);
        if (!new_data) return -1;
        
        f->data = new_data;
        f->size = new_pos;
    }

    memcpy((uint8_t *)f->data + fd_table[fd].offset, buf, len);
    fd_table[fd].offset += len;
    
    return (int)len;
}

int ramfs_close(int fd) {
    if (fd < 0 || fd >= MAX_FD) return -1;
    fd_table[fd].file_idx = -1;
    return 0;
}