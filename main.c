#include <flat.h>

/* 
 find boot/ -depth -print0 | cpio -o --null > image.cpio
find boot/ -depth -print0 | cpio -o --null -H newc > image.cpio 
hexdump -C image.cpio | less -R
cpio -itv < image.cpio
*/

int main (){
    ramfs_init();

    int fd = ramfs_open("/log.txt", O_CREAT | O_RDWR);
    ramfs_write(fd, "Hello ARM", 9);
    ramfs_close(fd);
    return 0;
}