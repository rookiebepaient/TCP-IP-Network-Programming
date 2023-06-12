#include <stdio.h>
#include <fcntl.h>

int main(void) {
    FILE *fp;
    // 使用open函数创建文件并返回文件描述符
    int fd = open("data.dat", O_WRONLY | O_CREAT, O_TRUNC);
    if (fd == -1) {
        fputs("file open error", stdout);
        return -1;
    }
    // 调用fdopen函数将文件描述符转换为FILE指针，此时向第二个参数传递了w，因此返回写模式的FILE指针
    fp = fdopen(fd, "w");
    // 利用获取的写模式FILE指针，调用标准输出函数fputs
    fputs("Network, C programming \n", fp);
    // 利用FILE指针关闭文件，此时完全关闭，因此无需通过文件描述符关闭。而且调用fclose函数后，文件描述符也变成毫无意义的指针
    fclose(fp);
    return 0;
}