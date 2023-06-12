/* \brief 标准IO函数和系统函数之间的性能对比
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 3

int main(int argc, char *argv[]) {
    int fd1 = open("./Data300m.txt", O_RDONLY);
    int fd2 = open("./cpy.txt", O_WRONLY | O_CREAT | O_TRUNC);

    int len;
    char buf[BUF_SIZE];
    while ((len = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, len);
    }
    close(fd1);
    close(fd2);
    return 0;
}
