#include <flat.h>

int main (){
    ramfs_init();

    int fd = ramfs_open("/log.txt", O_CREAT | O_RDWR);
    ramfs_write(fd, "Hello ARM", 9);
    ramfs_close(fd);
    return 0;
}