#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handling(char *msg);

int main(void) {
    int fd;
    char buf[] = "Let's go!\n";
    // 打开模式的参数组合依次为 如果不存在该文件则创建，只能写该文件，若存在txt文件则清空文件的全部数据
    fd = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC);
    if (fd == -1) {
        error_handling("open() erroe!");
    }
    printf("file descriptor : %d \n", fd);

    if (write(fd, buf, sizeof(buf)) == -1) {
        error_handling("write() erroe!");
    }
    close(fd);

    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}